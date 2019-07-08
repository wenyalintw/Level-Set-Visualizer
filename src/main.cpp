#include "mainwindow.h"
#include <QApplication>

#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include <QScreen>

int main(int argc, char *argv[])
{
    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication a(argc, argv);



    QScreen *screen = a.primaryScreen();
//    QFont f = a.font();
//    int pixelSize = (f.pointSize() * screen->logicalDotsPerInch()) / 72;
//    double bb = screen->devicePixelRatio();
//    std::cout << bb;
//    f.setPointSize(25);
//    a.setFont(f);
//    int newPixelSize = (f.pointSize() * screen->logicalDotsPerInch()) / 72;




    MainWindow w;
    w.show();

    return a.exec();
}