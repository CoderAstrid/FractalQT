#include "fractalworker.h"
#include <QImage>

FractalWorker::FractalWorker(int startY, int endY, double left, double top, double xInterval, double yInterval,
                             int width, int height, int maxIterations, IndexOfPt* imageData)
                   : startY(startY), endY(endY), left(left), top(top),
                     xInterval(xInterval), yInterval(yInterval), width(width),
                     height(height), maxIterations(maxIterations), imageData(imageData)
{

}
void FractalWorker::run()
{
    int widthEx = ((((width * 8) + 31) & ~31) >> 3);
    for (int y = startY; y < endY; ++y) {
        IndexOfPt* line = imageData + y * widthEx;
        for (int x = 0; x < width; ++x) {
            double cr = left + x * xInterval;
            double ci = top + y * yInterval;

            int iterations = 0;
            double zr = 0.0, zi = 0.0;
            while (zr * zr + zi * zi < 4.0 && iterations < maxIterations) {
                double temp = zr * zr - zi * zi + cr;
                zi = 2.0 * zr * zi + ci;
                zr = temp;
                iterations++;
            }

            line[x] = iterations; // Store result in imageData
        }
    }
}
