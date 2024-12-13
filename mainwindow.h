#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "devMacro.h"
#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <QTimer>

#include "colorlut.h"

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
    void onChangedMandelPt(Complex);
private:
    Ui::MainWindow *ui;
    std::vector<ColorLut>        colorTable;
    bool            isStarted;
};
#endif // MAINWINDOW_H
//EOF
