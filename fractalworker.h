#ifndef FRACTALWORKER_H
#define FRACTALWORKER_H

#include <QRunnable>
#include <QImage>
#include <QObject>
#include <QMutex>
#include "common_type.h"

class FractalWorker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    FractalWorker(int startY, int endY, double left, double top, double xInterval, double yInterval,
                      int width, int height, int maxIterations, IndexOfPt* imageData,
                      bool isJulia, Complex lastPoint)
            : startY(startY), endY(endY), left(left), top(top), xInterval(xInterval), yInterval(yInterval),
              width(width), height(height), maxIterations(maxIterations), imageData(imageData),
              isJulia(isJulia), lastPoint(lastPoint) {}

    ~FractalWorker() override = default;
    void run() override;
signals:
    void finished();

private:
    // Worker parameters
    int startY, endY;
    double left, top, xInterval, yInterval;
    int width, height, maxIterations;
    IndexOfPt* imageData;

    // Fractal-specific parameters
    bool isJulia;
    Complex lastPoint;
private:
    inline int calcPoint(Complex start, Complex c) const;
    inline int calcPointAVX(Complex z, Complex c) const;
};

#endif // FRACTALWORKER_H
