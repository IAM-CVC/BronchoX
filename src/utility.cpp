#include "utility.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <vtkActor2D.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkCoordinate.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProp.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>

// Null, because instance will be initialized on demand.
CUtility* CUtility::m_pInstance = 0;

CUtility* CUtility::getInstance() {
    if (m_pInstance == 0) {
        m_pInstance = new CUtility();
    }

    return m_pInstance;
}

CUtility::CUtility() {}

//vtkSmartPointer<CErrorObserver> CUtility::GetErrorInstance() {
//    if (m_error == nullptr) {
//        m_error = vtkSmartPointer<CErrorObserver>::New();
//    }
//    return m_error;
//}
//
///// Determines the equivalent subscript values corresponding to the absolute
///// index dimension of a multidimensional array.
///// @param siz size of the N-dimensional matrix
///// @param N the dimensions of the matrix
///// @param idx index in linear format
///// @param sub the output - subscript values written into an N - dimensional
///// integer array (created by caller)
//void CUtility::ind2sub(const size_t* siz, int N, int idx, int* sub) {
//    // Calculate the cumulative product of the size vector
//    std::vector<int> cumProd(N);
//    int nextProd = 1;
//    for (int k = 0; k < N; k++) {
//        cumProd[k] = nextProd;
//        nextProd *= siz[k];
//    }
//
//    // Validate range of linear index
//    int maxInd =
//        cumProd[N - 1] * siz[N - 1] - 1; // assuming that fit into a int data type
//    if ((idx < 0) || (idx > maxInd)) {
//        std::cout << "bad linear index maxsum::ind2sub"
//            << std::endl; // some kind of message
//        return;
//    }
//
//    // the computation
//    int remainder = idx;
//    for (int k = N - 1; k >= 0; k--) {
//        sub[k] = remainder / cumProd[k];
//        remainder = remainder % cumProd[k];
//    }
//}
//
////*********************************************
///*!
//\fn				int sub2ind(int *siz, int N, int *sub)
//\brief			This function determines the equivalent index value
//corresponding to the subscript values of a multidimensional array.
//
//\author			Kriti Sen Sharma
//\date			2011-05-04
//\param[in]	siz		size of the N-dimensional matrix
//\param[in]	N		the dimensions of the matrix
//\param[out]	sub		subscripts stored in an N-dimensional array
//\return		(integer) the linear index corresponding to the subscript
//
//\details			Example for a 2-D array of size [3, 5] : siz[] =
//{3, 5}; N = 2
//- if subscript is (0, 0), linear index is 0
//- if subscript is (0, 1), linear index is 1
//- if subscript is (1, 0), linear index is 5
//- if subscript is (2, 4), linear index is 14
//
//*/
//int CUtility::sub2ind(int* siz, int N, int* sub) {
//    int idx = 0;
//    if (0)
//        idx = sub[0] * siz[3] * siz[2] * siz[1] + sub[1] * siz[3] * siz[2] +
//        sub[2] * siz[3] + sub[3];
//    else {
//        for (int i = 0; i < N; i++) {
//            int prod = 1;
//            for (int j = N - 1; j > i; j--)
//                prod *= siz[j];
//            idx += sub[i] * prod;
//        }
//    }
//    return idx;
//}
//
//vtkSmartPointer<vtkActor>
//CUtility::getActor(const vtkSmartPointer<vtkPolyData>& data, float fRed, float fGreen, float fBlue) {
//    vtkNew<vtkPolyDataMapper> mapper;
//    vtkNew<vtkActor> actor;
//    mapper->SetInputData(data);
//    actor->GetProperty()->SetDiffuseColor(fRed, fGreen, fBlue);
//    actor->SetMapper(mapper);
//    return actor;
//}

