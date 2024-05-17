package main

import (
	_ "image/png"
	"os"

	"github.com/zostay/Mysook/internal/image/manic"
)

func main() {
	b := manic.NewBuilder(32, 16)
	idx, err := b.AddImageFile("congrats-logan.png")
	if err != nil {
		panic(err)
	}

	// Logan
	logan := b.AddKeyframe(idx, 0, 0, 1000)
	// Congratulations
	b.AddScanlineKeyframes(idx, 32, 0, 156, 0, 5000, 30, 156-32, manic.EaseInOutSine)
	// Logan
	b.AddRepeatKeyframe(logan, 1000)
	// High School Graduate
	b.AddScanlineKeyframes(idx, 32, 16, 188, 16, 5000, 30, 188-32, manic.EaseInOutSine)

	w, err := os.Create("congrats-logan.manic")
	if err != nil {
		panic(err)
	}
	defer w.Close()

	err = manic.Encode(w, b.Animation())
	if err != nil {
		panic(err)
	}
}
