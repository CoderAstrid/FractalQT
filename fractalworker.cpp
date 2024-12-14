#include "fractalworker.h"
#include <QImage>

void FractalWorker::run()
{    
    int widthEx = ((((width * 8) + 31) & ~31) >> 3);

#pragma omp parallel for
    for (int y = startY; y < endY; y++) {
        IndexOfPt* line = imageData + y * widthEx;
        double yCoord = top + y * yInterval;

        for (int x = 0; x < width; x++) {
            double xCoord = left + x * xInterval;
            Complex point(xCoord, yCoord);

            if (isJulia) {
                line[x] = calcPoint(point, lastPoint);
            } else {
                line[x] = calcPoint(Complex(0, 0), point);
            }
        }
    }
    emit finished();
}

inline int FractalWorker::calcPoint(Complex z, Complex c) const
{
    double zr = z.real(), zi = z.imag();
    double cr = c.real(), ci = c.imag();

    for (int i = 0; i < maxIterations; i++) {
        double zr2 = zr * zr, zi2 = zi * zi;
        if (zr2 + zi2 > 4.0) {
            return 255 * i / maxIterations; // Normalize iteration count to [0, 255]
            // return 255 - int(255.0 * std::log(i + 1 - std::log2(std::log2(std::abs(std::norm(z))))) / std::log(maxIterations));
        }
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
    }
    return 255;
}
