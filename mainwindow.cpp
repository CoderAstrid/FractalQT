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

    // timer for init drawing
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer->start(INT_DRAW_TIME);

    ui->setupUi(this);
    for(int i = 0; i < eCntPalette; i++)
        ui->cbPalette->addItem(NAMES_PALETTE[i]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    if(mandelbrot.isFinished() && updateTimer->isActive()) {
        updateTimer->stop();
    }
    int w = ui->gvMandel->width();
    int h = ui->gvMandel->height();
    const uchar* imageData=mandelbrot.getImageData();
    ui->gvMandel->Update(imageData, w, h, mandelbrot.area(), colorTable.table());
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    int w = ui->gvMandel->width();
    int h = ui->gvMandel->height();
    if(mandelbrot.setDimensions(w, h)) {
        if(mandelbrot.isFinished() && updateTimer->isActive()) {
            updateTimer->stop();
        }
        mandelbrot.runRenderer(std::thread::hardware_concurrency());

        if(updateTimer->isActive()==false) {
            updateTimer->start(INT_DRAW_TIME);
        }
    }
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
//EOF
