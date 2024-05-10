package piskel

import (
	"encoding/json"
	"io"
)

func Decode(r io.Reader) (*Piskel, error) {
	d := json.NewDecoder(r)

	var pImage Piskel
	err := d.Decode(&pImage)

	return &pImage, err
}
