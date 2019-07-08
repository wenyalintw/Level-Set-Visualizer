#include "GACControlPanel.h"
#include "ui_GACControlPanel.h"

#include "GACDriver.h"
#include "GACParameters.h"
#include "MyThread.h"
#include "ImageData.h"

#include <QDebug>
#include <QMessageBox>
#include <QDir>

GACControlPanel::GACControlPanel(QWidget *parent) :
        QDockWidget(parent),
        ui(new Ui::GACControlPanel)
{
    ui->setupUi(this);
}

GACControlPanel::GACControlPanel(QWidget *parent, GACDriver* gacDriver) :
        QDockWidget(parent),
        ui(new Ui::GACControlPanel)
{
    ui->setupUi(this);
    m_GACDriver = gacDriver;

    QString::number(m_GACDriver->m_gacparameters->m_propagationScaling);

    ui->propogationSlider->setValue(int(m_GACDriver->m_gacparameters->m_propagationScaling*2));
    ui->curvatureSlider->setValue(int(m_GACDriver->m_gacparameters->m_curvatureScaling*2));
    ui->advectionSlider->setValue(int(m_GACDriver->m_gacparameters->m_advectionScaling*2));

    ui->propogationEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_propagationScaling));
    ui->curvatureEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_curvatureScaling));
    ui->advectionEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_advectionScaling));

    connect(ui->stepList, SIGNAL(itemSelectionChanged()), this, SLOT(change_page()));

    connect(m_GACDriver, SIGNAL(iterationChange()), this, SLOT(iteration_change()));

    connect(m_GACDriver, SIGNAL(filterReset()), this, SLOT(filter_reset()));

    ui->stepList->setCurrentRow(0);
    // 讓x,y填滿widget
    ui->bubbleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->rewindButton->setIcon(QIcon(qApp->applicationDirPath() + "/resources/iconfinder_rewind_2561378.svg"));
    ui->playpauseButton->setIcon(QIcon(qApp->applicationDirPath() + "/resources/iconfinder_play_2561370.svg"));
    ui->stepButton->setIcon(QIcon(qApp->applicationDirPath() + "/resources/iconfinder_skip-forward_2561275.svg"));

    ui->sigmaSlider->setValue(int(m_GACDriver->m_gacparameters->m_sigma*10));
    ui->alphaSlider->setValue(int(m_GACDriver->m_gacparameters->m_alpha*10));
    ui->betaSlider->setValue(int(m_GACDriver->m_gacparameters->m_beta*10));
    ui->sigmaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_sigma));
    ui->alphaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_alpha));
    ui->betaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_beta));


    ui->aditerSlider->setValue(int(m_GACDriver->m_gacparameters->m_aditer));
    ui->conductanceSlider->setValue(int(m_GACDriver->m_gacparameters->m_conductance));
    ui->aditerEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_aditer));
    ui->conductanceEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_conductance));

    show_edgeimage();
}

void GACControlPanel::filter_reset() {
    ui->loadanchorButton->setEnabled(false);
    ui->loadanchorButton->setText("No Anchor Iteration");
}

void GACControlPanel::iteration_change() {
    ui->iterationEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_anchorIter +
    m_GACDriver->geodesicActiveContour->GetElapsedIterations()));
}

void GACControlPanel::change_page() {

    int index = ui->stepList->currentIndex().row();
    ui->stepStack->setCurrentIndex(index);
}


GACControlPanel::~GACControlPanel()
{
    delete ui;
}


void GACControlPanel::on_stepspinBox_valueChanged(int stepsize) {
    m_GACDriver->m_stepsize = stepsize;
}

void GACControlPanel::on_aditerSlider_valueChanged(int x) {
    m_GACDriver->m_gacparameters->m_aditer = x;
    ui->aditerEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_aditer));
    show_edgeimage();
}

void GACControlPanel::on_conductanceSlider_valueChanged(int x) {
    m_GACDriver->m_gacparameters->m_conductance = x;
    ui->conductanceEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_conductance));
    show_edgeimage();
}

void GACControlPanel::on_sigmaSlider_valueChanged(int x) {
    // 因Slider都是整數，這邊除以10以0.1為一單位
    m_GACDriver->m_gacparameters->m_sigma = x/10.;
    ui->sigmaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_sigma));
    show_edgeimage();
}

