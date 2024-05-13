package manic

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"io"
)

const Version0 = '0'

var (
	byteOrder      = binary.LittleEndian
	theMagicNumber = [5]byte{'M', 'A', 'N', 'I', 'C'}
)

// Animation is the internal representation of a manic animation.
type Animation struct {
	Version        byte
	FrameW, FrameH uint16
	Keyframes      []Keyframe
	Images         []image.Image
}

// Keyframe represents a single frame to use to fill the entire display
type Keyframe struct {
	ImgIndex         uint16
	OriginX, OriginY uint16
	Millis           uint16
}

// Image represents a single image within a manic animation.
type Image struct {
	Rect      image.Rectangle
	PixelData []byte
}

// At returns the color of the given pixel
func (i *Image) At(x, y int) color.Color {
	idx := (y-i.Rect.Min.Y)*i.Rect.Dx() + (x - i.Rect.Min.X)
	r := i.PixelData[idx]
	g := i.PixelData[idx]
	b := i.PixelData[idx]

	return color.NRGBA{R: r, G: g, B: b, A: 0xFF}
}

// Bounds returns the bounding rectangle of the image.
func (i *Image) Bounds() image.Rectangle {
	return i.Rect
}

// ColorModel returns NRGBAModel, but the alpha part is not stored and always set
// to 0xff on load.
func (i *Image) ColorModel() color.Model {
	return color.NRGBAModel
}

// New creates a new, empty manic animation.
func New(w, h uint16) *Animation {
	return &Animation{
		Version: Version0,
		FrameW:  w,
		FrameH:  h,
	}
}

// Decode loads an animation from the given io.Reader.
func Decode(r io.Reader) (anim *Animation, err error) {
	// Read Magic Number
	var magicNumber [5]byte
	if err = binary.Read(r, byteOrder, &magicNumber); err != nil {
		return
	}

	// Verify the magic number
	if bytes.Compare(magicNumber[:], theMagicNumber[:]) != 0 {
		return nil, fmt.Errorf("incorrect magic number, expected MANIC, go %s", string(magicNumber[:]))
	}

	// Read the version
	anim = &Animation{}
	if err = binary.Read(r, byteOrder, &anim.Version); err != nil {
		return
	}

	// Verify the version
	if anim.Version != Version0 {
		return nil, fmt.Errorf("unsupported manic animation version")
	}

	// Load the frame width/height
	if err = binary.Read(r, byteOrder, &anim.FrameW); err != nil {
		return
	}
	if err = binary.Read(r, byteOrder, &anim.FrameH); err != nil {
		return
	}

	// Load keyframe and image counts
	var keyframeCount, imageCount uint16
	if err = binary.Read(r, byteOrder, &keyframeCount); err != nil {
		return
	}
	if err = binary.Read(r, byteOrder, &imageCount); err != nil {
		return
	}

	// Load the key frames
	for range keyframeCount {
		var keyframe Keyframe
		if err = binary.Read(r, byteOrder, &keyframe.ImgIndex); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &keyframe.OriginX); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &keyframe.OriginY); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &keyframe.Millis); err != nil {
			return
		}
		anim.Keyframes = append(anim.Keyframes, keyframe)
	}

	// Throw away the internal image index
	for range imageCount {
		var index int64
		if err = binary.Read(r, byteOrder, &index); err != nil {
			return
		}
	}

	// Load the images
	for range imageCount {
		// Load the image extents
		var image Image
		if err = binary.Read(r, byteOrder, &image.Rect.Min.X); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &image.Rect.Min.Y); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &image.Rect.Max.X); err != nil {
			return
		}
		if err = binary.Read(r, byteOrder, &image.Rect.Max.Y); err != nil {
			return
		}

		// Calculate the byte size of the image
		pLen := image.Rect.Dx() * image.Rect.Dy() * 3
		image.PixelData = make([]byte, pLen)

		// Read in the pixel data of the image
		buf := make([]byte, 16384)
		ins := 0
		for {
			var n int
			n, err = r.Read(buf)
			if err != nil {
				return
			}
			copy(image.PixelData[ins:], buf[:n])
			ins += n
			if ins >= pLen {
				break
			}
		}

		// Attach the image
		anim.Images = append(anim.Images, &image)
	}

	return anim, nil
}

func writeIt(w io.Writer, seekPos *int, data any) error {
	if err := binary.Write(w, byteOrder, data); err != nil {
		return err
	}
	*seekPos += binary.Size(data)
	return nil
}

// Encode writes the given animation to the specified io.Writer.
func Encode(w io.Writer, anim *Animation) (err error) {
	// Write the magic number
	var seekPos int
	if err = writeIt(w, &seekPos, theMagicNumber); err != nil {
		return
	}

	// Write the version number
	if err = writeIt(w, &seekPos, anim.Version); err != nil {
		return
	}

	// Write the frame width/height
	if err = writeIt(w, &seekPos, anim.FrameW); err != nil {
		return
	}
	if err = writeIt(w, &seekPos, anim.FrameH); err != nil {
		return
	}

	// Write out the keyframe length
	countKeyframes := uint16(len(anim.Keyframes))
	if err = writeIt(w, &seekPos, countKeyframes); err != nil {
		return
	}

	// Write out the image length
	countImages := uint16(len(anim.Images))
	if err = writeIt(w, &seekPos, countImages); err != nil {
		return
	}

	// Write out the keyframes
	for _, kf := range anim.Keyframes {
		if err = writeIt(w, &seekPos, kf.ImgIndex); err != nil {
			return
		}
		if err = writeIt(w, &seekPos, kf.OriginX); err != nil {
			return
		}
		if err = writeIt(w, &seekPos, kf.OriginY); err != nil {
			return
		}
		if err = writeIt(w, &seekPos, kf.Millis); err != nil {
			return
		}
	}

	// Write out the image seek index
	seekPos += binary.Size(int64(0)) * len(anim.Images)
	for _, img := range anim.Images {
		if err = binary.Write(w, byteOrder, int64(seekPos)); err != nil {
			return
		}
		seekPos += binary.Size(int64(0)) * 4
		imgSize := img.Bounds().Size()
		pixelCount := imgSize.X * imgSize.Y
		seekPos += binary.Size(byte(0)) * 3 * pixelCount
	}

	// Write out each image
	for _, img := range anim.Images {
		// Write out the image extents
		if err = binary.Write(w, byteOrder, int64(img.Bounds().Min.X)); err != nil {
			return
		}
		if err = binary.Write(w, byteOrder, int64(img.Bounds().Min.Y)); err != nil {
			return
		}
		if err = binary.Write(w, byteOrder, int64(img.Bounds().Max.X)); err != nil {
			return
		}
		if err = binary.Write(w, byteOrder, int64(img.Bounds().Max.Y)); err != nil {
			return
		}

		// Write out the pixel data
		for y := img.Bounds().Min.Y; y < img.Bounds().Max.Y; y++ {
			for x := img.Bounds().Min.X; x < img.Bounds().Max.X; x++ {
				c := img.At(x, y)
				nc := color.NRGBAModel.Convert(c).(color.NRGBA)
				if err = binary.Write(w, byteOrder, nc.R); err != nil {
					return
				}
				if err = binary.Write(w, byteOrder, nc.G); err != nil {
					return
				}
				if err = binary.Write(w, byteOrder, nc.B); err != nil {
					return
				}
			}
		}
	}

	return nil
}
