#include "customslider.h"
#include <QStylePainter>
#include <QStyleOptionSlider>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>

CustomSlider::CustomSlider(QWidget *parent)
    : QSlider(parent)
{

}

CustomSlider::~CustomSlider()
{

}

void CustomSlider::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    emit focusIn();
}

void CustomSlider::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    emit focusOut();
}

void CustomSlider::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    //QSlider::paintEvent(event);

    QStylePainter p(this);
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect handle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    // draw tick marks
    // do this manually because they are very badly behaved with style sheets
    int interval = tickInterval();
    if (interval == 0)
    {
        interval = pageStep();
    }

    // draw the slider (this is basically copy/pasted from QSlider::paintEvent)
    opt.subControls = QStyle::SC_SliderGroove;
    p.drawComplexControl(QStyle::CC_Slider, opt);

    // draw the slider handle
    opt.subControls = QStyle::SC_SliderHandle;
    p.drawComplexControl(QStyle::CC_Slider, opt);

    if (tickPosition() != NoTicks)
    {
        if (orientation() == Qt::Vertical) {
            for (int i = minimum(); i <= maximum(); i += interval)
            {
                int y = round((double)(
                    (double)((double)(i - this->minimum()) / (double)(this->maximum() - this->minimum())) *
                    (double)(this->height() - handle.height()) + handle.height() / 2.0)) - 1;
                int h = 4;
                p.setPen(QColor("#ffffff"));
                if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove)
                {
                    int x = this->rect().left();
                    p.drawLine(x, y, x + h, y);
                }
                if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow)
                {
                    int x = this->rect().right();
                    p.drawLine(x, y, x - h, y);
                }
            }
        }
        else {
            for (int i = minimum(); i <= maximum(); i += interval)
            {
                int x = round((double)(
                    (double)((double)(i - this->minimum()) / (double)(this->maximum() - this->minimum())) *
                    (double)(this->width() - handle.width()) + handle.width() / 2.0)) - 1;
                int h = 4;
                p.setPen(QColor("#ffffff"));
                if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove)
                {
                    int y = this->rect().top();
                    p.drawLine(x, y, x, y + h);
                }
                if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow)
                {
                    int y = this->rect().bottom();
                    p.drawLine(x, y, x, y - h);
                }
            }
        }
    }

}
