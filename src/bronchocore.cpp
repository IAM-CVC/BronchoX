#include "bronchocore.h"


CBronchoCore::CBronchoCore()
    : SKIN_COLOR{ 0.9647, 0.5960, 0.4823 }, UNITY_SCALE{ 0.68, 0.68, 0.5 } {
    m_vtkOBJActor = nullptr;
    m_vtkUnityPathActor = nullptr;
}

CBronchoCore::~CBronchoCore() {}

#pragma region MATLAB_FUNCTION

/// <summary>
/// Open matlab engine
/// </summary>
/// <returns></returns>
bool CBronchoCore::openMatlabEngine() {
    BRONCHOX_TRACE("BRONCHOX STATUS: OPEN MATLAB ENGINE");
    if ((ep = engOpen(NULL))) {
        return true;
    }
    BRONCHOX_TRACE("BRONCHOX STATUS: OPEN MATLAB ENGINE FAILED");
    return false;
}

/// <summary>
/// Close matlab engine
/// </summary>
void CBronchoCore::closeMatlabEngine() {
    BRONCHOX_TRACE("BRONCHOX STATUS: CLOSE MATLAB ENGINE");
    engClose(ep);
    ep == nullptr;
}

/// Load the the OBJ geometrical description
/// @param the full path of the file
/// @return true if was possible to load, otherwise false
bool CBronchoCore::loadOBJ(const string& strFilename, bool bFlip = false) {
    // Read the OBJ bronchi
    vtkNew<vtkOBJReader> readerOBJ;
    readerOBJ->SetFileName(strFilename.c_str());
    auto progressFcn = [](vtkObject* caller, long unsigned int eventId,
        void* clientData, void* callData) {
            vtkOBJReader* filter = static_cast<vtkOBJReader*>(caller);
    };
    vtkNew<vtkCallbackCommand> progressCallback;
    progressCallback->SetCallback(progressFcn);
    readerOBJ->AddObserver(vtkCommand::ProgressEvent, progressCallback);
    readerOBJ->Update();

    // enhance the OBJ model, generating new normals
    vtkNew<vtkPolyDataNormals> normalGenerator;
    normalGenerator->SetInputData(readerOBJ->GetOutput());
    normalGenerator->ComputePointNormalsOn();
    normalGenerator->ComputeCellNormalsOff();
    normalGenerator->SetSplitting(0); // I want exactly one normal per vertex
    normalGenerator->Update();

    // create actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(normalGenerator->GetOutputPort());
    m_vtkOBJActor = nullptr; // delete previous actor
    m_vtkOBJActor = vtkSmartPointer<vtkActor>::New();
    m_vtkOBJActor->SetMapper(mapper);

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();            // this is the key line

    transform->Scale(m_spacing[0], m_spacing[1], m_spacing[2]);

    m_vtkOBJActor->SetUserTransform(transform);



    m_vtkOBJActor->GetProperty()->SetOpacity(1.0);
    m_vtkOBJActor->GetProperty()->SetColor(SKIN_COLOR.r, SKIN_COLOR.g, SKIN_COLOR.b);

    if (bFlip) {
        m_vtkOBJActor->RotateZ(180);
    }

    return true;
}


