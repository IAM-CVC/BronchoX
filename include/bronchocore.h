#ifndef _BRONCHOCORE_H_
#define _BRONCHOCORE_H_


#include <vtkActor2D.h>
#include <vtkKochanekSpline.h>
#include <vtkOBJReader.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCallbackCommand.h>
#include <vtkCardinalSpline.h>
#include <vtkContourFilter.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageReslice.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkWindowToImageFilter.h>
#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>
#include <vtkImageData.h>

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stack>
#include <string>
#include <map>

#include "logmanager.h"

#include "mat.h"
#include "mex.h"
#include "engine.h"
#include "bronchotree.h"

class CBronchoCore {
protected:
    const glm::dvec3 SKIN_COLOR; // indicates the color of the skin (diffuse
                                 // color)
    const glm::dvec3
        UNITY_SCALE; // indicates the scale made by Unity in order to replicate it
private:
    CBronchoTree m_bronchoTree;              // data structure of the tree
    vtkSmartPointer<vtkActor> m_vtkOBJActor; // vtk object to handle the obj
    vtkSmartPointer<vtkActor> m_vtkUnityPathActor; // vtk object to handle unity path
    vtkSmartPointer<vtkActor> m_vtkSkeletonActor;
    vtkSmartPointer<vtkActor> m_vtkSegmentLineActor;
    vtkSmartPointer<vtkActor> m_vtkSegmentPointActor;
    vtkSmartPointer<vtkActor> m_vtkRouterPointActor;
    vtkSmartPointer<vtkActor> m_vtkRouterPathPointActor;
    const static int MAX_NEARBY_NODE = 3;
    int nearbyNodes[MAX_NEARBY_NODE];
    int nodeSelected = -1;
    double m_spacing[3];

    Engine* ep;
public:
    CBronchoCore();
    ~CBronchoCore();

    inline CBronchoTree getTree() {return m_bronchoTree; };

    inline double* getSpacing() { return m_spacing; };
    inline void setSpacing(double* spacing) {
        m_spacing[0] = spacing[0];
        m_spacing[1] = spacing[1];
        m_spacing[2] = spacing[2];
    };

    // set & get
    inline vtkSmartPointer<vtkActor> getOBJActor() { return m_vtkOBJActor; };
    inline vtkSmartPointer<vtkActor> getUnityPathActor() { return m_vtkUnityPathActor; };
    inline vtkSmartPointer<vtkActor> getSkeletonActor() { return m_vtkSkeletonActor; };
    inline vtkSmartPointer<vtkActor> getSegmentLineActor() { return m_vtkSegmentLineActor; };
    inline vtkSmartPointer<vtkActor> getSegmentPointActor() { return m_vtkSegmentPointActor; };
    inline vtkSmartPointer<vtkActor> getRouterPointActor() { return m_vtkRouterPointActor; };
    inline vtkSmartPointer<vtkActor> getRouterPathPointActor() { return m_vtkRouterPathPointActor; };
    inline int getNodeSelected() { return nodeSelected; };

    inline void bronchoCoreReset() {
        m_vtkOBJActor = nullptr;
        m_vtkUnityPathActor = nullptr;
        m_vtkSkeletonActor = nullptr;
        m_vtkSegmentLineActor = nullptr;
        m_vtkSegmentPointActor = nullptr;
        m_vtkRouterPointActor = nullptr;
        m_vtkRouterPathPointActor = nullptr;
        m_bronchoTree.reset();
        nodeSelected = -1;
        for (int i = 0; i < MAX_NEARBY_NODE; i++) { nearbyNodes[i] = -1; }
    };

    inline bool isDataloaded() {
        return m_vtkOBJActor != nullptr && m_bronchoTree.getNumberOfNodes() > 0;
    }

    inline int* getNearbyNodes() { return nearbyNodes; };
    inline int getMaxNearbyNode() { return MAX_NEARBY_NODE; };

    inline double* getNodePosition(int nodeIndex) {
        double pos[] = { m_bronchoTree[nodeIndex].m_point.x, m_bronchoTree[nodeIndex].m_point.y, m_bronchoTree[nodeIndex].m_point.z };
        return pos;
    }

    inline vector<int> getNodePath() { return m_bronchoTree.getPathIndexTo(nodeSelected); };

    inline void setNodeSelectByIndex(int index) { nodeSelected = nearbyNodes[index]; };


#pragma region MATLAB_FUNCTIONS
    bool openMatlabEngine();
    void closeMatlabEngine();

    /// Function to read an OBJ file
    bool loadOBJ(const string& strFilename, bool bFlip);

    /// Function to load the Matlab file
    bool loadMatlabFile(const string& strFilename, bool bFlip);

    //  Function to write image of dicom file into matlab file
    bool writeMatlabDataFile_CTData(std::string strFileName, vtkSmartPointer<vtkImageData> m_imgDICOM);

    //  Function to call matlab script: segmentation
    bool callMatlabFunction_SegmentationPipeline(std::string inputDataFolder, std::string m_matlabScriptPath);

    //  Function to call matlab script: generate obj file and matlab file (path)
    bool callMatlabFunction_generateObjAndPathFile(std::string inputDataFolder, std::string m_matlabScriptPath);
#pragma endregion

#pragma region GENERATE_ROUTER_ACTOR
    /// Return the vertex/skeleton points of tree
    vtkSmartPointer<vtkActor> getSkeletonPoints(float fSphereSize);

    /// Return the actor of lines inside all segments
    vtkSmartPointer<vtkActor> getSegmentsLinesRandomColor(float fWidthLine);

    /// Return the actor of all segments in random colors
    vtkSmartPointer<vtkActor> getSegmentsPointsRandomColor(float fSphereSize);

    /// Return the actor with the route to node iEndNode
    vtkSmartPointer<vtkActor> getRoutePointsTo(int iEndNode, float fSphereSize);

#pragma endregion
    
#pragma region OTHER

    //  Function to find 
    void findNearbyNode(double pos[]);

    /// Generate output file represeting the tree
    void generateTreeFile(std::string strFilename) {
        m_bronchoTree.writeTreeInFile(strFilename);
    }

#pragma endregion

};

#endif //_BRONCHOCORE_H_