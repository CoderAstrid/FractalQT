#ifndef QTCUSTOMSLIDER_H
#define QTCUSTOMSLIDER_H

#include <QSlider>

class CustomSlider : public QSlider
{
    Q_OBJECT

public:
    explicit CustomSlider(QWidget *parent = nullptr);
    ~CustomSlider();

protected:
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void focusIn();
    void focusOut();
};

#endif // QTCUSTOMSLIDER_H
