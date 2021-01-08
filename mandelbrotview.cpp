#include "mandelbrotview.h"
#include <QPainter>
#include <QPoint>
#include <QDebug>

MandelBrotView::MandelBrotView(QWidget *parent) : QWidget(parent)
{
    image = nullptr;
}

MandelBrotView::~MandelBrotView()
{
    if(image) {
        delete image;
        image = nullptr;
    }
}

void MandelBrotView::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    if(image)
        painter.drawImage(QPoint(0,0), *image);
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

    update();
}

void MandelBrotView::UpdatePalette(const std::vector<QColor> &table)
{
    qDebug()<<"Updating palette...";

    if(image) {
        for(int i = 0; i < 256; i++) {
            image->setColor(i, qRgb(0, 0, 0));
        }
        for(uint i = 0; i < table.size(); i++)
            image->setColor(i + 1, table.at(i).rgba());
        update();
    }
}
//EOF