void GACControlPanel::on_alphaSlider_valueChanged(int x) {
    m_GACDriver->m_gacparameters->m_alpha = x/10.;
    ui->alphaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_alpha));
    show_edgeimage();
}

void GACControlPanel::on_betaSlider_valueChanged(int x) {
    m_GACDriver->m_gacparameters->m_beta = x/10.;
    ui->betaEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_beta));
    show_edgeimage();
}

void GACControlPanel::on_propogationSlider_valueChanged(int x) {
    // 因Slider都是整數，這邊除以2以0.5為一單位
    m_GACDriver->m_gacparameters->m_propagationScaling = x/2.;
    m_GACDriver->set_gac_parameter();
    ui->propogationEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_propagationScaling));
}


void GACControlPanel::on_curvatureSlider_valueChanged(int x ) {
    m_GACDriver->m_gacparameters->m_curvatureScaling = x/2.;
    m_GACDriver->set_gac_parameter();
    ui->curvatureEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_curvatureScaling));
}

void GACControlPanel::on_advectionSlider_valueChanged(int x) {
    m_GACDriver->m_gacparameters->m_advectionScaling = x/2.;
    m_GACDriver->set_gac_parameter();
    ui->advectionEdit->setText(QString::number(m_GACDriver->m_gacparameters->m_advectionScaling));
}

void GACControlPanel::on_playpauseButton_toggled(bool status){
    if (status){
        if(!m_gacthread){
            m_GACDriver->initial_gacfilter();
            m_GACDriver->threshold_filtering();
            m_gacthread = new MyThread(this, m_GACDriver);
        }
        m_gacthread->play_evaluation();
        ui->playpauseButton->setIcon(QIcon(qApp->applicationDirPath() + "/resources/iconfinder_pause_2561367.svg"));
    }
    else{
        // 因按下stop到響應整個停下來，大概還會讓程式偷跑2~3個iter，所以要呼叫iteration_changed更新到最新狀態，並顯示
        m_gacthread->stop_evaluation();
        iteration_change();
        m_GACDriver->display_update();
        m_GACDriver->m_stepanchor = m_GACDriver->geodesicActiveContour->GetElapsedIterations();
        ui->playpauseButton->setIcon(QIcon(qApp->applicationDirPath() + "/resources/iconfinder_play_2561370.svg"));
    }
}

void GACControlPanel::on_rewindButton_clicked(){
    m_GACDriver->reset_gacfilter();
}

void GACControlPanel::on_stepButton_clicked(){
    if(!m_gacthread){
        m_GACDriver->initial_gacfilter();
        m_GACDriver->threshold_filtering();
        m_gacthread = new MyThread(this, m_GACDriver);
    }
    // check if it is playing
    if (ui->playpauseButton->isChecked()){
        ui->playpauseButton->click();
    }
    for (int i=0; i<m_GACDriver->m_stepsize; i++){
        m_GACDriver->run_segmentation();
    }
}


void GACControlPanel::show_edgeimage(){
    m_GACDriver->set_edge_parameters();
    auto inputimage = m_GACDriver->m_imagedata->unsignedchar2float(m_GACDriver->m_imagedata->m_readimage);
    m_GACDriver->smoothing_filtering(inputimage);
    m_GACDriver->gradient_filtering(m_GACDriver->m_gacparameters->m_smoothingImage);
    m_GACDriver->sigmoid_filtering(m_GACDriver->m_gacparameters->m_gradientImage);
}

void GACControlPanel::on_edgeButton_clicked(){
    // 移到下一頁
    ui->stepList->setCurrentRow(1);
}

void GACControlPanel::on_pickbubbleButton_clicked(){

    QMessageBox::information(this, tr("instruction"), tr("Click On Image to Specify Initial Contour"), QMessageBox::Ok);
//    m_GACDriver->m_mainwindow->setCursor(Qt::CrossCursor);
    m_GACDriver->m_mainwindow->ui->qvtkopenglWidget->setCursor(Qt::CrossCursor);

    m_Connections = vtkEventQtSlotConnect::New();
    m_Connections->Connect(m_GACDriver->m_mainwindow->ui->qvtkopenglWidget->GetRenderWindow()->GetInteractor(),
                           vtkCommand::LeftButtonPressEvent,
                           m_GACDriver,
                           SLOT(seed_record()));
    connect(m_GACDriver, SIGNAL(seedClicked()), this, SLOT(seed_insert()));

    m_GACDriver->initial_bubble_image();
//    ui->edgeBox->click();
}

