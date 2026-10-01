#include "mainview.h"



namespace bronchox::ui {

#pragma region CONSTRUCTOR_DESTRUCTOR

    /// @param strConfigFile absolute path for the Config file
    /// @param parent main widget which represents the parent window
    MainView::MainView(QWidget* parent) : QMainWindow(parent), m_pUI(new Ui::MainView){
        initVariable();
        initWidgets();
        initAppManager();
        initViews();
    }

    MainView::~MainView() {
        if (m_pUI != nullptr)
            delete m_pUI;
    }

#pragma endregion CONSTRUCTOR_DESTRUCTOR

#pragma region MAINVIEW_FUNCTION
    void MainView::initVariable() {
        m_pUI->setupUi(this);
    }

    void MainView::initAppManager() {
        BRONCHOX_INFO("BRONCHOX STATUS: INIT APP CONTROLLER");

        controller = std::make_unique<manager::AppManager>();
    }

    void MainView::initWidgets() {
        BRONCHOX_INFO("BRONCHOX STATUS: INIT WIDGETS");

        widgets = new QVTKOpenGLWidget * [4]; // sorting 4 views in the widget
        widgets[0] = m_pUI->m_qvtkOpenGLWidget;
        widgets[1] = m_pUI->m_qvtkOpenGLWidgetAxial;
        widgets[2] = m_pUI->m_qvtkOpenGLWidgetSagittal;
        widgets[3] = m_pUI->m_qvtkOpenGLWidgetCoronal;

        bronchoTreeNodeComboBox = new QComboBox(this);
        m_pUI->toolBar->addWidget(bronchoTreeNodeComboBox);

    }

    void MainView::initViews() {
        BRONCHOX_INFO("BRONCHOX STATUS: INIT RENDERE VARIABLES");

        for (int k = 0; k < VIEW::TOTAL_VIEWS; k++) {
            m_rendersWindow[k] = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
            widgets[k]->SetRenderWindow(m_rendersWindow[k]);

            m_renders[k] = vtkSmartPointer<vtkRenderer>::New();
            widgets[k]->GetRenderWindow()->AddRenderer(m_renders[k]);

            // m_renders[k]->SetBackground(1, 1, 1);
            m_renders[k]->SetBackground(0, 0, 0);

            vtkNew<vtkInteractorStyleTrackballCamera> cameraStyle;
            // cameraStyle->AutoAdjustCameraClippingRangeOn();
            // vtkNew<vtkInteractorStyleFlight> cameraStyle;
            widgets[k]->GetInteractor()->SetRenderWindow(m_rendersWindow[k]);
            widgets[k]->GetInteractor()->SetInteractorStyle(cameraStyle);
            widgets[k]->GetInteractor()->Initialize();

            if (k >= VIEW::AXIAL) {
                // m_viewers[k] = vtkSmartPointer<vtkImageViewer2>::New();
                m_viewers[k] = nullptr;
                m_annotations[k] = nullptr;
                m_planes[k] = nullptr;
                m_seedWidgets[k] = nullptr;
            }
        }
        m_cubeWidget = nullptr;

        std::fill(std::begin(m_vecText), std::end(m_vecText), nullptr);

        // depth peeling on the main render
        int maxPeels = 50;
        //(0.0 means a perfect image, *>0.0 means a non - perfect image which in general results in faster rendering)
        double occulusionRatio = 0.1; 

        // 1. Use a render dWindowValue with alpha bits (as initial value is 0
        // (false)):
        m_renders[VIEW::MAIN]->GetRenderWindow()->SetAlphaBitPlanes(true);

        // 2. Force to not pick a framebuffer with a multisample buffer
        // (as initial value is 8):
        m_renders[VIEW::MAIN]->GetRenderWindow()->SetMultiSamples(0);

        // 3. Choose to use depth peeling (if supported) (initial value is 0 (false)):
        m_renders[VIEW::MAIN]->SetUseDepthPeeling(true);

        // 4. Set depth peeling parameters
        // - Set the maximum number of rendering passes (initial value is 4):
        m_renders[VIEW::MAIN]->SetMaximumNumberOfPeels(maxPeels);
        // - Set the occlusion ratio (initial value is 0.0, exact image):
        m_renders[VIEW::MAIN]->SetOcclusionRatio(occulusionRatio);
       // m_renders[0]->GetActiveCamera()->SetViewAngle(100);

        //  Configure window render interactor -> for thread
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = m_rendersWindow[VIEW::MAIN]->GetInteractor();
        renderWindowInteractor->Initialize();
        windowRendererCommand = vtkNew<WindowRendererCallBack>();
        windowRendererCommand->setup(m_renders[VIEW::MAIN]);
        renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, windowRendererCommand);

        //  Initialize combobox
        bronchoTreeNodeComboBox->addItem("RUTA 1");
        bronchoTreeNodeComboBox->addItem("RUTA 2");
        bronchoTreeNodeComboBox->addItem("RUTA 3");

