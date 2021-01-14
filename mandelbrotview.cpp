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
    colorTable.clear();
    recal();
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
    recal();
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
    if((event->button() == Qt::LeftButton && isMouseLButton) || (event->button() == Qt::RightButton && !isMouseLButton)) {
        mouseMode = 1;
        oldMousePt = pos;
    } else if((event->button() == Qt::RightButton && isMouseLButton) || (event->button() == Qt::LeftButton && !isMouseLButton)) {
        mouseMode = 2;
        emit number_chosen(Complex(left+(right-left)*double(pos.x())/double(width()),
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
    if(mouseMode == 1) {
        QPoint newpos = event->pos();
        QPoint dif = newpos-oldMousePt;
        left -= (right-left) * double(dif.x()) / double(width());
        right -= (right-left) * double(dif.x()) / double(width());
        top += (top-bottom) * double(dif.y()) / double(height());
        bottom += (top-bottom) * double(dif.y()) / double(height());
        oldMousePt = newpos;
        //recal();
        render->move_window(dif.x(), dif.y(), top, left, bottom, right);
        updateContents();
    } else if(mouseMode == 2) {
        mouseX = event->pos().x();
        mouseY = event->pos().y();
        emit number_chosen(Complex(left+(right-left)*double(event->pos().x())/double(width()),
                                  bottom+(top-bottom)*double(height()-event->pos().y())/double(height())));
    }
}

void MandelBrotView::mouseReleaseEvent(QMouseEvent *)
{
    mouseMode = 0;
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
        recal();
    }
}

void MandelBrotView::set_julia_number(Complex newnum)
{
    juliaPoint = newnum;
    recal();
}

void MandelBrotView::mod_changed(int newmod)
{

}
void MandelBrotView::recal()
{
    if(isJulia) {
        render->renderJulia(juliaPoint, left, top, right, bottom);
    } else {
        render->renderMandelbrot(left, top, right, bottom);
    }
    updateContents();
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
