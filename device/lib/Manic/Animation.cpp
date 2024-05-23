#include "Animation.h"
#include <Logger.h>

using namespace mysook;

template <class F>
AnimationKeyframe<F>::AnimationKeyframe(F &in) {
    in.read((char*) &img_index, 2);
    in.read((char*) &origin_x, 4;
    in.read((char*) &origin_y, 4);
    in.read((char*) &millis, 2);
    logf_ln("D [animation] Read keyframe: %d %d %d %d", img_index, origin_x, origin_y, millis);
}

template <class F>
virtual bool Animator<F>::ready_for_tick(unsigned long now) {

}

template <class F>
virtual void Animator<F>::tick(unsigned long now) {

}