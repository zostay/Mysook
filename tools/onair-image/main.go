package main

import (
	"image"
	"image/color"
	"os"

	"github.com/fogleman/gg"
	"github.com/pbnjay/pixfont"
	"github.com/zostay/Mysook/internal/image/manic"
)

const (
	onStr  = "ON"
	airStr = "AIR"
)

var (
	keyframes = []manic.Keyframe{
		{
			Origin: manic.Point{X: 0, Y: 0},
			Millis: 5000,
		},
		{
			Origin: manic.Point{X: 32, Y: 0},
			Millis: 5000,
		},
	}
)

func main() {
	im := image.NewRGBA(image.Rect(0, 0, 320, 320))
	dc := gg.NewContextForRGBA(im)

	red := color.RGBA{0xFF, 0x00, 0x00, 0xFF}
	black := color.RGBA{0x00, 0x00, 0x00, 0xFF}

	dc.DrawRectangle(0, 0, 32, 8)
	dc.SetColor(red)
	dc.Fill()

	onWidth := pixfont.MeasureString(onStr)
	pixfont.DrawString(im, 16-onWidth/2, 0, onStr, black)

	dc.DrawRectangle(0, 8, 32, 8)
	dc.SetColor(black)
	dc.Fill()

	airWidth := pixfont.MeasureString(airStr)
	pixfont.DrawString(im, 16-airWidth/2, 9, airStr, red)

	dc.DrawRectangle(32, 0, 32, 8)
	dc.SetColor(black)
	dc.Fill()

	pixfont.DrawString(im, 48-onWidth/2, 0, onStr, red)

	dc.DrawRectangle(32, 8, 32, 8)
	dc.SetColor(red)
	dc.Fill()

	pixfont.DrawString(im, 48-airWidth/2, 9, airStr, black)

	err := dc.SavePNG("onair.png")
	if err != nil {
		panic(err)
	}

	b := manic.NewBuilder(32, 16)
	idx, err := b.AddImageFile("onair.png")
	if err != nil {
		panic(err)
	}

	for _, kf := range keyframes {
		b.AddKeyframe(idx, kf.Origin.X, kf.Origin.Y, kf.Millis)
	}

	w, err := os.Create("onair.mysook")
	if err != nil {
		panic(err)
	}
	defer w.Close()

	err = manic.Encode(w, b.Animation())
	if err != nil {
		panic(err)
	}
}
