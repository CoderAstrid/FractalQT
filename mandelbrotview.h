#ifndef MANDELBROTVIEW_H
#define MANDELBROTVIEW_H

#include "devMacro.h"
#include <QWidget>
#include "fractalrenderer.h"
#include <QMutex>
#include <complex>

typedef enum E_MousePressType{
    eDownNone,
    eLDownJulia,
    eRDownMove,
    eCntMousePressType
}MousePressType;

class MandelBrotView : public QWidget
{
    Q_OBJECT
public:
    MandelBrotView(QWidget *parent = nullptr);
    virtual ~MandelBrotView();

    // virtual events
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
#if _DEV_VER101
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent*);

    void setJuliaView(bool mode)
    {
        isJulia = mode;

        resize(width(), height());
    }
    void reset();
#else
    // main public functions
    void setRender(FractalRenderer* _render);
    void UpdateMandel(const std::vector<QColor>& table);
    void Update(const uchar* img, int w, int h, int sz, const std::vector<QColor>& table);
#endif//_DEV_VER101
    void updatePalette(const std::vector<QColor>& table);    
#if _DEV_VER101
public slots:
    void setJuliaPoint(Complex newnum);
    void intervalChanged(int newmod);

signals:
    void juliaPointChanged(Complex newnum);
    void mandelPointChanged(Complex newnum);
#endif//_DEV_VER101

private:
    // for painting
    QImage              *image;
    QImage              *drawImage;

    // for engine
    FractalRenderer     *render;
#if _DEV_VER101
    double              left, right, top, bottom;
#endif//_DEV_VER101

    // for drawing
#if _DEV_VER101
    bool                isMouseLButton;
    MousePressType      mouseMode;
    int                 mouseX;
    int                 mouseY;
    std::vector<QColor> colorTable;
#else

#endif//_DEV_VER101
    QMutex              mutexDraw;
    QPoint              oldMousePt;

    // for julia set
    bool                isJulia;
#if _DEV_VER101
    Complex             juliaPoint;

    void recalcAll();
    void updateContents();
#endif//_DEV_VER101
};

#endif // MANDELBROTVIEW_H
//EOF
