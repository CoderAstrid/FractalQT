#include "mandelbrotview.h"
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>

MandelBrotView::MandelBrotView(QWidget *parent)
    : QGLWidget(parent)
    , image(nullptr)
    , drawImage(nullptr)
    , render(nullptr)
    , left(INIT_LEFT)
    , right(INIT_RIGHT)
    , top(INIT_TOP)
    , bottom(INIT_BOTTOM)
    , isMouseLButton(false)
    , mouseMode(eDownNone)
    , mouseX(0)
    , mouseY(0)
    , isJulia(false)
{
    render = new FractalRenderer(width(), height());
    juliaPoint.real(-1);
    juliaPoint.imag(0);
    colorTable.clear();
    recalcAll();
}

MandelBrotView::~MandelBrotView()
{
    if(image) {
        delete image;
        image = nullptr;
    }
    if(drawImage) {
        delete  drawImage;
        drawImage = nullptr;
    }
    render = nullptr;
}

void MandelBrotView::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    qDebug() << "Pained View" << width() << height();
    mutexDraw.lock();
    if(drawImage)
        painter.drawImage(QPoint(0,0), *drawImage);
    mutexDraw.unlock();
}

void MandelBrotView::resizeEvent(QResizeEvent * event)
{
    QSize nsize = event->size();
    render->resize(nsize.width(), nsize.height());
    recalcAll();
}

void MandelBrotView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if((event->button() == Qt::LeftButton && isMouseLButton) ||
            (event->button() == Qt::RightButton && !isMouseLButton)) {
        mouseMode = eRDownMove;
        oldMousePt = pos;
    } else if((event->button() == Qt::RightButton && isMouseLButton) ||
              (event->button() == Qt::LeftButton && !isMouseLButton)) {
        mouseMode = eLDownJulia;
        emit juliaPointChanged(Complex(left+(right-left)*double(pos.x())/double(width()),
                                  bottom+(top-bottom)*double(height()-pos.y())/double(height())));
    }
}

void MandelBrotView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint newpos = event->pos();
    QPoint dif = newpos - oldMousePt;

    if (mouseMode == eRDownMove) {
        // Adjust fractal boundaries based on mouse drag
        double dx = (right - left) * double(dif.x()) / double(width());
        double dy = (top - bottom) * double(dif.y()) / double(height());
        left -= dx;
        right -= dx;
        top += dy;
        bottom += dy;

        oldMousePt = newpos;

        // Move process to shift pixels and update only changed areas
        render->moveProcess(dif.x(), dif.y(), top, left, bottom, right);
        updateContents(); // Trigger repaint
    } else {
        mouseX = event->pos().x();
        mouseY = event->pos().y();

        // Map mouse position to complex plane
        Complex pt(
            left + (right - left) * double(mouseX) / double(width()),
            bottom + (top - bottom) * double(height() - mouseY) / double(height())
        );

        if (mouseMode == eLDownJulia) {
            emit juliaPointChanged(pt);
        }
        if (!isJulia) {
            emit mandelPointChanged(pt);
        }
    }
}

void MandelBrotView::mouseReleaseEvent(QMouseEvent *)
{
    mouseMode = eDownNone;
}

void MandelBrotView::wheelEvent(QWheelEvent* event)
{
    QPoint scr = event->angleDelta();
    if(!scr.isNull()) {
        double mult = scr.y() > 0 ? (100.0f/scr.y()) : 1/(-100.0f/scr.y());
        double zwx = left+double((right-left)*event->x())/double(width());
        double zwy = bottom+double((top-bottom)*(height()-event->y()))/double(height());
        double nw = (right-left) * mult;
        double nh = (top-bottom) * mult;
        left = zwx-(nw*(zwx-left) / (right-left));
        right = left+nw;
        bottom = zwy-(nh*(zwy-bottom) / (top-bottom));
        top = bottom + nh;
        recalcAll();
    }
}

void MandelBrotView::setJuliaPoint(Complex newnum)
{
    juliaPoint = newnum;
    recalcAll();
}

void MandelBrotView::intervalChanged(int newmod)
{

}
void MandelBrotView::recalcAll()
{
    if(isJulia) {
        render->renderJulia(juliaPoint, left, top, right, bottom);
    } else {
        render->renderMandelbrotMultithreaded(left, top, right, bottom);
    }
    updateContents();
}

void MandelBrotView::updateContents()
{
    int w = width();
    int h = height();
    if(!render || w < 1 || h < 1)
        return;

    const uchar* imageData=render->getImageData();
    if(image) {
        delete  image;
    }
    image = new QImage(w, h, QImage::Format_Indexed8);
    memcpy(image->bits(), imageData, render->area());

    for(int i = 0; i < 256; i++) {
        image->setColor(i, qRgb(0, 0, 0));
    }
    for(uint i = 0; i < colorTable.size(); i++)
        image->setColor(i + 1, colorTable.at(i).rgba());

    mutexDraw.lock();
    if(drawImage)
        delete drawImage;
    drawImage = new QImage(*image);
    mutexDraw.unlock();
    update();
}

void MandelBrotView::reset()
{
    if(isJulia) {
        left = INIT_LEFT_JULIA;
        right = INIT_RIGHT_JULIA;
        top = INIT_TOP_JULIA;
        bottom = INIT_BOTTOM_JULIA;
    }
    else {
        left = INIT_LEFT;
        right = INIT_RIGHT;
        top = INIT_TOP;
        bottom = INIT_BOTTOM;
    }
    recalcAll();
}

void MandelBrotView::updatePalette(const std::vector<QColor> &table)
{
    qDebug()<<"Updating palette...";
    colorTable = table;
    if(drawImage) {
        for(int i = 0; i < 256; i++) {
            drawImage->setColor(i, qRgb(0, 0, 0));
        }
        for(uint i = 0; i < table.size(); i++)
            drawImage->setColor(i + 1, table.at(i).rgba());
        update();
    }
}
//EOF
