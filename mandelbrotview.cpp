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
    , setJulia(false)
{
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
    if(setJulia) {
        QRect rc(posJulia.x()-3,posJulia.y()-3,6,6);
        painter.drawRect(rc);
    }
}

void MandelBrotView::resizeEvent(QResizeEvent * event)
{
    int w = width();
    int h = height();
    if(render && render->setDimensions(w, h)) {
        render->runRenderer(std::thread::hardware_concurrency());
    }
}

void MandelBrotView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    setJulia = true;
    posJulia = pos;
    update();
}

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
