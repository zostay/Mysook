package piskel

import (
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

const testPiskel = `{"modelVersion":2,"piskel":{"name":"policelights","description":"","fps":16,"height":16,"width":32,"layers":["{\"name\":\"Layer 1\",\"opacity\":1,\"frameCount\":19,\"chunks\":[{\"layout\":[[0],[1],[2],[3],[4],[5],[6],[7],[8],[9],[10],[11],[12],[13],[14],[15],[16],[17],[18]],\"base64PNG\":\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmAAAAAQCAYAAACx1sM4AAACdklEQVR4Xu3c2VFkMRBEUeHJ+O/UjCdMYMMhIhG6/BddW6Zuv6U/Ps/5PPD3AbE/IZSKP+dU/0+Y4i6HPyaf8+/yDap+c5D1/C37vf/p/v1F/a39v/ptg9f6+wjA7PBeC9CyR/r+BQCq/VMDXBtA9VsHbp+/HV8B2Nr/df9eB9C1/wZg5r/4/Qk//BvCbzfgb2gB/Qs1wLUBUPHnnOo3Ba3nb9kHYAGYOojF3+4/AZjNPwDD/t0efrsBaP+r3xAmALMN1P17/QrQ6/Wv9ReAmf4DMOzf7eF6AKwNQPtf/QGY7pDE6/69DiCv17/23wBM1P8LnoGy42N/CwLHx+F6AKwNQBtQ/aag9fwt+73+df9eB5DX61/rLwDDE2j9DACmj+/w7Q1Y69d4PQDWBlD91oHb5x+AmYNbtO3eV7TuXwC2nWAAhhrYjg+TP70FqR1UAwzA7lbQ7fMPwGz/LFrdJwC7XX8BGGpgLUBMvytg2MDbDQDL52/gAejWQQIw679Fq/oCsNv9NwBDDawFiOkHYNjA2w0Ayw/AUEFrAA3AzMEtWtUXgN3uvwEYamAtQEwfj4+eAbvdAHR/qt8QJgCzDdT9e/0ZqNfrX+svADP99zMU2L/bw/UAWBuA9r/6AzDdIYnX/XsdQF6vf+2/AZiov5+huB5AcfzdgsNrqGsDfH3+ho/7K+ABmE0wANvewwrA0IG348PkewuSG6gHQAByt4Jun78d3wHYent1/wKw7QQDMDyCt+PD5AMwbqAaYAB2t4Jun38AZvtn0Ww/fAU+ANtOMABDDWzHh8kHYNzA2w9gbUD1G8KsAdyy7wrY2v9VfwHYdoL/ASrNEBBmkgusAAAAAElFTkSuQmCC\"}]}"],"hiddenFrames":[null]}}`

func TestDecoder(t *testing.T) {
	pr := strings.NewReader(testPiskel)

	pimg, err := Decode(pr)
	require.NoError(t, err)

	r, g, b, a := pimg.At(0, 0).RGBA()
	assert.Equal(t, uint32(0xffff), r, "red is on")
	assert.Equal(t, uint32(0x0000), g, "green is off")
	assert.Equal(t, uint32(0x0000), b, "blue is off")
	assert.Equal(t, uint32(0xffff), a, "alpha is on")

	require.IsType(t, &Piskel{}, pimg, "image is actually a Piskel")

	ppsk, _ := pimg.(*Piskel)
	pmod := ppsk.Piskel

	assert.Equal(t, "policelights", pmod.Name, "name is policelights")
	assert.Equal(t, "", pmod.Description, "description is empty")

	assert.Equal(t, 16, pmod.FPS, "fps is 16")
	assert.Equal(t, 16, pmod.Height, "height is 16")
	assert.Equal(t, 32, pmod.Width, "width is 32")

	require.Len(t, pmod.Layers, 1, "len(Layers) is 1")

	play := pmod.Layers[0]

	assert.Equal(t, "Layer 1", play.Name, "layer name is Layer 1")
	assert.Equal(t, 1.0, play.Opacity, "layer opacity is 1")
	assert.Equal(t, 19, play.FrameCount, "layer frameCount is 19")

	require.Len(t, play.Chunks, 1, "len(Chunks) is 1")

	pchk := play.Chunks[0]

	assert.Len(t, pchk.Layout, 19, "len(Layout) is 19")

	require.NotNil(t, pchk.PNG, "PNG is not nil")

	png := pchk.PNG

	assert.Equal(t, 32*19, png.Bounds().Dx(), "width of png chunk is 32*19")
	assert.Equal(t, 16, png.Bounds().Dy(), "height of png chunk is 16")

	r, g, b, a = png.At(0, 0).RGBA()
	assert.Equal(t, uint32(0xffff), r, "red is on")
	assert.Equal(t, uint32(0x0000), g, "green is off")
	assert.Equal(t, uint32(0x0000), b, "blue is off")
	assert.Equal(t, uint32(0xffff), a, "alpha is on")
}
