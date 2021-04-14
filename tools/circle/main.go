package main

import (
	"math"
	"os"
	"text/template"
)

const (
	imageWidth  float64 = 320
	imageHeight float64 = 320
	panelWidth  float64 = 32
	panelHeight float64 = 16
	inc         float64 = 360 / (10 * 30)
)

var (
	imageCenterX float64 = imageWidth / 2
	imageCenterY float64 = imageHeight / 2

	panelCenterX float64 = panelWidth / 2
	panelCenterY float64 = panelHeight / 2

	radiusX float64 = imageWidth/2 - panelWidth/2
	radiusY float64 = imageHeight/2 - panelHeight/2
)

const tmplcstr = `#include <pgmspace.h>
#include <stdint.h>

const size_t key_frame_count = {{ .KeyFrames | len }};
uint16_t key_frames[] = {
{{- range .KeyFrames}}
    {{.X}}, {{.Y}}, // k={{.K}}, Θ={{.Theta | printf "%.1f"}}°
{{- end}}
};`

type KeyFrame struct {
	X     int
	Y     int
	K     int
	Theta float64
}

func main() {
	keyFrames := make([]KeyFrame, 0, int(360.00/inc))
	var theta float64
	k := 0
	for theta = 0; theta < 359.99; theta += inc {
		k++
		x := math.Cos(toRad(theta))*radiusX + imageCenterX - panelCenterX
		y := math.Sin(toRad(theta))*radiusY + imageCenterY - panelCenterY
		keyFrames = append(keyFrames, KeyFrame{int(math.Round(x)), int(math.Round(y)), k, theta})
	}

	tmpl := template.Must(template.New("keyframes").Parse(tmplcstr))
	tmpl.Execute(os.Stdout, map[string]interface{}{
		"KeyFrames": keyFrames,
	})
}

func toRad(deg float64) float64 {
	return math.Pi * 2 * deg / 360
}