/// Load the the .MAT file (from matlab)
/// @param the full path of the file
/// @return true if was possible to load, otherwise false
bool CBronchoCore::loadMatlabFile(const string& strFilename, bool bFlip) {

    MATFile* pFileMat = matOpen(strFilename.c_str(), "r");
    if (pFileMat == nullptr)
        return false;

    // function to return the proper x, y, z from matlab. Matlab us column-order
    // stored
    auto fcnRowOrderIndex = [](int x, int y, size_t width) -> size_t { // return the linear index of x, y
        return (width * y) + x;
    };

    // load variable GVal
    mxArray* pArray = matGetVariable(pFileMat, "GVal");
    assert(pArray != nullptr);

    // read the points v
    mxArray* pArrToV = mxGetField(pArray, 0, "v"); // this is an array
    const mwSize* dimsV = mxGetDimensions(pArrToV);
    size_t iWidth = dimsV[0]; // number of nodes

    // create tree, starting from 1)
    m_bronchoTree.reset();
    m_bronchoTree.setSize(iWidth + 1);

    // load v points#
    double* dataPoints = mxGetPr(pArrToV);
    for (int k = 0; k < iWidth; k++) {
        glm::dvec3 value;
        value.y = dataPoints[fcnRowOrderIndex(k, 0, iWidth)] * m_spacing[0];
        value.x = dataPoints[fcnRowOrderIndex(k, 1, iWidth)] * m_spacing[1];
        value.z = dataPoints[fcnRowOrderIndex(k, 2, iWidth)] * m_spacing[2];

        if (bFlip) {
            value.y = -value.y;
            value.x = -value.x;
        }

        m_bronchoTree[k + 1].setVPoint(value);
    }

    // get the label is exists
    mxArray* pArrToWL = mxGetField(pArray, 0, "label"); // is an array
    double* labelSegments = nullptr;
    if (pArrToWL != nullptr) {
        labelSegments = mxGetPr(pArrToWL);
        for (int k = 1; k < iWidth; k++) {
            auto p1 = fcnRowOrderIndex(k, 0, iWidth); // quadrant
            auto p2 = fcnRowOrderIndex(k, 1, iWidth); // order

            if (labelSegments[p1] == -1)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::ROOT, labelSegments[p2]);
            if (labelSegments[p1] == 0)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::UNMARKED, labelSegments[p2]);
            if (labelSegments[p1] == 1)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::QUAD_I, labelSegments[p2]);
            if (labelSegments[p1] == 2)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::QUAD_II, labelSegments[p2]);
            if (labelSegments[p1] == 3)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::QUAD_III, labelSegments[p2]);
            if (labelSegments[p1] == 4)
                m_bronchoTree[k + 1].setTypeAndOrder(NODE_TYPE::QUAD_IV, labelSegments[p2]);
        }
    }

    // compute the depth of each node

    // get the field e
    mxArray* pArrToE = mxGetField(pArray, 0, "e");
    const mwSize* dimsE = mxGetDimensions(pArrToE);
    size_t iHeight = dimsE[1]; // or dimsE[0]
    double* pValueOfE = mxGetPr(pArrToE);

    std::vector<int> vecDepthNode(iWidth + 1, -1);
    std::stack<int> stackNodes;
    std::vector<bool> vecVisitedNodes(iWidth + 1, false);
    vecDepthNode[1] = 0; //(depth = 0 is the root, node #1)

    stackNodes.push(1); // push the root node

    // bfs to get the depth on each level of the tree
    while (!stackNodes.empty()) {
        int iNode = stackNodes.top();
        stackNodes.pop();
        vecVisitedNodes[iNode] = true;
        // add all children with current depth + 1
        for (int k = 0; k < iWidth; k++) {
            // check is exists a connection & not visited
            if (pValueOfE[fcnRowOrderIndex(iNode - 1, k, iWidth)] >= 1.0 && !vecVisitedNodes[k + 1]) {
                stackNodes.push(k + 1);
                vecDepthNode[k + 1] = vecDepthNode[iNode] + 1;
            }
        }
    }

    int total = 0;
    // start the loop to get all we points
    for (int x = 0; x < iWidth; x++) {
        // select the node near to the root (less depth)
        int iLessDepth = (int)iWidth; // initializar with maximum depth
        int iIndexNode;
        for (int y = 0; y < iHeight; y++) {
            if (pValueOfE[fcnRowOrderIndex(x, y, iWidth)] >= 1.0) {
                if (vecDepthNode[y + 1] < iLessDepth) {
                    iLessDepth = vecDepthNode[y + 1];
                    iIndexNode = y + 1;
                }
            }
        }
        // iLessDepth represents the value of the less depth found for a particular
        // node's children iIndexNode is the index of the child with less value
        // iIndexNode
        // delete[] pValueOfE;
        // pValueOfE = nullptr;

        // get the field we
        mxArray* pArrToWEX = mxGetField(pArray, 0, "weUniformX");
        mxArray* pArrToWEY = mxGetField(pArray, 0, "weUniformY");
        mxArray* pArrToWEZ = mxGetField(pArray, 0, "weUniformZ");

        int iIndexComplete = 0;
        // stay with the less depth value (the closer to the root) and less then
        // itself
        if (iLessDepth != -1 && iLessDepth < vecDepthNode[x + 1]) {
            m_bronchoTree[x + 1].setParent(iIndexNode); // its parent
            m_bronchoTree[iIndexNode].m_vecChildren.push_back(x + 1);

            mxArray* pCellContentX = mxGetCell(pArrToWEX, fcnRowOrderIndex(iIndexNode - 1, x, iWidth));
            mxArray* pCellContentY = mxGetCell(pArrToWEY, fcnRowOrderIndex(iIndexNode - 1, x, iWidth));
            mxArray* pCellContentZ = mxGetCell(pArrToWEZ, fcnRowOrderIndex(iIndexNode - 1, x, iWidth));

            // now, we get the number of elements of weX, weY, weZ
            const mwSize* pNumElements2 = mxGetDimensions(pCellContentX);
            const mwSize pNumElements = mxGetDimensions(pCellContentX)[1]; // always is 1 x k, where k is the size. then, the
                                   // position 0 is always 1
            double* pValueWEX = mxGetPr(pCellContentX);
            double* pValueWEY = mxGetPr(pCellContentY);
            double* pValueWEZ = mxGetPr(pCellContentZ);

            vtkNew<vtkPoints> points;
            for (int k = 0; k < pNumElements; k++) {
                glm::dvec3 thepoint;
                thepoint.x = pValueWEY[k] * m_spacing[0];
                thepoint.y = pValueWEX[k] * m_spacing[1];
                thepoint.z = pValueWEZ[k] * m_spacing[2];
                m_bronchoTree[x + 1].addWEPoint(thepoint);
                points->InsertNextPoint(thepoint.x, thepoint.y, thepoint.z);
                iIndexComplete++;
            }
            pValueWEX = nullptr;
            pValueWEY = nullptr;
            pValueWEZ = nullptr;
            pCellContentX = nullptr;
            pCellContentY = nullptr;
            pCellContentZ = nullptr;
        }
    }
    mxDestroyArray(pArray);


    if (matClose(pFileMat) != 0) {
        cout << "Error closing the MAT file " << endl;
        return false;
    }


    int iWindow = 4;


    for (int i = 1; i < m_bronchoTree.getNumberOfNodes(); i++) {
        std::vector<int> path = m_bronchoTree.getPathIndexTo(i);

        for (int pathIndex = 1; pathIndex < path.size(); pathIndex++) {
            if (!m_bronchoTree[path.at(pathIndex)].m_sorted) {
                bool isSorted = true;
                if (pathIndex == 1) {
                    isSorted = m_bronchoTree[i].m_vecPoints.at(0).z > m_bronchoTree[i].m_vecPoints.at(m_bronchoTree[i].m_vecPoints.size() - 1).z;
                }
                else {
                    isSorted = m_bronchoTree.calcEuclideanDIstance(
                        m_bronchoTree[m_bronchoTree[path.at(pathIndex)].m_iParent].m_vecPoints.at(m_bronchoTree[m_bronchoTree[path.at(pathIndex)].m_iParent].m_vecPoints.size() - 1),
                        m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(0))
                        < m_bronchoTree.calcEuclideanDIstance(
                            m_bronchoTree[m_bronchoTree[path.at(pathIndex)].m_iParent].m_vecPoints.at(m_bronchoTree[m_bronchoTree[path.at(pathIndex)].m_iParent].m_vecPoints.size() - 1),
                            m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1));
                }

                if (!isSorted) {
                    std::vector<glm::dvec3> sortedPoints;
                    for (int i = m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1; i >= 0; i--) {
                        sortedPoints.push_back(m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(i));
                    }
                    m_bronchoTree[path.at(pathIndex)].m_vecPoints = sortedPoints;
                }
                m_bronchoTree[path.at(pathIndex)].m_sorted = true;
            }
        }

        if (m_bronchoTree[i].m_vecPoints.size() >= iWindow) {
            m_bronchoTree[i].m_vecPoints = m_bronchoTree.smoothingPoints(m_bronchoTree[i].m_vecPoints, iWindow);
        }
    }


    return true;
}

