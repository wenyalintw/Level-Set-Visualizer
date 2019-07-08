//
// Created by 温雅 on 2019-04-29.
//

#ifndef LEVELSET_VISUALIZER_IMAGEDATA_H
#define LEVELSET_VISUALIZER_IMAGEDATA_H

#include <itkImage.h>
#include <itkImageFileReader.h>


class MainWindow;


class ImageData {


public:
    ImageData(MainWindow*);
    ~ImageData();

    // image in itk format
public:
    static const unsigned int Dimension = 2;
    using UCPixelType = unsigned char;
    using UCImageType = itk::Image< UCPixelType, Dimension >;
    using FloatPixelType = float;
    using FloatImageType = itk::Image< FloatPixelType, Dimension >;
    using ReaderType = itk::ImageFileReader< UCImageType >;

    float m_imagecenter[2];

    UCImageType::Pointer m_readimage = nullptr;
    UCImageType::Pointer m_background_image = nullptr;
    UCImageType::Pointer m_frontground_image = nullptr;
    UCImageType::Pointer m_tempUCedge_image = nullptr;

    MainWindow* m_mainwindow = nullptr;

public:
    void loadImage();
    void saveImage();
    FloatImageType::Pointer unsignedchar2float(UCImageType::Pointer);
};


#endif //LEVELSET_VISUALIZER_IMAGEDATA_H
