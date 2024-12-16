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
                line[x] = calcPointSmoothAVX(point, lastPoint);
            } else {
                line[x] = calcPointSmoothAVX(Complex(0, 0), point);
            }
        }
    }
    emit finished();
}

inline int FractalWorker::calcPointAVX(Complex z, Complex c) const
{
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


inline int FractalWorker::calcPointSmoothAVX(Complex z, Complex c) const {
    // Initialize AVX variables
    __m256d zr = _mm256_set1_pd(z.real());
    __m256d zi = _mm256_set1_pd(z.imag());
    __m256d cr = _mm256_set1_pd(c.real());
    __m256d ci = _mm256_set1_pd(c.imag());
    __m256d iteration = _mm256_set1_pd(0.0);
    __m256d four = _mm256_set1_pd(4.0);

    __m256d mask;
    for (int i = 0; i < maxIterations; i++) {
        // Compute |z|^2
        __m256d zr2 = _mm256_mul_pd(zr, zr);
        __m256d zi2 = _mm256_mul_pd(zi, zi);
        __m256d magnitude = _mm256_add_pd(zr2, zi2);

        // Check bailout condition
        mask = _mm256_cmp_pd(magnitude, four, _CMP_LT_OQ); // mask = (|z|^2 < 4)
        if (!_mm256_movemask_pd(mask)) {
            break; // All points escaped
        }

        // Increment iterations for active points
        iteration = _mm256_add_pd(iteration, _mm256_and_pd(mask, _mm256_set1_pd(1.0)));

        // Update zr and zi
        __m256d zrzi = _mm256_mul_pd(zr, zi);
        zi = _mm256_add_pd(_mm256_add_pd(zrzi, zrzi), ci); // zi = 2 * zr * zi + ci
        zr = _mm256_add_pd(_mm256_sub_pd(zr2, zi2), cr);   // zr = zr^2 - zi^2 + cr
    }

    // Smooth coloring
    __m256d smooth = _mm256_sub_pd(iteration, _mm256_set1_pd(log2(log2(4.0))));
    smooth = _mm256_max_pd(_mm256_set1_pd(0.0), smooth); // Clamp to prevent negatives
    __m256d normalized = _mm256_mul_pd(smooth, _mm256_set1_pd(255.0 / maxIterations));

    // Extract the first result (scalar output)
    double smoothValues[4];
    _mm256_storeu_pd(smoothValues, normalized);

    return static_cast<int>(smoothValues[0]); // Return the first normalized value
}

inline int FractalWorker::calcPoint(Complex z, Complex c) const
{
    double zr = z.real(), zi = z.imag();
    double cr = c.real(), ci = c.imag();

    for (int i = 0; i < maxIterations; i++) {
        double zr2 = zr * zr, zi2 = zi * zi;
        double magnitude = zr2 + zi2; // |z|^2
        if (magnitude > 4.0) {
            return 255 * i / maxIterations; // Normalize iteration count to [0, 255]
#if 0
            // Smooth coloring
            double smooth = i + 1 - log2(log2(magnitude));
            smooth = std::max(0.0, smooth); // Prevent negative values

            // Normalize to [0, 255]
            return int(255.0 * smooth / maxIterations);
#endif
        }
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
    }
    return 0;
}
