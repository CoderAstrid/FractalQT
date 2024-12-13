#ifndef FRACTALWORKER_H
#define FRACTALWORKER_H

#include <QRunnable>
#include <QImage>
#include <QMutex>
#include "common_type.h"

class FractalWorker : public QRunnable
{
public:
    FractalWorker(int startY, int endY, double left, double top, double xInterval, double yInterval,
                  int width, int height, int maxIterations, IndexOfPt* imageData);

    void run() override;
private:
    int startY, endY;
    double left, top, xInterval, yInterval;
    int width, height, maxIterations;
    IndexOfPt* imageData;
};

#endif // FRACTALWORKER_H
