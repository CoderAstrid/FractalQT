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
    , left(-3.0f)
    , right(1.0f)
    , top(2.0f)
    , bottom(-2.0f)
    , isMouseLButton(false)
    , mouseMode(0)
    , mouseX(0)
    , mouseY(0)
#endif//_DEV_VER101
    , isJulia(false)
{
#if _DEV_VER101
    render = new FractalRenderer(width(), height());
    juliaPoint.real(-1);
    juliaPoint.imag(0);
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
#if _DEV_VER101
#else
    mutexDraw.lock();
    if(drawImage)
        painter.drawImage(QPoint(0,0), *drawImage);
    mutexDraw.unlock();
    if(isJulia) {
        QRect rc(oldMousePt.x()-3,oldMousePt.y()-3,6,6);
        painter.drawRect(rc);
    }
#endif//_DEV_VER101
}

void MandelBrotView::resizeEvent(QResizeEvent * /*event*/)
{
    int w = width();
    int h = height();
#if _DEV_VER101
#else
    if(render && render->setDimensions(w, h)) {
        render->runRenderer(std::thread::hardware_concurrency());
    }
#endif//_DEV_VER101
}

void MandelBrotView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
#if _DEV_VER101
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
void MandelBrotView::mouseMoveEvent(QMouseEvent *)
{

}

void MandelBrotView::mouseReleaseEvent(QMouseEvent *)
{

}

void MandelBrotView::wheelEvent(QWheelEvent*)
{

}

void MandelBrotView::set_julia_number(Complex newnum)
{

}

void MandelBrotView::mod_changed(int newmod)
{

}
void MandelBrotView::recal()
{
    if(isJulia) {
        render->renderJulia(juliaPoint, top, left, bottom, right);
    } else {
        render->renderMandelbrot(top, left, bottom, right);
    }
    /*
    QImage * zw = render->export_picture();
    QPixmap map = QPixmap::fromImage(*zw);

    if(isJulia) {
        QPainter *painter = new QPainter(&map);
        QFont font("", 12); // arbitrary (default) font, size 2
        painter->setFont(font);
        painter->drawText(QPointF(30,30), QString("(%1, %2)").arg(julia_value.r).arg(julia_value.i));
        painter->end();
    }
    // ui->m_picture->setPixmap(map);
    delete zw;
    */
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

void MandelBrotView::UpdatePalette(const std::vector<QColor> &table)
{
    qDebug()<<"Updating palette...";

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
