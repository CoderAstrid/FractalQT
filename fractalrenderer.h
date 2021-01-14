#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H

#include "common_type.h"
#include <vector>
#if _DEV_VER101
#else
#   include <chrono>
#   include <mutex>
#   include <thread>
#endif//_DEV_VER101

#include <QObject>

const unsigned int PALATE_SIZE = 256;
const int PALETE_STEP = 5;
const int MAX_INTERATION = 250;

const double INIT_LEFT = -3.0f;
const double INIT_TOP = -2.0f;
const double INIT_RIGHT = 1.0f;
const double INIT_BOTTOM = 2.0f;

const double INIT_LEFT_JULIA = -2.0f;
const double INIT_TOP_JULIA = -2.0f;
const double INIT_RIGHT_JULIA = 2.0f;
const double INIT_BOTTOM_JULIA = 2.0f;

class FractalRenderer
#if _DEV_QT
        : public QObject
#endif
{
#if _DEV_QT
    Q_OBJECT
#endif//_DEV_QT
public:
    FractalRenderer(int _w = 0, int _h = 0);
    virtual ~FractalRenderer();

    const IndexOfPt* getImageData() const
    {
        return imageData;
    }
    int area() const
    {
        return widthEx * height;
    }

#if _DEV_VER101
    void renderMandelbrot();
    void moveProcess(int dx, int dy, double top, double left, double down, double right);
    void resize(int _w, int _h);
    void setInterval(int newInt)
    {
        maxInterval = newInt;
    }
    void renderMandelbrot(double left, double top, double right, double bottom);
    void renderJulia(Complex c, double left, double top, double right, double bottom);
#else
    bool setDimensions(int x, int y);

    bool isFinished() const
    {
        return drawingFinished;
    }
    void updateJulia(int x, int y);

    void stop();
    void runRenderer(unsigned threads);
#endif//_DEV_VER101
private:    
    int             width;    
    int             height;
#if _DEV_VER101
    int             maxWidth;
    int             maxHeight;
    int             maxInterval;
    Complex         lastPoint;

    int             calcPoint(Complex start, Complex point) const;
    Complex         mandelFunc(Complex z, Complex c) const;
#else
    unsigned        alivedThreads;
    std::mutex      lock;
    std::chrono::milliseconds renderStartTime;
    bool            drawingFinished;
    bool            isStopped;

    unsigned char   value(int x, int y);
    void            render(int widthFrom, int widthTo);
#endif//_DEV_VER101
    int             widthEx;
    IndexOfPt       *imageData;
#if _DEV_QT
#   if _DEV_VER101
signals:
    void doneUpdate();
#   endif//_DEV_VER101
#endif//_DEV_QT
};

#endif // FRACTALRENDERER_H
//EOF
