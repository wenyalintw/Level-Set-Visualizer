//
// Created by 温雅 on 2019-04-29.
//

#ifndef LEVELSET_VISUALIZER_GACPARAMETERS_H
#define LEVELSET_VISUALIZER_GACPARAMETERS_H


#include <itkImage.h>
#include "mainwindow.h"

class GACParameters {

public:
    GACParameters() = default;
    ~GACParameters() = default;

public:
    static constexpr unsigned int Dimension = 2;
    using FloatPixelType = float;
    using FloatImageType = itk::Image< FloatPixelType, Dimension >;
    using UCPixelType = unsigned char;
    using UCImageType = itk::Image< UCPixelType, Dimension >;

public:
    int m_seedPosX = 56;
    int m_seedPosY = 92;
    double m_initialDistance = 5.;
    double m_sigma = 1.;
    double m_alpha = -0.3;
    double m_beta  = 2.0;
    double m_propagationScaling = 10.;
    double m_curvatureScaling = 1.;
    double m_advectionScaling = 1.;
    double m_maximumRMSError = 0.002;
    double m_seedValue = - m_initialDistance;

    int m_radius = 5.;

    int m_aditer = 5;
    int m_conductance = 9.0;
    int m_anchorIter = 0;


    FloatImageType::Pointer m_smoothingImage;
    FloatImageType::Pointer m_gradientImage;
    FloatImageType::Pointer m_sigmoidImage;
    FloatImageType::Pointer m_fastmarchingImage;
    FloatImageType::Pointer m_anchorImage;
    UCImageType::Pointer m_thresholdImage;
    UCImageType::Pointer m_bubbleImage;

};


#endif //LEVELSET_VISUALIZER_GACPARAMETERS_H
