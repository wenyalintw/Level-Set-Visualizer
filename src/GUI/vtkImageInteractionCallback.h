//
// Created by 温雅 on 2019-05-05.
//

#ifndef LEVELSET_VISUALIZER_VTKIMAGEINTERACTIONCALLBACK_H
#define LEVELSET_VISUALIZER_VTKIMAGEINTERACTIONCALLBACK_H


#include <vtkVersion.h>
#include <vtkAssemblyPath.h>
#include <vtkCell.h>
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkTIFFReader.h>
#include <vtkPointData.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkImageNoiseSource.h>
#include <vtkImageCast.h>
#include <vtkMath.h>


// Template for image value reading
template<typename T>
void vtkValueMessageTemplate(vtkImageData* image, int* position,
                             std::string& message)
{
    T* tuple = ((T*)image->GetScalarPointer(position));
    int components = image->GetNumberOfScalarComponents();
    for (int c = 0; c < components; ++c)
    {
        message += vtkVariant(tuple[c]).ToString();
        if (c != (components - 1))
        {
            message += ", ";
        }
    }
    message += " )";
}

// The mouse motion m_callback, to pick the image and recover pixel values
class vtkImageInteractionCallback : public vtkCommand
{
public:
    static vtkImageInteractionCallback *New()
    {
        return new vtkImageInteractionCallback;
    }

    vtkImageInteractionCallback()
    {
        this->Viewer     = NULL;
        this->Picker     = NULL;
        this->Annotation = NULL;
    }

    ~vtkImageInteractionCallback()
    {
        this->Viewer     = NULL;
        this->Picker     = NULL;
        this->Annotation = NULL;
    }

    void SetPicker(vtkPropPicker *picker)
    {
        this->Picker = picker;
    }

    void SetAnnotation(vtkCornerAnnotation *annotation)
    {
        this->Annotation = annotation;
    }

    void SetViewer(vtkImageViewer2 *viewer)
    {
        this->Viewer = viewer;
    }

    virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void *)
    {
        vtkRenderWindowInteractor *interactor =
                this->Viewer->GetRenderWindow()->GetInteractor();
        vtkRenderer* renderer = this->Viewer->GetRenderer();
        vtkImageActor* actor = this->Viewer->GetImageActor();
        vtkImageData* image = this->Viewer->GetInput();
        vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
                interactor->GetInteractorStyle());

#if VTK_MAJOR_VERSION <= 5
        image->Update();
#endif

        int x = interactor->GetEventPosition()[0];
        int y = interactor->GetEventPosition()[1];
        int z = interactor->GetEventPosition()[2];

        this->Picker->Pick(x, y, 0.0, renderer);
        // Pick at the mouse location provided by the interactor
        this->Picker->Pick(x, y, 0.0, renderer);

        // There could be other props assigned to this picker, so
        // make sure we picked the image actor
         vtkAssemblyPath* path = this->Picker->GetPath();
        bool validPick = false;

        if (path)
        {
            vtkCollectionSimpleIterator sit;
            path->InitTraversal(sit);
            vtkAssemblyNode *node;
            for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
            {
                node = path->GetNextNode(sit);
                if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
                {
                    validPick = true;
                }
            }
        }

        if (!validPick)
        {
            this->Annotation->SetText(0, "Off Image");
            interactor->Render();
            return;
        }

        // Get the world coordinates of the pick
        double pos[3];
        this->Picker->GetPickPosition(pos);


        int axis = this->Viewer->GetSliceOrientation();
        switch (axis)
        {
            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                image_coordinate[0] = vtkMath::Round(pos[0]);
                image_coordinate[1] = this->Viewer->GetSlice();
                image_coordinate[2] = vtkMath::Round(pos[2]);
                break;
            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                image_coordinate[0] = this->Viewer->GetSlice();
                image_coordinate[1] = vtkMath::Round(pos[0]);
                image_coordinate[2] = vtkMath::Round(pos[1]);
                break;
            default:  // vtkImageViewer2::SLICE_ORIENTATION_XY
                image_coordinate[0] = vtkMath::Round(pos[0]);
                image_coordinate[1] = vtkMath::Round(pos[1]);
                image_coordinate[2] = this->Viewer->GetSlice();
                break;
        }

        std::string message = "Location: ( ";
        message += vtkVariant(image_coordinate[0]).ToString();
        message += ", ";
        message += vtkVariant(image_coordinate[1]).ToString();
//        message += ", ";
//        message += vtkVariant(image_coordinate[2]).ToString();
        message += " )\nValue: ( ";

        switch (image->GetScalarType())
        {
            vtkTemplateMacro((vtkValueMessageTemplate<VTK_TT>(image,
                                                              image_coordinate,
                                                              message)));
            default:
                return;
        }

        this->Annotation->SetText( 0, message.c_str() );
        interactor->Render();
    }

public:
    int image_coordinate[3];

private:
    vtkImageViewer2*      Viewer;      // Pointer to the viewer
    vtkPropPicker*        Picker;      // Pointer to the picker
    vtkCornerAnnotation*  Annotation;  // Pointer to the annotation
};


#endif //LEVELSET_VISUALIZER_VTKIMAGEINTERACTIONCALLBACK_H