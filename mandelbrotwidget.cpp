/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include<mpi.h>

#include "mandelbrotwidget.h"

const double DefaultCenterX = -0.637011f;
const double DefaultCenterY = -0.0395159f;
const double DefaultScale = 0.00253897f;
//const double DefaultScale = 0.00403897f;

const double ZoomInFactor = 0.8f;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

MandelbrotWidget::MandelbrotWidget(struct __parameterCurrent *paraCur, QWidget *parent)
    : QWidget(parent)
{
    this->R_MODE = paraCur->R_MODE;
    this->rowMax = paraCur->rowMax;
    this->colMax = paraCur->colMax;
    this->Passes = paraCur->Passes;

    //Amir
//    QCoreApplication::arguments().at(QCoreApplication::arguments().indexOf("-type")+1);
//    QStringList x = QCoreApplication::arguments();
    //Amir

    renderingDone = new bool[rowMax * colMax];
    renderingDoneLevel = new int[rowMax * colMax];
    pixmap  = new QPixmap[rowMax * colMax];

    switch(R_MODE)
    {
    case MODE_THREAD:
        threads = new RenderThread[rowMax * colMax];
        for (int i = 0; i < rowMax * colMax ; i++)
        {
            threads[i].setInstanceNumber(i);
            threads[i].setNumberPasses(Passes);
            connect(threads+i, SIGNAL(renderedImage(QImage,double,int)),
                    this, SLOT(updatePixmap(QImage,double,int)));
            connect(threads+i, SIGNAL(renderedDone(int,bool,int)),
                    this, SLOT(renderDone(int,bool,int)));
        }
    break;
    case MODE_MPI:
        this->instance_listenMPI = new listenMPI();
        connect(instance_listenMPI, SIGNAL(renderedImage(QImage,double,int)),
                this, SLOT(updatePixmap(QImage,double,int)));
        connect(instance_listenMPI, SIGNAL(renderedDone(int,bool,int)),
                this, SLOT(renderDone(int,bool,int)));
        instance_listenMPI->start();
        break;
    case MODE_GPU:
        break;
    }

    centerX = DefaultCenterX;
    centerY = DefaultCenterY;
    pixmapScale = DefaultScale;
    curScale = DefaultScale;

    qRegisterMetaType<QImage>("QImage");

    setWindowTitle(tr("Mandelbrot - Parallel Processing"));
#ifndef QT_NO_CURSOR
    setCursor(Qt::CrossCursor);
#endif
    //    resize(550, 400);
    resize(1024, 768);
}

MandelbrotWidget::~MandelbrotWidget()
{
    if (R_MODE == MODE_MPI)
    {
        for (int i=1;i<rowMax * colMax;i++)
            MPI_Send((void *)'E', 1, MPI_BYTE, i, 1, MPI_COMM_WORLD);
        delete instance_listenMPI;
    }
    delete [] renderingDone;
    delete [] renderingDoneLevel;
    delete [] pixmap;
    delete [] threads;
}

