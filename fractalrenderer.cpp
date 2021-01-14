#include "fractalrenderer.h"
#include <complex>

#if _DEV_QT
#   include <QDebug>
#endif//_DEV_QT

static int sign(int x)
{
    return ((x > 0) - (x < 0));
}

FractalRenderer::FractalRenderer(int _w, int _h)
    : QObject()
    , width(_w)
    , height(_h)
    , imageData(NULL_ADD)
    , widthEx(0)
#if _DEV_VER101
    , maxWidth(_w)
    , maxHeight(_h)
    , maxInterval(256)
#else
    , alivedThreads(0)
    , drawingFinished(false)
    , isStopped(false)
#endif//_DEV_VER101
{
#if _DEV_VER101
    lastPoint.real(-100);
    lastPoint.imag(-100);
    resize(_w, _h);
#else
    if(_w > 0 && _h > 0)
        setDimensions(_w, _h);
#endif//_DEV_VER101
}

FractalRenderer::~FractalRenderer()
{
    if(imageData) {
        delete[] imageData;
        imageData = NULL_ADD;
    }
}

#if _DEV_VER101
void FractalRenderer::moveProcess(int dx, int dy, double y0, double x0, double y1, double x1)
{
    if(!imageData)
        return;
    bool is_julia = (lastPoint.real() != -100);
    if(dx == 0 && dy == 0) {
        return;
    }
    int px = 0, py = 0;
    if(dx > 0) {
        px = width-1;
    }
    if(dy > 0) {
        py = height-1;
    }
    px -= dx;
    py -= dy;
    int sx = px;

    int xstep = -sign(dx);
    if(xstep == 0) {
        xstep = 1;
    }
    int ystep = -sign(dy);
    if(ystep == 0) {
        ystep = 1;
    }
    while(py+dy >= 0 && py+dy < height && py >= 0 && py <height) {
        px = sx;
        IndexOfPt* line = imageData + py * widthEx;
        IndexOfPt* lineMove = imageData + (py + dy) * widthEx;
        while(px+dx >= 0 && px+dx < width && px >= 0 && px < width) {
            lineMove[px+dx] = line[px];
            px += xstep;
        }
        py += ystep;
    }

    Complex keyPt = Complex(x0,y0);
    double realDx = (x1-x0)/double(width);
    double realDy = (y1-y0)/double(height);
    int sy = 0, ey = height;
    if(dy < 0) {
        py = height-1;
    } else {
        py = 0;
    }
    int yLimit = py+dy;
    if(yLimit < py) {
        ey = yLimit;
        int zw = py;
        py = yLimit;
        yLimit = zw;
    } else {
        sy = yLimit;
    }
    if(dy) {
#pragma omp parallel for
        for(int y = py;y<yLimit;y++) {
            IndexOfPt* line = imageData + widthEx * y;
            for(int x = 0;x<width;x++) {
                int res = 0;
                if(!is_julia) {
                    res =  calcPoint(Complex(0,0), keyPt + Complex(x*realDx, y*realDy));
                } else {
                    res = calcPoint(keyPt+Complex(x*realDx,y*realDy), lastPoint);
                }
                line[x] = res;
            }
        }
    }
    if(dx < 0) {
        px = width-1;
    } else {
        px = 0;
    }
    int xLimit = px + dx;
    if(xLimit < px) {
        int zw = px;
        px = xLimit;
        xLimit = zw;
    }
    if(dx) {
#pragma omp parallel for
        for(int y = sy;y<ey;y++) {
            IndexOfPt* line = imageData + y * widthEx;
            for(int x = px;x<xLimit;x++) {
                int res = 0;
                if(!is_julia) {
                    res =  calcPoint(Complex(0,0), keyPt+Complex(x*realDx,y*realDy));
                } else {
                    res = calcPoint(keyPt+Complex(x*realDx,y*realDy), lastPoint);
                }
                line[x] = res;
            }
        }
    }
}

void FractalRenderer::resize(int newx, int newy)
{
    if(newx > 0 && newy > 0 && (newx != width || newy != height)) {       
        width = newx; height = newy;
        widthEx = ((((width * 8) + 31) & ~31) >> 3);
        if(imageData) {
            delete[] imageData;
        }
        imageData = new IndexOfPt[widthEx * height];
    }
}

