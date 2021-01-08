#include "colorlut.h"

ColorLut::ColorLut(int count, Palette pal)
{
    Generate(count, pal);
}

void ColorLut::Generate(int sz, Palette pal)
{
    if(sz < 1)
        return;
    Size = sz;
    if(pal == ePalGray) {
        PaletteTable.clear();
        for(int i = 0; i < sz; i++) {
            int g = 256 * i / sz;
            PaletteTable.push_back(qRgb(g, g, g));
        }
    }
    else if(pal == ePalHeightMap) {
        double golden_ratio = 0.618033988749895;
        const int s = 245, v = 245, a = 255;
        PaletteTable.clear();
        for(int i = 0; i < sz; i++) {
            double h = golden_ratio * 360 * i / sz;
            PaletteTable.push_back(QColor::fromHsv(int(h), s, v, a));
        }
    }
}
//EOF
