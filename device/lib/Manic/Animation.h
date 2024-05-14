#ifndef __MANIC_ANIMATION_H
#define __MANIC_ANIMATION_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace mysook {

/**
 * The Point class represents a 2D point with integer coordinates.
*/
struct Point {
    std::int32_t x;
    std::int32_t y;
};

/**
 * The Rect class represents a rectangle with integer coordinates.
*/
struct Rect {
    Point min;
    Point max;
};

/**
 * The Keyframe class represents a keyframe in an animation.
*/
class Keyframe {
    std::uint16_t img_index;
    std::uint16_t origin_x, origin_y;
    std::uint16_t millis;

public:
    Keyframe(std::istream &in);
};

class Image {
    Rect bounds;
    std::uint32_t pixel_origin;
    std::uint32_t stride;

public:
    Image(Rect &bounds);

    /**
     * This constuctor reads an Image from an input stream, reading the bounds.
    */
   Image(std::istream &in);
};

class Animation {
    char version;
    std::uint16_t frame_w, frame_h;
    std::vector<std::unique_ptr<Keyframe>> keyframes;
    std::vector<std::unique_ptr<Image>> images;

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
};


};

#endif//__MANIC_ANIMATION_H
