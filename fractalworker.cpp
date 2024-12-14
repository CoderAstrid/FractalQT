#include "fractalworker.h"
#include <QImage>

void FractalWorker::run()
{
    int widthEx = ((((width * 8) + 31) & ~31) >> 3); // Aligned width
    for (int y = startY; y < endY; y++) {
        IndexOfPt* line = imageData + y * widthEx;
        for (int x = 0; x < width; x++) {
            // Always recalculate all pixels in this region
            Complex point(left + x * xInterval, top + y * yInterval);
            line[x] = isJulia ? calcPoint(point, lastPoint) : calcPoint(Complex(0, 0), point);
        }
    }
    emit finished();
}

inline int FractalWorker::calcPoint(Complex z, Complex c) const
{
    for (int i = 0; i < maxIterations; i++) {
        z = z * z + c;
        if (std::norm(z) > 4.0) {
            return 255 * i / maxIterations; // Normalize to [0, 255]
            // Smooth coloring
            //return 255 - int(255.0 * std::log(i + 1 - std::log2(std::log2(std::abs(std::norm(z))))) / std::log(maxIterations));
        }
    }
    return 0; // Inside the set
}
