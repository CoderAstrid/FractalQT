#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <QPixmap>
#include <QIcon>
#include <QLabel>

const int INT_DRAW_TIME = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isStarted(false)
{
    colorTable.Generate(MAX_INTERATION, Palette::ePalHeightMap);

    ui->setupUi(this);

#if _DEV_VER101
    updatePalette();
    ui->gvJulia->setJuliaView(true);
    connect(ui->gvMandel, SIGNAL(juliaPointChanged(Complex)), ui->gvJulia, SLOT(setJuliaPoint(Complex)));
    connect(ui->gvMandel, SIGNAL(mandelPointChanged(Complex)), this, SLOT(onChangedMandelPt(Complex)));
#else
    ui->gvMandel->setRender(&mandelbrot);
    connect(&mandelbrot, SIGNAL(doneUpdate()), this, SLOT(onDoneUpdate()));
#endif//_DEV_VER101
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    if(!isStarted) {
        isStarted = true;
        initialColorCombo();
        qDebug() << "First Paint";
        return;
    }
    qDebug() << "Normal Paint";
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    ui->cbPalette->setIconSize(QSize(ui->cbPalette->width(), 23));
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
    ui->gvMandel->updatePalette(colorTable.table());

#if _DEV_VER101
    ui->gvJulia->updatePalette(colorTable.table());
#endif//_DEV_VER101
}

void MainWindow::initialColorCombo()
{
    double pix_width = ui->cbPalette->width(); // Since the array size is 256, so the picture size need to go to a multiple of 256
    double pix_height = 23;
    double step = pix_width / 256.0;
    for(int iPal = 0; iPal < eCntPalette; iPal++) {
        ColorLut lut1(256, (Palette)iPal);
        QPixmap pix(QSize(pix_width, pix_height));
        QPainter painter(&pix);
        for(int i = 0; i < 256; i++) {
            QRgb col = lut1.at(i);
            QRectF source(step * i, 0, step, pix_height);
            painter.fillRect(source, col);
        }
        ui->cbPalette->addItem(QIcon(pix), ""/*NAMES_PALETTE[iPal]*/);
        ui->cbPalette->setIconSize(QSize(pix_width, pix_height));
        ui->cbPalette->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }
}

#if _DEV_VER101
void MainWindow::onChangedMandelPt(Complex pt)
{
    QString sMsg("%1, %2");
    QString status = QString::number(pt.real(), 'g') + ", " + QString::number(pt.imag(), 'g');
    ui->sBar->showMessage(status);
}
#else
void MainWindow::onDoneUpdate()
{
    ui->gvMandel->UpdateMandel(colorTable.table());

    qDebug()<<"Finished from Thread";
}
#endif//_DEV_VER101

void MainWindow::on_cmdReset_clicked()
{
    ui->gvMandel->reset();
    ui->gvJulia->reset();
}

//EOF

