#include "mainwindow.h"
#include "ImageData.h"
#include "GACDriver.h"
#include "GACControlPanel.h"
#include "Thumbnail.h"
#include "GACParameters.h"

#include <QPixmap>
#include <QFileDialog>
#include <QDebug>
#include <QSize>
#include <QWindow>

#include <itkIndex.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkRGBPixel.h>
#include <itkScalarToRGBColormapImageFilter.h>

#include <vtkRenderWindow.h>
#include <vtkImageActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkImageProperty.h>
#include <vtkPropPicker.h>
#include <vtkCornerAnnotation.h>
#include <vtkAssemblyPath.h>
#include <vtkMath.h>
#include <vtkVariant.h>
#include <vtkInteractorStyleImage.h>
#include <vtkCamera.h>
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_imagedata = new ImageData(this);
    m_Connections = vtkEventQtSlotConnect::New();
    m_Connections->Connect(ui->qvtkopenglWidget->GetRenderWindow()->GetInteractor(),
                           vtkCommand::MouseMoveEvent,
                           this,
                           SLOT(updateCoords(vtkObject*)));

    // qApp就是現在的application，用來檢測在螢幕間切換的pixel問題
    connect(qApp->topLevelWindows().first(), SIGNAL(screenChanged(QScreen*)), this, SLOT(screenChanged()));

    m_signalMapper = new QSignalMapper(this);
    connect(ui->originThumbnail, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    connect(ui->edgeThumbnail, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    connect(ui->gacThumbnail, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(ui->originThumbnail, 0);
    m_signalMapper->setMapping(ui->edgeThumbnail, 1);
    m_signalMapper->setMapping(ui->gacThumbnail, 2);

    connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(display_thumbnail(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::display_thumbnail(int n){
    switch(n) {
        case 0:
            m_imagedata->m_frontground_image = m_imagedata->m_readimage;
            break;
        case 1:
            m_imagedata->m_frontground_image = m_GACDriver->caster->GetOutput();
            break;
        case 2:
            m_imagedata->m_frontground_image = m_GACDriver->thresholder->GetOutput();
            break;
        default:
            break;
    }
    displayImage();
}

void MainWindow::screenChanged() {
    m_devicePixelRatio = ui->qvtkopenglWidget->devicePixelRatio();
    if (m_ImageViewer){
        m_cornerAnnotation->SetMaximumFontSize(20 * m_devicePixelRatio);
        displayImage();
    }
}

void MainWindow::show_originThumbnail() {
    UCImageType ::RegionType region;
    region = m_imagedata->m_readimage->GetLargestPossibleRegion();
    UCImageType::SizeType size = region.GetSize();
    int rows = size.m_Size[0];
    int cols = size.m_Size[1];
    uchar imagio [cols][rows];
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            UCImageType::IndexType pixelIndex;
            pixelIndex[0] = row;
            pixelIndex[1] = col;
            imagio[col][row] = m_imagedata->m_readimage->GetPixel(pixelIndex);
        }
    }
    QImage image((uchar*)imagio, rows, cols, rows, QImage::Format_Grayscale8);
    int w = ui->originThumbnail->width();
    int h = ui->originThumbnail->height();
    int backlash = ui->originThumbnail->lineWidth()*2;
    ui->originThumbnail->setPixmap(QPixmap::fromImage(image).scaled(w-backlash, h-backlash, Qt::IgnoreAspectRatio));
}

void MainWindow::show_edgeThumbnail() {
    UCImageType ::RegionType region;
    region = m_GACDriver->m_gacparameters->m_sigmoidImage->GetLargestPossibleRegion();
    UCImageType::SizeType size = region.GetSize();
    int rows = size.m_Size[0];
    int cols = size.m_Size[1];
    uchar imagio [cols][rows];
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            UCImageType::IndexType pixelIndex;
            pixelIndex[0] = row;
            pixelIndex[1] = col;
            imagio[col][row] = m_imagedata->m_tempUCedge_image->GetPixel(pixelIndex);
        }
    }
    QImage image((uchar*)imagio, rows, cols, rows, QImage::Format_Grayscale8);
    int w = ui->edgeThumbnail->width();
    int h = ui->edgeThumbnail->height();
    int backlash = ui->edgeThumbnail->lineWidth()*2;
    ui->edgeThumbnail->setPixmap(QPixmap::fromImage(image).scaled(w-backlash, h-backlash, Qt::IgnoreAspectRatio));
    m_imagedata->m_tempUCedge_image = nullptr;
}

void MainWindow::show_gacThumbnail() {
    // 會到show_gacThumbnail代表已按下GAC_Panel的Quit，顯示後可開啟點選thunmbnail的功能
    UCImageType ::RegionType region;
    region = m_GACDriver->m_gacparameters->m_anchorImage->GetLargestPossibleRegion();
    UCImageType::SizeType size = region.GetSize();
    int rows = size.m_Size[0];
    int cols = size.m_Size[1];
    uchar imagio [cols][rows];
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            UCImageType::IndexType pixelIndex;
            pixelIndex[0] = row;
            pixelIndex[1] = col;

            float p = m_GACDriver->m_gacparameters->m_anchorImage->GetPixel(pixelIndex);
            imagio[col][row] = p > -1000 && p < 0 ? 255 : 0;
        }
    }
    QImage image((uchar*)imagio, rows, cols, rows, QImage::Format_Grayscale8);
    int w = ui->gacThumbnail->width();
    int h = ui->gacThumbnail->height();
    int backlash = ui->gacThumbnail->lineWidth()*2;
    ui->gacThumbnail->setPixmap(QPixmap::fromImage(image).scaled(w-backlash, h-backlash, Qt::IgnoreAspectRatio));
    // thumbnail clickable!
    Thumbnail::clickable=true;
}

