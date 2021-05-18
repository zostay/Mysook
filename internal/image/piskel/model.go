package piskel

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"strings"
)

type Piskel struct {
	ModelVersion int   `json:"modelVersion"`
	Piskel       Model `json:"piskel"`
}

type Model struct {
	Name         string  `json:"name"`
	Description  string  `json:"description"`
	FPS          int     `json:"fps"`
	Height       int     `json:"height"`
	Width        int     `json:"width"`
	Layers       []Layer `json:"layers"`
	HiddenFrames []int   `json:"hiddenFrames"`
}

type Layer struct {
	Name       string  `json:"name"`
	Opacity    float64 `json:"opacity"`
	FrameCount int     `json:"frameCount"`
	Chunks     []Chunk `json:"chunks"`
}

type Chunk struct {
	Layout [][]int
	PNG    image.Image
}

func (p *Piskel) ColorModel() color.Model {
	return p.Piskel.Layers[0].Chunks[0].PNG.ColorModel()
}

func (p *Piskel) Bounds() image.Rectangle {
	return image.Rect(0, 0, p.Piskel.Width, p.Piskel.Height)
}

func (p *Piskel) At(x, y int) color.Color {
	return p.Piskel.Layers[0].Chunks[0].PNG.At(x, y)
}

func (l *Layer) UnmarshalJSON(blob []byte) error {
	var rawStr string

	err := json.Unmarshal(blob, &rawStr)
	if err != nil {
		return err
	}

	var raw struct {
		Name       string  `json:"name"`
		Opacity    float64 `json:"opacity"`
		FrameCount int     `json:"frameCount"`
		Chunks     []Chunk `json:"chunks"`
	}
	err = json.Unmarshal([]byte(rawStr), &raw)
	if err != nil {
		return err
	}

	l.Name = raw.Name
	l.Opacity = raw.Opacity
	l.FrameCount = raw.FrameCount
	l.Chunks = raw.Chunks

	return nil
}

const PNGPrefix = "data:image/png;base64,"

func (c *Chunk) UnmarshalJSON(blob []byte) error {
	var raw struct {
		Layout    [][]int `json:"layout"`
		Base64PNG string  `json:"base64PNG"`
	}

	err := json.Unmarshal(blob, &raw)
	if err != nil {
		return err
	}

	c.Layout = raw.Layout

	if strings.HasPrefix(raw.Base64PNG, PNGPrefix) {
		rPNG := base64.NewDecoder(
			base64.StdEncoding,
			strings.NewReader(raw.Base64PNG[len(PNGPrefix):]),
		)

		png, err := png.Decode(rPNG)
		if err != nil {
			return fmt.Errorf("chunks base64PNG could not be decoded: %w", err)
		}
		c.PNG = png
	} else {
		return fmt.Errorf("chunks base64PNG did not have the expected %q prefix", PNGPrefix)
	}

	return nil
}

func (c *Chunk) ColorModel() color.Model {
	return c.PNG.ColorModel()
}

func (c *Chunk) Bounds() image.Rectangle {
	return c.PNG.Bounds()
}

func (c *Chunk) At(x, y int) color.Color {
	return c.PNG.At(x, y)
}
