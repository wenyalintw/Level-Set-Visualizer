//
// Created by 温雅 on 2019-04-29.
//

#include "GACDriver.h"
#include "GACParameters.h"
#include "ImageData.h"
#include <itkImageDuplicator.h>


GACDriver::GACDriver(MainWindow* mainWindow, ImageData* imageData) {
    m_gacparameters = new GACParameters();
    m_mainwindow = mainWindow;
    m_imagedata = imageData;
    // 用caster把Float轉UC再display
    caster_filtering();
}

void GACDriver::smoothing_filtering(FloatImageType::Pointer inputimage) {
    smoothing->SetTimeStep( 0.125 );
    smoothing->SetNumberOfIterations( m_gacparameters->m_aditer );
    smoothing->SetConductanceParameter( m_gacparameters->m_conductance );
    smoothing->SetInput( inputimage );
    m_gacparameters->m_smoothingImage = smoothing->GetOutput();

}

void GACDriver::gradient_filtering(FloatImageType::Pointer inputimage) {
    gradientMagnitude->SetInput( inputimage );
    m_gacparameters->m_gradientImage = gradientMagnitude->GetOutput();
}

void GACDriver::sigmoid_filtering(FloatImageType::Pointer inputimage) {
    sigmoid->SetOutputMinimum( 0.0 );
    sigmoid->SetOutputMaximum( 1.0 );
    sigmoid->SetInput( inputimage );
    m_gacparameters->m_sigmoidImage = sigmoid->GetOutput();

    // display
    caster->SetInput(m_gacparameters->m_sigmoidImage);
    m_imagedata->m_frontground_image = caster->GetOutput();
    m_imagedata->m_tempUCedge_image = caster->GetOutput();
    m_mainwindow->displayImage();

    m_mainwindow->show_edgeThumbnail();
}

void GACDriver::fastmarching_filtering(FloatImageType::Pointer inputimage) {
    using NodeContainer = FastMarchingFilterType::NodeContainer;

    NodeContainer::Pointer seeds = NodeContainer::New();
    seeds->Initialize();
    for (int i=0; i < node_list.size() ; i++){
        seeds->InsertElement( i, node_list[i] );
    }

    fastMarching->SetTrialPoints( seeds );
    fastMarching->SetSpeedConstant( 1.0 );
    fastMarching->SetOutputSize( inputimage->GetBufferedRegion().GetSize() );
    m_gacparameters->m_fastmarchingImage = fastMarching->GetOutput();
}

void GACDriver::initial_gacfilter() {
    geodesicActiveContour->SetInput(  m_gacparameters->m_fastmarchingImage );
    geodesicActiveContour->SetFeatureImage( m_gacparameters->m_sigmoidImage );
    geodesicActiveContour->SetManualReinitialization(true);
    geodesicActiveContour->SetNumberOfIterations(0);
    geodesicActiveContour->UpdateLargestPossibleRegion();
}

void GACDriver::threshold_filtering() {
    thresholder->SetLowerThreshold( -1000.0 );
    thresholder->SetUpperThreshold( 0.0 );
    thresholder->SetOutsideValue( itk::NumericTraits< UCPixelType >::min() );
    thresholder->SetInsideValue( itk::NumericTraits< UCPixelType >::max() );
    thresholder->SetInput( geodesicActiveContour->GetOutput() );
}

void GACDriver::reset_gacfilter() {
    // delete anchor image
     m_gacparameters->m_anchorImage = nullptr;

    set_gac_parameter();

    geodesicActiveContour->SetInput(  m_gacparameters->m_fastmarchingImage );
    geodesicActiveContour->SetStateToUninitialized();
    geodesicActiveContour->SetNumberOfIterations(0);

    display_update();
    emit iterationChange();
    emit filterReset();
}

void GACDriver::caster_filtering() {
    caster->SetOutputMinimum( itk::NumericTraits< UCPixelType >::min() );
    caster->SetOutputMaximum( itk::NumericTraits< UCPixelType >::max() );
}

