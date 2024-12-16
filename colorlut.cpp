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

void ColorLut::interpolateColors(const QVector<QColor>& colors, int size)
{
    QVector<QRgb> palette;
    PaletteTable.clear();
    for (int i = 0; i < size; i++) {
        double t = double(i) / (size - 1);
        int index = t * (colors.size() - 1);
        QColor c1 = colors[index];
        QColor c2 = colors[qMin(index + 1, colors.size() - 1)];

        double ratio = t * (colors.size() - 1) - index;

        int r = (1 - ratio) * c1.red() + ratio * c2.red();
        int g = (1 - ratio) * c1.green() + ratio * c2.green();
        int b = (1 - ratio) * c1.blue() + ratio * c2.blue();

        PaletteTable.push_back(qRgb(r, g, b));
    }
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
        QVector<QColor> base ;
        base.append(QColor(158, 1, 66));
        base.append(QColor(213, 62, 79));
        base.append(QColor(244, 109, 67));
        base.append(QColor(253, 174, 97));
        base.append(QColor(254, 224, 139));
        base.append(QColor(255, 255, 191));
        base.append(QColor(230, 245, 152));
        base.append(QColor(171, 221, 164));
        base.append(QColor(102, 194, 165));
        base.append(QColor(50, 136, 189));
        base.append(QColor(94, 79, 162));

        interpolateColors(base);
    } else if(pal == eYlGnBu) {
        QVector<QColor> base = {
            QColor(255, 255, 217), QColor(237, 248, 177), QColor(199, 233, 180),
            QColor(127, 205, 187), QColor(65, 182, 196), QColor(29, 145, 192),
            QColor(34, 94, 168), QColor(37, 52, 148), QColor(8, 29, 88)
        };
        interpolateColors(base);
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
