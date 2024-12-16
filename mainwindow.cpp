#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <QPixmap>
#include <QIcon>
#include <QLabel>
#include <QDir>

const int INT_DRAW_TIME = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isStarted(false)
{
//    colorTable.Generate(MAX_INTERATION, Palette::ePalHeightMap);
    ColorLut gray(MAX_INTERATION, ePalGray);
    colorTable.push_back(gray);
    ColorLut hmap(MAX_INTERATION, ePalHeightMap);
    colorTable.push_back(hmap);
    ColorLut spectral(MAX_INTERATION, eSpectral);
    colorTable.push_back(spectral);
    ColorLut ylgnbu(MAX_INTERATION, eYlGnBu);
    colorTable.push_back(ylgnbu);
    ColorLut heatMap(MAX_INTERATION, ePalHeatMap);
    colorTable.push_back(heatMap);
    ColorLut sineMap(MAX_INTERATION, ePalSineMap);
    colorTable.push_back(sineMap);

    QString path = QDir::currentPath() + "/colormap";
    QDir directory(path);
    QStringList images = directory.entryList(QStringList() << "*.map" << "*.MAP", QDir::Files);
    foreach(QString filename, images) {
        ColorLut lut(path + "/" + filename, filename);
        colorTable.push_back(lut);
    }

    ui->setupUi(this);

    updatePalette();
    ui->gvJulia->setJuliaView(true);
    connect(ui->gvMandel, SIGNAL(juliaPointChanged(Complex)), ui->gvJulia, SLOT(setJuliaPoint(Complex)));
    connect(ui->gvMandel, SIGNAL(mandelPointChanged(Complex)), this, SLOT(onChangedMandelPt(Complex)));
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
    if(idxPal < 0)
        return;
    ui->gvMandel->updatePalette(colorTable[idxPal].table());

#if _DEV_VER101
    ui->gvJulia->updatePalette(colorTable[idxPal].table());
#endif//_DEV_VER101
}

void MainWindow::initialColorCombo()
{
    double pix_width = 512; // Since the array size is 256, so the picture size need to go to a multiple of 256
    double pix_height = 23;

    for(int iPal = 0; iPal < colorTable.size(); iPal++) {
        const ColorLut& lut1 = colorTable[iPal];
        QPixmap pix(QSize(pix_width, pix_height));
        QPainter painter(&pix);
        int tableSize = lut1.size();
        double step = pix_width / tableSize;
        for(int i = 0; i < tableSize; i++) {
            QRgb col = lut1.at(i);
            QRectF source(step * i, 0, step, pix_height);
            painter.fillRect(source, col);
        }
        ui->cbPalette->addItem(QIcon(pix), lut1.name());
        ui->cbPalette->setIconSize(QSize(pix_width, pix_height));
        ui->cbPalette->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }
    updateGeometry();
}

void MainWindow::onChangedMandelPt(Complex pt)
{
    QString sMsg("%1, %2");
    QString status = QString::number(pt.real(), 'g') + ", " + QString::number(pt.imag(), 'g');
    ui->sBar->showMessage(status);
}

void MainWindow::on_cmdReset_clicked()
{
    ui->gvMandel->reset();
    ui->gvJulia->reset();
}

//EOF

