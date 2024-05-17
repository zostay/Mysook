package manic

import (
	"image"
	"image/color"
)

// Image represents a single image within a manic animation.
type Image struct {
	Rect      Rect
	stride    int32
	PixelData []byte
}

var _ image.Image = (*Image)(nil)

// At returns the color of the given pixel.
func (i *Image) At(x, y int) color.Color {
	idx := (int32(y)-i.Rect.Min.Y)*i.stride + (int32(x)-i.Rect.Min.X)*3
	r := i.PixelData[idx]
	g := i.PixelData[idx+1]
	b := i.PixelData[idx+2]

	return color.NRGBA{R: r, G: g, B: b, A: 0xFF}
}

// Bounds returns the bounding rectangle of the image.
func (i *Image) Bounds() image.Rectangle {
	return image.Rect(
		int(i.Rect.Min.X), int(i.Rect.Min.Y),
		int(i.Rect.Max.X), int(i.Rect.Max.Y))
}

// ColorModel returns NRGBAModel, but the alpha part is not stored and always set
// to 0xff on load.
func (i *Image) ColorModel() color.Model {
	return color.NRGBAModel
}

// NewImage creates a new image with the given bounds and pixel data.
func NewImage(bounds image.Rectangle, pixelData []byte) *Image {
	return &Image{
		Rect: Rect{
			Min: Point{
				X: int32(bounds.Min.X),
				Y: int32(bounds.Min.Y),
			},
			Max: Point{
				X: int32(bounds.Max.X),
				Y: int32(bounds.Max.Y),
			},
		},
		stride:    int32(bounds.Max.X-bounds.Min.X) * 3,
		PixelData: pixelData,
	}
}