void MandelbrotWidget::paintEvent(QPaintEvent * /* event */)
{ 
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (pixmap[0].isNull()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter,
                         tr("Rendering initial image, please wait..."));
        return;
    }

    int textWidth, pointCur;
    QRectF screen;
    QString textStat;
    QRectF wholescreen = QRectF(0, 0, this->width(), this->height()); //Amir
    QFontMetrics metrics = painter.fontMetrics();




    for (int rowCur=0; rowCur< rowMax; rowCur++) //Amir
        for (int colCur=0; colCur< colMax; colCur++) //Amir
        {
        pointCur = rowCur * colMax + colCur;
        screen = QRectF(colCur * this->width() / colMax, rowCur * this->height() / rowMax,
                        this->width() / colMax - borderThreshold , this->height() / rowMax - borderThreshold); //Amir

        if (curScale == pixmapScale) {
            //        painter.drawPixmap(pixmapOffset, pixmap);
            painter.drawPixmap(screen, pixmap[pointCur], wholescreen); //Amir
        } else {
//            double scaleFactor = pixmapScale / curScale;
//            int newWidth = int(pixmap[pointCur].width() * scaleFactor);
//            int newHeight = int(pixmap[pointCur].height() * scaleFactor);
//            int newX = pixmapOffset.x() + (pixmap[pointCur].width() - newWidth) / 2;
//            int newY = pixmapOffset.y() + (pixmap[pointCur].height() - newHeight) / 2;

            painter.save();
//            painter.translate(newX, newY);
//            painter.scale(scaleFactor, scaleFactor);
//            QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
            painter.drawPixmap(screen, pixmap[pointCur], wholescreen); //Amir
                //painter.drawPixmap(exposed, pixmap, exposed);
            painter.restore();
        }

        if (renderingDone[pointCur])
        {
            textStat = tr("Process Done");
            textWidth = metrics.width(textStat);
        }
        else
        {
            textStat = tr("Process Pass ") + QString(renderingDoneLevel[pointCur]+0x30);
            textWidth = metrics.width(textStat);
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 127));
        painter.drawRect(screen.x(),
                         screen.y() + screen.height() -20,
                         100,
                         20);
        painter.setPen(Qt::white);
        painter.drawText(screen.x() + 3,
                         screen.y() + screen.height() - 5,
                         textStat);
    }
    QString text = tr("Use mouse wheel or the '+' and '-' keys to zoom. "
                      "Press and hold left mouse button to scroll.");
//    QFontMetrics metrics = painter.fontMetrics();
    textWidth = metrics.width(text);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 127));
    painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10,
                     metrics.lineSpacing() + 5);
    painter.setPen(Qt::white);
    painter.drawText((width() - textWidth) / 2,
                     metrics.leading() + metrics.ascent(), text);
}

void MandelbrotWidget::resizeEvent(QResizeEvent * /* event */)
{
    for (int i = 0; i< rowMax * colMax ; i++)
        this->renderWrapper(centerX, centerY, curScale, size(), i);
//        threads[i].render(centerX, centerY, curScale, size());
}

void MandelbrotWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoom(ZoomInFactor);
        break;
    case Qt::Key_Minus:
        zoom(ZoomOutFactor);
        break;
    case Qt::Key_Left:
        scroll(-ScrollStep, 0);
        break;
    case Qt::Key_Right:
        scroll(+ScrollStep, 0);
        break;
    case Qt::Key_Down:
        scroll(0, -ScrollStep);
        break;
    case Qt::Key_Up:
        scroll(0, +ScrollStep);
        break;
    case Qt::Key_0:
        speedCall(-0.637011f, -0.0395159f, 0.00253897f);
        break;
    case Qt::Key_1:
        speedCall(-1.40, 0.00099, 0.00040);
        break;
    case Qt::Key_2:
        speedCall(-1.40, 0.00050, 0.000009);
        break;
    case Qt::Key_3:
        speedCall(0.20001, -0.5588, 0.0000119);
        break;
    case Qt::Key_4:
        speedCall(-0.7721, 0.1137, 0.0000971);
        break;
    case Qt::Key_5:
        speedCall(-1.36, -0.017, 0.0000461);
        break;
    case Qt::Key_6:
        speedCall(-1.390, 0.0140, 0.0000096);
        break;
    case Qt::Key_7:
        speedCall(-0.6495, 0.3623, 0.0000234);
        break;
    case Qt::Key_9:
        variaty();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void MandelbrotWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15.0f;
    zoom(pow(ZoomInFactor, numSteps));
}

void MandelbrotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        lastDragPos = event->pos();
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = event->pos();
        update();
    }
}

void MandelbrotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = QPoint();

        int deltaX = (width() - pixmap[0].width()) / 2 - pixmapOffset.x();
        int deltaY = (height() - pixmap[0].height()) / 2 - pixmapOffset.y();
        scroll(deltaX, deltaY);
    }
}

void MandelbrotWidget::updatePixmap(const QImage &image, double scaleFactor, int instance)
{
    if (!lastDragPos.isNull())
        return;

    pixmap[instance] = QPixmap::fromImage(image);
    pixmapOffset = QPoint();
    lastDragPos = QPoint();
    pixmapScale = scaleFactor;
    update();
}

