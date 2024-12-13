#include "common_type.h"
#include "colorlut.h"
#include <fstream>

ColorLut::ColorLut(int count, Palette pal)
{
    Generate(count, pal);
    mapName = NAMES_PALETTE[pal];
}

ColorLut::ColorLut(const QString& path, const QString& name)
{
    int len_name = name.length();
    mapName.clear();
    if(name.right(4).compare(".map") == 0) {
        mapName = name.left(len_name-4);
    }

    std::ifstream input;
    input.open(path.toStdString().c_str());
    PaletteTable.clear();
    int r, g, b;
      for (int i=0; i<PALATE_SIZE; i++) {
        input >> r >> g >> b;
        PaletteTable.push_back(qRgb(r,g,b));
      }
      Size = PALATE_SIZE;
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
#if _DEV_QT
            PaletteTable.push_back(qRgb(g, g, g));
#else
#endif//_DEV_QT
        }
    }
    else if(pal == ePalHeightMap) {
        double golden_ratio = 0.618033988749895;
        const int s = 245, v = 245, a = 255;
        PaletteTable.clear();
        for(int i = 0; i < sz; i++) {
            double h = golden_ratio * 360 * i / sz;
#if _DEV_QT
            PaletteTable.push_back(QColor::fromHsv(int(h), s, v, a));
#else
#endif//_DEV_QT
        }
    } else if(pal == eSpectral) {
        PaletteTable.push_back(QColor(158, 1, 66));
        PaletteTable.push_back(QColor(213, 62, 79));
        PaletteTable.push_back(QColor(244, 109, 67));
        PaletteTable.push_back(QColor(253, 174, 97));
        PaletteTable.push_back(QColor(254, 224, 139));
        PaletteTable.push_back(QColor(255, 255, 191));
        PaletteTable.push_back(QColor(230, 245, 152));
        PaletteTable.push_back(QColor(171, 221, 164));
        PaletteTable.push_back(QColor(102, 194, 165));
        PaletteTable.push_back(QColor(50, 136, 189));
        PaletteTable.push_back(QColor(94, 79, 162));
    } else if(pal == eYlGnBu) {
        PaletteTable = {
            QColor(255, 255, 217), QColor(237, 248, 177), QColor(199, 233, 180),
            QColor(127, 205, 187), QColor(65, 182, 196), QColor(29, 145, 192),
            QColor(34, 94, 168), QColor(37, 52, 148), QColor(8, 29, 88)
        };
    } else if(pal == ePalHeatMap) {
        PaletteTable.clear();
        for(int i = 0; i < sz; i++) {
            double t = i / double(255);
            int r = int(255 * t);
            int g = int(255 * (1 - t));
            int b = int(128 * (1 - t));
            PaletteTable.push_back(QColor(r, g, b));
        }
    } else if(pal == ePalSineMap) {
        PaletteTable.clear();
        for(int i = 0; i < sz; i++) {
            double t = i / double(255);
            int r = int(127.5 * (1 + sin(2 * M_PI * t))); // Red oscillates
            int g = int(127.5 * (1 + sin(2 * M_PI * t + M_PI / 3))); // Green offset
            int b = int(127.5 * (1 + sin(2 * M_PI * t + 2 * M_PI / 3))); // Blue offset
            PaletteTable.push_back(QColor(r, g, b));
        }
    }
}
//EOF
