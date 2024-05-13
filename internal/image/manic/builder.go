package manic

import (
	"image"
	"os"
)

// Builder is a toolkit for helping with making manic animations.
type Builder struct {
	anim *Animation
}

// NewBuilder constructs a Builder for an animation with the given frame width
// and height.
func NewBuilder(w, h uint16) *Builder {
	return &Builder{New(w, h)}
}

// AddImageFile adds image file from the OS to the animation.
func (b *Builder) AddImageFile(file string) (uint16, error) {
	r, err := os.Open(file)
	if err != nil {
		return 0, err
	}

	img, _, err := image.Decode(r)
	if err != nil {
		return 0, err
	}

	if err = r.Close(); err != nil {
		return 0, err
	}

	b.anim.Images = append(b.anim.Images, img)
	return uint16(len(b.anim.Images) - 1), nil
}

// AddKeyframe adds a single keyframe to the animation.
func (b *Builder) AddKeyframe(
	idx uint16,
	x, y uint16,
	millis uint16,
) uint16 {
	b.anim.Keyframes = append(b.anim.Keyframes, Keyframe{
		ImgIndex: idx,
		OriginX:  x,
		OriginY:  y,
		Millis:   millis,
	})

	return uint16(len(b.anim.Keyframes) - 1)
}

// AddRepeatKeyframe adds a keyframe that repeats the previous keyframe.
func (b *Builder) AddRepeatKeyframe(
	kfIdx uint16,
	millis uint16,
) uint16 {
	kf := b.anim.Keyframes[kfIdx]
	b.anim.Keyframes = append(b.anim.Keyframes, kf)
	newIdx := uint16(len(b.anim.Keyframes) - 1)
	b.anim.Keyframes[newIdx].Millis = millis
	return newIdx
}

// AddScanlineKeyframes adds a series of keyframes that scan across the image.
func (b *Builder) AddScanlineKeyframes(
	idx uint16, // image index
	sx, sy uint16, // start x, y
	ex, ey uint16, // end x, y
	millis uint16, // milliseconds for the whole animation
	count int, // number of keyframes to add
	ease Easing, // easing function (nil for linear)
) (uint16, uint16) {
	if ease == nil {
		ease = Linear
	}

	dx := float64(ex-sx) / float64(count)
	dy := float64(ey-sy) / float64(count)
	dMillis := float64(millis) / float64(count)

	var (
		x = float64(sx)
		y = float64(sy)
		f = float64(0)
	)
	for range count {
		fm := uint16(dMillis * ease(f))
		b.AddKeyframe(idx, uint16(x), uint16(y), fm)
		x += dx
		y += dy
		f += 1 / float64(count)
	}

	return uint16(len(b.anim.Keyframes) - count), uint16(len(b.anim.Keyframes))
}

// Animation returns the constructed animation.
func (b *Builder) Animation() *Animation {
	return b.anim
}
