#ifndef MANDELBROTVIEW_H
#define MANDELBROTVIEW_H

#include <QWidget>
#include "fractalrenderer.h"
#include <QMutex>

class MandelBrotView : public QWidget
{
    Q_OBJECT
public:
    MandelBrotView(QWidget *parent = nullptr);
    virtual ~MandelBrotView();

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    void setRender(FractalRenderer* _render);
    void UpdateMandel(const std::vector<QColor>& table);
    void Update(const uchar* img, int w, int h, int sz, const std::vector<QColor>& table);
    void UpdatePalette(const std::vector<QColor>& table);
private:
    QImage* image;
    QImage* drawImage;
    FractalRenderer* render;
    QMutex mutexDraw;
    QPoint posJulia;
    bool setJulia;
};

#endif // MANDELBROTVIEW_H
