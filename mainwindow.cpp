#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>

const int INT_DRAW_TIME = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    mandelbrot.setDimensions(this->width(),this->height());
    mandelbrot.runRenderer(std::thread::hardware_concurrency());

    // init gray palete step( 7 * 34 = 238 < 255 )
    rMult = PALETE_STEP;
    gMult = PALETE_STEP;
    bMult = PALETE_STEP;

    // timer for init drawing
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer->start(INT_DRAW_TIME);

    ui->setupUi(this);
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
    imageData=mandelbrot.getImageData();

    QPainter painter(this);

    QImage img_fractal(this->width(),this->height(),QImage::Format_RGB32);
    for(int x=0; x<this->width(); x++) {
        for(int y=0; y<this->height(); y++) {
            img_fractal.setPixel(x,y,qRgb((imageData[x][y]*rMult)%PALATE_SIZE,
                                      (imageData[x][y]*gMult)%PALATE_SIZE,
                                      (imageData[x][y]*bMult)%PALATE_SIZE));
        }
    }

    painter.drawImage(QPoint(0,0), img_fractal);
}

void MainWindow::resizeEvent(QResizeEvent * /*event*/)
{
    mandelbrot.stop();
    mandelbrot.setDimensions(this->width(),this->height());
    mandelbrot.runRenderer(std::thread::hardware_concurrency());

    if(updateTimer->isActive()==false){updateTimer->start(INT_DRAW_TIME);}
}
//EOF
