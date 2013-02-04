#ifndef MANDELBROTCORE_H
#define MANDELBROTCORE_H

#include <QObject>
#include <QImage>
#include <mpi.h>
#include <iostream>
#include "renderthread.h"
#include "listenmpi.h"
#include "argument.h"

class mandelbrotcore : public QObject
{
    Q_OBJECT
public:
    mandelbrotcore(struct __parameterCurrent *paraCur, int _numtasks, int _rank, QObject *parent=0);
    ~mandelbrotcore();

public slots:
    void MPI_updatePixmap(const QImage &image, double scaleFactor, int instance);
    void MPI_renderDone(int instance, bool _done, int _level);
    void renderThread(double centerX, double centerY, double scaleFactor, QSize resultSize);

private:
    RenderThread *thread;
    int Passes, numtasks, rank;
    _RUNNING_MODE R_MODE;
    listenMPI instance_listenMPI;
    uchar buffer[MPI_BUFFER_SIZE];
    QMutex lockSend;
};

#endif // MANDELBROTCORE_H
