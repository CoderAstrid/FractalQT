#include "fractalrenderer.h"
#include <complex>
#include <QThreadPool>
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

    Complex keyPt(newLeft, newTop);
    double realDx = (newRight - newLeft) / double(width);
    double realDy = (newBottom - newTop) / double(height);

    // Temporary buffer to safely move pixels
    std::vector<IndexOfPt> tempImage(widthEx * height);
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

    // Update missing rows (vertical movement)
    if (dy != 0) {
        int startY = (dy > 0) ? 0 : height + dy;
        int endY = (dy > 0) ? dy : height;

#pragma omp parallel for schedule(dynamic, 16)
        for (int y = startY; y < endY; y++) {
            IndexOfPt* line = imageData + y * widthEx;
#pragma omp simd
            for (int x = 0; x < width; x++) {
                line[x] = calcPointHelper(isJulia, keyPt, lastPoint, x, y, realDx, realDy);
            }
        }
    }

    // Update missing columns (horizontal movement)
    if (dx != 0) {
        int startX = (dx > 0) ? 0 : width + dx;
        int endX = (dx > 0) ? dx : width;

#pragma omp parallel for schedule(dynamic, 16)
        for (int y = 0; y < height; y++) {
            IndexOfPt* line = imageData + y * widthEx;
#pragma omp simd
            for (int x = startX; x < endX; x++) {
                line[x] = calcPointHelper(isJulia, keyPt, lastPoint, x, y, realDx, realDy);
            }
        }
    }
}

int FractalRenderer::calcPointHelper(bool isJulia, const Complex& keyPt, const Complex& lastPoint,
                                     int x, int y, double realDx, double realDy)
{
    double real = keyPt.real() + x * realDx;
    double imag = keyPt.imag() + y * realDy;
    if (isJulia) {
        return calcPoint(Complex(real, imag), lastPoint);
    } else {
        return calcPoint(Complex(0, 0), Complex(real, imag));
    }
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

inline int FractalRenderer::calcPoint(Complex start, Complex c) const
{
    Complex z = start;
    for(int i = 0;i<MAX_INTERATION;i++) {
        z = z*z+c;
        if(abs(z) > 1000) {
            return i;
        }
    }
    return MAX_INTERATION;
}

inline Complex FractalRenderer::mandelFunc(Complex z, Complex c) const
{
    return z * z + c;
}

void FractalRenderer::renderMandelbrot(double left, double top, double right, double bottom)
{
    if(!imageData)
        return;
    lastPoint = Complex(-100, -100);
    Complex rangeul = Complex(left, top);
    double xinterval = (right-left) / double(width);
    double yinterval = (bottom-top) / double(height);
#pragma omp parallel for
    for(int y = 0;y<height;y++) {
        IndexOfPt* line = imageData + y * widthEx;
        for(int x = 0;x<width;x++) {
            int res = calcPoint(Complex(0,0), rangeul+Complex(x*xinterval, y*yinterval));
            line[x] = res;
        }
    }
}

void FractalRenderer::renderMandelbrotMultithreaded(double left, double top, double right, double bottom)
{
    if (!imageData) {
        qDebug() << "Error: imageData is null!";
        return;
    }

    QThreadPool* threadPool = QThreadPool::globalInstance();
    int threadCount = QThread::idealThreadCount(); // Number of threads
    int rowsPerThread = height / threadCount;

    double xInterval = (right - left) / width;
    double yInterval = (bottom - top) / height;

    for (int i = 0; i < threadCount; ++i) {
        int startY = i * rowsPerThread;
        int endY = (i == threadCount - 1) ? height : startY + rowsPerThread;

        FractalWorker* worker = new FractalWorker(startY, endY, left, top, xInterval, yInterval,
                                                  width, height, MAX_INTERATION, imageData);
        threadPool->start(worker); // Start the worker
    }

    threadPool->waitForDone(); // Wait for all threads to complete
    qDebug() << "Fractal rendering completed.";
}

void FractalRenderer::renderJulia(Complex c, double left, double top, double right, double bottom)
{
    if(!imageData)
        return;
    lastPoint = c;
    Complex rangeul = Complex(left, top);
    double xinterval = (right-left) / double(width);
    double yinterval = (bottom-top) / double(height);
#pragma omp parallel for
    for(int y = 0;y<height;y++) {
        IndexOfPt* line = imageData + y * widthEx;
        for(int x = 0;x<width;x++) {
            int res = calcPoint(rangeul + Complex(x*xinterval, y*yinterval), c);
            line[x] = res;
        }
    }
}

void FractalRenderer::renderMandelbrot()
{
    renderMandelbrotMultithreaded(INIT_LEFT, INIT_TOP, INIT_RIGHT, INIT_BOTTOM);
}
//EOF
