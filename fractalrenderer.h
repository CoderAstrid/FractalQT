#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H

#include <vector>
#include <chrono>
#include <mutex>
#include <thread>

const unsigned int PALATE_SIZE = 256;
const int PALETE_STEP = 5;
const int MAX_INTERATION = 50;

class FractalRenderer
{
public:
    FractalRenderer();

    void stop();
    void setDimensions(unsigned x,unsigned y);
    void runRenderer(unsigned threads);
    bool isFinished(){return drawingFinished;}
    std::vector< std::vector<unsigned> > getImageData(){return imageData;}
private:
    std::vector< std::vector<unsigned> > imageData;
    unsigned width;
    unsigned height;
    unsigned threadsAlive;
    std::mutex lock;
    std::chrono::milliseconds renderStartTime;
    bool drawingFinished;
    bool isStopped;

    unsigned value(int x, int y);
    void render(unsigned widthFrom, unsigned widthTo);
};

#endif // FRACTALRENDERER_H
//EOF
