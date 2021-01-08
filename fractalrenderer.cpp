#include "fractalrenderer.h"
#include <complex>

#include <QDebug>

FractalRenderer::FractalRenderer()
{
    width=800;
    height=600;
    threadsAlive=0;
    drawingFinished=false;
    isStopped=false;
    std::vector<std::vector<unsigned> > temp(width, std::vector<unsigned> (height, 0));
    imageData = temp;
}

void FractalRenderer::setDimensions(unsigned x, unsigned y)
{
    width=x; height=y;
    std::vector<std::vector<unsigned> > temp(width, std::vector<unsigned> (height, 0));
    imageData = temp;
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

   for(unsigned i=0;i<threadsCount;i++)
   {
       workers[i] = std::thread(&FractalRenderer::render, this,
                                i*widthPart,
                                (i+1)*widthPart);
       threadsAlive++;
       workers[i].detach();
   }
}

void FractalRenderer::render(unsigned widthFrom, unsigned widthTo)
{
    for(unsigned x=widthFrom;x<widthTo;x++)
    {
        for(unsigned y=0;y<height;y++)
        {
            imageData[x][y]=value(x,y);
            if(isStopped){break;}
        }
        if(isStopped){break;}
     }

    lock.lock();
    threadsAlive--;

    if(threadsAlive==0)
    {
        std::chrono::milliseconds renderEndTime=std::chrono::duration_cast
        <std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
        renderEndTime-=renderStartTime;
        drawingFinished=true;
        qDebug()<<"Finished in"<<renderEndTime.count()<<"ms";
    }
    lock.unlock();
}

unsigned FractalRenderer::value(int x, int y)
{
    std::complex<float> point((float)x/width-1.5, (float)y/height-0.5);
    std::complex<float> z(0, 0);
    unsigned iterations = 0;

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
    while(threadsAlive>0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
//EOF
