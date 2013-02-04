#include "mandelbrotcore.h"

mandelbrotcore::mandelbrotcore(struct __parameterCurrent *paraCur, int _numtasks, int _rank, QObject *parent) :
    QObject(parent)
{

    this->numtasks = _numtasks;
    this->rank = _rank;
    this->R_MODE = paraCur->R_MODE;
    this->Passes = paraCur->Passes;

    thread = new RenderThread();
    thread->setInstanceNumber(this->rank);
    thread->setNumberPasses(Passes);

    connect(thread, SIGNAL(renderedImage(QImage,double,int)),
            this, SLOT(MPI_updatePixmap(QImage,double,int)));
    connect(thread, SIGNAL(renderedDone(int,bool,int)),
            this, SLOT(MPI_renderDone(int,bool,int)));
    connect(&instance_listenMPI, SIGNAL(renderThread(double, double, double, QSize)),
            this ,SLOT(renderThread(double, double, double, QSize)));
    instance_listenMPI.start();
}

mandelbrotcore::~mandelbrotcore()
{
delete thread;
}

void mandelbrotcore::MPI_updatePixmap(const QImage &image, double scaleFactor, int instance)
{
    __protocolImage pImage;

    pImage.scaleFactor = scaleFactor;
    pImage.height = image.height();
    pImage.width  = image.width();
    pImage.size   = image.byteCount();
//    lockSend.lock();
    memcpy(this->buffer , &pImage, sizeof(__protocolImage));
    memcpy(this->buffer + sizeof(__protocolImage), image.bits() , image.byteCount());

//    qDebug() << pImage.width << pImage.height << image.byteCount()<< sizeof(__protocolImage);
//    qDebug() <<"QImage size --> "<<sizeof(__protocolImage) + image.byteCount() << "<" << MPI_BUFFER_SIZE;

    int rc = MPI_Send(buffer, sizeof(__protocolImage) + image.byteCount(), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
//    lockSend.unlock();
//    qDebug("Qimage-exit");

}

void mandelbrotcore::MPI_renderDone(int instance, bool _done, int _level)
{
    __protocolDone pDone;
    pDone.done = _done;
    pDone.level = _level;

//    lockSend.lock();
//    qDebug("renderdone");
    int rc = MPI_Send(&pDone, sizeof(pDone), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
//    lockSend.unlock();
//    qDebug("renderdone-exit %d", _level);
}

void mandelbrotcore::renderThread(double centerX, double centerY, double scaleFactor, QSize resultSize)
{
    thread->render(centerX, centerY, scaleFactor, resultSize);
}
