//
// Created by 温雅 on 2019-04-29.
//

#include "ImageData.h"
#include <QFileDialog>
#include <QDebug>

#include "mainwindow.h"


#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageFileWriter.h>

ImageData::ImageData(MainWindow *window){
    m_mainwindow = window;
}

ImageData::~ImageData(){

}

void ImageData::loadImage(){

    QString fname = QFileDialog::getOpenFileName(m_mainwindow, QObject::tr("Load Image"), qApp->applicationDirPath(), "Image Files (*.jpg *.png *.bmp *.dcm *DCM)");
    qDebug() << fname;

    if (!fname.isNull()) {
        QByteArray byteArray = fname.toLocal8Bit();
        const char *inputFileName = byteArray.data();

        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputFileName);
        reader->Update();

        m_readimage = reader->GetOutput();

        m_frontground_image = m_readimage;

        m_mainwindow->m_readimagechanged = true;

        UCImageType ::RegionType region = m_readimage->GetLargestPossibleRegion();
        UCImageType::SizeType size = region.GetSize();

        m_imagecenter[0] = size.m_Size[0]/2.;
        m_imagecenter[1] = size.m_Size[1]/2.;

    }
    else {
        m_mainwindow->m_readimagechanged = false;
    }
}

void ImageData::saveImage(){
    QString fname = QFileDialog::getSaveFileName(m_mainwindow, QObject::tr("Save Image"), "", "(*.png)");
    QByteArray byteArray = fname.toLocal8Bit();
    const char *outputFileName = byteArray.data();

    using WriterType = itk::ImageFileWriter< UCImageType  >;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFileName);
    writer->SetInput(m_frontground_image);
    try
    {
        writer->Update();
    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
    }
}

ImageData::FloatImageType::Pointer ImageData::unsignedchar2float(UCImageType::Pointer ucimage) {
    using RescaleType = itk::RescaleIntensityImageFilter< UCImageType , UCImageType >;
    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetInput( ucimage );
    rescale->SetOutputMinimum( 0 );
    rescale->SetOutputMaximum( itk::NumericTraits< UCPixelType >::max() );
    rescale->Update();

    using FilterType = itk::CastImageFilter< UCImageType, FloatImageType >;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( rescale->GetOutput() );
    filter->Update();

    return filter->GetOutput();
}