void MainWindow::display_setup()
{
    //// Display Widget Setup
    // frontground
    m_ImageViewer  = vtkSmartPointer< vtkImageViewer2 >::New();
    m_Renderer      = vtkSmartPointer< vtkRenderer >::New();
    m_ImageViewer->SetRenderer(m_Renderer);
    m_ImageViewer->SetupInteractor(ui->qvtkopenglWidget->GetRenderWindow()->GetInteractor());
    m_Renderer->SetBackground(0.1,0.3,0.8);
    m_ImageViewer->GetImageActor()->InterpolateOff();
    m_ImageViewer->SetSliceOrientationToXY();
    // 用一些opacity或是lookuptable轉換的話，actor會變成translucent mode，在pickprop會有問題，所以強制Opaque
    m_ImageViewer->GetImageActor()->SetForceOpaque(1);

    // background
    m_BackGroundImageViewer  = vtkSmartPointer< vtkImageViewer2 >::New();
    m_BackgroundRenderer      = vtkSmartPointer< vtkRenderer >::New();
    m_BackGroundImageViewer->SetRenderer(m_BackgroundRenderer);
    m_BackGroundImageViewer->GetImageActor()->InterpolateOff();
    m_BackgroundRenderer->SetBackground(0.1,0.3,0.8);
    m_BackgroundRenderer->InteractiveOff();
    m_BackGroundImageViewer->SetSliceOrientationToXY();

    // for origin at top-left, set two renderer have same Camera to Zoom together
    m_Renderer->GetActiveCamera()->SetPosition(0,0,-1);
    m_Renderer->GetActiveCamera()->SetViewUp(0,-1,0);//
    m_BackgroundRenderer->SetActiveCamera(m_Renderer->GetActiveCamera());

}

void MainWindow::picker_setup() {
    //// Pixel-Picking Setup
    m_propPicker = vtkSmartPointer<vtkPropPicker>::New();
    m_cornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
    m_callback = vtkSmartPointer<vtkImageInteractionCallback>::New();

    // Picker to pick pixels
    m_propPicker->PickFromListOn();
    // Give the picker a prop to pick
    m_propPicker->AddPickList(m_ImageViewer->GetImageActor());
    // Annotate the image with window/level and mouse over pixel information
    m_cornerAnnotation->SetLinearFontScaleFactor(2);
    m_cornerAnnotation->SetNonlinearFontScaleFactor(1);
    // set font size
    m_devicePixelRatio = ui->qvtkopenglWidget->devicePixelRatio();
    m_cornerAnnotation->SetMaximumFontSize(20 * m_devicePixelRatio);
    m_cornerAnnotation->SetText(0, "Off Image");
    m_cornerAnnotation->GetTextProperty()->SetColor(1, 0, 0);
    m_cornerAnnotation->GetTextProperty()->SetBold(true);
    // Callback listens to MouseMoveEvents invoked by the interactor's style
    m_callback->SetViewer(m_ImageViewer);
    m_callback->SetAnnotation(m_cornerAnnotation);
    m_callback->SetPicker(m_propPicker);
    m_Renderer->AddViewProp(m_cornerAnnotation);
    m_ImageViewer->GetInteractorStyle()->AddObserver(vtkCommand::MouseMoveEvent, m_callback);
}

void MainWindow::on_loadButton_clicked()
{
    m_imagedata->loadImage();
   // 確定有load進image，再進行顯示的setup
    if (m_readimagechanged){
        // only setup display & pick @ first loading
        if (!m_ImageViewer){
            display_setup();
            picker_setup();
        }
        if (m_gacpanel_dialog){
            gac_reset();
            // There's a m_Background Renderer issue when loading different image
            // use this method to flush the buffer
            m_imagedata->m_background_image = nullptr;
            m_imagedata->m_background_image = m_imagedata->m_frontground_image;
            displayImage();
        }
        m_imagedata->m_background_image = nullptr;
        displayImage();
        show_originThumbnail();
        Thumbnail::clickable=false;
    }
}

