#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H

#include "common_type.h"
#include <vector>
#include <QObject>


const int PALETE_STEP = 5;
const int MAX_INTERATION = 255;

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

    void renderMandelbrot();
    void moveProcess(int dx, int dy, double top, double left, double down, double right);
    void resize(int _w, int _h);
    void setInterval(int newInt)
    {
        maxInterval = newInt;
    }
    void renderMandelbrot(double left, double top, double right, double bottom);
    void renderMandelbrotMultithreaded(double left, double top, double right, double bottom);
    void renderJulia(Complex c, double left, double top, double right, double bottom);

private:    
    int             width;    
    int             height;
    int             maxWidth;
    int             maxHeight;
    int             maxInterval;
    Complex         lastPoint;

    int             calcPoint(Complex start, Complex point) const;
    Complex         mandelFunc(Complex z, Complex c) const;
    int calcPointHelper(bool isJulia, const Complex& keyPt, const Complex& lastPoint,
                                         int x, int y, double realDx, double realDy);
    int             widthEx;
    IndexOfPt       *imageData;
#if _DEV_QT
signals:
    void doneUpdate();
#endif//_DEV_QT
};

#endif // FRACTALRENDERER_H
//EOF
