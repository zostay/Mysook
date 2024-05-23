#ifndef __MANIC_ANIMATION_H
#define __MANIC_ANIMATION_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <memory>

#ifdef ARDUINO
#include <LittleFS.h>
#include <Stream.h>
#endif//ARDUINO

#include <Firmware.h>
#include <Panel.h>
#include <Reader.h>
#include <Logger.h>

namespace mysook {

/**
 * The Point class represents a 2D point with integer coordinates.
*/
struct AnimationPoint {
    std::int32_t x;
    std::int32_t y;
};

/**
 * The Rect class represents a rectangle with integer coordinates.
*/
struct AnimationRect {
    AnimationPoint min;
    AnimationPoint max;
};

/**
 * The Keyframe class represents a keyframe in an animation.
*/
class AnimationKeyframe {
    std::uint16_t img_index;
    std::int32_t origin_x, origin_y;
    std::uint16_t millis;
    Logger &log;

public:
    AnimationKeyframe(Logger &log, Reader &in) : log(log) {
        in.read((char*) &img_index, 2);
        in.read((char*) &origin_x, 4);
        in.read((char*) &origin_y, 4);
        in.read((char*) &millis, 2);
        log.logf_ln("D [animation] Read keyframe: %d %d %d %d", img_index, origin_x, origin_y, millis);
    }

    std::uint16_t get_img_index() const { return img_index; }
    std::uint16_t get_origin_x() const { return origin_x; }
    std::uint16_t get_origin_y() const { return origin_y; }
    std::uint16_t get_millis() const { return millis; }
};

class AnimationImage {
    AnimationRect bounds;
    std::uint32_t pixel_origin;
    std::uint32_t stride;
    Logger &log;

public:
    /**
     * This constuctor reads an Image from an input stream, reading the bounds.
     */
    AnimationImage(Logger &log, Reader &in) : log(log) {
        in.read((char*) &bounds, sizeof(bounds));
        in.read((char*) pixel_origin, 4);
        stride = (bounds.max.x - bounds.min.x) * 3;
        log.logf_ln("D [animation] Read image: %x %x %x %x %x %x %x", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, pixel_origin, stride);
    }

    AnimationRect get_bounds() const { return bounds; }
    std::uint32_t get_pixel_origin() const { return pixel_origin; }
    std::uint32_t get_stride() const { return stride; }
};

class Animation {
    char version;
    std::uint16_t frame_w, frame_h;
    std::vector<std::unique_ptr<AnimationKeyframe>> keyframes;
    std::vector<std::unique_ptr<AnimationImage>> images;
    Reader &in;
    std::unique_ptr<std::uint8_t[]> pixels;

    Logger &log;

public:
    /**
     * This constructor loads an Animation from an input stream, decoding the
     * header, keyframes, and uses the image index to load the bounds of each
     * image and the start of the pixel data for each image. This assumes a
     * seekable stream that can be held open for reading to actual display the
     * animation.
     * 
     * The stream should be left open. The animation can then use the stream to
     * render individual frames.
     */
    Animation(Logger &log, Reader &in) : log(log), in(in) {
        log.logf_ln("I [animation] Reading Animation header");
        char magic[5];
        in.read((char*) magic, 5);
        in.read((char*) &version, 1);
        in.read((char*) &frame_w, 2);
        in.read((char*) &frame_h, 2);

        pixels.reset(new std::uint8_t[frame_w*frame_h*3]);

        std::uint16_t numKeyframes, numImages;
        in.read((char*) &numKeyframes, 2);
        in.read((char*) &numImages, 2);

        log.logf_ln("I [animation] Reading %d keyframes", numKeyframes);
        for (int i = 0; i < numKeyframes; i++) {
            std::unique_ptr<AnimationKeyframe> keyframe(new AnimationKeyframe(log, in));
            log.logf_ln("I [animation] Animation header read: %d %d", keyframe.get()->get_img_index(), keyframe.get()->get_millis());
            keyframes.push_back(std::move(keyframe));
        }

        log.logf_ln("I [animation] Reading %d images", numImages);
        for (int i = 0; i < numImages; i++) {
            std::unique_ptr<AnimationImage> image(new AnimationImage(log, in));
            images.push_back(std::move(image));
        }

        log.logf_ln("I [animation] Animation loaded");
    }
    
    /**
     * This is the amount of time the animation should hold on the given frame.
     */
    std::uint16_t frame_time(std::uint16_t frame_num) const {
        return keyframes[frame_num].get()->get_millis();
    }

