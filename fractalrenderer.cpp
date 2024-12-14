#include "fractalrenderer.h"
#include <complex>
#include <QThreadPool>
#include <QObject>
#include "fractalworker.h"

#if _DEV_QT
#   include <QDebug>
#endif//_DEV_QT

static int sign(int x)
{
    return ((x > 0) - (x < 0));
}

FractalRenderer::FractalRenderer(int _w, int _h)
    : QObject()
    , width(_w)
    , height(_h)
    , imageData(NULL_ADD)
    , widthEx(0)
    , maxWidth(_w)
    , maxHeight(_h)
    , maxInterval(256)
{
    lastPoint.real(-100);
    lastPoint.imag(-100);
    isJulia = false;
    resize(_w, _h);
}

FractalRenderer::~FractalRenderer()
{
    if(imageData) {
        delete[] imageData;
        imageData = NULL_ADD;
    }
}

void FractalRenderer::moveProcess(int dx, int dy, double newTop, double newLeft, double newBottom, double newRight)
{
    if (!imageData) return;

    bool isJulia = (lastPoint.real() != -100);
    if (dx == 0 && dy == 0) return;

    double realDx = (newRight - newLeft) / double(width);
    double realDy = (newBottom - newTop) / double(height);

    // Temporary buffer for pixel movement
    std::vector<IndexOfPt> tempImage(widthEx * height, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcX = x - dx;
            int srcY = y - dy;
            if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                tempImage[y * widthEx + x] = imageData[srcY * widthEx + srcX];
            }
        }
    }
    memcpy(imageData, tempImage.data(), widthEx * height * sizeof(IndexOfPt));

    // Recalculate missing areas
    QThreadPool* threadPool = QThreadPool::globalInstance();
    int threadCount = QThread::idealThreadCount();
    int rowsPerThread = (height + threadCount - 1) / threadCount; // Divide evenly

    for (int i = 0; i < threadCount; ++i) {
        int threadStartY = i * rowsPerThread;
        int threadEndY = std::min((i + 1) * rowsPerThread, height); // Avoid overflo

        FractalWorker* worker = new FractalWorker(
            threadStartY, threadEndY, newLeft, newTop, realDx, realDy,
            width, height, maxInterval, imageData, isJulia, lastPoint);

        worker->setAutoDelete(true);
        threadPool->start(worker);
    }

    threadPool->waitForDone();
}


void FractalRenderer::resize(int newx, int newy)
{
    if(newx > 0 && newy > 0 && (newx != width || newy != height)) {       
        width = newx;
        height = newy;

        widthEx = ((((width * 8) + 31) & ~31) >> 3);
        if(imageData) {
            delete[] imageData;
        }
        imageData = new IndexOfPt[widthEx * height];
    }
}

void FractalRenderer::renderMandelbrotMultithreaded(double left, double top, double right, double bottom)
{
    if (!imageData) {
        qDebug() << "Error: imageData is null!";
        return;
    }

    double zoomLevel = std::log2(1.0 / (right - left));
    int maxIteration = std::min(255 + int(zoomLevel * 50), 2000); // Adjust dynamically
    if(maxIteration <= MAX_INTERATION)
        maxIteration = MAX_INTERATION;

    QThreadPool* threadPool = QThreadPool::globalInstance();
    int threadCount = QThread::idealThreadCount(); // Number of threads
    int rowsPerThread = (height + threadCount - 1) / threadCount; // Divide evenly

    double xInterval = (right - left) / width;
    double yInterval = (bottom - top) / height;

    for (int i = 0; i < threadCount; ++i) {
        int startY = i * rowsPerThread;
        int endY = std::min((i + 1) * rowsPerThread, height); // Avoid overflow

        FractalWorker* worker = new FractalWorker(startY, endY, left, top, xInterval, yInterval,
                                                  width, height, maxIteration, imageData, false, Complex(0,0));
        worker->setAutoDelete(true);
        threadPool->start(worker); // Start the worker
    }

    threadPool->waitForDone(); // Wait for all threads to complete
    qDebug() << "Fractal rendering completed.";
}

void FractalRenderer::renderJulia(Complex c, double left, double top, double right, double bottom)
{
    if (!imageData) {
        qDebug() << "Error: imageData is null!";
        return;
    }

    lastPoint = c; // Store the Julia set parameter
    isJulia = true; // Indicate that we are rendering a Julia set

    // Dynamically adjust MAX_ITERATION based on zoom level
    double zoomLevel = std::log2(1.0 / (right - left));
    int maxIteration = std::min(255 + int(zoomLevel * 50), 2000); // Adjust dynamically
    if (maxIteration <= MAX_INTERATION)
        maxIteration = MAX_INTERATION;

    QThreadPool* threadPool = QThreadPool::globalInstance();
    int threadCount = QThread::idealThreadCount(); // Number of threads
    int rowsPerThread = (height + threadCount - 1) / threadCount; // Divide evenly

    double xInterval = (right - left) / width;
    double yInterval = (bottom - top) / height;

    for (int i = 0; i < threadCount; ++i) {
        int startY = i * rowsPerThread;
        int endY = std::min((i + 1) * rowsPerThread, height); // Avoid overflow

        FractalWorker* worker = new FractalWorker(
            startY, endY, left, top, xInterval, yInterval,
            width, height, maxIteration, imageData,
            true, c); // Pass 'c' as both 'keyPt' and 'lastPoint'

        worker->setAutoDelete(true); // Worker will be automatically deleted by QThreadPool
        threadPool->start(worker);
    }

    threadPool->waitForDone(); // Wait for all threads to complete
    qDebug() << "Julia set rendering completed.";
}

void FractalRenderer::renderMandelbrot()
{
    renderMandelbrotMultithreaded(INIT_LEFT, INIT_TOP, INIT_RIGHT, INIT_BOTTOM);
}
//EOF
