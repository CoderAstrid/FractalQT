#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>

const int INT_DRAW_TIME = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isStarted(false)
{
    colorTable.Generate(MAX_INTERATION, Palette::ePalHeightMap);

    ui->setupUi(this);
    for(int i = 0; i < eCntPalette; i++)
        ui->cbPalette->addItem(NAMES_PALETTE[i]);

    ui->gvMandel->setRender(&mandelbrot);

    connect(&mandelbrot, SIGNAL(doneUpdate()), this, SLOT(onDoneUpdate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    if(!isStarted) {
        isStarted = true;
        qDebug() << "First Paint";
        return;
    }
    qDebug() << "Normal Paint";
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
}

void MainWindow::onChangedPalette(int newPalette)
{
    Q_UNUSED(newPalette)
    updatePalette();
}

void MainWindow::onChangedCount(int newCountOfColor)
{
    Q_UNUSED(newCountOfColor)
    updatePalette();
}

void MainWindow::updatePalette()
{
    int cnt = ui->hsCount->value();
    int idxPal = ui->cbPalette->currentIndex();
    colorTable.Generate(cnt, (Palette)idxPal);
    ui->gvMandel->UpdatePalette(colorTable.table());
}

void MainWindow::onDoneUpdate()
{
    ui->gvMandel->UpdateMandel(colorTable.table());

    qDebug()<<"Finished from Thread";
}
//EOF
