package manic

import (
	"encoding/binary"
	"image/color"
	"io"
)

type Encoder struct {
	w       io.Writer
	seekPos int
}

// NewEncoder creates a new animation encoder for the given io.Writer.
func NewEncoder(w io.Writer) *Encoder {
	return &Encoder{w: w}
}

func (e *Encoder) writeIt(data any) error {
	if err := binary.Write(e.w, byteOrder, data); err != nil {
		return err
	}
	e.seekPos += binary.Size(data)
	return nil
}

// Encode writes the given animation to the specified io.Writer.
func (e *Encoder) Encode(anim *Animation) (err error) {
	// Write the magic number
	if err = e.writeIt(theMagicNumber); err != nil {
		return
	}

	// Write the version number
	if err = e.writeIt(anim.Version); err != nil {
		return
	}

	// Write the frame width/height
	if err = e.writeIt(anim.FrameW); err != nil {
		return
	}
	if err = e.writeIt(anim.FrameH); err != nil {
		return
	}

	// Write out the keyframe length
	countKeyframes := uint16(len(anim.Keyframes))
	if err = e.writeIt(countKeyframes); err != nil {
		return
	}

	// Write out the image length
	countImages := uint16(len(anim.Images))
	if err = e.writeIt(countImages); err != nil {
		return
	}

	// Write out the keyframes
	for _, kf := range anim.Keyframes {
		if err = e.writeIt(kf.ImgIndex); err != nil {
			return
		}
		if err = e.writeIt(kf.Origin.X); err != nil {
			return
		}
		if err = e.writeIt(kf.Origin.Y); err != nil {
			return
		}
		if err = e.writeIt(kf.Millis); err != nil {
			return
		}
	}

	// This is the length of the image index, so all image index seek positions must come after this.
	indexLen := binary.Size(int32(0)) * 5 * len(anim.Images)

	// This holds the current seek position now. We will offset it after each
	// image to account for the byte length of that image.
	seekPos := int32(e.seekPos + indexLen)

	for _, img := range anim.Images {
		// Write out the image extents
		if err = e.writeIt(int32(img.Bounds().Min.X)); err != nil {
			return
		}
		if err = e.writeIt(int32(img.Bounds().Min.Y)); err != nil {
			return
		}
		if err = e.writeIt(int32(img.Bounds().Max.X)); err != nil {
			return
		}
		if err = e.writeIt(int32(img.Bounds().Max.Y)); err != nil {
			return
		}

		// Write out the image seek index position
		if err = e.writeIt(int32(seekPos)); err != nil {
			return
		}
		pixelCount := img.stride * int32(img.Bounds().Dy())
		seekPos += int32(binary.Size(byte(0))) * 3 * pixelCount
	}

	// Write out each image
	for _, img := range anim.Images {
		// Write out the pixel data
		for y := img.Bounds().Min.Y; y < img.Bounds().Max.Y; y++ {
			for x := img.Bounds().Min.X; x < img.Bounds().Max.X; x++ {
				c := img.At(x, y)
				nc := color.NRGBAModel.Convert(c).(color.NRGBA)
				if err = e.writeIt(nc.R); err != nil {
					return
				}
				if err = e.writeIt(nc.G); err != nil {
					return
				}
				if err = e.writeIt(nc.B); err != nil {
					return
				}
			}
		}
	}

	return nil
}

// Encode writes the given animation to the specified io.Writer.
func Encode(w io.Writer, anim *Animation) error {
	return NewEncoder(w).Encode(anim)
}
