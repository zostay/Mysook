#include "Animation.h"

using namespace mysook;

Keyframe::Keyframe(std::istream &in) {
    in.read((char*) &img_index, 2);
    in.read((char*) &origin_x, 2);
    in.read((char*) &origin_y, 2);
    in.read((char*) &millis, 2);
}

Image::Image(std::istream &in) {
    in.read((char*) bounds.min.x, 4);
    in.read((char*) bounds.min.y, 4);
    in.read((char*) bounds.max.x, 4);
    in.read((char*) bounds.max.y, 4);
    in.read((char*) pixel_origin, 4);
    stride = (bounds.max.x - bounds.min.x) * 3;
}

Animation::Animation(std::istream &in) {
    in.read((char*) &version, 1);
    in.read((char*) &frame_w, 2);
    in.read((char*) &frame_h, 2);

    int numKeyframes;
    in.read((char*) &numKeyframes, 4);
    for (int i = 0; i < numKeyframes; i++) {
        std::unique_ptr<Keyframe> keyframe(new Keyframe(in));
        keyframes.push_back(std::move(keyframe));
    }

    int numImages;
    in.read((char*) &numImages, 4);
    for (int i = 0; i < numImages; i++) {
        std::unique_ptr<Image> image(new Image(in));
        images.push_back(std::move(image));
    }
}

/**
 * Render will seek
*/