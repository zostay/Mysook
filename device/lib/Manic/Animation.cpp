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
    char magic[5];
    in.read((char*) magic, 5);
    in.read((char*) &version, 1);
    in.read((char*) &frame_w, 2);
    in.read((char*) &frame_h, 2);

    std::uint16_t numKeyframes, numImages;
    in.read((char*) &numKeyframes, 2);
    in.read((char*) &numImages, 2);

    for (int i = 0; i < numKeyframes; i++) {
        std::unique_ptr<AnimationKeyframe> keyframe(new AnimationKeyframe(in));
        keyframes.push_back(std::move(keyframe));
    }

    for (int i = 0; i < numImages; i++) {
        std::unique_ptr<AnimationImage> image(new AnimationImage(in));
        images.push_back(std::move(image));
    }
}

#if defined(ARDUINO) || defined(ESP_PLATFORM)
AnimationKeyframe::AnimationKeyframe(Stream &stream) {
    stream.readBytes((char*) &img_index, 2);
    stream.readBytes((char*) &origin_x, 4);
    stream.readBytes((char*) &origin_y, 4);
    stream.readBytes((char*) &millis, 2);
    log_info("Read keyframe: %d %d %d %d", img_index, origin_x, origin_y, millis);
}

AnimationImage::AnimationImage(Stream &stream) {
    stream.readBytes((char*) &bounds, sizeof(bounds));
    stream.readBytes((char*) &pixel_origin, 4);
    stride = (bounds.max.x - bounds.min.x) * 3;
    log_info("Read image: %x %x %x %x %x %x %x", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, pixel_origin, stride);
}

Animation::Animation(Stream &stream) {
    log_info("Reading Animation header");
    char magic[5];
    stream.readBytes((char*) magic, 5);
    stream.readBytes((char*) &version, 1);
    stream.readBytes((char*) &frame_w, 2);
    stream.readBytes((char*) &frame_h, 2);

    std::uint16_t numKeyframes, numImages;
    stream.readBytes((char*) &numKeyframes, 2);
    stream.readBytes((char*) &numImages, 2);

    log_info("Reading %d keyframes", numKeyframes);
    for (int i = 0; i < numKeyframes; i++) {
        std::unique_ptr<AnimationKeyframe> keyframe(new AnimationKeyframe(stream));
        log_info("Animation header read: %d %d", keyframe.get()->get_img_index(), keyframe.get()->get_millis());
        keyframes.push_back(std::move(keyframe));
    }

    log_info("Reading %d images", numImages);
    for (int i = 0; i < numImages; i++) {
        std::unique_ptr<AnimationImage> image(new AnimationImage(stream));
        images.push_back(std::move(image));
    }
    log_info("Animation loaded");
}
#endif//ARDUINO