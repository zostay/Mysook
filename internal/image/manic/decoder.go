package manic

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
)

var (
	byteOrder      = binary.LittleEndian
	theMagicNumber = [5]byte{'M', 'A', 'N', 'I', 'C'}
)

type Decoder struct {
	r io.Reader
}

// NewDecoder creates a new animation decoder for the given io.Reader.
func NewDecoder(r io.Reader) *Decoder {
	return &Decoder{r: r}
}

func (d *Decoder) decodeKeyframe(keyframe *Keyframe) (err error) {
	if err = binary.Read(d.r, byteOrder, &keyframe.ImgIndex); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &keyframe.Origin.X); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &keyframe.Origin.Y); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &keyframe.Millis); err != nil {
		return
	}

	return
}

func (d *Decoder) decodeImage(image *Image) (err error) {
	if err = binary.Read(d.r, byteOrder, &image.Rect.Min.X); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &image.Rect.Min.Y); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &image.Rect.Max.X); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &image.Rect.Max.Y); err != nil {
		return
	}

	image.stride = image.Rect.Max.X - image.Rect.Min.X

	// Throw away the image index
	var ignore int32
	if err = binary.Read(d.r, byteOrder, &ignore); err != nil {
		return
	}

	return
}

// Decode loads an animation from the given io.Reader.
func (d *Decoder) Decode(anim *Animation) (err error) {
	// Read Magic Number
	var magicNumber [5]byte
	if !bytes.Equal(magicNumber[:], theMagicNumber[:]) {
		return fmt.Errorf("incorrect magic number, expected MANIC, go %s", string(magicNumber[:]))
	}

	// Read the version
	if err = binary.Read(d.r, byteOrder, &anim.Version); err != nil {
		return
	}

	// Verify the version
	if anim.Version != Version0 {
		return fmt.Errorf("unsupported manic animation version")
	}

	// Load the frame width/height
	if err = binary.Read(d.r, byteOrder, &anim.FrameW); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &anim.FrameH); err != nil {
		return
	}

	// Load keyframe and image counts
	var keyframeCount, imageCount uint16
	if err = binary.Read(d.r, byteOrder, &keyframeCount); err != nil {
		return
	}
	if err = binary.Read(d.r, byteOrder, &imageCount); err != nil {
		return
	}

	// Load the key frames
	for range keyframeCount {
		var keyframe Keyframe
		if err = d.decodeKeyframe(&keyframe); err != nil {
			return
		}
		anim.Keyframes = append(anim.Keyframes, keyframe)
	}

	for range imageCount {
		// Load the image extents
		var image Image
		if err = d.decodeImage(&image); err != nil {
			return
		}

		// Attach the image
		anim.Images = append(anim.Images, image)
	}

	// Load the images
	for i := range imageCount {
		image := &anim.Images[i]

		// Calculate the byte size of the image
		dy := image.Rect.Max.Y - image.Rect.Min.Y
		pLen := image.stride * dy * 3
		image.PixelData = make([]byte, pLen)

		// Read in the pixel data of the image
		buf := make([]byte, 16384)
		var ins int32
		for {
			remain := pLen - ins
			var n int
			if remain > int32(len(buf)) {
				n, err = d.r.Read(buf)
			} else {
				n, err = d.r.Read(buf[:remain])
			}
			if err != nil {
				return
			}

			copy(image.PixelData[ins:], buf[:n])
			ins += int32(n)
			if ins >= pLen {
				break
			}
		}
	}

	return
}

// Decode loads an animation from the given io.Reader.
func Decode(r io.Reader) (*Animation, error) {
	d := NewDecoder(r)
	anim := &Animation{}
	if err := d.Decode(anim); err != nil {
		return nil, err
	}

	return anim, nil
}
