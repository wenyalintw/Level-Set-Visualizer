//
// Created by 温雅 on 2019-04-15.
//

#ifndef GACQVTK_MYTHREAD_H
#define GACQVTK_MYTHREAD_H

#include <QThread>
#include <QTimer>

#include <itkImage.h>

class GACDriver;

class MyThread : public QThread{
    Q_OBJECT

public:
    MyThread(QObject *parent = 0, GACDriver* = 0);
    void run();
    void stop_evaluation();
    void play_evaluation();
    void restart_evaluation();

signals:

public slots:


public:
    bool STOP{false};
    QTimer* myTimer = nullptr;
    GACDriver* m_GACDriver = nullptr;

public:
    static const unsigned int Dimension = 2;
    using UCPixelType = unsigned char;
    using UCImageType = itk::Image< UCPixelType, Dimension >;
    using FloatPixelType = float;
    using FloatImageType = itk::Image< FloatPixelType, Dimension >;

};


#endif //GACQVTK_MYTHREAD_H
