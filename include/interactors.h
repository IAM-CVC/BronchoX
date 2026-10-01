/**
 * @brief File which contains the class for custom interactor integrated into
 * code.
 *
 * The VTK AddObserver function requires a vtkCommand class (or function) to
 * catch events performed. This file contains the classes implemented & used
 * into BronchoX. These classes follow the vtkCommand's inheritance signature.
 */

#ifndef _INTERACTORS_H_
#define _INTERACTORS_H_

#include <vtkAlgorithmOutput.h>
#include <vtkCommand.h>
#include <vtkHandleWidget.h>

// class to handle the movement of the seed in the vtkImageViewer2 (axial,
// sagittal, coronal)
class vtkSeedInteractionCallback : public vtkCommand {
public:
  static vtkSeedInteractionCallback *New() {
    return new vtkSeedInteractionCallback;
  }

  vtkSeedInteractionCallback() {
    this->m_renderer = nullptr;
    this->m_sphereActors = nullptr;
  }

  ~vtkSeedInteractionCallback() {}

  void SetMainRenderer(vtkSmartPointer<vtkRenderer> renderer) {
      this->m_renderer = renderer;
  }

  void SetOtherSeedWitdget(vtkSmartPointer<vtkSeedWidget> otherSeedWitdget_1, vtkSmartPointer<vtkSeedWidget> otherSeedWitdget_2) {
      this->m_otherSeedWitdget_1 = otherSeedWitdget_1;
      this->m_otherSeedWitdget_2 = otherSeedWitdget_2;
  }

  void SetOtherRenderer(vtkSmartPointer<vtkRenderer> otherRenderer_1, vtkSmartPointer<vtkRenderer> otherRenderer_2) {
      this->m_otherRenderer_1 = otherRenderer_1;
      this->m_otherRenderer_2 = otherRenderer_2;
  }

  void SetVectorActor(std::vector<vtkSmartPointer<vtkActor>> &vectorActors) {
    this->m_sphereActors = &vectorActors;
  }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *) {

    

    vtkSeedWidget *seedWidget = static_cast<vtkSeedWidget *>(caller); // vtkHandleWidget
    vtkSmartPointer<vtkSeedRepresentation> seedRep = static_cast<vtkSeedRepresentation *>(seedWidget->GetRepresentation());


    if (eventId == vtkCommand::InteractionEvent) // when is moving
    {
      int id = seedRep->GetActiveHandle();
      //vtkHandleWidget* seed = seedWidget->GetSeed(id);
      //seed->SetAllowHandleResize(3);
      //    limited to a 1 seed for view
      if (id > 0) {
          int e = id - 1;
          seedWidget->DeleteSeed(e);
      }
      else {
          m_otherSeedWitdget_1->DeleteSeed(0);
          m_otherSeedWitdget_2->DeleteSeed(0);
          m_otherRenderer_1->GetRenderWindow()->Render();
          m_otherRenderer_2->GetRenderWindow()->Render();
      }

      m_renderer->GetRenderWindow()->Render();
    }
  }

private:
  std::vector<vtkSmartPointer<vtkActor>> *m_sphereActors;
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkSeedWidget> m_otherSeedWitdget_1;
  vtkSmartPointer<vtkSeedWidget> m_otherSeedWitdget_2;
  vtkSmartPointer<vtkRenderer> m_otherRenderer_1;
  vtkSmartPointer<vtkRenderer> m_otherRenderer_2;
};

#include <navigation.h>

class WindowRendererCallBack : public vtkCommand
{
public:

    vtkSmartPointer<vtkRenderer> m_renderer;
    CNavigation* navigation;


    static WindowRendererCallBack* New()
    {
        return new WindowRendererCallBack;
    }

    void setup(vtkSmartPointer<vtkRenderer> renderer) {
        m_renderer = renderer;
    };

    void setNavigation(CNavigation* nav) {
        navigation = nav;
    }

    void Execute(vtkObject* caller, unsigned long eventId, void*)
    {
        navigation->next();
        m_renderer->GetRenderWindow()->Render();
    }
};


// Template for image value reading
template <typename T>
void vtkValueMessageTemplate(vtkImageData *image, int *position,
                             std::string &message) {
  T *tuple = ((T *)image->GetScalarPointer(position));
  int components = image->GetNumberOfScalarComponents();
  for (int c = 0; c < components; ++c) {
    message += vtkVariant(tuple[c]).ToString();
    if (c != (components - 1)) {
      message += ", ";
    }
  }
}

// The mouse motion callback, to pick the image and recover pixel values
class vtkImageInteractionCallback : public vtkCommand {
public:
  static vtkImageInteractionCallback *New() {
    return new vtkImageInteractionCallback;
  }

