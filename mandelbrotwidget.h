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

#ifndef MANDELBROTWIDGET_H
#define MANDELBROTWIDGET_H

#include <QPixmap>
#include <QWidget>
#include <QQueue>

#include "renderthread.h"
#include "listenmpi.h"
#include "argument.h"

class MandelbrotWidget : public QWidget
{
    Q_OBJECT

public:
    MandelbrotWidget(struct __parameterCurrent *paraCur, QWidget *parent = 0);
    ~MandelbrotWidget();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void updatePixmap(const QImage &image, double scaleFactor, int instance);
    void renderDone(int instance, bool _done, int _level);

private:
    void zoom(double zoomFactor);
    void scroll(int deltaX, int deltaY);
    void speedCall(float _x, float _y, float _scale);
    void variaty();
    void MPI_startListen();
    void renderWrapper(double _centerX, double _centerY, double _scaleFactor,
                       QSize _resultSize, int _instance);


    enum {borderThreshold = 2};

    struct _instance{
        RenderThread *thread;
        QPixmap pixmap;
        bool renderingDone;
        int  renderingDoneLevel;
        double centerX;
        double centerY;
        double pixmapScale;
        double curScale;
    } *instances;

    QPoint pixmapOffset;
    QPoint lastDragPos;
//    QQueue<int> renderedInstance;
    QMutex  mutexQueue;
    int rowMax, colMax, Passes;
    _RUNNING_MODE R_MODE;
    QMutex  Lock;
    listenMPI *instance_listenMPI;

};

#endif