void CUtility::viewportBorderColor(vtkSmartPointer<vtkRenderer>& renderer, double dRed, double dGreen, double dBlue, bool last) {
    // points start at upper right and proceed anti-clockwise
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(4);
    points->InsertPoint(0, 1, 1, 0);
    points->InsertPoint(1, 0, 1, 0);
    points->InsertPoint(2, 0, 0, 0);
    points->InsertPoint(3, 1, 0, 0);

    // create cells, and lines
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    cells->Initialize();

    vtkSmartPointer<vtkPolyLine> lines = vtkSmartPointer<vtkPolyLine>::New();

    // only draw last line if this is the last viewport
    // this prevents double vertical lines at right border
    // if different colors are used for each border, then do
    // not specify last
    if (last) {
        lines->GetPointIds()->SetNumberOfIds(5);
    }
    else {
        lines->GetPointIds()->SetNumberOfIds(4);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        lines->GetPointIds()->SetId(i, i);
    }
    if (last) {
        lines->GetPointIds()->SetId(4, 0);
    }
    cells->InsertNextCell(lines);

    // now make tge polydata and display it
    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->Initialize();
    poly->SetPoints(points);
    poly->SetLines(cells);

    // use normalized viewport coordinates since
    // they are independent of window size
    vtkSmartPointer<vtkCoordinate> coordinate =
        vtkSmartPointer<vtkCoordinate>::New();
    coordinate->SetCoordinateSystemToNormalizedViewport();

    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
        vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(poly);
    mapper->SetTransformCoordinate(coordinate);

    vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(dRed, dGreen, dBlue);
    // line width should be at least 2 to be visible at extremes

    actor->GetProperty()->SetLineWidth(4.0); // Line Width

    renderer->AddViewProp(actor);
}

vtkSmartPointer<vtkOrientationMarkerWidget> CUtility::createCubeMarker(vtkSmartPointer<vtkRenderer>& renderer) {
    // vtkSmartPointer<vtkOrientationMarkerWidget> m_cubeWidget =
    // vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    vtkNew<vtkOrientationMarkerWidget> m_cubeWidget;
    m_cubeWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());
    m_cubeWidget->SetDefaultRenderer(renderer);
    m_cubeWidget->SetViewport(0.9, 0.0, 1.0, 0.1); // to the right

    // add cube marker
    // vtkSmartPointer<vtkAnnotatedCubeActor> cube =
    // vtkSmartPointer<vtkAnnotatedCubeActor>::New();
    vtkNew<vtkAnnotatedCubeActor> cube;
    cube->SetXPlusFaceText("P");
    cube->SetXMinusFaceText("A");
    cube->SetYPlusFaceText("L");
    cube->SetYMinusFaceText("R");
    cube->SetZPlusFaceText("S");
    cube->SetZMinusFaceText("I");
    cube->SetFaceTextScale(0.666667);
    cube->SetFaceTextScale(0.65);
    auto property = cube->GetCubeProperty();
    property->SetColor(0.462, 0.474, 0.486); // grey
    property = cube->GetTextEdgesProperty();
    property->SetLineWidth(1);
    property->SetDiffuse(0);
    property->SetAmbient(1);
    property->SetColor(0.58, 0.58, 0.58);
    // property->SetColor(0.239, 0.682, 0.913);  //blue
    cube->GetXMinusFaceProperty()->SetColor(0.937, 0.941, 0.945); // white
    cube->GetXPlusFaceProperty()->SetColor(0.937, 0.941, 0.945);  // white
    cube->GetYMinusFaceProperty()->SetColor(0.937, 0.941, 0.945); // white
    cube->GetYPlusFaceProperty()->SetColor(0.937, 0.941, 0.945);  // white
    cube->GetZMinusFaceProperty()->SetColor(0.937, 0.941, 0.945); // white
    cube->GetZPlusFaceProperty()->SetColor(0.937, 0.941, 0.945);  // white
    m_cubeWidget->SetOrientationMarker(cube);

    return m_cubeWidget;
}

