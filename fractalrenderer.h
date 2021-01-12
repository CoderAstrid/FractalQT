#ifndef FRACTALRENDERER_H
#define FRACTALRENDERER_H

#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <QObject>

const unsigned int PALATE_SIZE = 256;
const int PALETE_STEP = 5;
const int MAX_INTERATION = 250;

class FractalRenderer : public QObject
{
    Q_OBJECT
public:
    FractalRenderer();
    virtual ~FractalRenderer();
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
    void updateJulia(int x, int y);
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
signals:
    void doneUpdate();
};

#endif // FRACTALRENDERER_H
//EOF