inline int FractalRenderer::calcPoint(Complex start, Complex c) const
{
    Complex z = start;
    for(int i = 0;i<MAX_INTERATION;i++) {
        z = z*z+c;
        if(abs(z) > 1000) {
            return i;
        }
    }
    return MAX_INTERATION;
}

inline Complex FractalRenderer::mandelFunc(Complex z, Complex c) const
{
    return z * z + c;
}

void FractalRenderer::renderMandelbrot(double left, double top, double right, double bottom)
{
    if(!imageData)
        return;
    lastPoint = Complex(-100, -100);
    Complex rangeul = Complex(left, top);
    double xinterval = (right-left) / double(width);
    double yinterval = (bottom-top) / double(height);
#pragma omp parallel for
    for(int y = 0;y<height;y++) {
        for(int x = 0;x<width;x++) {
            int res = calcPoint(Complex(0,0), rangeul+Complex(x*xinterval, y*yinterval));
            imageData[y * widthEx + x] = res;
        }
    }
}

void FractalRenderer::renderJulia(Complex c, double left, double top, double right, double bottom)
{
    if(!imageData)
        return;
    lastPoint = c;
    Complex rangeul = Complex(left, top);
    double xinterval = (right-left) / double(width);
    double yinterval = (bottom-top) / double(height);
#pragma omp parallel for
    for(int y = 0;y<height;y++) {
        for(int x = 0;x<width;x++) {
            int res = calcPoint(rangeul + Complex(x*xinterval, y*yinterval), c);
            imageData[y * widthEx + x] = res;
        }
    }
}

void FractalRenderer::renderMandelbrot()
{
    renderMandelbrot(INIT_LEFT, INIT_TOP, INIT_RIGHT, INIT_BOTTOM);
}
#else
bool FractalRenderer::setDimensions(int _w, int _h)
{
    bool change = false;
    if(_w > 0 && _h > 0 && (_w != width || _h != height)) {
        if(!isStopped) {
            stop();
        }
        width = _w; height = _h;
        widthEx = ((((width * 8) + 31) & ~31) >> 3);
        if(imageData) {
            delete[] imageData;
        }
        imageData = new IndexOfPt[widthEx * height];
        change = true;
    }
    return change;
}

void FractalRenderer::updateJulia(int _x, int _y)
{

}

void FractalRenderer::runRenderer(unsigned threadsCount)
{
    if(threadsCount==0){threadsCount=1;}

    if(isStopped)
    {
        qDebug()<<"Reseting...";
        drawingFinished=false;
        isStopped=false;
    }
    qDebug()<<"Starting"<<threadsCount<<"threads";
    renderStartTime=std::chrono::duration_cast< std::chrono::milliseconds >
            (std::chrono::system_clock::now().time_since_epoch());

    unsigned widthPart=width/threadsCount;
    std::vector<std::thread> workers(threadsCount);

   for(unsigned i=0;i<threadsCount;i++) {
       workers[i] = std::thread(&FractalRenderer::render, this,
                                i*widthPart,
                                (i+1)*widthPart);
       alivedThreads++;
       workers[i].detach();
   }
}

void FractalRenderer::render(int widthFrom, int widthTo)
{
    if(!imageData)
        return;
    for(int y=0;y<height;y++) {
        IndexOfPt* tmp = imageData + y * widthEx + widthFrom;
        for(int x=widthFrom;x<widthTo;x++, tmp++) {
            *tmp = value(x,y);
            if(isStopped) {
                break;
            }
        }
        if(isStopped) {
            break;
        }
     }

    lock.lock();
    alivedThreads--;

    if(alivedThreads==0)
    {
        std::chrono::milliseconds renderEndTime=std::chrono::duration_cast
        <std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
        renderEndTime-=renderStartTime;
        drawingFinished=true;
        qDebug()<<"Finished in"<<renderEndTime.count()<<"ms";
        emit doneUpdate();
    }
    lock.unlock();
}

unsigned char FractalRenderer::value(int x, int y)
{
    std::complex<float> point((float)x/width * 2-1.5, (float)y/height*2-1);
    std::complex<float> z(0, 0);
    unsigned char iterations = 0;

    while (abs(z) < 2 && iterations < MAX_INTERATION)
    {
        z = z * z + point;
        iterations++;
    }

    if (iterations < MAX_INTERATION)
    {
        return iterations;
    }
    else
    {
        return 0;
    }
}

void FractalRenderer::stop()
{
    isStopped=true;
    while(alivedThreads>0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
#endif//_DEV_VER101
//EOF
