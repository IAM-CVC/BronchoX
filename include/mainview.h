#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "ui_mainview.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QtWidgets/QToolButton>
#include <qcombobox.h>
#include <QtConcurrent/QtConcurrent>
#include <qopenglcontext.h>

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCompassRepresentation.h>
#include <vtkCompassWidget.h>
#include <vtkConstrainedPointHandleRepresentation.h>
#include <vtkCornerAnnotation.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkDICOMImageReader.h>
#include <vtkDICOMMetaData.h>
#include <vtkDepthPeelingPass.h>
#include <vtkDepthSortPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageData.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkInteractorStyleImage.h> 
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLODActor.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkMapper2D.h>
#include <vtkMarchingCubes.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty2D.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSmartPointer.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtksys/SystemTools.hxx>
#include <vtkWidgetEvent.h>

#include <array>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "logmanager.h"
#include "appmanager.h"
#include "utility.h"
#include "interactors.h"
#include "dialogDICOM.h"
#include "bronchocore.h"
#include "navigation.h"


/*

#include <vtkImagePlaneWidget.h>
#include <vtkImageViewer2.h>
#include <vtkSmartPointer.h>

#include <map>

#include <nlohmann/json.hpp>
*/


namespace bronchox {
    namespace ui {

        /**
         * @brief Handle the link between the GUI and the logic part
         *
         * A QObject class which receives the interaction from the GUI (m_pUI variable)
         * and link with the CAppManager object (m_appManager variable). All slots of
         * GUI are handled here. The signals are in the ui generated file.
         */
        class MainView : public QMainWindow {
            Q_OBJECT
        protected:

        public:
            //explicit MainView(std::string strConfigFile, QWidget* parent = 0);
            explicit MainView(QWidget* parent = 0);
            ~MainView();


        private:
            enum VIEW : unsigned char {
                MAIN = 0,
                AXIAL,
                SAGITTAL,
                CORONAL,
                TOTAL_VIEWS
            }; // indexes for views


            Ui::MainView* m_pUI; // pointer to the window GUI
            QTimer* m_pTimer;    // pointer to the timer to animation
            QComboBox* bronchoTreeNodeComboBox;

            std::unique_ptr<manager::AppManager> controller;
            CBronchoCore bronchoCore;
            CNavigation navigation;

            //  GUI VARIABLES
            QVTKOpenGLWidget** widgets; //  VIEW
            vtkSmartPointer<vtkOrientationMarkerWidget> m_cubeWidget; // cube widget shows in the main view
            vtkSmartPointer<vtkRenderer> m_renders[4]; // VTK rendered object (vtkRenderWindow is the specific object)
            vtkSmartPointer<vtkImageViewer2> m_viewers[4]; // only 3 are used, but only for legibility/readibility
            vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_rendersWindow[4]; // only 3 are used, but only for legibility/readibility
            vtkSmartPointer<vtkImagePlaneWidget> m_planes[4]; // cut planes of the volume
            vtkSmartPointer<vtkCornerAnnotation> m_annotations[4]; // corner annotations
            vtkSmartPointer<vtkSeedWidget> m_seedWidgets[4]; // seed widgets for the anatomical views
            vtkSmartPointer<vtkActor2D> m_vecText[12]; // 4 guide/orientation letters for each view (3 views in total)
            //std::array<std::vector<vtkSmartPointer<vtkActor>>, 4> m_seedActors; // actors which represent the seed in the 3D view (axial, sagittal & coronal)
            std::vector<vtkSmartPointer<vtkActor>> m_seedActors; // actors which represent the seed in the 3D view (axial, sagittal & coronal)
            vtkSmartPointer<WindowRendererCallBack> windowRendererCommand;
            vtkSmartPointer<vtkLight> m_light;
            vtkSmartPointer<vtkCamera> camera;

            const double SEED_NODE_SIZE = 4.0;
            vtkSmartPointer<vtkActor> seedActor;
            const double SEED_ACTOR_COLOR[3] = { 0.76, 0.96, 0.22 };
            vtkSmartPointer<vtkActor> nodeSelectedActor;
            const double NODE_SELECTED_ACTOR_COLOR[3] = { 0.9, 0.3, 0.8 };
            const double PATH_NODE_SIZE = 1.0;
            const double PATH_SELECTED_ACTOR_COLOR[3] = { 0.2, 0.6, 0.3 };

            //  FUNCTIONS  
            void initVariable();
            void initWidgets();
            void initAppManager();
            void initViews();
            void loadDICOMImg(vtkSmartPointer<vtkImageData> dicomImg);
            void updateWidgetViews();
            void changeSliderValue(int view, int value);
            void setVisibleView(int view, bool setVisible);
            void loadObjFile();
            void enableSeedsSelection(bool bValue);
            void findNearbyNode(double pos[]);

          
        public slots:

            //  MENU BAR
            virtual void openDICOMDlg();
            virtual void setSeeds(bool);

            virtual void showAllView();
            virtual void showOnly3D();
            virtual void showOnlyAxial();
            virtual void showOnlySagittal();
            virtual void showOnlyCoronal();

            //  TOOL BAR
            virtual void showOBJ(bool);
            virtual void showAxial(bool);
            virtual void showSagittal(bool);
            virtual void showCoronal(bool);
            virtual void showAnnotations(bool);
            virtual void showFull3D(bool);
            //virtual void showSeeds(bool);

            //  WORKING MENU
            virtual void segmentation();
            virtual void showSkeleton(bool);
            virtual void showLines(bool);
            virtual void showSegments(bool);

            virtual void setNum(int);
            virtual void showRoute(bool);

            virtual void prevVideo();  
            virtual void playVideo();
            virtual void stopVideo();
            virtual void nextVideo(); 
            virtual void resetVideo();
            virtual void forkImgGen();

            //  VIEW
            virtual void sliderAxialChanged(int);
            virtual void sliderSagittalChanged(int);
            virtual void sliderCoronalChanged(int);

            //  COMBO BOX
            void bronchoTreeNodeComboBoxIndexChanged(int);
        };

    }
}

#endif // MAINVIEW_H
