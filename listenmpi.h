#ifndef LISTENMPI_H
#define LISTENMPI_H

#include <QThread>
#include <QImage>
#include <QDebug>
#include <mpi.h>

#define MPI_BUFFER_SIZE 3500000 //3145760

class listenMPI : public QThread
{
    Q_OBJECT
public:
    listenMPI(QObject *parent = 0);
    void run();

signals:
    void renderedImage(const QImage &image, double scaleFactor, int instance);
    void renderedDone(int instance, bool done, int level);
    void renderThread(double centerX, double centerY, double scaleFactor, QSize resultSize);   
};

//---------------------------------------------------------

struct __protocolRender
{
    __protocolRender()
    {
        id = 'R';
    }
    char  id;
    double centerX;
    double centerY;
    double scaleFactor;
    int    resultSize_h;
    int    resultSize_w;
};

struct __protocolImage
{
    __protocolImage()
    {
        id = 'I';
    }
    char  id;
    double scaleFactor;
    int    size;
    int    height;
    int    width;
//    const uchar  *data;
};

struct __protocolDone
{
    __protocolDone()
{
    id = 'D';
}
    char  id;
    int    level;
    bool   done;
};

#endif // LISTENMPI_H
