#include "fractalworker.h"
#include <QImage>
#include <immintrin.h>

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
                line[x] = calcPointAVX(point, lastPoint);
            } else {
                line[x] = calcPointAVX(Complex(0, 0), point);
            }
        }
    }
    emit finished();
}

inline int FractalWorker::calcPointAVX(Complex z, Complex c) const {
    __m256d zr = _mm256_set1_pd(z.real());
    __m256d zi = _mm256_set1_pd(z.imag());
    __m256d cr = _mm256_set1_pd(c.real());
    __m256d ci = _mm256_set1_pd(c.imag());

    for (int i = 0; i < maxIterations; i++) {
        __m256d zr2 = _mm256_mul_pd(zr, zr);
        __m256d zi2 = _mm256_mul_pd(zi, zi);
        __m256d norm = _mm256_add_pd(zr2, zi2);

        // Break condition
        __m256d mask = _mm256_cmp_pd(norm, _mm256_set1_pd(4.0), _CMP_GT_OQ);
        if (_mm256_movemask_pd(mask)) {
            return i;
        }

        zi = _mm256_add_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), _mm256_mul_pd(zr, zi)), ci);
        zr = _mm256_add_pd(_mm256_sub_pd(zr2, zi2), cr);
    }
    return 0;
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
    return 0;
}
