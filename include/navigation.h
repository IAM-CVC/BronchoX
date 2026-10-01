#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include <glm/glm.hpp>
#include <vector>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <thread>         
#include <chrono>         
#include <vtkRenderWindowInteractor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

#include "bronchocore.h"

#include <iostream>
#include <cmath>

class CNavigation {
private:
	const int ADVANCE_POS = 2;
	const int FORWARD_POS = 3;
	vtkSmartPointer<vtkCamera> m_camera;
	vtkSmartPointer<vtkLight> m_light;
	vtkSmartPointer<vtkRenderer> m_renderer;
	int velocity = 5;

	const double LIGHT_COLOR[3] = { 1.5, 1.5, 1.5 };
	//	std::vector<glm::dvec3> m_vecPoints;    // point's path
	bool m_isRunning;
	int m_interactorTimerId;
	int m_iIndex;


	CBronchoTree m_bronchoTree;    
	std::vector<int> m_path;
	int m_pathIndex;
	int m_vecIndex;

public:
	CNavigation();
	~CNavigation();

	//	void addPointsPath(const std::vector<glm::dvec3>& vecPoints);
	void setup(vtkSmartPointer<vtkCamera> camera, vtkSmartPointer<vtkLight> light);	
	void resetIndex(bool);
	
	inline void setRender(vtkSmartPointer<vtkRenderer> render) { m_renderer = render; };
	inline void setTree(CBronchoCore bronchoCore) { m_bronchoTree = bronchoCore.getTree(); }
	inline void setPath(int nodeSelected) { m_path = m_bronchoTree.getPathIndexTo(nodeSelected); }
	inline bool isRunning() { return m_isRunning; };
	inline int getInteractorTimerId() { return m_interactorTimerId; };

	void setVelocity(int v);
	double getVelocity();

	void next();
	void prev();
	void start(int interactorTimerId);
	void reset();
	void stop();

	void moveTo(int pathIndex, int vecIndex);
	glm::dvec3 moveToNode(int pathIndex);

	void saveViewToPng(vtkSmartPointer<vtkRenderer> renderer, std::string pngFileName);
};

#endif //_NAVIGATION_H_