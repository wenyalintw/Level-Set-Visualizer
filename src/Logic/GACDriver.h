//
// Created by 温雅 on 2019-04-29.
//

#ifndef LEVELSET_VISUALIZER_GACDRIVER_H
#define LEVELSET_VISUALIZER_GACDRIVER_H

#include <QObject>

#include <itkImage.h>
#include <itkGeodesicActiveContourLevelSetImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkFastMarchingImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

class GACParameters;
class MainWindow;
class ImageData;

class GACDriver : public QObject{

    Q_OBJECT

public:
    GACDriver(MainWindow*, ImageData*);
    ~GACDriver() = default;

public:
    static constexpr unsigned int Dimension = 2;
    using FloatPixelType = float;
    using FloatImageType = itk::Image< FloatPixelType, Dimension >;
    using UCPixelType = unsigned char;
    using UCImageType = itk::Image< UCPixelType, Dimension >;
    using GeodesicActiveContourFilterType = itk::GeodesicActiveContourLevelSetImageFilter< FloatImageType, FloatImageType >;
    using SmoothingFilterType = itk::CurvatureAnisotropicDiffusionImageFilter< FloatImageType, FloatImageType >;
    using GradientFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter< FloatImageType, FloatImageType >;
    using SigmoidFilterType = itk::SigmoidImageFilter< FloatImageType, FloatImageType >;
    using FastMarchingFilterType = itk::FastMarchingImageFilter< FloatImageType, FloatImageType >;
    using RescaleFilterType = itk::RescaleIntensityImageFilter< FloatImageType, UCImageType >;
    using ThresholdingFilterType = itk::BinaryThresholdImageFilter< FloatImageType, UCImageType >;
    using NodeType = FastMarchingFilterType::NodeType;

public slots:
    void run_segmentation();
    void set_gac_parameter();
    void seed_record();

signals:
    void seedClicked();
    void iterationChange();
    void filterReset();

public:
    void smoothing_filtering(FloatImageType::Pointer);
    void gradient_filtering(FloatImageType::Pointer);
    void sigmoid_filtering(FloatImageType::Pointer);
    void fastmarching_filtering(FloatImageType::Pointer);
    void threshold_filtering();
    void initial_gacfilter();
    void caster_filtering();
    void reset_gacfilter();
    void set_edge_parameters();
    void set_fm_parameters();
    void create_bubble_image();
    void initial_bubble_image();
    void backgroundimage_changed();
    void save_anchorimage();
    void load_anchorimage();
    void display_update();

public:
    SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
    GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
    SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();
    GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
    RescaleFilterType::Pointer caster = RescaleFilterType::New();
    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    FloatImageType::IndexType  seedPosition;
    NodeType node;
    std::vector<NodeType>  node_list;

    GACParameters* m_gacparameters = nullptr;
    MainWindow* m_mainwindow = nullptr;
    ImageData* m_imagedata = nullptr;

    int m_stepsize = 1;
    int m_stepanchor = 0;

};

#endif //LEVELSET_VISUALIZER_GACDRIVER_H