    /**
     * This function returns the pixels for the given frame number. The pointer
     * returned is allocated by this class and owned by this class and should
     * not be freed. The data in this pointer will be overwritten during a
     * subsequent call to this function.
     */
    std::uint8_t *frame_pixels(std::uint16_t frame_num) const {
        // weird state, reset to 0
        if (frame_num >= keyframes.size()) {
            return nullptr;
        }

        AnimationKeyframe *keyframe = keyframes[frame_num].get();
        AnimationImage *image = images[keyframe->get_img_index()].get();

        // we start each row at the same x position and read a whole row at a time
        int img_x = keyframe->get_origin_x() - image->get_bounds().min.x;

        // the maximum y position in the image for bounds checking
        int max_y = image->get_bounds().max.y - image->get_bounds().min.y;
        
        std::uint8_t *pdata = pixels.get();
        int offset = 0;
        for (int y = 0; y < frame_h; y++) {
            // calculate the y position in the image
            int img_y = y + keyframe->get_origin_y() - image->get_bounds().min.y;

            // prevent reading past the end of the image
            if (img_y > max_y) {
                break;
            }

            // seek to the correct position in the file to read the row
            int seekpos = image->get_pixel_origin() + img_y * image->get_stride() + img_x * 3;
            in.seekg(seekpos);
            // log_debug("seekpos: %x", image->get_pixel_origin());

            // read the row of pixels and update the panel
            in.read((char*) pdata+offset, frame_w*3);
            offset += frame_w*3;
        }

        return pdata;
    }
};

/*
 * The Animator animates an Animation.
 */
template<int W, int H> 
class Animator : public Ticking {
    RGBPanel<W, H> &panel;
    Animation *animation;

    bool ready_to_load = true;
    unsigned long next_tick = 0;
    std::uint16_t frame_num = 0;

public:
    Animator(RGBPanel<W, H> &panel) : panel(panel) { }

    void set_animation(Animation *animation) {
        if (animation == nullptr) {
            clear_animation();
            return;
        }

        if (animation->frame_h != H || animation->frame_w != W) {
            logf_ln("E [animation] Animation frame size %dx%d does not match panel size %dx%d", animation->frame_w, animation->frame_h, W, H);
            clear_animation();
            return;
        }

        ready_to_load = true;
        frame_num = 0;
        next_tick = 0;

        this->animation = animation;
    }

    void clear_animation() {
        ready_to_load = true;
        frame_num = 0;
        next_tick = 0;

        this->animation = nullptr;
    }

    virtual bool ready_for_tick(unsigned long now) {
        return animation != nullptr && (ready_to_load || next_tick <= now);
    }

    virtual void tick(unsigned long now) {
        if (animation == nullptr) {
            return;
        }

        if (ready_to_load) {
            load_frame();
        }

        if (next_tick <= now) {
            render_frame(now);
        }
    }

private:
    void load_frame() {
        std::uint8_t *pixels = animation->frame_pixels(frame_num);

        // something weird happened try to reset
        if (pixels == nullptr) {
            logf_ln("E [animation] Frame %d has no pixels; flipping to frame 0", frame_num);
            frame_num = 0;
            return;
        }

        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                i = y * W + x;
                panel.put_pixel(x, y, pixels[i*3], pixels[i*3+1], pixels[i*3+2]);
            }
        }
    }

    void render_frame() {
        frame_num = render();
        next_tick = now + animation->frame_time(frame_num) * 1000;
        ready_to_load = true;
    }


    /**
     * Render a frame of the animation to the panel. You specify the panel to
     * write to and the frame number to render. This function returns the next
     * frame number in the sequence.
     */
    std::uint16_t render() {
        panel.draw();
        // weird state, reset to 0
        if (frame_num >= keyframes.size()) {
            return 0;
        }

        // ensure we don't update outside the bounds of the panel or the frame
        int w = frame_w > W ? W : frame_w;
        int h = frame_h > H ? H : frame_h;

        AnimationKeyframe *keyframe = keyframes[frame_num].get();
        AnimationImage *image = images[keyframe->get_img_index()].get();

        // we start each row at the same x position and read a whole row at a time
        int img_x = keyframe->get_origin_x() - image->get_bounds().min.x;

        // the maximum y position in the image for bounds checking
        int max_y = image->get_bounds().max.y - image->get_bounds().min.y;
        
        std::unique_ptr<uint8_t[]> pixels(new uint8_t[w*3]);
        for (int y = 0; y < h; y++) {
            // calculate the y position in the image
            int img_y = y + keyframe->get_origin_y() - image->get_bounds().min.y;

            // prevent reading past the end of the image
            if (img_y > max_y) {
                break;
            }

            // seek to the correct position in the file to read the row
            int seekpos = image->get_pixel_origin() + img_y * image->get_stride() + img_x * 3;
            in.seekg(seekpos, fs::SeekSet);
            // log_debug("seekpos: %x", image->get_pixel_origin());

            // read the row of pixels and update the panel
            uint8_t *row = pixels.get();
            in.read((char*) row, w*3);
            for (int x = 0; x < w; x++) {
                //log_debug("put_pixel(%d, %d) <- #%02x%02x%02x", x, y, row[x*3], row[x*3+1], row[x*3+2]);
                panel->put_pixel(x, y, row[x*3], row[x*3+1], row[x*3+2]);
            }
        }

        // next frame or 0 if we reached the end of the animation
        return (frame_num + 1) % keyframes.size();
    }
};

};

#endif//__MANIC_ANIMATION_H
