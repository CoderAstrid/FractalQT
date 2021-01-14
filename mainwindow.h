#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "devMacro.h"
#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <QTimer>

#include "colorlut.h"
#if _DEV_VER101
#else
#   include "fractalrenderer.h"
#endif//_DEV_VER101

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
    void updatePalette();
    void initialColorCombo();
public slots:
    void onChangedPalette(int newPalette);
    void onChangedCount(int newCountOfColor);
    void on_cmdReset_clicked();
#if _DEV_VER101
    void onChangedMandelPt(Complex);
#else
    void onDoneUpdate();
#endif//_DEV_VER101
private:
    Ui::MainWindow *ui;    
#if _DEV_VER101
#else
    FractalRenderer mandelbrot;
#endif//_DEV_VER101
    ColorLut        colorTable;
    bool            isStarted;
};
#endif // MAINWINDOW_H
//EOF
