#ifndef MANDELBROTVIEW_H
#define MANDELBROTVIEW_H

#include "devMacro.h"
#include <QGLWidget>
#include "fractalrenderer.h"
#include <QMutex>
#include <complex>

typedef enum E_MousePressType{
    eDownNone,
    eLDownJulia,
    eRDownMove,
    eCntMousePressType
}MousePressType;

class MandelBrotView : public QGLWidget
{
    Q_OBJECT
public:
    MandelBrotView(QWidget *parent = nullptr);
    virtual ~MandelBrotView();

    // virtual events
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent*);

    void setJuliaView(bool mode)
    {
        isJulia = mode;

        resize(width(), height());
    }
    void reset();
    void updatePalette(const std::vector<QColor>& table);
public slots:
    void setJuliaPoint(Complex newnum);
    void intervalChanged(int newmod);

signals:
    void juliaPointChanged(Complex newnum);
    void mandelPointChanged(Complex newnum);

private:
    // for painting
    QImage              *image;
    QImage              *drawImage;

    // for engine
    FractalRenderer     *render;
    double              left, right, top, bottom;

    // for drawing
    bool                isMouseLButton;
    MousePressType      mouseMode;
    int                 mouseX;
    int                 mouseY;
    std::vector<QColor> colorTable;
    QMutex              mutexDraw;
    QPoint              oldMousePt;

    // for julia set
    bool                isJulia;
    Complex             juliaPoint;

    void recalcAll();
    void updateContents();
};

#endif // MANDELBROTVIEW_H
//EOF
