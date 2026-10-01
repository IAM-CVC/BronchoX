#include "navigation.h"
#include <algorithm>
#include <iterator>
#include <vtkActor.h>
#include <vtkCardinalSpline.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkSplineFilter.h>

CNavigation::CNavigation() { m_iIndex = -1; }

CNavigation::~CNavigation() {}



//void CNavigation::addPointsPath(const std::vector<glm::dvec3>& vecPoints) {
//    m_vecPoints.clear();
//    std::copy(vecPoints.begin(), vecPoints.end(), std::back_inserter(m_vecPoints));
//}


/// <summary>
/// Configure camera and light for navigation of router path
/// </summary>
/// <param name="camera">Main viewer camera</param>
/// <param name="light">Main viewer light (unmodifiable)</param>
void CNavigation::setup(vtkSmartPointer<vtkCamera> camera, vtkSmartPointer<vtkLight> light) {
  this->m_camera = camera;
  this->m_light = light;

  double dNear, dFar;
  m_camera->SetViewUp(0, 1, 0);
  m_camera->GetClippingRange(dNear, dFar);
  m_camera->SetClippingRange(0.005, 2000);
  //m_camera->SetViewAngle(75);

  // light
  //m_light->SetAttenuationValues(0.75, 0.0002, 0.00005);
  //m_light->SetLightTypeToCameraLight();
  //m_light->SetColor(1.5, 1.5, 1.5);
  //m_light->SetConeAngle(90.0);
  //m_light->PositionalOn();
}

/// <summary>
/// Restart position index
/// </summary>
/// <param name="move">update camera position</param>
void CNavigation::resetIndex(bool move) {
    m_iIndex = 0;
    m_pathIndex = 0;
    m_vecIndex = 0;

    if (m_path.size() != 0) {
        while (m_pathIndex < m_path.size() && m_bronchoTree[m_path.at(m_pathIndex)].m_vecPoints.size() == 0) {
            m_pathIndex++;
        }

        if (move) {
            moveTo(m_pathIndex, m_vecIndex);
        }
    }
}

//  ALWAYS FORWARD

/// <summary>
/// Update camera position by point of router
/// </summary>
/// <param name="pathIndex"></param>
/// <param name="vecIndex"></param>
void CNavigation::moveTo(int pathIndex, int vecIndex) {
    if (pathIndex >= m_path.size() || vecIndex >= m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() - 1) return;
    //  if (m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() - 1 == vecIndex) return;

    glm::dvec3 cameraPoint = m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.at(vecIndex);

    this->m_camera->SetPosition(cameraPoint.x, cameraPoint.y, cameraPoint.z);
    this->m_light->SetPosition(cameraPoint.x, cameraPoint.y, cameraPoint.z);

    glm::dvec3 focusPoint;
    int nextPoint = pathIndex + 1;
    if (nextPoint < m_path.size() &&  m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() - vecIndex <= 10) {
        int p = 10 - (m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() - vecIndex);
        if (p >= m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.size() - 1) {
            focusPoint = m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.back();
        }
        else {
            focusPoint = m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.at(p);
        }
    }else {
        focusPoint = m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.back();
    }

    this->m_camera->SetFocalPoint(focusPoint.x, focusPoint.y, focusPoint.z);
    this->m_light->SetFocalPoint(focusPoint.x, focusPoint.y, focusPoint.z);

    this->m_camera->OrthogonalizeViewUp();
    this->m_camera->SetViewUp(0, 0, -1);
}

/// <summary>
/// Update camera position by node
/// </summary>
/// <param name="pathIndex"></param>
/// <returns></returns>
glm::dvec3 CNavigation::moveToNode(int pathIndex) {
    int nextPoint = pathIndex + 1;

    if (m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() == 0 || m_path.size() <= nextPoint) {
        return glm::dvec3(-1, -1, -1);
    }


    glm::dvec3 cameraPoint;
    glm::dvec3 focusPoint;
    glm::dvec3 nodePoint;

    if (m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() > 10) {
        cameraPoint = m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.at(m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.size() - 10);
    }
    else {
        cameraPoint = m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.at(0);
    }

    this->m_camera->SetPosition(cameraPoint.x, cameraPoint.y, cameraPoint.z);
    this->m_light->SetPosition(cameraPoint.x, cameraPoint.y, cameraPoint.z);

    focusPoint = m_bronchoTree[m_path.at(pathIndex)].m_vecPoints.back();

    this->m_camera->SetFocalPoint(focusPoint.x, focusPoint.y, focusPoint.z);
    this->m_light->SetFocalPoint(focusPoint.x, focusPoint.y, focusPoint.z);

    if (m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.size() > 10) {
        nodePoint = m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.at(10);
    }
    else {
        nodePoint = m_bronchoTree[m_path.at(nextPoint)].m_vecPoints.back();
    }

    this->m_camera->OrthogonalizeViewUp();
    this->m_camera->SetViewUp(0, 0, -1);

    return nodePoint;
}

