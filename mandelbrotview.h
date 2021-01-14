#ifndef MANDELBROTVIEW_H
#define MANDELBROTVIEW_H
#include "devMacro.h"
#include <QWidget>
#include "fractalrenderer.h"
#include <QMutex>
#include <complex>

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
    }
#else
    // main public functions
    void setRender(FractalRenderer* _render);
    void UpdateMandel(const std::vector<QColor>& table);
    void Update(const uchar* img, int w, int h, int sz, const std::vector<QColor>& table);
#endif//_DEV_VER101
    void UpdatePalette(const std::vector<QColor>& table);

    /* ------------------- new updating ------------------- */
#if _DEV_VER101
public slots:
    void set_julia_number(Complex newnum);
    void mod_changed(int newmod);

signals:
    void number_chosen(Complex newnum);
    /* ------------------- new updating ------------------- */
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
    int                 mouseMode;
    int                 mouseX;
    int                 mouseY;
#else
    QMutex              mutexDraw;
#endif//_DEV_VER101
    QPoint              oldMousePt;

    // for julia set
    bool                isJulia;
#if _DEV_VER101
    Complex             juliaPoint;

    void recal();
#endif//_DEV_VER101
};

#endif // MANDELBROTVIEW_H
//EOF