        connect(bronchoTreeNodeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(bronchoTreeNodeComboBoxIndexChanged(int)));
    }

    void MainView::loadDICOMImg(vtkSmartPointer<vtkImageData> dicomImg) {
        BRONCHOX_INFO("BRONCHOX STATUS: LOAD DICOM IMAGE");

        auto dimensions = dicomImg->GetDimensions();

        double range[2];
        dicomImg->GetScalarRange(range);
        double dWindowValue = range[1] - range[0];
        double dLevelValue = 0.5 * (range[1] + range[0]);

        vtkNew<vtkLookupTable> lut;
        lut->SetTableRange(range[0], range[1]);
        lut->SetAlphaRange(.60, .60);  // SetAlphaRange(.35, .35)
        lut->SetValueRange(0, 1);      // from black to white
        lut->SetSaturationRange(0, 0); // no color saturation
        lut->SetRampToLinear();
        lut->Build();

        // remove all current actors
        m_renders[VIEW::MAIN]->RemoveAllViewProps();

        // text annotations (indicates the current slice)
        auto fcnSetupAnnotation = [&](int iIndex, std::string strText) {  // function to create annotation
            if (m_annotations[iIndex] == nullptr) // 1st time
            {
                m_annotations[iIndex] = vtkSmartPointer<vtkCornerAnnotation>::New();
                m_annotations[iIndex]->SetLinearFontScaleFactor(2);
                m_annotations[iIndex]->SetNonlinearFontScaleFactor(1);
                m_annotations[iIndex]->SetMaximumFontSize(14);
            }
            else {
                m_annotations[iIndex]->ClearAllTexts();
            }
            m_annotations[iIndex]->SetText(2, strText.c_str()); // 2 = upper left
            m_annotations[iIndex]->GetTextProperty()->SetColor(0.937, 0.941, 0.945);
            m_annotations[iIndex]->GetTextProperty()->ShadowOn();
        };

        fcnSetupAnnotation(VIEW::AXIAL, std::to_string((dimensions[2] / 2)) + "/" + std::to_string(dimensions[2]));
        fcnSetupAnnotation(VIEW::SAGITTAL, std::to_string((dimensions[1] / 2)) + " / " + std::to_string(dimensions[1]));
        fcnSetupAnnotation(VIEW::CORONAL, std::to_string((dimensions[0] / 2)) + " / " + std::to_string(dimensions[0]));

        // anatomical cutting planes into 3D view
        auto fcnSetupAnatomicalPlanes = [&](int iIndex, int iSliceIndex,
            int iOrientation) {
                if (m_planes[iIndex] == nullptr) {
                    m_planes[iIndex] = vtkSmartPointer<vtkImagePlaneWidget>::New();
                    m_planes[iIndex]->DisplayTextOff();
                    m_planes[iIndex]->RestrictPlaneToVolumeOn();
                    m_planes[iIndex]->SetDefaultRenderer(m_renders[VIEW::MAIN]);
                    m_planes[iIndex]->SetInteractor(m_renders[VIEW::MAIN]->GetRenderWindow()->GetInteractor());
                }
                m_planes[iIndex]->SetInputData(dicomImg);
                m_planes[iIndex]->SetLookupTable(lut);
                m_planes[iIndex]->SetWindowLevel(6000, 0);
                m_planes[iIndex]->SetPlaneOrientation(iOrientation);
                m_planes[iIndex]->SetSliceIndex(iSliceIndex);
        };

        fcnSetupAnatomicalPlanes(VIEW::AXIAL, static_cast<int>(dimensions[2] / 2), 2); // 2 = Z axis
        fcnSetupAnatomicalPlanes(VIEW::SAGITTAL, static_cast<int>(dimensions[1] / 2), 1); // 1 = Y axis
        fcnSetupAnatomicalPlanes(VIEW::CORONAL, static_cast<int>(dimensions[0] / 2), 0); // 0 = X axis

        // set the border's color
        m_planes[VIEW::AXIAL]->GetPlaneProperty()->SetColor(1, 0, 0);
        m_planes[VIEW::SAGITTAL]->GetPlaneProperty()->SetColor(0, 1, 0);
        m_planes[VIEW::CORONAL]->GetPlaneProperty()->SetColor(0, 0, 1);

        // create the views axial, sagittal and coronal
        auto fcnSetupViewers = [=](int iIndex, int iSlice, int iOrientation) {
            if (m_viewers[iIndex] == nullptr) {
                m_viewers[iIndex] = vtkSmartPointer<vtkImageViewer2>::New();
                m_viewers[iIndex]->SetRenderer(m_renders[iIndex]);
                m_viewers[iIndex]->SetRenderWindow(m_rendersWindow[iIndex]);
                m_viewers[iIndex]->SetupInteractor(m_rendersWindow[iIndex]->GetInteractor());
                m_viewers[iIndex]->GetInteractorStyle()->AutoAdjustCameraClippingRangeOff(); // to fit in the range (important!)
            }

            m_viewers[iIndex]->SetInputData(dicomImg);
            m_viewers[iIndex]->SetSliceOrientation(iOrientation);   // SetSliceOrientationToXY();
            m_viewers[iIndex]->SetSlice(iSlice); // set at center
            m_viewers[iIndex]->SetColorWindow(dWindowValue);
            m_viewers[iIndex]->SetColorLevel(dLevelValue);
            

        };

        fcnSetupViewers(VIEW::AXIAL, static_cast<int>(dimensions[2] / 2), vtkImageViewer2::SLICE_ORIENTATION_XY);
        fcnSetupViewers(VIEW::SAGITTAL, static_cast<int>(dimensions[1] / 2), vtkImageViewer2::SLICE_ORIENTATION_XZ);
        fcnSetupViewers(VIEW::CORONAL, static_cast<int>(dimensions[0] / 2), vtkImageViewer2::SLICE_ORIENTATION_YZ);

        // 4 letters in a plane-image view representing the spatial orientation
        auto fcnCreateTextIndicator = [=](std::string strLetters, int iStart,
            int iIndex) {
                const int N = 4;
                vtkNew<vtkTextMapper> txtIndicators[N];
                for (int k = 0; k < N; k++) {
                    char* c = new char[2];
                    c[0] = strLetters[k];
                    c[1] = '\0';
                    txtIndicators[k]->SetInput(c);
                    txtIndicators[k]->GetTextProperty()->SetJustificationToCentered();

                    m_vecText[iStart + k] = vtkSmartPointer<vtkActor2D>::New();
                    m_vecText[iStart + k]->SetMapper(txtIndicators[k]);
                    m_vecText[iStart + k]
                        ->GetPositionCoordinate()
                        ->SetCoordinateSystemToNormalizedViewport();
                    // m_vecText[iStart + k]->GetPositionCoordinate()->SetValue(0.5, 0.945);
                    delete[] c;
                }
                m_vecText[iStart + 0]->GetPositionCoordinate()->SetValue(0.5, 0.945);
                m_vecText[iStart + 1]->GetPositionCoordinate()->SetValue(0.5, 0.005);
                m_vecText[iStart + 2]->GetPositionCoordinate()->SetValue(0.02, 0.5);
                m_vecText[iStart + 3]->GetPositionCoordinate()->SetValue(0.985, 0.5);

                for (int k = 0; k < N; k++)
                    m_viewers[iIndex]->GetRenderer()->AddActor2D(m_vecText[iStart + k]);
        };

        fcnCreateTextIndicator("APRL", 0, VIEW::AXIAL);
        fcnCreateTextIndicator("SIAP", 4, VIEW::SAGITTAL);
        fcnCreateTextIndicator("SIRL", 8, VIEW::CORONAL);

        // add annotations once viewers were created
        m_viewers[VIEW::AXIAL]->GetRenderer()->AddViewProp(m_annotations[VIEW::AXIAL]);
        m_viewers[VIEW::SAGITTAL]->GetRenderer()->AddViewProp( m_annotations[VIEW::SAGITTAL]);
        m_viewers[VIEW::CORONAL]->GetRenderer()->AddViewProp(m_annotations[VIEW::CORONAL]);




        // set colors (from 0-1 in RGB) on each viewport window
        CUtility::getInstance()->viewportBorderColor(m_renders[VIEW::AXIAL], 1, 0, 0, true);
        CUtility::getInstance()->viewportBorderColor(m_renders[VIEW::SAGITTAL], 0, 1, 0, true);
        CUtility::getInstance()->viewportBorderColor(m_renders[VIEW::CORONAL], 0, 0, 1, true);

        // this static function improves the appearance of the text edges
        // since they are overlaid on a surface rendering of the cube's faces
        vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

        if (m_cubeWidget == nullptr) {
            m_cubeWidget = CUtility::getInstance()->createCubeMarker(m_renders[VIEW::MAIN]); // just once
        }

        m_cubeWidget->EnabledOn();
        m_cubeWidget->InteractiveOff(); // not able to move

        // Create the representation for the seed widget and its handles
        auto fcnSetupSeeds = [&](int iIndex) {
            //vtkSmartPointer<vtkPointHandleRepresentation2D> handleRep = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
            //handleRep->GetProperty()->SetColor(0.239, 0.682, 0.913); // make some color for the marks
            //handleRep->GetProperty()->SetPointSize(5.0);
            vtkSmartPointer<vtkConstrainedPointHandleRepresentation> handle = vtkSmartPointer<vtkConstrainedPointHandleRepresentation>::New();
            handle->GetProperty()->SetColor(0.239, 0.682, 0.913);
            //handle->GetProperty()->SetPointSize(3);
            //handle->GetSelectedProperty()->SetPointSize(3);
            //handle->GetProperty()->SetSelectionPointSize(3);
            handle->SetDragable(false);
            handle->GetProperty()->SetDiffuseColor(1, 0, 0);
            handle->GetProperty()->SetVertexColor(1, 1, 0);
            handle->GetProperty()->SetEdgeColor(0, 1, 1);
            handle->GetProperty()->SetAmbientColor(0, 1, 0);
            handle->GetProperty()->SetSpecularColor(1, 1, 0);

            //vtkSmartPointer<vtkSphereHandleRepresentation> h = vtkSmartPointer<vtkSphereHandleRepresentation>::New();
            //h->GetProperty()->SetColor(1, 0, 0);
            //h->GetProperty()->SetPointSize(5.0);

            vtkSmartPointer<vtkSeedRepresentation> widgetRep = vtkSmartPointer<vtkSeedRepresentation>::New();
            widgetRep->SetHandleRepresentation(handle);

            // Create the seed widgets
            m_seedWidgets[iIndex] = vtkSmartPointer<vtkSeedWidget>::New();
            m_seedWidgets[iIndex]->SetInteractor( m_rendersWindow[iIndex]->GetInteractor());
            m_seedWidgets[iIndex]->SetRepresentation(widgetRep);
        };

        fcnSetupSeeds(VIEW::AXIAL);
        fcnSetupSeeds(VIEW::SAGITTAL);
        fcnSetupSeeds(VIEW::CORONAL);

        // clear the seedwidget Actors
        //for_each(m_seedActors.begin(), m_seedActors.end(), [](std::vector<vtkSmartPointer<vtkActor>> theVector) {
        //        theVector.clear();
        //    });
        for (auto actor : m_seedActors) {
            m_renders[VIEW::MAIN]->RemoveActor(actor);
        }
        m_seedActors.clear();

        // interactor to move the seeds once placed (using the mouse)
        vtkNew<vtkSeedInteractionCallback> callbackSeed[3];

        //callbackSeed[0]->SetVectorActor(m_seedActors[VIEW::AXIAL]);
        callbackSeed[0]->SetMainRenderer(m_renders[VIEW::MAIN]);
        m_seedWidgets[VIEW::AXIAL]->AddObserver(vtkCommand::InteractionEvent, callbackSeed[0]);

        //callbackSeed[1]->SetVectorActor(m_seedActors[VIEW::SAGITTAL]);
        callbackSeed[1]->SetMainRenderer(m_renders[VIEW::MAIN]);
        m_seedWidgets[VIEW::SAGITTAL]->AddObserver(vtkCommand::InteractionEvent, callbackSeed[1]);

        //callbackSeed[2]->SetVectorActor(m_seedActors[VIEW::CORONAL]);
        callbackSeed[2]->SetMainRenderer(m_renders[VIEW::MAIN]);
        m_seedWidgets[VIEW::CORONAL]->AddObserver(vtkCommand::InteractionEvent, callbackSeed[2]);

        callbackSeed[0]->SetOtherSeedWitdget(m_seedWidgets[VIEW::SAGITTAL], m_seedWidgets[VIEW::CORONAL]);
        callbackSeed[1]->SetOtherSeedWitdget(m_seedWidgets[VIEW::AXIAL], m_seedWidgets[VIEW::CORONAL]);
        callbackSeed[2]->SetOtherSeedWitdget(m_seedWidgets[VIEW::AXIAL], m_seedWidgets[VIEW::SAGITTAL]);
        callbackSeed[0]->SetOtherRenderer(m_renders[VIEW::SAGITTAL], m_renders[VIEW::CORONAL]);
        callbackSeed[1]->SetOtherRenderer(m_renders[VIEW::AXIAL], m_renders[VIEW::CORONAL]);
        callbackSeed[2]->SetOtherRenderer(m_renders[VIEW::AXIAL], m_renders[VIEW::SAGITTAL]);


        // Picker to pick pixels
        auto fcnSetupPicker = [=](int iIndex) {
            vtkNew<vtkPropPicker> propPicker;
            propPicker->PickFromListOn();
            propPicker->InitializePickList(); // give the picker a prop to pick

            m_viewers[iIndex]->GetImageActor()->InterpolateOff(); // optional?
            propPicker->AddPickList(m_viewers[iIndex]->GetImageActor()); // ctor should exists

            // Callback listens to MouseMoveEvents invoked by the interactor's style
            vtkSmartPointer<vtkImageInteractionCallback> callback = vtkSmartPointer<vtkImageInteractionCallback>::New();
            callback->SetViewer(m_viewers[iIndex]);
            callback->SetPicker(propPicker);
            callback->SetCornerAnnotation(m_annotations[iIndex]);

            m_viewers[iIndex]->GetInteractorStyle()->AddObserver(vtkCommand::MouseMoveEvent, callback); // add the observer
        };

        fcnSetupPicker(VIEW::AXIAL);
        fcnSetupPicker(VIEW::SAGITTAL);
        fcnSetupPicker(VIEW::CORONAL);

        // set slider values at the center
        std::array<int, 6> ranges = std::array<int, 6>{
            m_viewers[VIEW::AXIAL]->GetSliceMin() + 1,
            m_viewers[VIEW::AXIAL]->GetSliceMax() + 1,
            m_viewers[VIEW::SAGITTAL]->GetSliceMin() + 1,
            m_viewers[VIEW::SAGITTAL]->GetSliceMax() + 1,
            m_viewers[VIEW::CORONAL]->GetSliceMin() + 1,
            m_viewers[VIEW::CORONAL]->GetSliceMax() + 1
        };

        m_pUI->sliderAxial->setRange(ranges[0], ranges[1]);
        m_pUI->sliderAxial->setValue(ranges[1] / 2);

        m_pUI->sliderSagittal->setRange(ranges[2], ranges[3]);
        m_pUI->sliderSagittal->setValue(ranges[3] / 2);

        m_pUI->sliderCoronal->setRange(ranges[4], ranges[5]);
        m_pUI->sliderCoronal->setValue(ranges[5] / 2);

        // enable the anatomical planes into the 3D view
        m_planes[VIEW::AXIAL]->EnabledOff();
        m_planes[VIEW::SAGITTAL]->EnabledOff();
        m_planes[VIEW::CORONAL]->EnabledOff();

        m_planes[VIEW::AXIAL]->EnabledOn();
        m_planes[VIEW::SAGITTAL]->EnabledOn();
        m_planes[VIEW::CORONAL]->EnabledOn();

        // disable the windows/level and picking
        m_planes[VIEW::AXIAL]->InteractionOff();
        m_planes[VIEW::SAGITTAL]->InteractionOff();
        m_planes[VIEW::CORONAL]->InteractionOff();

        m_pUI->action3Dairways->setChecked(false);
        m_pUI->actionAxial->setChecked(true);
        m_pUI->actionSagittal->setChecked(true);
        m_pUI->actionCoronal->setChecked(true);
        m_pUI->actionAnnotations->setChecked(true);
        m_pUI->actionFullScreen->setChecked(false);

        m_renders[VIEW::AXIAL]->GetActiveCamera()->SetViewUp(0, -1, 0);
        //m_renders[VIEW::SAGITTAL]->GetActiveCamera()->SetViewUp(0, -1, -1);
        //m_renders[VIEW::CORONAL]->GetActiveCamera()->SetViewUp(0, -1, -1);
        m_renders[VIEW::MAIN]->GetActiveCamera()->SetViewUp(0, -1, 0);


        updateWidgetViews();


        navigation.setRender(m_renders[VIEW::MAIN]);
    }
    
    void MainView::updateWidgetViews() {
        BRONCHOX_INFO("BRONCHOX STATUS: UPDATE 4 VIEWS");
        std::for_each(m_renders, m_renders + TOTAL_VIEWS, [](auto render) { 
            render->ResetCamera();
            render->GetRenderWindow()->Render(); 
            }
        );
    }

    void MainView::changeSliderValue(int view, int value) {
        if (controller->isDataLoaded()) {
            m_viewers[view]->SetSlice(value - 1);
            m_planes[view]->SetSliceIndex(value - 1);
            std::string strText;

            strText = "(0, 0): 0\n" + std::to_string(value) + " / " + std::to_string(m_viewers[view]->GetSliceMax() + 1);
            m_annotations[view]->ClearAllTexts();
            m_annotations[view]->SetText(2, strText.c_str());

            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
            m_renders[view]->GetRenderWindow()->Render();

            //m_renders[VIEW::AXIAL]->GetRenderWindow()->Render();
            //m_renders[VIEW::SAGITTAL]->GetRenderWindow()->Render();
            //m_renders[VIEW::CORONAL]->GetRenderWindow()->Render();
        }
    }

    void MainView::setVisibleView(int view, bool setVisible) {
        if (controller->isDataLoaded()) {
            if (setVisible) {
                m_planes[view]->EnabledOn();
            }
            else {
                m_planes[view]->EnabledOff();
            }
            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
        }
    }

    void MainView::loadObjFile() {

        std::string objFilePath = controller->getDataOutputDir() + "\\airwaySeg.obj";
        std::string matlabFilePath = controller->getDataOutputDir() + "\\GVal.mat";

        BRONCHOX_INFO("BRONCHOX STATUS: LOAD OBJ FILE (" + objFilePath + ")");

        std::ifstream fileObj(objFilePath);
        if (fileObj.good()) {
            if (bronchoCore.loadOBJ(objFilePath, false) == false){
                BRONCHOX_INFO("BRONCHOX STATUS: LOAD OBJ FILE FAILED (" + objFilePath + ")");
            }
            else {
                m_renders[VIEW::MAIN]->AddActor(bronchoCore.getOBJActor());


                std::ifstream fileMat(matlabFilePath);
                if (fileMat.good()) {
                    if (bronchoCore.loadMatlabFile(matlabFilePath, false) == false) {
                        BRONCHOX_INFO("BRONCHOX STATUS: LOAD MATLAB FILE FAILED (" + matlabFilePath + ")");
                    }
                    else {

                        //  Configure camera and light --> we need fix this, investigate how take this apart to initView function 
                        m_renders[VIEW::MAIN]->ResetCamera();
                        // m_renders[VIEW::MAIN]->UseFXAAOn();
                        camera = m_renders[VIEW::MAIN]->GetActiveCamera();
                        double dNear, dFar;
                        //camera->SetViewUp(0, 0, -1);
                        camera->GetClippingRange(dNear, dFar);
                        camera->SetClippingRange(0.015, dFar);
                       
                        //camera->OrthogonalizeViewUp();
                        //camera->SetViewUp(0, 0, -1);

                        //camera->SetViewAngle(75);
                        if (m_light == nullptr) {
                            m_light = vtkSmartPointer<vtkLight>::New();
                            double LIGHT_COLOR[3] = { 1.5, 1.5, 1.5 };
                            m_light->SetAttenuationValues(0.75, 0.0002, 0.0005);
                            m_light->SetLightTypeToHeadlight();
                            m_light->SetColor(1.5, 1.5, 1.5);
                            m_light->SetConeAngle(90.0);
                            m_light->PositionalOn();
                            m_renders[VIEW::MAIN]->AddLight(m_light);
                        }

                        navigation.setTree(bronchoCore);

                        double* pos = controller->readPathNodeFile();
                        if (pos != nullptr) {
                            findNearbyNode(pos);
                        }
                    }
                }
            }
        }
    }

    /// Enable/Disable the seedSelection. When is disable, it allows to move each
    /// side. Also, it is not able to delete them
    /// @param bValue true to enable, false to disable
    void MainView::enableSeedsSelection(bool bValue) {
        if (bValue) {
            auto fcnProcessSeeds = [](vtkSmartPointer<vtkSeedWidget> m_seedWidget) {
                if (m_seedWidget->GetEnabled()) {
                    m_seedWidget->RestartInteraction();
                }
                else {
                    m_seedWidget->EnabledOn();
                }
            };
            fcnProcessSeeds(m_seedWidgets[VIEW::AXIAL]);
            fcnProcessSeeds(m_seedWidgets[VIEW::SAGITTAL]);
            fcnProcessSeeds(m_seedWidgets[VIEW::CORONAL]);
        }
        else {

            if (seedActor != nullptr) {
                m_renders[VIEW::MAIN]->RemoveActor(seedActor);
                seedActor = nullptr;
            }

            double pos[3] = { -1, -1, -1 };
            auto fncGetSpherePos = [&](vtkSmartPointer<vtkSeedWidget> m_seedWidget, double pos[3]) {
                m_seedWidget->CompleteInteraction();
                if (pos[0] == -1) {
                    vtkSmartPointer<vtkSeedRepresentation> seedRep = static_cast<vtkSeedRepresentation*>(m_seedWidget->GetRepresentation());
                    int n = seedRep->GetNumberOfSeeds();
                    if (n != 0) {
                        seedRep->GetSeedWorldPosition(0, pos);
                    }
                }
            };

            fncGetSpherePos(m_seedWidgets[VIEW::AXIAL], pos);
            fncGetSpherePos(m_seedWidgets[VIEW::SAGITTAL], pos);
            fncGetSpherePos(m_seedWidgets[VIEW::CORONAL], pos);

            if (pos[0] != -1) {
                vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
                sphereSource->SetCenter(pos[0], pos[1], pos[2]);
                sphereSource->SetRadius(SEED_NODE_SIZE);
                sphereSource->Update();

                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputConnection(sphereSource->GetOutputPort());

                seedActor = vtkSmartPointer<vtkActor>::New();
                seedActor->SetMapper(mapper);
                seedActor->GetProperty()->SetColor(SEED_ACTOR_COLOR[0], SEED_ACTOR_COLOR[1], SEED_ACTOR_COLOR[2]); // a color to notice in the anatomical planes

                // m_seedActors.push_back(actor); // actor is added

                m_renders[VIEW::MAIN]->AddActor(seedActor);
                seedActor->SetVisibility(m_pUI->checkShowRoute->isChecked());
                m_renders[VIEW::MAIN]->GetRenderWindow()->Render();

                findNearbyNode(pos);
            }
        }
    }

    /// <summary>
    /// Call function findNearbyNode of bronchoCore and find 3 nodes of bronchoTree closest to the parameter point
    /// </summary>
    /// <param name="pos">point selected by user in 3d space</param>
    void MainView::findNearbyNode(double pos[]) {
        controller->writePathNodeFile(pos);
        if (bronchoCore.isDataloaded()) {
            bronchoCore.findNearbyNode(pos);
            bronchoTreeNodeComboBoxIndexChanged(bronchoTreeNodeComboBox->currentIndex());
        }
    }


