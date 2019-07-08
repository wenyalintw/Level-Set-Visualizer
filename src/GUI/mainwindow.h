#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QSignalMapper>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkLevelSetFunction.h>
#include <itkFiniteDifferenceFunction.h>
#include <itkFiniteDifferenceImageFilter.h>

#include <vtkInteractorStyle.h>
#include <vtkImageViewer2.h>
#include <vtkRenderer.h>
#include <vtkImageInteractionCallback.h>
#include <vtkWorldPointPicker.h>
#include <vtkSmartPointer.h>
#include <vtkEventQtSlotConnect.h>


class ImageData;
class GACDriver;
class GACControlPanel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_levelsetButton_clicked();
    void updateCoords(vtkObject* obj);
    void screenChanged();
    void show_originThumbnail();
    void show_edgeThumbnail();
    void show_gacThumbnail();
    void display_thumbnail(int);

public:
    void displayImage();
    void display_setup();
    void picker_setup();
    void gac_reset();

public:
    Ui::MainWindow *ui;
    static const unsigned int Dimension = 2;
    using UCPixelType = unsigned char;
    using UCImageType = itk::Image< UCPixelType, Dimension >;
    using FloatPixelType = float;
    using FloatImageType = itk::Image< FloatPixelType, Dimension >;
    using ImageToVTKImageFilterType = itk::ImageToVTKImageFilter<UCImageType>;

    vtkSmartPointer< vtkImageViewer2 > m_ImageViewer;
    vtkSmartPointer< vtkRenderer > m_Renderer;
    vtkSmartPointer< vtkImageViewer2 > m_BackGroundImageViewer;
    vtkSmartPointer< vtkRenderer > m_BackgroundRenderer;
    vtkEventQtSlotConnect* m_Connections;
    vtkSmartPointer<vtkPropPicker> m_propPicker;
    vtkSmartPointer<vtkCornerAnnotation> m_cornerAnnotation;
    vtkSmartPointer<vtkImageInteractionCallback> m_callback;

    ImageData* m_imagedata = nullptr;
    GACDriver* m_GACDriver = nullptr;
    GACControlPanel* m_gacpanel_dialog = nullptr;
    QSignalMapper* m_signalMapper = nullptr;

    ImageToVTKImageFilterType::Pointer itkToVtk = ImageToVTKImageFilterType::New();
    ImageToVTKImageFilterType::Pointer itkToVtk2 = ImageToVTKImageFilterType::New();

    bool m_readimagechanged = false;
    int m_devicePixelRatio = 1;
};

#endif // MAINWINDOW_H
