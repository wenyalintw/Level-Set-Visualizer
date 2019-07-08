//
// Created by 温雅 on 2019-04-15.
//

#include "MyThread.h"
#include "GACDriver.h"
#include "ImageData.h"
#include "GACParameters.h"

MyThread::MyThread(QObject *parent, GACDriver* gacDriver) : QThread(parent)
{
    myTimer = new QTimer(this);
    m_GACDriver = gacDriver;
    QObject::connect(myTimer,SIGNAL(timeout()), m_GACDriver, SLOT(run_segmentation()));
}


void MyThread::run(){
}

void MyThread::stop_evaluation() {
    myTimer->stop();
}

void MyThread::play_evaluation() {
    myTimer->start(10);
}

void MyThread::restart_evaluation() {

    m_GACDriver->reset_gacfilter();

}

