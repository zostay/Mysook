// Package mysook provides an encoder for the simplified image format used on my
// doorlight project. The format divides the image bytes into two sections:
// header and data.
//
// The header is as follows:
//
//  +----------------------+----------------+
//  | Magic ID             | 6 bytes        |
//  +----------------------+----------------+
//  | Dimension (w, h)     | 4 bytes        |
//  | Color Format         | 1 byte         |
//  | Palette              | 0 or 768 bytes |
//  +----------------------+----------------+
//  | Number of Key Frames | 2 bytes        |
//  | Key Frames           | 6-1800 bytes   |
//  +----------------------+----------------+
//
// The header length is of variable length. The minimum header length is 19
// bytes and the maximum is 2581 bytes.
//
// The Magic ID is just an identifier to help verify the file is the expected
// format and set the version of the file. As of this writing, all files must
// have the Magic ID set to "MYS00K" representing version 00 of the image
// format.
//
// The Dimensions field is the width and height as unsigend short integers. A
// value of 0 in either dimension is illegal. All other values are considered
// legal.
//
// The Color Format is an byte that must be set to either 1 or 2. A value of 1
// indicates this is a full color image. In this case, the Palette field will be
// 0 bytes in length. If the color format is 2, it is an image with a 256 index
// color palette and teh Palette field will be exactly 768 bytes long.
//
// The Palette field is always exactly 768 bytes long when present. It
// represents 256 triples. Each triple represents the RGB color for its index in
// the palette.
//
// The Number of Key Frames field is an unsigned short describing the number of
// key frames in the header. A value of 0 is illegal.
//
// The Key Frames field must contain at least one key frame, but may contain up to
// 300 key frames. Each key frame is 6 bytes long. The first 4 bytes are the
// (x, y) coordinate of the upper right point in the key frame and the remaining
// 2 bytes are the number of milliseconds to pause on that key frame.
//
// The data section of the image is of variable size determined by the number of
// pixels in the image and the bytes per pixel. The bytes per pixel is 3 if
// Color Format is 1. The bytes per pixel is 1 of the Color Format is 2. From
// there this is the formula for how long the image data section is:
//
//  len = w * h * bpp
//
// Any data after that point is ignored.
package mysook

import (
	"encoding/binary"
	"errors"
	"fmt"
	"image"
	"image/color"
	"io"

	"github.com/zostay/Mysook/internal/image/piskel"
)

const (
	KeyframeMax = 500
)

var (
	ByteOrder = binary.LittleEndian
)

type Keyframe struct {
	Index  uint16
	Millis uint16
}

type Keyframes []Keyframe

type ImgFrame struct {
	Offset uint32
	Bounds image.Rectangle
	Pixels []uint8
}

type ImgFrames []ImgFrame

type Mysook struct {
	Version   []byte
	W, H      uint16
	Keyframes Keyframes
	ImgFrames ImgFrames
}

func (m *Mysook) Validate() error {
	if len(m.Keyframes) == 0 {
		return errors.New("there must be at least one keyframe")
	}

	if len(m.Version) != 2 {
		return errors.New("version must be exactly 2 bytes")
	}

	for i, imf := range m.ImgFrames {
		if len(imf.Pixels) != imf.Bounds.Dx()*imf.Bounds.Dy()*3 {
			return fmt.Errorf("image frame %d has incorrect pixel count", i)
		}
	}

	return nil
}

func (m *Mysook) FixOffsets() {
	offset := uint32(12 + 6*len(m.Keyframes))

	for _, imf := range m.ImgFrames {
		imf.Offset = offset
		offset += uint32(8 + len(imf.Pixels))
	}
}

func Encode(w io.Writer, m *Mysook) error {
	err := m.Validate()
	if err != nil {
		return err
	}

	m.FixOffsets()

	w.Write([]byte{'M', 'Y', 'S', m.Version[0], m.Version[1], 'K'})

	binary.Write(w, ByteOrder, m.W)
	binary.Write(w, ByteOrder, m.H)

	var kflen uint16 = uint16(len(m.Keyframes))
	binary.Write(w, ByteOrder, kflen)

	for _, kf := range m.Keyframes {
		offset := m.ImgFrames[kf.Index].Offset
		binary.Write(w, ByteOrder, offset)
		binary.Write(w, ByteOrder, kf.Millis)
	}

	for _, imf := range m.ImgFrames {
		binary.Write(w, ByteOrder, uint16(imf.Bounds.Min.X))
		binary.Write(w, ByteOrder, uint16(imf.Bounds.Min.Y))
		binary.Write(w, ByteOrder, uint16(imf.Bounds.Dx()))
		binary.Write(w, ByteOrder, uint16(imf.Bounds.Dy()))

		w.Write(imf.Pixels)
	}

	return nil
}
