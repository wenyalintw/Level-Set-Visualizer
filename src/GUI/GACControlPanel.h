#ifndef GACControlPanel_H
#define GACControlPanel_H

#include <QDockWidget>
#include <Logic/GACDriver.h>
#include <vtkEventQtSlotConnect.h>

#include "MyThread.h"

class GACDriver;
class MyThread;


namespace Ui {
class GACControlPanel;
}

class GACControlPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit GACControlPanel(QWidget *parent = nullptr);
    explicit GACControlPanel(QWidget *parent = nullptr, GACDriver* gacDriver =  nullptr);
    ~GACControlPanel();

//private:
public:
    Ui::GACControlPanel *ui;

public:
    GACDriver* m_GACDriver = nullptr;
    MyThread* m_gacthread = nullptr;

    vtkEventQtSlotConnect* m_Connections;

//    void display_bubble();

public slots:
    void change_page();
    void on_rewindButton_clicked();
    void on_edgeButton_clicked();
    void on_pickbubbleButton_clicked();
    void on_bubbleButton_clicked();
    void on_originalBox_clicked(bool);
    void on_edgeBox_clicked(bool);

    void on_setanchorButton_clicked();
    void on_loadanchorButton_clicked();

    void on_quitButton_clicked();

    void on_playpauseButton_toggled(bool);

    void on_stepButton_clicked();

    void on_propogationSlider_valueChanged(int);
    void on_curvatureSlider_valueChanged(int);
    void on_advectionSlider_valueChanged(int);
    void on_stepspinBox_valueChanged(int);

    void on_sigmaSlider_valueChanged(int);
    void on_alphaSlider_valueChanged(int);
    void on_betaSlider_valueChanged(int);

    void on_aditerSlider_valueChanged(int);
    void on_conductanceSlider_valueChanged(int);

    void seed_insert();
    void iteration_change();
    void filter_reset();
    void show_edgeimage();

};

#endif // GACControlPanel_H