///// @brief Convert 3D points into a vtkActor able to render them as spheres (glyphs)
///// @param points vector of 3D points
///// @param color RGB color of actors in the range [0, 1]
///// @param fScale size of the sphere to rendering it
///// @return a vtkActor representing the 3D points in a particular color
//vtkSmartPointer<vtkActor> CUtility::getActorFromPoints(std::vector<glm::dvec3> points, glm::ivec3 color, float fScale = 0.025)
//{
//    vtkNew<vtkPoints> pointData;
//    vtkNew<vtkPolyData> polyData;
//    vtkNew<vtkGlyph3D> glyph;
//    vtkNew<vtkSphereSource> sphereSource;
//    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
//    vtkNew<vtkPolyDataMapper> mapper;
//
//    for (const auto& p : points)
//        pointData->InsertNextPoint(p.x, p.y, p.z);
//
//    polyData->SetPoints(pointData);
//
//    glyph->SetColorModeToColorByScalar();
//    glyph->ScalingOff();
//    sphereSource->SetRadius(fScale);
//    glyph->SetSourceConnection(sphereSource->GetOutputPort());
//    glyph->SetInputData(polyData);
//    glyph->Update();
//
//    mapper->SetInputConnection(glyph->GetOutputPort());
//    actor->GetProperty()->SetDiffuseColor(color.r, color.g, color.b);
//    actor->SetMapper(mapper);
//    return actor;
//}
//
///// @brief Given a set of 3D points, this function smooths them! using a convolutional 1D kernel (box blur)
///// 
///// @param vecPoints input 3D points to process
///// @param iWindow size of the kernel
///// @return smooth vecPoints of same size. Both ends copy the same points
//std::vector<glm::dvec3> CUtility::SmoothingPoints(std::vector<glm::dvec3> vecPoints, int iWindow)
//{
//    std::vector<glm::dvec3> vecOutput;
//    size_t iIndex = 0;
//
//    int iHalfWindow = iWindow / 2;  // iWindow >> 1 also works
//
//    // before the range
//    for (iIndex = 0; iIndex < iHalfWindow; iIndex++)
//        vecOutput.push_back(vecPoints[iIndex]);
//
//    for (iIndex = iHalfWindow; iIndex < vecPoints.size() - iHalfWindow; iIndex++)
//    {
//        size_t iStart = iIndex - iHalfWindow;
//        size_t iEnd = iIndex + iHalfWindow;
//
//        // compute the average
//        glm::dvec3 vecSum = std::accumulate(vecPoints.begin() + iStart, vecPoints.begin() + iEnd, glm::dvec3(0), [](const auto& a, const auto& b) {return a + b; });
//        vecSum /= (iEnd - iStart);
//
//        vecOutput.push_back(vecSum);
//    }
//
//    // after the range
//    for (iIndex = vecPoints.size() - iHalfWindow; iIndex < vecPoints.size(); iIndex++)
//        vecOutput.push_back(vecPoints[iIndex]);
//
//    return vecOutput;
//}
//
//
///// @brief Given a set of 3D points those are store in CSV format
///// @param points vector of 3D points to be stored
///// @param strFilename filename where be stored points
///// @param normalize indicate if points should be normalized or not
//void CUtility::saveToCSV(std::vector<glm::dvec3> points, std::string strFilename, bool normalize)
//{
//    fstream fout;
//    fout.open(strFilename, ios::out | ios::app);
//    // header
//    fout << "pos_x,pos_y,pos_z,vec_x,vec_y,vec_z" << endl;
//    for (int i = 0; i < points.size() - 1; i++)
//    {
//        fout << points[i].x << "," << points[i].y << "," << points[i].z << ",";
//        glm::dvec3 v = (points[i + 1] - points[i]);
//        if (normalize)
//            v = glm::normalize(v);
//        fout << v.x << "," << v.y << "," << v.z << std::endl;
//    }
//    fout.close();
//}
