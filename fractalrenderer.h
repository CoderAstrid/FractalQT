#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H
#include "devMacro.h"

#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <QObject>
#include <complex>

const unsigned int PALATE_SIZE = 256;
const int PALETE_STEP = 5;
const int MAX_INTERATION = 250;

const double INIT_LEFT = -3.0f;
const double INIT_TOP = -2.0f;
const double INIT_RIGHT = 1.0f;
const double INIT_BOTTOM = 2.0f;

typedef std::complex<double>        Complex;
typedef unsigned char               IndexOfPt;

class FractalRenderer : public QObject
{
    Q_OBJECT
public:
    FractalRenderer(int _w = 0, int _h = 0);
    virtual ~FractalRenderer();

    bool setDimensions(int x, int y);

    bool isFinished() const
    {
        return drawingFinished;
    }
    const IndexOfPt* getImageData() const
    {
        return imageData;
    }
    int area() const
    {
        return widthEx * height;
    }
    void updateJulia(int x, int y);

    void stop();
    void runRenderer(unsigned threads);
#if _DEV_VER101
    void renderMandelbrot();
    void move_window(int xmove, int ymove, double top, double left, double down, double right);
    void resize(int newx, int newy);
    void setInterval(int newInt)
    {
        maxInterval = newInt;
    }
    void renderMandelbrot(double left, double top, double right, double bottom);
    void renderJulia(Complex c, double left, double top, double right, double bottom);
#endif//_DEV_VER101
private:    
    int             width;    
    int             height;
#if _DEV_VER101
    int             maxWidth;
    int             maxHeight;
    int             maxInterval;
    Complex         lastPoint;
#endif//_DEV_VER101
    int             widthEx;


    IndexOfPt       *imageData;

    unsigned        alivedThreads;
    std::mutex      lock;
    std::chrono::milliseconds renderStartTime;
    bool drawingFinished;
    bool isStopped;

    unsigned char value(int x, int y);
    void render(int widthFrom, int widthTo);
#if _DEV_VER101
    int calcPoint(Complex start, Complex point) const;
    Complex mandelFunc(Complex z, Complex c) const;

#endif//_DEV_VER101
signals:
    void doneUpdate();
};

#endif // FRACTALRENDERER_H
//EOF
