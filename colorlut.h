#ifndef COLORLUT_H
#define COLORLUT_H

#include "common_type.h"
#include <vector>

typedef enum E_PALETTE{
    ePalGray,
    ePalHeightMap,
    eCntPalette
}Palette;

const SVString NAMES_PALETTE[eCntPalette] = {
    "Gray Palette", "Height-Map Palette"
};

class ColorLut
{
public:
    ColorLut(int count = 255, Palette pal = ePalGray);
    int size() const
    {
        return Size;
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
private:
    int Size;
    std::vector<SVColor> PaletteTable;
};

#endif // COLORLUT_H
