#ifndef MANDELBROTVIEW_H
#define MANDELBROTVIEW_H

#include <QWidget>

class MandelBrotView : public QWidget
{
    Q_OBJECT
public:
    MandelBrotView(QWidget *parent = nullptr);
    virtual ~MandelBrotView();

    virtual void paintEvent(QPaintEvent *event);

    void Update(const uchar* img, int w, int h, int sz, const std::vector<QColor>& table);
    void UpdatePalette(const std::vector<QColor>& table);
private:
    QImage* image;
};

#endif // MANDELBROTVIEW_H
