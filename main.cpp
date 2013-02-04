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

#include <QApplication>
#include <QTimer>

#include "mandelbrotwidget.h"
#include "mandelbrotcore.h"
#include "argument.h"

void processArguments(int argc, char *argv[], struct __parameterCurrent *paramCurrent)
{
    //Parameters row, col, pass, process(thread,mpi,gpu)
    paramCurrent->rowMax =2;
    paramCurrent->colMax =2;
    paramCurrent->Passes =4;
    paramCurrent->R_MODE = MODE_THREAD;

    int c;
    while((c = getopt(argc, argv, "r:c:p:m:t:h?")) != -1)
    {
        switch(c)
        {
        case 'r':
            paramCurrent->rowMax = atoi(optarg);
            break;
        case 'c':
            paramCurrent->colMax = atoi(optarg);
            break;
        case 'p':
            paramCurrent->Passes = atoi(optarg);
            break;
        case 'm':
            if (!strcmp(optarg,"thread"))
                paramCurrent->R_MODE = MODE_THREAD;
            else if (!strcmp(optarg,"mpi"))
                paramCurrent->R_MODE = MODE_MPI;
            else if (!strcmp(optarg,"gpu"))
                paramCurrent->R_MODE = MODE_GPU;
            break;
        case '?':
        case 'h':
        default :
            std::cout << "Mandelbrot parallel processing application(Version: 8 Jan 2013)" <<std::endl;
            std::cout << "Expanded by Amir Hossein Mandegar<amandegar@computer.org>" <<std::endl;
            std::cout << "Usage: mandelbrot [OPTION]"<<std::endl;
            std::cout << " -r [NUMBER]   Number of instance in row(default=2)" <<std::endl;
            std::cout << " -c [NUMBER]   Number of instance in column(default=2)" <<std::endl;
            std::cout << " -p [NUMBER]   Number of mandelrot passes level(default=4)" <<std::endl;
            std::cout << " -m [STRING]   Processing mod. thread(default), mpi, gpu" <<std::endl;
            std::cout << std::endl;
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    int ret;
    struct __parameterCurrent paramCur;

    processArguments(argc, argv, &paramCur);
    switch(paramCur.R_MODE)
    {
    case MODE_MPI:
        int numtasks, rank, init_provided;

//        MPI_Init(&argc,&argv);
        MPI_Init_thread(&argc, &argv,MPI_THREAD_MULTIPLE,&init_provided);
        if (init_provided != MPI_THREAD_MULTIPLE)
        {
            qDebug() << "MPI_THREAD_MULTIPLE not set.";
            exit(1);
        }
        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank != 0)
        {
            QCoreApplication app(argc, argv);
            mandelbrotcore mand(&paramCur, numtasks, rank);
            ret = app.exec();
            MPI_Finalize();
            return ret;
        }
    case MODE_GPU:
        break;
    }
    QApplication app(argc, argv);
    MandelbrotWidget mand(&paramCur);
    mand.show();
    ret = app.exec();
    if (paramCur.R_MODE) MPI_Finalize();
    return ret;

}