/// <summary>
/// Transform images of DICOM to matlab file
/// </summary>
/// <param name="outputCTDataFilePath"></param>
/// <param name="imgDICOM"></param>
/// <returns></returns>
bool CBronchoCore::writeMatlabDataFile_CTData(std::string outputCTDataFilePath, vtkSmartPointer<vtkImageData> imgDICOM) {
    BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE CTData.mat STARTED");
        // MATFile* pFileMat = matOpen(strFileName.c_str(), "w7.3"); // compressed version
        MATFile* pFileMat = matOpen(outputCTDataFilePath.c_str(), "w");
        if (pFileMat != nullptr) {
            vtkDataArray* imgData = imgDICOM->GetPointData()->GetScalars();
            signed short* pData = static_cast<signed short*>(imgData->GetVoidPointer(0)); // assuming signed short on input data


            int* dimensions = imgDICOM->GetDimensions();

            size_t dims[3];
            dims[0] = dimensions[0];
            dims[1] = dimensions[1];
            dims[2] = dimensions[2];

            mxArray* pArray = mxCreateNumericArray(3, dims, mxSINGLE_CLASS, mxREAL);
            float* pointerRaw = static_cast<float*>(mxGetData(pArray));

            for (int k = 0; k < imgData->GetNumberOfValues(); k++) {
                pointerRaw[k] = pData[k];
            }

            // std::memcpy(pointerRaw, pFloatArray, imgData->GetNumberOfValues() * sizeof(float));
            BRONCHOX_TRACE("Writing volume variable into Matlab's file");
            auto status = matPutVariable(pFileMat, "imaVOL", pArray);

            mxDestroyArray(pArray);
            matClose(pFileMat);

            if (status != 0) {
                BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE CTData.mat FAILED");
                return false;
            }
            BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE CTData.mat DONE");
            return true;
        }
    
    BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE CTData.mat FAILED");
    return false;
}

