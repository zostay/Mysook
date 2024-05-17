package manic

import (
	"image"
	"image/color"
	"math"
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

// pixelData extracts the pixel data from an image.Image.
func pixelData(img image.Image) []byte {
	bounds := img.Bounds()
	w, h := bounds.Dx(), bounds.Dy()
	data := make([]byte, 0, w*h*3)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			c := color.NRGBAModel.Convert(img.At(x, y)).(color.NRGBA)
			data = append(data, []byte{c.R, c.G, c.B}...)
		}
	}
	return data
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

	mImg := NewImage(img.Bounds(), pixelData(img))

	b.anim.Images = append(b.anim.Images, *mImg)
	return uint16(len(b.anim.Images) - 1), nil
}

// AddKeyframe adds a single keyframe to the animation.
func (b *Builder) AddKeyframe(
	idx uint16,
	x, y int32,
	millis uint16,
) uint16 {
	b.anim.Keyframes = append(b.anim.Keyframes, Keyframe{
		ImgIndex: idx,
		Origin: Point{
			X: x,
			Y: y,
		},
		Millis: millis,
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
	millisTotal uint16, // milliseconds for the whole animation
	millisPerFrame uint16, // milliseconds per frame
	count int, // number of keyframes to add
	ease Easing, // easing function (nil for linear)
) (uint16, uint16) {
	if ease == nil {
		ease = Linear
	}

	firstFrame := len(b.anim.Keyframes)

	frameCount := float64(millisTotal) / float64(millisPerFrame)
	for f := 0.0; f < frameCount; f++ {
		x := float64(sx) + float64(ex-sx)*ease(f/frameCount)
		y := float64(sy) + float64(ey-sy)*ease(f/frameCount)
		// TODO Compress the frames by combining sequences of idential (x, y) frames.
		b.AddKeyframe(idx, int32(math.Round(x)), int32(math.Round(y)), millisPerFrame)
	}

	lastFrame := len(b.anim.Keyframes)

	return uint16(firstFrame), uint16(lastFrame)
}

// Animation returns the constructed animation.
func (b *Builder) Animation() *Animation {
	return b.anim
}