void MandelbrotWidget::renderDone(int instance, bool _done, int _level)
{
    renderingDone[instance] = _done;
    renderingDoneLevel[instance] = _level;
    update();
}

void MandelbrotWidget::zoom(double zoomFactor)
{
    curScale *= zoomFactor;
    update();
    for (int i = 0; i< rowMax * colMax ; i++)
        this->renderWrapper(centerX, centerY, curScale, size(), i);
//        threads[i].render(centerX, centerY, curScale, size());
}

void MandelbrotWidget::scroll(int deltaX, int deltaY)
{
    centerX += deltaX * curScale;
    centerY += deltaY * curScale;
    update();
    for (int i = 0; i< rowMax * colMax ; i++)
        this->renderWrapper(centerX, centerY, curScale, size(), i);
//        threads[i].render(centerX, centerY, curScale, size());
}

void MandelbrotWidget::speedCall(float _x, float _y, float _scale)
{
    centerX = _x;
    centerY = _y;
    curScale = _scale;

    for (int i = 0; i< rowMax * colMax ; i++)
        this->renderWrapper(centerX, centerY, curScale, size(), i);
//        threads[i].render(centerX, centerY, curScale, size());
}

void MandelbrotWidget::variaty()
{
    for (int i = 0; i< rowMax * colMax ; i++)
        switch(i)
        {
        case 0:
            this->renderWrapper(-0.637011f, -0.0395159f, 0.00253897f, size(), i);
//            threads[i].render(-0.637011f, -0.0395159f, 0.00253897f, size());
        break;
        case 1:
            this->renderWrapper(-0.7721, 0.1137, 0.0000171, size(), i);
//            threads[i].render(-0.7721, 0.1137, 0.0000171, size());
        break;
        case 2:
            this->renderWrapper(-1.40, 0.00050, 0.000009, size(), i);
//            threads[i].render(-1.40, 0.00050, 0.000009, size());
        break;
        case 3:
            this->renderWrapper(0.20001, -0.5588, 0.0000119, size(), i);
//            threads[i].render(0.20001, -0.5588, 0.0000119, size());
        break;
        case 4:
            this->renderWrapper(-1.390, 0.0140, 0.0000096, size(), i);
//            threads[i].render(-1.390, 0.0140, 0.0000096, size());
        break;
        case 5:
            this->renderWrapper(-1.36, -0.017, 0.0000461, size(), i);
//            threads[i].render(-1.36, -0.017, 0.0000461, size());
        break;
        case 6:
            this->renderWrapper(-0.7721, 0.1137, 0.0000971, size(), i);
//            threads[i].render(-0.7721, 0.1137, 0.0000971, size());
        break;
        case 7:
            this->renderWrapper(-0.6495, 0.3623, 0.0000234, size(), i);
//            threads[i].render(-0.6495, 0.3623, 0.0000234, size());
        break;
        case 8:
            this->renderWrapper(-1.40, 0.00099, 0.00040, size(), i);
//            threads[i].render(-1.40, 0.00099, 0.00040, size());
        break;
        case 9:
            this->renderWrapper(-0.7721, 0.1137, 0.0000971, size(), i);
//            threads[i].render(-0.7721, 0.1137, 0.0000971, size());
        break;

        }

}

void MandelbrotWidget::renderWrapper(double _centerX, double _centerY, double _scaleFactor,
                   QSize _resultSize, int _instance)
{
    this->Lock.lock();
    switch(this->R_MODE){
    case MODE_THREAD:
        threads[_instance].render(_centerX, _centerY, _scaleFactor, _resultSize);
        break;
    case MODE_MPI:
    {
        __protocolRender pRender;
        pRender.centerX = _centerX;
        pRender.centerY = _centerY;
        pRender.scaleFactor = _scaleFactor;
        pRender.resultSize_h = _resultSize.height();
        pRender.resultSize_w = _resultSize.width();
        int rc = MPI_Send(&pRender, sizeof(pRender), MPI_BYTE, _instance+1, 1, MPI_COMM_WORLD);
        break;
    }
    case MODE_GPU:
        qDebug("Not yet defined?");
        break;
    }
    this->Lock.unlock();
}