#pragma endregion MAINVIEW_FUNCTION

#pragma region slot_function

    //  MENU BAR

    /// <summary>
    /// UI Function: Open DICOM dialog, if user select a DICOM file then prepare window viewer
    /// </summary>
    void MainView::openDICOMDlg() {
        BRONCHOX_INFO("BRONCHOX STATUS: OPEN DICOM DIALOG");
        navigation.stop();
        CDialogDICOM* dlg = new CDialogDICOM(this);
        if (int result = dlg->exec(); result == 1) {
            controller->setDicomData(dlg->getDICOMdir(), dlg->getMetaDICOM(), dlg->getImgDICOM(), dlg->getDICOMSerieSelected());
            loadDICOMImg(dlg->getImgDICOM());
            bronchoCore.bronchoCoreReset();
            bronchoCore.setSpacing(dlg->getSpacing());
        }
        delete dlg;
    }

    /// <summary>
    /// UI Function: enable selection of seed in axial, coronal and saggital viewer
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::setSeeds(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SELECT NODE: ", bStatus);
        if (controller->isDataLoaded()) {
            enableSeedsSelection(bStatus);
        }
    }

    /// <summary>
    /// UI Function: show all viewer
    /// </summary>
    void MainView::showAllView() {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ALL VIEWS");
        m_pUI->actionFullScreen->setChecked(false);
        m_pUI->sliderAxial->show();
        m_pUI->sliderSagittal->show();
        m_pUI->sliderCoronal->show();
        m_pUI->m_qvtkOpenGLWidgetAxial->show();
        m_pUI->m_qvtkOpenGLWidgetSagittal->show();
        m_pUI->m_qvtkOpenGLWidgetCoronal->show();
        m_pUI->m_qvtkOpenGLWidget->show();
        m_pUI->m_qvtkOpenGLWidget->update();
    }

    /// <summary>
    /// UI Function:Show only main viewer (3d)
    /// </summary>
    void MainView::showOnly3D() {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ONLY 3D VIEW");
        m_pUI->actionFullScreen->setChecked(true);
        m_pUI->sliderAxial->hide();
        m_pUI->sliderSagittal->hide();
        m_pUI->sliderCoronal->hide();
        m_pUI->m_qvtkOpenGLWidgetAxial->hide();
        m_pUI->m_qvtkOpenGLWidgetSagittal->hide();
        m_pUI->m_qvtkOpenGLWidgetCoronal->hide();
        m_pUI->m_qvtkOpenGLWidget->show();
        m_pUI->m_qvtkOpenGLWidget->update();
    }

    /// <summary>
    /// UI Function: Show only axial viewer
    /// </summary>
    void MainView::showOnlyAxial() {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ONLY AXIAL VIEW");
        m_pUI->actionFullScreen->setChecked(false);
        m_pUI->sliderAxial->show();
        m_pUI->sliderSagittal->hide();
        m_pUI->sliderCoronal->hide();
        m_pUI->m_qvtkOpenGLWidgetAxial->show();
        m_pUI->m_qvtkOpenGLWidgetSagittal->hide();
        m_pUI->m_qvtkOpenGLWidgetCoronal->hide();
        m_pUI->m_qvtkOpenGLWidget->hide();
        m_pUI->m_qvtkOpenGLWidget->update();
    }


    /// <summary>
    /// UI Function: Show only sagittal viewer
    /// </summary>
    void MainView::showOnlySagittal() {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ONLY SAGITTAL VIEW");
        m_pUI->actionFullScreen->setChecked(false);
        m_pUI->sliderAxial->hide();
        m_pUI->sliderSagittal->show();
        m_pUI->sliderCoronal->hide();
        m_pUI->m_qvtkOpenGLWidgetAxial->hide();
        m_pUI->m_qvtkOpenGLWidgetSagittal->show();
        m_pUI->m_qvtkOpenGLWidgetCoronal->hide();
        m_pUI->m_qvtkOpenGLWidget->hide();
        m_pUI->m_qvtkOpenGLWidget->update();
    }

    /// <summary>
    /// UI Function: Show only coronal viewer
    /// </summary>
    void MainView::showOnlyCoronal() {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ONLY CORONAL VIEW");
        m_pUI->actionFullScreen->setChecked(false);
        m_pUI->sliderAxial->hide();
        m_pUI->sliderSagittal->hide();
        m_pUI->sliderCoronal->show();
        m_pUI->m_qvtkOpenGLWidgetAxial->hide();
        m_pUI->m_qvtkOpenGLWidgetSagittal->hide();
        m_pUI->m_qvtkOpenGLWidgetCoronal->show();
        m_pUI->m_qvtkOpenGLWidget->hide();
        m_pUI->m_qvtkOpenGLWidget->update();
    }


    //  TOOL BAR

    /// <summary>
    /// UI Function: Set visibility of obj actor, if actor is not loading, then load this by file
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showOBJ(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW OBJ: ", bStatus);
        if (controller->isDataLoaded()) {
            if (!bronchoCore.isDataloaded()) {
                loadObjFile();
            }

            if (bronchoCore.getOBJActor() != nullptr) {
                bronchoCore.getOBJActor()->SetVisibility(bStatus);
                m_renders[VIEW::MAIN]->GetActiveCamera()->SetViewUp(0, -1, 0);
                m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
            }
        }
    }

    /// <summary>
    /// UI Function: Show axial viewer
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showAxial(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW AXIAL VIEW: ", bStatus);
        setVisibleView(VIEW::AXIAL, bStatus);
    }


    /// <summary>
    /// UI Function: Show sagittal viewer
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showSagittal(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW SAGITTAL VIEW: ", bStatus);
       setVisibleView(VIEW::SAGITTAL, bStatus);
    }

    /// <summary>
    /// UI Function: Show coronal viewer
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showCoronal(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW CORONAL VIEW: ", bStatus);
        setVisibleView(VIEW::CORONAL, bStatus);
    }


    /// <summary>
    /// UI Function: Set visibility of annotation
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showAnnotations(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ANNOTATION: ", bStatus);
        if (controller->isDataLoaded()) {
            if (bStatus) {
                std::for_each(std::begin(m_vecText), std::end(m_vecText), [](auto guideText) { guideText->VisibilityOn(); });
                m_annotations[VIEW::AXIAL]->VisibilityOn();
                m_annotations[VIEW::SAGITTAL]->VisibilityOn();
                m_annotations[VIEW::CORONAL]->VisibilityOn();
            }
            else {
                std::for_each(std::begin(m_vecText), std::end(m_vecText), [](auto guideText) { guideText->VisibilityOff(); });
                m_annotations[VIEW::AXIAL]->VisibilityOff();
                m_annotations[VIEW::SAGITTAL]->VisibilityOff();
                m_annotations[VIEW::CORONAL]->VisibilityOff();
            }
            m_renders[VIEW::AXIAL]->GetRenderWindow()->Render();
            m_renders[VIEW::SAGITTAL]->GetRenderWindow()->Render();
            m_renders[VIEW::CORONAL]->GetRenderWindow()->Render();
        }
    }

    /// <summary>
    /// UI Function: Make main viewer full window
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showFull3D(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ONLY 3D VIEW: ", bStatus);
        if (!bStatus) {
            m_pUI->sliderAxial->show();
            m_pUI->sliderSagittal->show();
            m_pUI->sliderCoronal->show();
            m_pUI->m_qvtkOpenGLWidgetAxial->show();
            m_pUI->m_qvtkOpenGLWidgetSagittal->show();
            m_pUI->m_qvtkOpenGLWidgetCoronal->show();
            m_pUI->m_qvtkOpenGLWidget->show();
            m_pUI->m_qvtkOpenGLWidget->update();
        }
        else {
            m_pUI->sliderAxial->hide();
            m_pUI->sliderSagittal->hide();
            m_pUI->sliderCoronal->hide();
            m_pUI->m_qvtkOpenGLWidgetAxial->hide();
            m_pUI->m_qvtkOpenGLWidgetSagittal->hide();
            m_pUI->m_qvtkOpenGLWidgetCoronal->hide();
            m_pUI->m_qvtkOpenGLWidget->show();
            m_pUI->m_qvtkOpenGLWidget->update();
        }
    }

    /// <summary>
    /// UI Function: Start segmentation: generate segmentation data file and obj and path file by matlab script if not exist
    /// </summary>
    void MainView::segmentation() {
        BRONCHOX_INFO("BRONCHOX STATUS: SEGMENTATION");

        if (controller->isDataLoaded()) {
            std::string ctDataFilePath = controller->getDataOutputDir() + "\\CTData.mat";
            std::ifstream fileCTData(ctDataFilePath);
            if (!fileCTData.good()) {
                BRONCHOX_TRACE("BRONCHOX STATUS: CREATE CTData.mat FILE");
                bronchoCore.writeMatlabDataFile_CTData(ctDataFilePath, controller->get_m_imgDICOM());
            }
            else {
                BRONCHOX_TRACE("BRONCHOX STATUS: CTData.mat FILE FOUND");
            }

            if (bronchoCore.openMatlabEngine()){
                std::string airwaySegFilePath = controller->getDataOutputDir() + "\\AirwaySegLeakage.mat";
                std::ifstream fileAirwaySeg(airwaySegFilePath);
                if (!fileAirwaySeg.good()) {
                    BRONCHOX_TRACE("BRONCHOX STATUS: CREATE AirwaySegLeakage.mat FILE");
                    bronchoCore.callMatlabFunction_SegmentationPipeline(controller->getDataOutputDir(), controller->getMatlabScriptPath());
                }
                else {
                    BRONCHOX_TRACE("BRONCHOX STATUS: AirwaySegLeakage.mat FILE FOUND");
                }

                std::string airwaySegObjFilePath = controller->getDataOutputDir() + "\\airwaySeg.obj";
                std::ifstream fileAirwayObjSeg(airwaySegObjFilePath);
                if (!fileAirwayObjSeg.good()) {
                    BRONCHOX_TRACE("BRONCHOX STATUS: CREATE AirwaySegLeakage.obj FILE");
                    bronchoCore.callMatlabFunction_generateObjAndPathFile(controller->getDataOutputDir(), controller->getMatlabScriptPath());
                }
                else {
                    BRONCHOX_TRACE("BRONCHOX STATUS: AirwaySegLeakage.obj FILE FOUND");
                }
                bronchoCore.closeMatlabEngine();
            }
        }
        else {
            BRONCHOX_ERROR("BRONCHOX STATUS: DICOM FILE IS NOT OPEN");
        }
    }

    /// <summary>
    /// UI Function: Set visibility of skeleton actor, if this is not loading then load it
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showSkeleton(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW SKELETON: ", bStatus);
        if (!bronchoCore.isDataloaded()) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (bronchoCore.getSkeletonActor() == nullptr) {
                bronchoCore.getSkeletonPoints(0.5);
                m_renders[VIEW::MAIN]->AddActor(bronchoCore.getSkeletonActor());
            }
            bronchoCore.getSkeletonActor()->SetVisibility(bStatus);
            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
        }
    }


    /// <summary>
    /// UI Function: Set visibility of segment line actor, if this is not loading then load it
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showLines(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW LINE: ", bStatus);
        if (!bronchoCore.isDataloaded()) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (bronchoCore.getSegmentLineActor() == nullptr) {
                bronchoCore.getSegmentsLinesRandomColor(2);
                m_renders[VIEW::MAIN]->AddActor(bronchoCore.getSegmentLineActor());
            }
            bronchoCore.getSegmentLineActor()->SetVisibility(bStatus);
            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
        }
    }

    /// <summary>
    /// UI Function: Set visibility of segment point actor, if this is not loading then load it
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showSegments(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW SEGMENTS: ", bStatus);
        if (!bronchoCore.isDataloaded()) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (bronchoCore.getSegmentPointActor() == nullptr) {
                bronchoCore.getSegmentsPointsRandomColor(0.5);
                m_renders[VIEW::MAIN]->AddActor(bronchoCore.getSegmentPointActor());
            }
            bronchoCore.getSegmentPointActor()->SetVisibility(bStatus);
            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
        }
    }

    /// <summary>
    /// UI Function: Set visibility of router selected actor, if this is not loading then load it
    /// </summary>
    /// <param name="bStatus"></param>
    void MainView::showRoute(bool bStatus) {
        BRONCHOX_INFO("BRONCHOX STATUS: SHOW ROUTER: ", bStatus);
        if (!bronchoCore.isDataloaded() || bronchoCore.getNearbyNodes()[0] < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (seedActor != nullptr) {
                seedActor->SetVisibility(bStatus);
            }

            if (nodeSelectedActor != nullptr) {
                nodeSelectedActor->SetVisibility(bStatus);
            }


            if (bronchoCore.getRouterPointActor() != nullptr) {
                bronchoCore.getRouterPointActor()->SetVisibility(bStatus);
                m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
            }
        }
    }

    /// <summary>
    /// UI Function: go to previous node of router
    /// </summary>
    void MainView::prevVideo() {
        BRONCHOX_INFO("BRONCHOX STATUS: PREV SLIDE");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (!navigation.isRunning()) {
                navigation.prev();
                m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
            }
        }
    }

    /// <summary>
    /// UI Function: Reproduce all router path
    /// </summary>
    void MainView::playVideo() {
        
        BRONCHOX_INFO("BRONCHOX STATUS: PLAY SLIDE");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = m_renders[VIEW::MAIN]->GetRenderWindow()->GetInteractor();
            windowRendererCommand->setNavigation(&navigation);
            int timerId = renderWindowInteractor->CreateRepeatingTimer(navigation.getVelocity());
            navigation.setup(camera, m_light);
            navigation.start(timerId);
        }
    }

    /// <summary>
    /// UI Function: Stop reproduce router path
    /// </summary>
    void MainView::stopVideo() {
        BRONCHOX_INFO("BRONCHOX STATUS: STOP SLIDE");
        navigation.stop();
    }

    /// <summary>
    /// UI Function: Go to next point of router path
    /// </summary>
    void MainView::nextVideo() {
        BRONCHOX_INFO("BRONCHOX STATUS: NEXT SLIDE");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            if (!navigation.isRunning()) {
                navigation.next();
                m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
            }
        }
    }


    /// <summary>
    /// UI Function: Reset router path reproduction
    /// </summary>
    void MainView::resetVideo() {
        BRONCHOX_INFO("BRONCHOX STATUS: RESET SLIDE");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            //if (bronchoCore.getNavigation()->isRunning()) { bronchoCore.getNavigation()->stop(); }

            navigation.stop();
            navigation.resetIndex(true);
            navigation.setup(camera, m_light);
           // bronchoCore.getNavigation()->moveToFirstPoint();

            m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
        }
    }

    /// <summary>
    /// UI Function: Set velocity of reproduction of router path
    /// </summary>
    /// <param name="num"></param>
    void MainView::setNum(int num) {
        BRONCHOX_INFO("BRONCHOX STATUS: UPDATE SLIDE VELOCITY");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            navigation.setVelocity(num);
        }
    }

    /// <summary>
    /// UI Function: Export image of fork of router path
    /// </summary>
    void MainView::forkImgGen() {
        BRONCHOX_INFO("BRONCHOX STATUS: GENERATE FORK IMAGE");
        if (!bronchoCore.isDataloaded() || bronchoCore.getNodeSelected() < 0) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
        }
        else {
            
            camera->SetViewAngle(70);
            vector<int> nodePath = bronchoCore.getNodePath();
            std::string dirName = controller->getDataOutputImgDir();
            vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            vtkSmartPointer<vtkActor> nodeActor = vtkSmartPointer<vtkActor>::New();
            glm::dvec3 point;
            for (int i = 0; i < nodePath.size(); i++) {
                point = navigation.moveToNode(i);
                
                if (point.x != -1) {
                    sphereSource->SetCenter(point.x, point.y, point.z);
                    sphereSource->SetRadius(0.5);
                    sphereSource->Update();
                    mapper->SetInputConnection(sphereSource->GetOutputPort());
                    nodeActor->SetMapper(mapper);
                    nodeActor->GetProperty()->SetColor(0.2, 0.6, 0.3);
                    m_renders[VIEW::MAIN]->AddActor(nodeActor);

                    std::string fileName = dirName + std::to_string(nodePath.at(i)) + ".png";
                    m_renders[VIEW::MAIN]->GetRenderWindow()->Render();
                    navigation.saveViewToPng(m_renders[VIEW::MAIN], fileName);
                }

                m_renders[VIEW::MAIN]->RemoveActor(nodeActor);
                nodeActor = nullptr;
                nodeActor = vtkSmartPointer<vtkActor>::New();
            }
        }
    }


    //  VIEW

    /// <summary>
    /// UI Function: Change axial medical image
    /// </summary>
    /// <param name="value"></param>
    void MainView::sliderAxialChanged(int value) {
        changeSliderValue(VIEW::AXIAL, value);
    }

    /// <summary>
    /// UI Function: Change sagittal medical image
    /// </summary>
    /// <param name="value"></param>
    void MainView::sliderSagittalChanged(int value) {
        changeSliderValue(VIEW::SAGITTAL, value);

    }

    /// <summary>
    /// UI Function: Change coronal medical image
    /// </summary>
    /// <param name="value"></param>
    void MainView::sliderCoronalChanged(int value) {
        changeSliderValue(VIEW::CORONAL, value);
    }

    //  COMBO BOX
    /// <summary>
    /// UI Function: Generate router actor by node selected
    /// </summary>
    /// <param name="value"></param>
    void MainView::bronchoTreeNodeComboBoxIndexChanged(int index) {
        BRONCHOX_INFO("BRONCHOX STATUS: UPDATE NODE SELECTED");
        if (!bronchoCore.isDataloaded()) {
            BRONCHOX_ERROR("BRONCHO DATA IS INCOMPLET");
            return;
        }


        bronchoCore.setNodeSelectByIndex(index);

        navigation.stop();
        if (bronchoCore.getRouterPointActor() != nullptr) {
            m_renders[VIEW::MAIN]->RemoveActor(bronchoCore.getRouterPointActor());
        }

        if (nodeSelectedActor != nullptr) {
            m_renders[VIEW::MAIN]->RemoveActor(nodeSelectedActor);
            nodeSelectedActor = nullptr;
        }

        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        double* pos = bronchoCore.getNodePosition(bronchoCore.getNodeSelected());
        sphereSource->SetCenter(pos[0], pos[1], pos[2]);
        sphereSource->SetRadius(SEED_NODE_SIZE);
        sphereSource->Update();
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());
        nodeSelectedActor = vtkSmartPointer<vtkActor>::New();
        nodeSelectedActor->SetMapper(mapper);
        nodeSelectedActor->GetProperty()->SetColor(NODE_SELECTED_ACTOR_COLOR[0], NODE_SELECTED_ACTOR_COLOR[1], NODE_SELECTED_ACTOR_COLOR[2]);
        nodeSelectedActor->SetVisibility(m_pUI->checkShowRoute->isChecked());
        m_renders[VIEW::MAIN]->AddActor(nodeSelectedActor);


        bronchoCore.getRoutePointsTo(bronchoCore.getNodeSelected(), 0.5);
        m_renders[VIEW::MAIN]->AddActor(bronchoCore.getRouterPointActor());
        bronchoCore.getRouterPointActor()->SetVisibility(m_pUI->checkShowRoute->isChecked());
        navigation.setup(camera, m_light);
        // bronchoCore.setNavigationPathPoints();
        navigation.setPath(bronchoCore.getNodeSelected());
        navigation.resetIndex(false);

        m_renders[VIEW::MAIN]->GetRenderWindow()->Render();

    }


#pragma endregion slot_function


};