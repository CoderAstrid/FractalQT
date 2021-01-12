#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <QPixmap>
#include <QIcon>

const int INT_DRAW_TIME = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isStarted(false)
{
    colorTable.Generate(MAX_INTERATION, Palette::ePalHeightMap);

    ui->setupUi(this);

    //for(int i = 0; i < eCntPalette; i++)
    //    ui->cbPalette->addItem(NAMES_PALETTE[i]);

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
        initialize_color();
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
    ui->gvMandel->UpdatePalette(colorTable.table());
}

void MainWindow::initialize_color()
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

void MainWindow::onDoneUpdate()
{
    ui->gvMandel->UpdateMandel(colorTable.table());

    qDebug()<<"Finished from Thread";
}
//EOF