  vtkImageInteractionCallback() {
    this->Viewer = NULL;
    this->Picker = NULL;
  }

  ~vtkImageInteractionCallback() {
    this->Viewer = NULL;
    this->Picker = NULL;
  }

  void SetPicker(vtkPropPicker *picker) { this->Picker = picker; }

  void
  SetCornerAnnotation(vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation) {
    this->m_annotation = cornerAnnotation;
  }

  void SetViewer(vtkImageViewer2 *viewer) { this->Viewer = viewer; }

  virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void *) {
    vtkRenderWindowInteractor *interactor =
        this->Viewer->GetRenderWindow()->GetInteractor();
    vtkRenderer *renderer = this->Viewer->GetRenderer();
    vtkImageActor *actor = this->Viewer->GetImageActor();
    vtkImageData *image = this->Viewer->GetInput();
    vtkInteractorStyle *style =
        vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());

    // Pick at the mouse location provided by the interactor
    this->Picker->Pick(interactor->GetEventPosition()[0],
                       interactor->GetEventPosition()[1], 0.0, renderer);

    // double ptActual[3];
    // Picker->GetSelectionPoint(ptActual);

    // There could be other props assigned to this picker, so
    // make sure we picked the image actor
    vtkAssemblyPath *path = this->Picker->GetPath();
    bool validPick = false;

    if (path) {
      vtkCollectionSimpleIterator sit;
      path->InitTraversal(sit);
      vtkAssemblyNode *node;
      for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i) {
        node = path->GetNextNode(sit);
        if (actor == vtkImageActor::SafeDownCast(node->GetViewProp())) {
          validPick = true;
        }
      }
    }

    if (!validPick) {
      m_annotation->ClearAllTexts();
      std::string msg = "(0, 0): 0\n";
      msg += vtkVariant(this->Viewer->GetSlice() + 1).ToString() + " / " +
             std::to_string(this->Viewer->GetSliceMax() + 1);

      m_annotation->SetText(2, msg.c_str());
      interactor->Render();
      // Pass the event further on
      style->OnMouseMove();
      return;
    }

    // Get the world coordinates of the pick
    double pos[3];
    this->Picker->GetPickPosition(pos);
    int image_coordinate[3];

    int axis = this->Viewer->GetSliceOrientation();
    switch (axis) {
    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
      image_coordinate[0] = vtkMath::Round(pos[0]);
      image_coordinate[1] = vtkMath::Round(pos[2]);
      image_coordinate[2] = this->Viewer->GetSlice();
      break;
    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
      image_coordinate[0] = vtkMath::Round(pos[1]);
      image_coordinate[1] = vtkMath::Round(pos[2]);
      image_coordinate[2] = this->Viewer->GetSlice();
      break;
    default: // vtkImageViewer2::SLICE_ORIENTATION_XY
      image_coordinate[0] = vtkMath::Round(pos[0]);
      image_coordinate[1] = vtkMath::Round(pos[1]);
      image_coordinate[2] = this->Viewer->GetSlice();
      break;
    }

    // this print in the format (x, y): value
    std::string message = "(";
    message += vtkVariant(image_coordinate[0]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[1]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[2]).ToString();
    message += "): ";

    switch (axis) {
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
    default: // vtkImageViewer2::SLICE_ORIENTATION_XY
      image_coordinate[0] = vtkMath::Round(pos[0]);
      image_coordinate[1] = vtkMath::Round(pos[1]);
      image_coordinate[2] = this->Viewer->GetSlice();
      break;
    }
    // at this point, coordinates are ordered as (x, y, z)
    switch (image->GetScalarType()) {
      vtkTemplateMacro(
          (vtkValueMessageTemplate<VTK_TT>(image, image_coordinate, message)));
    default:
      return;
    }
    // this print in the format Slice/NumberOfSlices (e.g. 100/150)
    message += "\n";
    // starting from 1 to GetSliceMax
    message += vtkVariant(image_coordinate[2] + 1).ToString();
    message += " / ";
    message += vtkVariant(Viewer->GetSliceMax() + 1).ToString();

    m_annotation->ClearAllTexts();
    m_annotation->SetText(2, message.c_str());

    interactor->Render();
    style->OnMouseMove();
  }

private:
  vtkSmartPointer<vtkImageViewer2> Viewer; // Pointer to the viewer
  vtkSmartPointer<vtkPropPicker> Picker;   // Pointer to the picker
  vtkSmartPointer<vtkCornerAnnotation> m_annotation;
};

#endif //_INTERACTORS_H_