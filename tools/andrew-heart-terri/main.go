package main

import (
	_ "image/png"
	"os"

	"github.com/zostay/Mysook/internal/image/manic"
)

func main() {
	b := manic.NewBuilder(32, 16)
	idx, err := b.AddImageFile("andrew-heart-terri.png")
	if err != nil {
		panic(err)
	}

	// Andrew
	b.AddScanlineKeyframes(idx, 0, 0, 96, 0, 5000, 30, manic.EaseInSine)
	// Heart
	for range 3 {
		for i := range 9 {
			b.AddKeyframe(idx, 32*int32(i), 16, 100)
		}
	}
	// Terri
	b.AddScanlineKeyframes(idx, 12, 32, 86, 32, 5000, 30, manic.EaseOutSine)
	// Bang Bang
	b.AddKeyframe(idx, 125, 32, 1000)

	w, err := os.Create("andrew-heart-terri.manic")
	if err != nil {
		panic(err)
	}
	defer w.Close()

	err = manic.Encode(w, b.Animation())
	if err != nil {
		panic(err)
	}
}