/// <summary>
/// Advance position index of router path
/// </summary>
void CNavigation::next() {
    if (m_pathIndex >= m_path.size()) return;
    if (m_bronchoTree[m_path.at(m_pathIndex)].m_vecPoints.size() - 1 <= (m_vecIndex + 1)) {
        if (m_path.size() <= (m_pathIndex + 1)) {
            stop();
            return;
        }
        else {
            m_pathIndex++;
            if (m_bronchoTree[m_path.at(m_pathIndex)].m_vecPoints.size() == 0) {
                next();
                return;
            }
            else {
                m_vecIndex = 0;
            }
        }
    }
    else {
        m_vecIndex++;
    }
    
    moveTo(m_pathIndex, m_vecIndex);
}


/// <summary>
/// Go back one point of index of router path
/// </summary>
void CNavigation::prev() {
    if (m_vecIndex - 1 < 0) {
        if (m_pathIndex - 1 < 0 || m_bronchoTree[m_path.at(m_pathIndex - 1)].m_vecPoints.size() == 0) {
            return;
        }
        else {
            m_pathIndex--;
            m_vecIndex = m_bronchoTree[m_path.at(m_pathIndex)].m_vecPoints.size() - 1;
        }
    }
    else {
        m_vecIndex--;
    }
    moveTo(m_pathIndex, m_vecIndex);
}


/// <summary>
/// Start automatically navigation of router path
/// </summary>
/// <param name="interactorTimerId"></param>
void CNavigation::start(int interactorTimerId) {
    //reset();
    m_isRunning = true;
    m_interactorTimerId = interactorTimerId;
}

/// <summary>
/// Reset navigation
/// </summary>
void CNavigation::reset() {
    m_isRunning = false;
    /*moveTo(m_iIndex);*/
}


/// <summary>
/// Stop navigation
/// </summary>
void CNavigation::stop() {
    m_isRunning = false;
    if (m_renderer != nullptr) {
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = m_renderer->GetRenderWindow()->GetInteractor();
        renderWindowInteractor->DestroyTimer(m_interactorTimerId);
    }
}


/// <summary>
/// Set velocity of navigation
/// </summary>
/// <param name="i"></param>
void CNavigation::setVelocity(int i) {
    velocity = i;
    if (m_isRunning) {
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = m_renderer->GetRenderWindow()->GetInteractor();
        renderWindowInteractor->DestroyTimer(m_interactorTimerId);
        m_interactorTimerId = renderWindowInteractor->CreateRepeatingTimer(getVelocity());
    }
}

double CNavigation::getVelocity() {
    if (velocity >= 5) {
        return pow(2, (velocity - 5)) * 1000;
    }
    else {
        return (velocity / 5.0) * 1000.0;
    }
}


/// <summary>
/// Export current screen of main window to png image
/// </summary>
/// <param name="renderer"></param>
/// <param name="pngFileName"></param>
void CNavigation::saveViewToPng(vtkSmartPointer<vtkRenderer> renderer, std::string pngFileName) {
    vtkSmartPointer<vtkWindowToImageFilter> filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    filter->SetInput(renderer->GetRenderWindow());
    filter->SetInputBufferTypeToRGB();
    filter->ReadFrontBufferOff();
    filter->Update();

    vtkNew<vtkPNGWriter> writer;
    writer->SetCompressionLevel(0);
    writer->SetFileName(pngFileName.c_str());
    writer->SetInputConnection(filter->GetOutputPort());
    writer->Write();
}