void MainWindow::gac_reset()
{
    ui->originThumbnail->setText("original image");
    ui->edgeThumbnail->setText("edge image");
    ui->gacThumbnail->setText("levelset image");

    // STUPID delete pattern, need to be revised later
    if (m_gacpanel_dialog){
        if(m_gacpanel_dialog->m_gacthread){
            if (m_gacpanel_dialog->m_gacthread->myTimer){
                delete m_gacpanel_dialog->m_gacthread->myTimer;
                m_gacpanel_dialog->m_gacthread->myTimer = nullptr;
            }
            delete m_gacpanel_dialog->m_gacthread;
            m_gacpanel_dialog->m_gacthread = nullptr;
        }
        delete m_gacpanel_dialog;
        m_gacpanel_dialog = nullptr;
    }
    if (m_GACDriver){
        if (m_GACDriver->m_gacparameters){
            delete m_GACDriver->m_gacparameters;
            m_GACDriver->m_gacparameters = nullptr;
        }
        delete m_GACDriver;
        m_GACDriver = nullptr;
    }
}

void MainWindow::on_saveButton_clicked()
{
    m_imagedata->saveImage();
}

void MainWindow::displayImage() {

    if (!m_imagedata->m_background_image){
        ui->qvtkopenglWidget->GetRenderWindow()->ClearInRenderStatus();
        itkToVtk->SetInput(m_imagedata->m_frontground_image);
        itkToVtk->Update();
        m_ImageViewer->SetInputData(itkToVtk->GetOutput());
        m_ImageViewer->GetImageActor()->GetProperty()->SetInterpolationTypeToNearest();

        m_ImageViewer->UpdateDisplayExtent();
        m_ImageViewer->SetRenderWindow(ui->qvtkopenglWidget->GetRenderWindow());

        m_Renderer->ResetCamera();
        ui->qvtkopenglWidget->GetRenderWindow()->Render();
    }
    else {
//        m_BackgroundRenderer->Clear();
        ui->qvtkopenglWidget->GetRenderWindow()->ClearInRenderStatus();
        itkToVtk->SetInput(m_imagedata->m_frontground_image);
        itkToVtk->Update();

        // Map the scalar values in the image to colors with a lookup table:
        vtkSmartPointer<vtkLookupTable> lookupTable =
                vtkSmartPointer<vtkLookupTable>::New();

        lookupTable->SetNumberOfTableValues(2);
        lookupTable->SetRange(0.0, 1.0);
        // alpha完全設0也會上pickprop抓不到（沒render），所以給一個給低的值來逼近透明就好
        lookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.01 ); //label 0 is almost transparent
        lookupTable->SetTableValue( 1, 0.8, 0.1, 0.1, 0.8 ); //label 1 is opaque and green
        lookupTable->Build();

        // Pass the original image and the lookup table to a filter to create a color image:
        vtkSmartPointer<vtkImageMapToColors> scalarValuesToColors =
                vtkSmartPointer<vtkImageMapToColors>::New();
        scalarValuesToColors->SetLookupTable(lookupTable);
        scalarValuesToColors->PassAlphaToOutputOn();
        scalarValuesToColors->SetInputData(itkToVtk->GetOutput());
        scalarValuesToColors->Update();

        m_ImageViewer->SetInputData(scalarValuesToColors->GetOutput());
        m_ImageViewer->UpdateDisplayExtent();
        m_Renderer->ResetCamera();

        itkToVtk2->SetInput(m_imagedata->m_background_image);
        itkToVtk2->Update();

        m_BackGroundImageViewer->SetInputData(itkToVtk2->GetOutput());
        m_BackGroundImageViewer->UpdateDisplayExtent();
        m_BackgroundRenderer->ResetCamera();

        m_BackgroundRenderer->SetLayer(0);
        m_Renderer->SetLayer(1);
        ui->qvtkopenglWidget->GetRenderWindow()->SetNumberOfLayers(2);
        ui->qvtkopenglWidget->GetRenderWindow()->AddRenderer(m_BackgroundRenderer);
        ui->qvtkopenglWidget->GetRenderWindow()->AddRenderer(m_Renderer);
        ui->qvtkopenglWidget->GetRenderWindow()->Render();
    }
}

void MainWindow::updateCoords(vtkObject* obj)
{
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(obj);
    int event_pos[2];
    iren->GetEventPosition(event_pos);
    QString str;
    str.sprintf("Display Window Coordinate: x=%d , y=%d", event_pos[0], event_pos[1]);
    ui->statusBar->showMessage(str);

}

void MainWindow::on_levelsetButton_clicked() {
    if (m_readimagechanged) {
        m_GACDriver = new GACDriver(this, m_imagedata);
        m_gacpanel_dialog = new GACControlPanel(this, m_GACDriver);
        m_gacpanel_dialog->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        m_gacpanel_dialog->setAllowedAreas(Qt::AllDockWidgetAreas);
        addDockWidget(Qt::RightDockWidgetArea, m_gacpanel_dialog);
        m_readimagechanged = false;
    }
    else if (m_gacpanel_dialog){
        addDockWidget(Qt::RightDockWidgetArea, m_gacpanel_dialog);
    }
}