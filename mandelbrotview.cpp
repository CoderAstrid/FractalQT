#include "mandelbrotview.h"
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>

MandelBrotView::MandelBrotView(QWidget *parent)
    : QWidget(parent)
    , image(nullptr)
    , drawImage(nullptr)
    , render(nullptr)
#if _DEV_VER101
    , left(INIT_LEFT)
    , right(INIT_RIGHT)
    , top(INIT_TOP)
    , bottom(INIT_BOTTOM)
    , isMouseLButton(false)
    , mouseMode(eDownNone)
    , mouseX(0)
    , mouseY(0)
#endif//_DEV_VER101
    , isJulia(false)
{
#if _DEV_VER101
    render = new FractalRenderer(width(), height());
    juliaPoint.real(-1);
    juliaPoint.imag(0);
    colorTable.clear();
    recalcAll();
#endif//_DEV_VER101
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
#if _DEV_VER101
#else
    if(isJulia) {
        QRect rc(oldMousePt.x()-3,oldMousePt.y()-3,6,6);
        painter.drawRect(rc);
    }
#endif//_DEV_VER101
}

void MandelBrotView::resizeEvent(QResizeEvent * event)
{
#if _DEV_VER101
    QSize nsize = event->size();
    render->resize(nsize.width(), nsize.height());
    recalcAll();
#else
    int w = width();
    int h = height();
    if(render && render->setDimensions(w, h)) {
        render->runRenderer(std::thread::hardware_concurrency());
    }
#endif//_DEV_VER101
}

void MandelBrotView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
#if _DEV_VER101
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
#else
    isJulia = true;
    oldMousePt = pos;
    if(render) {
        render->updateJulia(pos.x(), pos.y());
    }
    update();
#endif//_DEV_VER101
}

#if _DEV_VER101
void MandelBrotView::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseMode == eRDownMove) {
        QPoint newpos = event->pos();
        QPoint dif = newpos-oldMousePt;
        left -= (right-left) * double(dif.x()) / double(width());
        right -= (right-left) * double(dif.x()) / double(width());
        top += (top-bottom) * double(dif.y()) / double(height());
        bottom += (top-bottom) * double(dif.y()) / double(height());
        oldMousePt = newpos;
        //recal();
        render->moveProcess(dif.x(), dif.y(), top, left, bottom, right);
        updateContents();
    } else {
        mouseX = event->pos().x();
        mouseY = event->pos().y();
        Complex pt(left+(right-left)*double(event->pos().x())/double(width()),
                   bottom+(top-bottom)*double(height()-event->pos().y())/double(height()));
        if(mouseMode == eLDownJulia) {
            emit juliaPointChanged(pt);
        }
        if(!isJulia) {
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
        render->renderMandelbrot(left, top, right, bottom);
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
#else

void MandelBrotView::setRender(FractalRenderer *_render)
{
    render = _render;
}

void MandelBrotView::UpdateMandel(const std::vector<QColor>& table)
{
    if(render) {
        int w = width();
        int h = height();
        const uchar* imageData=render->getImageData();
        Update(imageData, w, h, render->area(), table);
    }
}

void MandelBrotView::Update(const uchar *img, int w, int h, int sz, const std::vector<QColor>& table)
{
    qDebug()<<"Updating View...";
    if(image) {
        delete  image;
    }
    image = new QImage(w, h, QImage::Format_Indexed8);
    memcpy(image->bits(), img, sz);

    for(int i = 0; i < 256; i++) {
        image->setColor(i, qRgb(0, 0, 0));
    }
    for(uint i = 0; i < table.size(); i++)
        image->setColor(i + 1, table.at(i).rgba());

    mutexDraw.lock();
    if(drawImage)
        delete drawImage;
    drawImage = new QImage(*image);
    mutexDraw.unlock();

    update();
}
#endif//_DEV_VER101

void MandelBrotView::updatePalette(const std::vector<QColor> &table)
{
    qDebug()<<"Updating palette...";
#if _DEV_VER101
    colorTable = table;
#endif//colorTable
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
