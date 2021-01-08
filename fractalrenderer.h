#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H

#include <vector>
#include <chrono>
#include <mutex>
#include <thread>

const unsigned int PALATE_SIZE = 256;
const int PALETE_STEP = 5;
const int MAX_INTERATION = 250;

class FractalRenderer
{
public:
    FractalRenderer();
    ~FractalRenderer();
    void stop();
    bool setDimensions(int x, int y);
    void runRenderer(unsigned threads);
    bool isFinished() const
    {
        return drawingFinished;
    }
    const unsigned char* getImageData() const
    {
        return imageData;
    }
    int area() const
    {
        return widthEx * height;
    }
private:    
    int             width;    
    int             height;
    unsigned char   *imageData;
    int             widthEx;
    unsigned        alivedThreads;
    std::mutex      lock;
    std::chrono::milliseconds renderStartTime;
    bool drawingFinished;
    bool isStopped;

    unsigned char value(int x, int y);
    void render(int widthFrom, int widthTo);
};

#endif // FRACTALRENDERER_H
//EOF