void GACControlPanel::seed_insert(){
    ui->bubbleTable->insertRow(ui->bubbleTable->rowCount());
    int x = m_GACDriver->m_gacparameters->m_seedPosX;
    int y = m_GACDriver->m_gacparameters->m_seedPosY;
    ui->bubbleTable->setItem(ui->bubbleTable->rowCount()-1,0,
            new QTableWidgetItem(QString::number(x)));
    ui->bubbleTable->setItem(ui->bubbleTable->rowCount()-1,1,
            new QTableWidgetItem(QString::number(y)));

    m_GACDriver->create_bubble_image();

    if (ui->bubbleTable->rowCount() == 1){
        ui->edgeBox->click();
        ui->bubbleButton->setEnabled(true);
        m_GACDriver->create_bubble_image();
    }
}


void GACControlPanel::on_bubbleButton_clicked() {
    auto inputimage = m_GACDriver->m_imagedata->unsignedchar2float(m_GACDriver->m_imagedata->m_readimage);

    const int bubble_number = ui->bubbleTable->rowCount();

    m_GACDriver->node_list.resize(bubble_number);

    for (int i = 0; i<m_GACDriver->node_list.size(); i++){
        m_GACDriver->seedPosition[0] = ui->bubbleTable->item(i,0)->text().toInt();
        m_GACDriver->seedPosition[1] = ui->bubbleTable->item(i,1)->text().toInt();
        m_GACDriver->node_list[i].SetValue(m_GACDriver->m_gacparameters->m_seedValue);
        m_GACDriver->node_list[i].SetIndex(m_GACDriver->seedPosition);
    }

    m_GACDriver->fastmarching_filtering(inputimage);
    ui->stepList->setCurrentRow(2);

    m_Connections->Disconnect(m_GACDriver->m_mainwindow->ui->qvtkopenglWidget->GetRenderWindow()->GetInteractor(),
                              vtkCommand::LeftButtonPressEvent,
                              m_GACDriver,
                              SLOT(seed_record()));
    m_GACDriver->m_mainwindow->ui->qvtkopenglWidget->setCursor(Qt::ArrowCursor);

}

void GACControlPanel::on_originalBox_clicked(bool checked) {
    if (checked){
        m_GACDriver->m_imagedata->m_background_image = m_GACDriver->m_imagedata->m_readimage;
        ui->edgeBox->setCheckable(false);
    }
    else {
        if (m_GACDriver->m_imagedata->m_background_image == m_GACDriver->m_imagedata->m_readimage) {
            m_GACDriver->m_imagedata->m_background_image = nullptr;
            ui->edgeBox->setCheckable(true);
        }
    }
    // TODO better display method
    m_GACDriver->m_mainwindow->displayImage();
}

void GACControlPanel::on_edgeBox_clicked(bool checked) {
    if (checked){
        m_GACDriver->caster_filtering();
        m_GACDriver->caster->SetInput(m_GACDriver->m_gacparameters->m_sigmoidImage);
        m_GACDriver->m_imagedata->m_background_image = m_GACDriver->caster->GetOutput();
        ui->originalBox->setCheckable(false);
    }
    else {
        if (m_GACDriver->m_imagedata->m_background_image == m_GACDriver->caster->GetOutput()) {
            m_GACDriver->m_imagedata->m_background_image = nullptr;
            ui->originalBox->setCheckable(true);
        }
    }
    m_GACDriver->m_mainwindow->displayImage();
}

void GACControlPanel::on_setanchorButton_clicked() {
    m_GACDriver->save_anchorimage();
    ui->loadanchorButton->setText("Back to Iteraion "+ui->iterationEdit->text());
    ui->loadanchorButton->setEnabled(true);
}

void GACControlPanel::on_loadanchorButton_clicked() {
    m_GACDriver->load_anchorimage();
    iteration_change();
}

void GACControlPanel::on_quitButton_clicked() {
    ui->playpauseButton->setChecked(false);
    m_GACDriver->save_anchorimage();
    m_GACDriver->m_mainwindow->show_gacThumbnail();
    m_GACDriver->m_imagedata->m_background_image->DisconnectPipeline();
    m_GACDriver->m_imagedata->m_background_image = nullptr;
    m_GACDriver->m_mainwindow->displayImage();
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    this->close();
}
