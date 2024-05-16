#ifndef __MANIC_ANIMATION_H
#define __MANIC_ANIMATION_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include <Panel.h>

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
    std::uint16_t origin_x, origin_y;
    std::uint16_t millis;

public:
    AnimationKeyframe(std::istream &in);
};

class AnimationImage {
    AnimationRect bounds;
    std::uint32_t pixel_origin;
    std::uint32_t stride;

public:
    AnimationImage(AnimationRect &bounds);

    /**
     * This constuctor reads an Image from an input stream, reading the bounds.
    */
   AnimationImage(std::istream &in);
};

class Animation {
    char version;
    std::uint16_t frame_w, frame_h;
    std::vector<std::unique_ptr<AnimationKeyframe>> keyframes;
    std::vector<std::unique_ptr<AnimationImage>> images;

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
    Animation(std::istream &in);

    /**
     * Render a frame of the animation to the panel. You specify the panel to
     * write to and the frame number to render. This function returns the next
     * frame number in the sequence.
     */
    template<int W, int H> 
    std::uint16_t render(mysook::RGBPanel<W, H> *panel, std::uint16_t frame_num);
};


};

#endif//__MANIC_ANIMATION_H
