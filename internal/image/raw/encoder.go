package raw

import (
	"errors"
	"image"
	"image/color"
	"io"
)

func Encode(w io.Writer, m image.Image) error {
	if m.ColorModel() != color.RGBAModel {
		return errors.New("image argument must use the RGBA color model")
	}

	for x := m.Bounds().Min.X; x < m.Bounds().Max.X; x++ {
		for y := m.Bounds().Min.Y; y < m.Bounds().Max.Y; y++ {
			c := m.At(x, y).(color.RGBA)
			w.Write([]byte{c.R, c.G, c.B})
		}
	}

	return nil
}
