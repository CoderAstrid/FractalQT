#include "fractalrenderer.h"
#include <complex>

#include <QDebug>

FractalRenderer::FractalRenderer()
    : width(800)
    , height(600)
    , imageData(nullptr)
    , widthEx(0)
    , alivedThreads(0)
    , drawingFinished(false)
    , isStopped(false)
{
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
        imageData = new unsigned char[widthEx * height];
        change = true;
    }
    return change;
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
    for(int y=0;y<height;y++) {
        unsigned char* tmp = imageData + y * widthEx + widthFrom;
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
//EOF
