#include "Panel.h"

using namespace mysook;

template <int W, int H> 
void RGBPanelGrid<W,H>::fill_screen(Color c) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            grid[x][y] = c;
        }
    }
}
