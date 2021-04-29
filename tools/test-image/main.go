package main

import (
	"image"
	"image/color"
	"image/png"
	"os"
)

const (
	W = 320
	H = 320

	DA = 7
	DB = 11
	DC = 13
)

var (
	testColors = color.Palette{
		color.RGBA{0xFF, 0x00, 0x00, 0xFF}, // red
		color.RGBA{0xFF, 0xFF, 0x00, 0xFF}, // yellow
		color.RGBA{0x00, 0xFF, 0x00, 0xFF}, // green
		color.RGBA{0x00, 0xFF, 0xFF, 0xFF}, // cyan
		color.RGBA{0x00, 0x00, 0xFF, 0xFF}, // blue
		color.RGBA{0xFF, 0x00, 0xFF, 0xFF}, // magenta
	}
)

func main() {
	ti := image.NewRGBA(image.Rect(0, 0, W, H))

	ca, cb := 0, 4
	var c int
	for y := 0; y < H; y++ {
		if y%2 == 0 {
			c = ca
			ca = (ca + DA) % len(testColors)
			if ca == cb {
				ca = (ca + 1) % len(testColors)
			}
		} else {
			c = cb
			cb = (cb + DB) % len(testColors)
			if cb == ca {
				cb = (cb + 1) % len(testColors)
			}
		}

		for x := 0; x < W; x++ {
			ti.Set(x, y, testColors[c])

			if x%80 == 0 {
				c = (c + DC) % len(testColors)
			}
		}
	}

	w, err := os.Create("test-image.png")
	if err != nil {
		panic(err)
	}

	err = png.Encode(w, ti)
	if err != nil {
		panic(err)
	}
}
