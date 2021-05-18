package piskel

import (
	"encoding/json"
	"image"
	"io"
)

func Decode(r io.Reader) (image.Image, error) {
	d := json.NewDecoder(r)

	var pImage Piskel
	err := d.Decode(&pImage)

	return &pImage, err
}
