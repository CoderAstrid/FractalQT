#ifndef COLORLUT_H
#define COLORLUT_H

#include "common_type.h"
#include <vector>
#if _DEV_QT
#include <QString>
#endif//_DEV_QT

typedef enum E_PALETTE{
    ePalGray,
    ePalHeightMap,
    eSpectral,
    eYlGnBu,
    ePalHeatMap,
    ePalSineMap,
    eCntPalette
}Palette;

const SVString NAMES_PALETTE[eCntPalette] = {
    "Gray Palette", "Height-Map Palette", "Spectral", "YlGnBu"
};

class ColorLut
{
public:
    ColorLut(int count = 255, Palette pal = ePalGray);
    ColorLut(const QString& path, const QString& name);

    int size() const
    {
        return PaletteTable.size();
    }
    void Generate(int sz = 255, Palette pal = ePalGray);
    QRgb at(int idx) const
    {
        return PaletteTable.at(idx).rgba();
    }
    const std::vector<SVColor>& table() const
    {
        return PaletteTable;
    }

    const QString& name() const
    {
        return mapName;
    }
private:
    void interpolateColors(const QVector<QColor>& colors, int size = 256);
private:
    int Size;
    std::vector<SVColor> PaletteTable;
    QString mapName;
};

#endif // COLORLUT_H
