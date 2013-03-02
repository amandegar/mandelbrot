#include "listenmpi.h"

listenMPI::listenMPI(QObject *parent) :
    QThread(parent)
{
}

void listenMPI::run()
{
    int dest, source, rc, count;
    uchar buffer[MPI_BUFFER_SIZE];
    MPI_Status Stat;
    qDebug("MPI listen start");

    while(true)
    {
        source = -1;
        rc = MPI_Recv(buffer, MPI_BUFFER_SIZE, MPI_BYTE, source, 1, MPI_COMM_WORLD, &Stat);
        rc = MPI_Get_count(&Stat, MPI_BYTE, &count);
        source = Stat.MPI_SOURCE - 1;

        switch(buffer[0])
        {
        case 'E': //Exit
        {
            qDebug() <<"EXIT";
            exit(0);
        }
        case 'I': // Image
        {
            qDebug() <<"IMAGE from" << source;
            __protocolImage *pImage = (__protocolImage *)buffer;
            QImage teImage(buffer + sizeof(__protocolImage), pImage->width, pImage->height, QImage::Format_RGB32);
            QImage tImage = teImage.copy();
            emit renderedImage(tImage, pImage->scaleFactor, source);
            break;
        }
        case 'D': // Done
        {
            qDebug() << "Done from" << source;
            __protocolDone *pDone = (__protocolDone *)buffer;
            emit renderedDone(source, pDone->done, pDone->level);
            break;
        }
        case 'R': //Render
        {
            qDebug() << "RENDER";
            __protocolRender *pRender = (__protocolRender *)buffer;
            emit renderThread(pRender->centerX, pRender->centerY, pRender->scaleFactor, QSize(pRender->resultSize_w, pRender->resultSize_h));
            break;
        }
        default:
            qDebug("Error: unknown MPI packet received at Mandelbrot Core!");
        }
    }
}
//NOTE Protocol definition
// Render (H->N)
// Exit   (H->N)
// Image  (N->H)
// Done   (N->H)