void GACDriver::run_segmentation(){
    geodesicActiveContour->SetNumberOfIterations(geodesicActiveContour->GetElapsedIterations() + 1);
    geodesicActiveContour->UpdateLargestPossibleRegion();

    if ((geodesicActiveContour->GetElapsedIterations()-m_stepanchor) % m_stepsize==0){
        display_update();
        emit iterationChange();
    }
}

void GACDriver::display_update(){
    thresholder->Update();
    m_imagedata->m_frontground_image = thresholder->GetOutput();
    m_mainwindow->displayImage();
}

void GACDriver::save_anchorimage() {
    using DuplicatorType = itk::ImageDuplicator< FloatImageType >;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(geodesicActiveContour->GetOutput());
    duplicator->Update();
    m_gacparameters->m_anchorImage = duplicator->GetOutput();
    m_gacparameters->m_anchorIter += geodesicActiveContour->GetElapsedIterations();
    load_anchorimage();
}

void GACDriver::load_anchorimage() {
    geodesicActiveContour->SetStateToUninitialized();
    geodesicActiveContour->SetInput(m_gacparameters->m_anchorImage);
    geodesicActiveContour->SetNumberOfIterations(0);
    geodesicActiveContour->UpdateLargestPossibleRegion();
    display_update();
}

void GACDriver::backgroundimage_changed(){
    m_mainwindow->displayImage();
}

void GACDriver::set_gac_parameter(){
    geodesicActiveContour->SetMaximumRMSError( m_gacparameters->m_maximumRMSError );
    geodesicActiveContour->SetPropagationScaling(m_gacparameters->m_propagationScaling);
    geodesicActiveContour->SetCurvatureScaling(m_gacparameters->m_curvatureScaling);
    geodesicActiveContour->SetAdvectionScaling(m_gacparameters->m_advectionScaling);
    m_gacparameters->m_anchorIter=0;
}

void GACDriver::seed_record(){
    m_gacparameters->m_seedPosX = m_mainwindow->m_callback->image_coordinate[0];
    m_gacparameters->m_seedPosY = m_mainwindow->m_callback->image_coordinate[1];
    emit seedClicked();
}

void GACDriver::set_edge_parameters() {
    gradientMagnitude->SetSigma( m_gacparameters->m_sigma );
    sigmoid->SetAlpha( m_gacparameters->m_alpha );
    sigmoid->SetBeta( m_gacparameters->m_beta );
}

// deprecated
void GACDriver::set_fm_parameters() {
    seedPosition[0] = m_gacparameters->m_seedPosX;
    seedPosition[1] = m_gacparameters->m_seedPosY;
    node.SetValue( m_gacparameters->m_seedValue );
    node.SetIndex( seedPosition );
}

void GACDriver::initial_bubble_image(){
    m_gacparameters->m_bubbleImage = UCImageType::New();
    UCImageType::RegionType region = m_imagedata->m_readimage->GetLargestPossibleRegion();
    UCImageType::SizeType size = region.GetSize();
    m_gacparameters->m_bubbleImage->SetRegions(region);
    m_gacparameters->m_bubbleImage->Allocate(true);
}

void GACDriver::create_bubble_image(){
    // Disconnect
    m_gacparameters->m_bubbleImage->DisconnectPipeline();

    UCImageType ::RegionType region = m_gacparameters->m_bubbleImage->GetLargestPossibleRegion();
    UCImageType::SizeType size = region.GetSize();

    double radius=5;
    int x=m_gacparameters->m_seedPosX , y=m_gacparameters->m_seedPosY;
    for(int row = 0; row < size.m_Size[0]; row++)
    {
        for(int col = 0; col < size.m_Size[1]; col++)
        {
            UCImageType::IndexType pixelIndex;
            pixelIndex[0] = row;
            pixelIndex[1] = col;

            if ( sqrt(pow(row-x, 2)+pow(col-y, 2)) < radius){
                m_gacparameters->m_bubbleImage->SetPixel(pixelIndex, 255);
            }
        }
    }
    m_imagedata->m_frontground_image = m_gacparameters->m_bubbleImage;
    m_mainwindow->displayImage();
}