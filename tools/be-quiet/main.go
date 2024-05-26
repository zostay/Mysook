package main

import (
	_ "image/png"
	"os"

	"github.com/zostay/Mysook/internal/image/manic"
)

func main() {
	b := manic.NewBuilder(32, 16)
	idx, err := b.AddImageFile("be-quiet.png")
	if err != nil {
		panic(err)
	}

	// Police Attention #1
	for range 10 {
		b.AddKeyframe(idx, 0, 0, 100)
		b.AddKeyframe(idx, 0, 48, 100)
	}

	// Police Attention #2
	for range 10 {
		b.AddKeyframe(idx, 0, 16, 100)
		b.AddKeyframe(idx, 0, 32, 100)
	}

	// Shhh Emoji
	b.AddKeyframe(idx, 32, 0, 3000)

	// Police Attention #1
	for range 10 {
		b.AddKeyframe(idx, 0, 0, 100)
		b.AddKeyframe(idx, 0, 48, 100)
	}

	// Police Attention #2
	for range 10 {
		b.AddKeyframe(idx, 0, 16, 100)
		b.AddKeyframe(idx, 0, 32, 100)
	}

	// Be
	b.AddKeyframe(idx, 32, 16, 1000)
	// Quiet
	b.AddKeyframe(idx, 32, 32, 1000)
	// !!
	b.AddKeyframe(idx, 32, 48, 1000)

	w, err := os.Create("be-quiet.manic")
	if err != nil {
		panic(err)
	}
	defer w.Close()

	err = manic.Encode(w, b.Animation())
	if err != nil {
		panic(err)
	}
}
