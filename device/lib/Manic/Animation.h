#ifndef __MANIC_ANIMATION_H
#define __MANIC_ANIMATION_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <memory>

#ifdef ARDUINO
#ifdef USE_LittleFS
  #include <LittleFS.h>
#endif//USE_LittleFS
#include <Stream.h>
#endif//ARDUINO

#ifdef ESP_PLATFORM
#include "esp_log.h"
static const char *ANIM_TAG = "Animation";
#define log_verbose(fmt, ...) ESP_LOGV(ANIM_TAG, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)   ESP_LOGD(ANIM_TAG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    ESP_LOGI(ANIM_TAG, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   ESP_LOGE(ANIM_TAG, fmt, ##__VA_ARGS__)
#else//ESP_PLATFORM
#define log_verbose(fmt, ...)
#define log_debug(fmt, ...)
#define log_info(fmt, ...)
#define log_error(fmt, ...)
#endif//ESP_PLATFORM

#include <Firmware.h>
#include <Panel.h>
#include <Reader.h>

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

public:
    AnimationKeyframe(F &in) {
        in.read((char*) &img_index, 2);
        in.read((char*) &origin_x, 4;
        in.read((char*) &origin_y, 4);
        in.read((char*) &millis, 2);
        logf_ln("D [animation] Read keyframe: %d %d %d %d", img_index, origin_x, origin_y, millis);
    }

    std::uint16_t get_img_index() const { return img_index; }
    std::uint16_t get_origin_x() const { return origin_x; }
    std::uint16_t get_origin_y() const { return origin_y; }
    std::uint16_t get_millis() const { return millis; }
};

template <class F>
class AnimationImage {
    AnimationRect bounds;
    std::uint32_t pixel_origin;
    std::uint32_t stride;

public:
    AnimationImage(AnimationRect &bounds);

    /**
     * This constuctor reads an Image from an input stream, reading the bounds.
     */
    AnimationImage(F &in) {
        in.read((char*) &bounds, sizeof(bounds));
        in.read((char*) pixel_origin, 4);
        stride = (bounds.max.x - bounds.min.x) * 3;
        logf_ln("D [animation] Read image: %x %x %x %x %x %x %x", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, pixel_origin, stride);
    }

    AnimationRect get_bounds() const { return bounds; }
    std::uint32_t get_pixel_origin() const { return pixel_origin; }
    std::uint32_t get_stride() const { return stride; }
};

template <class F>
class Animation {
    char version;
    std::uint16_t frame_w, frame_h;
    std::vector<std::unique_ptr<AnimationKeyframe<F>>> keyframes;
    std::vector<std::unique_ptr<AnimationImage<F>>> images;
    F &in;

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
    Animation(F &in) {
        logf_ln("I [animation] Reading Animation header");
        char magic[5];
        in.read((char*) magic, 5);
        in.read((char*) &version, 1);
        in.read((char*) &frame_w, 2);
        in.read((char*) &frame_h, 2);

        std::uint16_t numKeyframes, numImages;
        in.read((char*) &numKeyframes, 2);
        in.read((char*) &numImages, 2);

        logf_ln("I [animation] Reading %d keyframes", numKeyframes);
        for (int i = 0; i < numKeyframes; i++) {
            std::unique_ptr<AnimationKeyframe> keyframe(new AnimationKeyframe(in));
            logf_ln("I [animation] Animation header read: %d %d", keyframe.get()->get_img_index(), keyframe.get()->get_millis());
            keyframes.push_back(std::move(keyframe));
        }

        logf_ln("I [animation] Reading %d images", numImages);
        for (int i = 0; i < numImages; i++) {
            std::unique_ptr<AnimationImage> image(new AnimationImage(in));
            images.push_back(std::move(image));
        }

        logf_ln("I [animation] Animation loaded");
    }
    
    /**
     * Render a frame of the animation to the panel. You specify the panel to
     * write to and the frame number to render. This function returns the next
     * frame number in the sequence.
     */
    template<int W, int H> 
    std::uint16_t render(mysook::RGBPanel<W, H> *panel, std::uint16_t frame_num, std::istream &in) {
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
            in.seekg(seekpos);

            // read the row of pixels and update the panel
            std::uint8_t *row = pixels.get();
            in.read((char*) row, w*3);
            for (int x = 0; x < w; x++) {
                panel->put_pixel(x, y, row[x*3], row[x*3+1], row[x*3+2]);
            }
        }

        // next frame or 0 if we reached the end of the animation
        return (frame_num + 1) % keyframes.size();
    }

#ifdef ARDUINO
    template<int W, int H> 
    std::uint16_t render(mysook::RGBPanel<W, H> *panel, std::uint16_t frame_num, fs::File &stream) {
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
            stream.seek(seekpos, fs::SeekSet);
            // log_debug("seekpos: %x", image->get_pixel_origin());

            // read the row of pixels and update the panel
            uint8_t *row = pixels.get();
            stream.readBytes((char*) row, w*3);
            for (int x = 0; x < w; x++) {
                //log_debug("put_pixel(%d, %d) <- #%02x%02x%02x", x, y, row[x*3], row[x*3+1], row[x*3+2]);
                panel->put_pixel(x, y, row[x*3], row[x*3+1], row[x*3+2]);
            }
        }

        // next frame or 0 if we reached the end of the animation
        return (frame_num + 1) % keyframes.size();
    }
#endif//ARDUINO

    /**
     * This is the amount of time the animation should hold on the given frame.
     */
    std::uint16_t frame_time(std::uint16_t frame_num) const {
        return keyframes[frame_num].get()->get_millis();
    }
};

/*
 * The Animator animates an Animation.
 */
template<int W, int H> 
class Animator : public Ticking {
    RGBPanel<W, H> &panel;
    Animation *animation;

    unsigned long next_tick = 0;

public:
    Animator(RGBPanel<W, H> &panel) : panel(panel) { }

    void set_animation(Animation *animation) {
        this->animation = animation;
    }

    void clear_animation() {
        this->animation = nullptr;
    }

    virtual bool ready_for_tick(unsigned long now) {
        return animation != nullptr && next_tick <= now;
    }

    virtual void tick(unsigned long now) {
        if (animation == nullptr) {
            return;
        }

        next_tick = now + animation->frame_time(0) * 1000;
        animation->render(&panel, 0);
    }

    /**
     * Render a frame of the animation to the panel. You specify the panel to
     * write to and the frame number to render. This function returns the next
     * frame number in the sequence.
     */
    template<int W, int H, class F> 
    std::uint16_t render(std::uint16_t frame_num, F &in) {
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
#endif//ARDUINO
};

};

#endif//__MANIC_ANIMATION_H
