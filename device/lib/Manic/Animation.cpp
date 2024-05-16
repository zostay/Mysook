#include "Animation.h"

using namespace mysook;

AnimationKeyframe::AnimationKeyframe(std::istream &in) {
    in.read((char*) &img_index, 2);
    in.read((char*) &origin_x, 2);
    in.read((char*) &origin_y, 2);
    in.read((char*) &millis, 2);
}

AnimationImage::AnimationImage(std::istream &in) {
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
        std::unique_ptr<AnimationKeyframe> keyframe(new AnimationKeyframe(in));
        keyframes.push_back(std::move(keyframe));
    }

    int numImages;
    in.read((char*) &numImages, 4);
    for (int i = 0; i < numImages; i++) {
        std::unique_ptr<AnimationImage> image(new AnimationImage(in));
        images.push_back(std::move(image));
    }
}

template<int W, int H> 
std::uint16_t Animation::render(mysook::RGBPanel<W, H> *panel, std::uint16_t frame_num) {
    if (frame_num >= keyframes.size()) {
        return 0;
    }

    AnimationKeyframe *keyframe = keyframes[frame_num].get();
    AnimationImage *image = images[keyframe->img_index].get();

    for (int y = image->bounds.min.y; y < image->bounds.max.y; y++) {
        for (int x = image->bounds.min.x; x < image->bounds.max.x; x++) {
            int pixel = image->pixel_origin + (y * image->stride) + (x * 3);
            panel->put_pixel(x - keyframe->origin_x, y - keyframe->origin_y, 
                             pixel[pixel], pixel[pixel + 1], pixel[pixel + 2]);
        }
    }

    return frame_num + 1;
}