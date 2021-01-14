#include "fractalrenderer.h"
#include <complex>

#include <QDebug>

static int sign(int x)
{
    return ((x > 0) - (x < 0));
}

FractalRenderer::FractalRenderer(int _w, int _h)
    : QObject()
    , width(_w)
    , height(_h)
    , imageData(nullptr)
    , widthEx(0)
    , alivedThreads(0)
    , drawingFinished(false)
    , isStopped(false)
#if _DEV_VER101
    , maxWidth(_w)
    , maxHeight(_h)
    , maxInterval(256)
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
        imageData = nullptr;
    }
}

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
#if _DEV_VER101
void FractalRenderer::move_window(int xmove, int ymove, double top, double left, double bottom, double right)
{
    if(!imageData)
        return;
    bool is_julia = (lastPoint.real() != -100);
    if(xmove == 0 && ymove == 0) {
        return;
    }
    int posx = 0,posy = 0;
    if(xmove > 0) {
        posx = width-1;
    }
    if(ymove > 0) {
        posy = height-1;
    }
    posx -= xmove;
    posy -= ymove;
    int startx = posx;

    int xstep = -sign(xmove);
    if(xstep == 0) {
        xstep = 1;
    }
    int ystep = -sign(ymove);
    if(ystep == 0) {
        ystep = 1;
    }
    while(posy+ymove >= 0 && posy+ymove < height && posy >= 0 && posy <height) {
        posx = startx;
        IndexOfPt* line = imageData + posy * widthEx;
        IndexOfPt* lineMove = imageData + (posy + ymove) * widthEx;
        while(posx+xmove >= 0 && posx+xmove < width && posx >= 0 && posx < width) {
            lineMove[posx+xmove] = line[posx];
            posx += xstep;
        }
        posy += ystep;
    }

    Complex rangeul = Complex(left,top);
    double xinterval = (right-left)/double(width);
    double yinterval = (bottom-top)/double(height);
    int ystart = 0,yend = height;
    if(ymove < 0) {
        posy = height-1;
    } else {
        posy = 0;
    }
    int ylimit = posy+ymove;
    if(ylimit < posy) {
        yend = ylimit;
        int zw = posy;
        posy = ylimit;
        ylimit = zw;
    } else {
        ystart = ylimit;
    }
    if(ymove) {
#pragma omp parallel for
        for(int y = posy;y<ylimit;y++) {
            IndexOfPt* line = imageData + widthEx * y;
            for(int x = 0;x<width;x++) {
                int res = 0;
                if(!is_julia) {
                    res =  calcPoint(Complex(0,0),rangeul+Complex(x*xinterval,y*yinterval));
                } else {
                    res = calcPoint(rangeul+Complex(x*xinterval,y*yinterval),lastPoint);
                }
                line[x] = res;
            }
        }
    }
    if(xmove < 0) {
        posx = width-1;
    } else {
        posx = 0;
    }
    int xlimit = posx + xmove;
    if(xlimit < posx) {
        int zw = posx;
        posx = xlimit;
        xlimit = zw;
    }
    if(xmove) {
#pragma omp parallel for
        for(int y = ystart;y<yend;y++) {
            IndexOfPt* line = imageData + y * widthEx;
            for(int x = posx;x<xlimit;x++) {
                int res = 0;
                if(!is_julia) {
                    res =  calcPoint(Complex(0,0),rangeul+Complex(x*xinterval,y*yinterval));
                } else {
                    res = calcPoint(rangeul+Complex(x*xinterval,y*yinterval),lastPoint);
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

inline int FractalRenderer::calcPoint(Complex start, Complex point) const
{
    Complex akt = start;
    for(uint i = 0;i<MAX_INTERATION;i++) {
        akt = akt*akt+point;
        if(abs(akt) > 1000) {
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
#endif//_DEV_VER101
//EOF
