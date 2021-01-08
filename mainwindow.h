#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <QTimer>
#include "fractalrenderer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    QColor value(int x, int y);
private:
    Ui::MainWindow *ui;
    unsigned rMult, gMult, bMult;
    FractalRenderer mandelbrot;
    QTimer *updateTimer;
    std::vector< std::vector<unsigned> > imageData;

};
#endif // MAINWINDOW_H
//EOF
