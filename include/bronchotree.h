#ifndef _BRONCHOTREE_H_
#define _BRONCHOTREE_H_


//#include <vtkCellData.h>
//#include <vtkPointData.h>
//#include <vtkDataArray.h>
//#include <vtkPoints.h>
//#include <vtkSmartPointer.h>

#include <fstream>
#include <numeric>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

//  #include "graph.h"
//  #include "utility.h"

#include <glm/vec3.hpp>



using namespace std;


enum class NODE_TYPE : unsigned char {
  ROOT = 0,
  UNMARKED,
  QUAD_I,
  QUAD_II,
  QUAD_III,
  QUAD_IV
};

/// A node element, which contains who is its father, and the list of point to
/// reach it
class CNodeTree {
public:
  glm::dvec3 m_point;
  std::vector<glm::dvec3> m_vecPoints;
  NODE_TYPE m_type;
  int m_iOrder;
  int m_iParent;
  std::vector<int> m_vecChildren;
  bool m_sorted = false;

  CNodeTree() : m_type{NODE_TYPE::UNMARKED}, m_iOrder{1}, m_iParent{-1} {}
  ~CNodeTree() {
    m_vecPoints.clear();
    m_vecChildren.clear();
  }

  void setVPoint(const glm::dvec3 &point) { this->m_point = point; }
  void setTypeAndOrder(const NODE_TYPE &type, const int &iOrder) {
    this->m_type = type;
    this->m_iOrder = iOrder;
  }
  void setParent(const int &iNode) { this->m_iParent = iNode; }
  void addWEPoint(const glm::dvec3 &point) {
    this->m_vecPoints.push_back(point);
  }
};

class CBronchoTree {
private:
  std::vector<CNodeTree> m_bronchoTree;

public:
  CBronchoTree() = default;
  ~CBronchoTree();
  // CBronchoTree(const CBronchoTree& broncho);

#pragma region  Tree_Basic_Functions

  inline void reset() { m_bronchoTree.clear(); }
  inline void setSize(const size_t iSize) { m_bronchoTree.resize(iSize); }
  inline int getNumberOfNodes() { return (int)m_bronchoTree.size(); }
  /// overloading the [] to access at a CNodeTree position in m_bronchoTree
  inline CNodeTree& operator[](int iPos) { return m_bronchoTree[iPos]; }
#pragma endregion

#pragma region  Math_Functions

  double calcEuclideanDIstance(glm::dvec3 pos1, glm::dvec3 pos2);
  std::vector<glm::dvec3> smoothingPoints(std::vector<glm::dvec3> vecPoints, int iWindow);
#pragma endregion

#pragma region  Tree_Node_Path_Functions
  //    Return all node point of tree
  std::vector<glm::dvec3> getPath();

  /// Return index of node in the path from root to iNode
  std::vector<int> getPathIndexTo(const int &iNode);

  //    Return nodes in the path from root to iNode
  std::vector<glm::dvec3> getPathTo(const int& iNode);

  //    Return all internal points of a node
  std::vector<glm::dvec3> getFullPathOf(const int& iNode);

  /// Return all internal points in the path from root to iNode
  std::vector<glm::dvec3> getFullPathTo(const int &iNode);

  std::vector<glm::dvec3> getFullPath();

  ///// Print nodes in the path from root to iNode
  //void printPath(const int& iNode);

  ///// Print all internal nodes in the path from root to iNode
  //void printFullPathTo(const int& iNode);

  //std::vector<glm::dvec3> getFullPathSortedTo(const int& iNode, std::map<int, int>& nodePositionMap);

#pragma endregion


#pragma region  Export_Tree_Functions
  /// Export the tree into a txt file
  void writeTreeInFile(std::string strFilename);

  /// Export distance of all node of tree into a txt file
  void writeTreeNodeDistanceInFile(std::string strFilename);

  void writeTreeNodeDistanceInFile_2(std::string strFilename);
#pragma endregion
};

#endif //_BRONCHOTREE_H_