/// <summary>
/// Call matlab script: call segmentation pipeline which generate all matlab file
/// </summary>
/// <param name="ctDataFolder">path of data file</param>
/// <param name="m_matlabScriptPath">path of matlab script</param>
/// <returns></returns>
bool CBronchoCore::callMatlabFunction_SegmentationPipeline(std::string ctDataFolder, std::string m_matlabScriptPath) {
    BRONCHOX_TRACE("BRONCHOX STATUS: OPEN MATLAB ENGINE");
    //Engine* ep;
    //if ((ep = engOpen(NULL))) {
        BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE OPEN SUCCESS");
        BRONCHOX_TRACE("BRONCHOX STATUS: START SEGMENTATION");

        std::string segmentationScritpFolder = "SegmentationScriptPath = '" + m_matlabScriptPath + "\\Segmentation'";
        std::string dataFolder = "DataFolder = '" + ctDataFolder + "'";
        std::string outputFolder = "OutPutDataFolder = '" + ctDataFolder + "'";

        engEvalString(ep, "clear all");
        engEvalString(ep, segmentationScritpFolder.c_str());
        engEvalString(ep, dataFolder.c_str());
        engEvalString(ep, outputFolder.c_str());
        engEvalString(ep, "addpath(genpath(SegmentationScriptPath))");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 0 SETTING: START");
        if (int res = engEvalString(ep, "SegmentationPipeline_0"); res > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 0 SETTING: FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 0 SETTING: DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 1 (Volume Body ROI, Lungs and Trachea, Vessels): START");
        if (engEvalString(ep, "SegmentationPipeline_1") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 1 (Volume Body ROI, Lungs and Trachea, Vessels): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 1 (Volume Body ROI, Lungs and Trachea, Vessels): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2 (MAIN BRONCHI SEGMENTATION): START");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.1 (Multiresolution Energy and Segmentation): START");
        if (engEvalString(ep, "SegmentationPipeline_2_1") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.1 (Multiresolution Energy and Segmentation): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.1 (Multiresolution Energy and Segmentation): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.2 (MainAirways Energy): START");
        if (engEvalString(ep, "SegmentationPipeline_2_2") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.2 (MainAirways Energy): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.2 (MainAirways Energy): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.3 (Computer score, Leakage Removal): START");
        if (engEvalString(ep, "SegmentationPipeline_2_3") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.3 (Computer score, Leakage Removal): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2.3 (MainAirways Energy): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 2 (MAIN BRONCHI SEGMENTATION): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3 (DISTAL BRONCHI): START");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.1 (Distal Energy): START");
        if (engEvalString(ep, "SegmentationPipeline_3_1") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.1 (Distal Energy): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.1 (Distal Energy): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.2 (LEAKAGE REMOVAL): START");
        if (engEvalString(ep, "SegmentationPipeline_3_2") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.2 (LEAKAGE REMOVAL): FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3.2 (LEAKAGE REMOVAL): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: SEGMENTATION 3 (DISTAL BRONCHI): DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE CLOSE");
        return true;
    //}
    //BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE OPEN FAILED");
    //return false;
}

/// <summary>
/// Call matlab script: generate obj file and matlab file which containing the route information
/// </summary>
/// <param name="ctDataFolder">path of data file</param>
/// <param name="m_matlabScriptPath">path of matlab script</param>
/// <returns></returns>
bool CBronchoCore::callMatlabFunction_generateObjAndPathFile(std::string ctDataFolder, std::string m_matlabScriptPath) {
    //BRONCHOX_TRACE("BRONCHOX STATUS: OPEN MATLAB ENGINE");
    //Engine* ep;
    //if ((ep = engOpen(NULL))) {
        BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE OPEN SUCCESS");
        BRONCHOX_TRACE("BRONCHOX STATUS: START GENERATE OBJ AND PATH FILE");

        std::string segmentationScritpFolder = "SegmentationScriptPath = '" + m_matlabScriptPath + "\\Segmentation2OBJ'";
        std::string dataFolder = "DataFolder = '" + ctDataFolder + "'";
        std::string outputFolder = "OutPutDataFolder = '" + ctDataFolder + "'";

        engEvalString(ep, "clear all");
        engEvalString(ep, segmentationScritpFolder.c_str());
        engEvalString(ep, dataFolder.c_str());
        engEvalString(ep, outputFolder.c_str());
        engEvalString(ep, "addpath(genpath(SegmentationScriptPath))");

        BRONCHOX_TRACE("BRONCHOX STATUS: LOAD imaVOLROI FILE");
        std::string loadimaVOLROIFile = "load('" + ctDataFolder + "\\imaVOLROI.mat')";
        engEvalString(ep, loadimaVOLROIFile.c_str());

        BRONCHOX_TRACE("BRONCHOX STATUS: LOAD AirwaySeg FILE");
        std::string loadAirwaySegFile = "load('" + ctDataFolder + "\\AirwaySegLeakage.mat')";
        engEvalString(ep, loadAirwaySegFile.c_str());


        BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE OBJ FILE");
        if (engEvalString(ep, "CTSeg2OBJ_Script") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE OBJ FILE: FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE OBJ FILE: DONE");

        BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE PATH FILE");
        if (engEvalString(ep, "CreateGraph_script") > 0) {
            BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE PATH FILE: FAILED");
            return false;
        }
        BRONCHOX_TRACE("BRONCHOX STATUS: GENERATE PATH FILE: DONE");


        //engClose(ep);
        BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE CLOSE");
        return true;
    //}
    //BRONCHOX_TRACE("BRONCHOX STATUS: MATLAB ENGINE OPEN FAILED");
    //return false;
}


#pragma endregion

#pragma region GENERATE_ROUTER_ACTOR

/// Return the skeleton/vertex points
/// @param fSphereSize size of the glyph
/// @return a vtkActor of skeleton points
vtkSmartPointer<vtkActor> CBronchoCore::getSkeletonPoints(float fSphereSize) {
    std::vector<glm::dvec3> bronchoTreeNodes = m_bronchoTree.getPath();
    vtkNew<vtkPoints> vec;
    for_each(bronchoTreeNodes.begin(), bronchoTreeNodes.end(), [&](auto node) {
        vec->InsertNextPoint(node.x, node.y, node.z);
        });


    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(vec);

    vtkNew<vtkGlyph3D> glyph;
    vtkNew<vtkSphereSource> sphereSource;
    glyph->SetColorModeToColorByScalar();
    //  glyph->tra
    glyph->ScalingOff();
    glyph->SetSourceConnection(sphereSource->GetOutputPort());
    glyph->SetInputData(polyData);
    glyph->Update();

    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputConnection(glyph->GetOutputPort());
    // pointMapper->SetInputData(polyData);

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    // pointActor->GetProperty()->SetPointSize(fSphereSize);
    pointActor->GetProperty()->SetColor(1, 1, .4);
    //pointActor->SetScale(0.74, 0.74, 0.5);
    m_vtkSkeletonActor = nullptr;
    m_vtkSkeletonActor = pointActor;
    return m_vtkSkeletonActor;
}

/// Get the actor for lines of all segments in tree in different colors
/// @param fSphereSize size of the glyph
/// @return actor representing the lines of tree's segments
vtkSmartPointer<vtkActor> CBronchoCore::getSegmentsLinesRandomColor(float fWidthLine) {
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->Allocate();

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetName("colors");
    colors->SetNumberOfComponents(3);

    //for each point, get this list of points
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    int iLastIndex = 0;

    for (int i = 0; i < m_bronchoTree.getNumberOfNodes(); i++) {
        unsigned char color[3] = { (unsigned char)vtkMath::Random(0, 255), (unsigned char)vtkMath::Random(0, 255), (unsigned char)vtkMath::Random(0, 255) };
        for (int j = 0; j < m_bronchoTree[i].m_vecPoints.size(); j++)
        {
            points->InsertNextPoint(m_bronchoTree[i].m_vecPoints[j].x, m_bronchoTree[i].m_vecPoints[j].y, m_bronchoTree[i].m_vecPoints[j].z);

            if (j > 0)
            {
                vtkIdType line[2]{ iLastIndex - 1, iLastIndex };
                polyData->InsertNextCell(VTK_LINE, 2, line);
                colors->InsertNextTypedTuple(color);
            }
            iLastIndex++;
        }
    }


    polyData->SetPoints(points);
    polyData->GetCellData()->SetScalars(colors);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(0.05, 0.8, 0.05);
    //actor->SetScale(0.74, 0.74, 0.5);

    m_vtkSegmentLineActor = nullptr;
    m_vtkSegmentLineActor = actor;
    return m_vtkSegmentLineActor;
}


/// Get the actor for all segments colored on random colors
/// @param fSphereSize size of the glyph
/// @return actor representing the random colored segments of tree
vtkSmartPointer<vtkActor> CBronchoCore::getSegmentsPointsRandomColor(float fSphereSize) {
    vtkNew<vtkPolyData> polydata;
    vtkNew<vtkPoints> points;
    //vtkNew<vtkUnsignedCharArray> colors;
    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetName("colors");
    colors->SetNumberOfComponents(3);

    // iterate over all nodes in the tree
    for (int i = 1; i < m_bronchoTree.getNumberOfNodes(); i++) {
        unsigned char color[3]{
            (unsigned char)vtkMath::Random(0, 255),
            (unsigned char)vtkMath::Random(0, 255),
            (unsigned char)vtkMath::Random(0, 255)
        };

        //set points and colors
        std::for_each(m_bronchoTree[i].m_vecPoints.begin(), m_bronchoTree[i].m_vecPoints.end(), [&](auto p) {
            points->InsertNextPoint(p.x, p.y, p.z);
            colors->InsertNextTypedTuple(color);
            });
    }

    polydata->SetPoints(points);
    polydata->GetPointData()->SetScalars(colors);



    vtkNew<vtkGlyph3D> glyph;
    vtkNew<vtkSphereSource> sphereSource;
    glyph->SetColorModeToColorByScalar();
    glyph->ScalingOff();
    glyph->SetSourceConnection(sphereSource->GetOutputPort());
    glyph->SetInputData(polydata);
    glyph->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(0.5, 0.05, 0.05);
    actor->GetProperty()->SetPointSize(10);
    //actor->SetScale(0.74, 0.74, 0.5);

    m_vtkSegmentPointActor = nullptr;
    m_vtkSegmentPointActor = actor;
    return m_vtkSegmentPointActor;
}

/// Return the vertex points from root to a node iEndNode
/// @param iEndNode indicates the ending node to reach
/// @param fSphereSize size of the glyph
/// @return a vtkActor of points int he route
vtkSmartPointer<vtkActor> CBronchoCore::getRoutePointsTo(int iEndNode, float fSphereSize) {
    std::vector<glm::dvec3> routerPoints = m_bronchoTree.getFullPathTo(iEndNode);
    vtkNew<vtkPoints> vec;
    for_each(routerPoints.begin(), routerPoints.end(), [&](auto p) {
        vec->InsertNextPoint(p.x, p.y, p.z);
        });



    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(vec);

    vtkNew<vtkGlyph3D> glyph;
    vtkNew<vtkSphereSource> sphereSource;
    glyph->SetColorModeToColorByScalar();
    glyph->ScalingOff();
    glyph->SetSourceConnection(sphereSource->GetOutputPort());
    glyph->SetInputData(polyData);
    glyph->Update();

    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputConnection(glyph->GetOutputPort());
    // pointMapper->SetInputData(polyData);

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetPointSize(fSphereSize);
    pointActor->GetProperty()->SetColor(0.9, 0.9, 0.9);
    //pointActor->SetScale(0.74, 0.74, 0.5);

    m_vtkRouterPointActor = nullptr;
    m_vtkRouterPointActor = pointActor;
    return m_vtkRouterPointActor;
}

#pragma endregion

#pragma region OTHER

/// <summary>
///  find 3 nodes of bronchoTree closest to the parameter point
/// </summary>
/// <param name="pos">point selected by user in 3d space</param>
void CBronchoCore::findNearbyNode(double pos[]) {
    std::vector<double> distList;
    std::map<double, int> distMap;

    auto fncCalc3dDistance = [](double pos1[], double pos2[]) {
        return sqrt(pow(pos1[0] - pos2[0], 2) +
            pow(pos1[1] - pos2[1], 2) +
            pow(pos1[2] - pos2[2], 2));
    };

    auto sortedSmallerThan = [](double dist1, double dist2) { return dist1 < dist2; };


    for (int i = 1; i < m_bronchoTree.getNumberOfNodes(); i++) {
        double pos2[] = { m_bronchoTree[i].m_point.x, m_bronchoTree[i].m_point.y, m_bronchoTree[i].m_point.z };
        double dist = fncCalc3dDistance(pos, pos2);
        distList.push_back(dist);
        distMap[dist] = i;
    }

    std::sort(distList.begin(), distList.end(), sortedSmallerThan);

    nearbyNodes[0] = distMap[distList[0]];
    nearbyNodes[1] = distMap[distList[1]];
    nearbyNodes[2] = distMap[distList[2]];

    int e = 3;
}

#pragma endregion

