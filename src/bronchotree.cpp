#include "bronchotree.h"

CBronchoTree::~CBronchoTree() { m_bronchoTree.clear(); }

#pragma region MATH_FUNCTIONS

double CBronchoTree::calcEuclideanDIstance(glm::dvec3 pos1, glm::dvec3 pos2) {
    return sqrt(pow(pos1.x - pos2.x, 2) +
        pow(pos1.y - pos2.y, 2) +
        pow(pos1.z - pos2.z, 2));
}

/// @brief Given a set of 3D points, this function smooths them! using a convolutional 1D kernel (box blur)
/// 
/// @param vecPoints input 3D points to process
/// @param iWindow size of the kernel
/// @return smooth vecPoints of same size. Both ends copy the same points
std::vector<glm::dvec3> CBronchoTree::smoothingPoints(std::vector<glm::dvec3> vecPoints, int iWindow)
{
    std::vector<glm::dvec3> vecOutput;
    size_t iIndex = 0;

    int iHalfWindow = iWindow / 2;  // iWindow >> 1 also works

    // before the range
    for (iIndex = 0; iIndex < iHalfWindow; iIndex++)
        vecOutput.push_back(vecPoints[iIndex]);

    for (iIndex = iHalfWindow; iIndex < vecPoints.size() - iHalfWindow; iIndex++)
    {
        size_t iStart = iIndex - iHalfWindow;
        size_t iEnd = iIndex + iHalfWindow;

        // compute the average
        glm::dvec3 vecSum = std::accumulate(vecPoints.begin() + iStart, vecPoints.begin() + iEnd, glm::dvec3(0), [](const auto& a, const auto& b) {return a + b; });
        vecSum /= (iEnd - iStart);

        vecOutput.push_back(vecSum);
    }

    // after the range
    for (iIndex = vecPoints.size() - iHalfWindow; iIndex < vecPoints.size(); iIndex++)
        vecOutput.push_back(vecPoints[iIndex]);

    return vecOutput;
}

#pragma endregion

#pragma region GET_PATH

std::vector<glm::dvec3> CBronchoTree::getPath() {
    std::vector<glm::dvec3> vec;

    for (CNodeTree node : m_bronchoTree) {
        vec.push_back(node.m_point);
    }

    return vec;
}


/// From root (node 1) to iNode, return all nodes in the path
/// @param iNode destination node
std::vector<int> CBronchoTree::getPathIndexTo(const int& iNode) {
    int iTempNode = iNode;
    std::vector<int> path;
    while (iTempNode != 1) // until reach root
    {
        path.push_back(iTempNode);
        iTempNode = m_bronchoTree[iTempNode].m_iParent;
    }
    path.push_back(1);
    std::reverse(path.begin(), path.end()); // are stored in backward order, then reverse it

    return path;
}

std::vector<glm::dvec3> CBronchoTree::getPathTo(const int& iNode) {
    std::vector<glm::dvec3> vec;
    std::vector<int> path = getPathIndexTo(iNode);

    for_each(path.begin(), path.end(), [&](int index) {
        vec.push_back(m_bronchoTree[index].m_point);
        });

    return vec;
}

std::vector<glm::dvec3> CBronchoTree::getFullPathOf(const int& iNode) {
    std::vector<glm::dvec3> vec;

    if (m_bronchoTree[iNode].m_vecPoints.size() > 0) {
        for_each(m_bronchoTree[iNode].m_vecPoints.begin(), m_bronchoTree[iNode].m_vecPoints.end(), [&](auto point) {
            vec.push_back(point);
            });
    }

    return vec;
}

/// From root (node 1), this prints all internal points to reach until iNode
/// @param iNode destination node
std::vector<glm::dvec3> CBronchoTree::getFullPathTo(const int& iNode) {
    std::vector<glm::dvec3> vec;
    std::vector<int> path = getPathIndexTo(iNode);

    for_each(path.begin(), path.end(), [&](int index) {
        for_each(m_bronchoTree[index].m_vecPoints.begin(), m_bronchoTree[index].m_vecPoints.end(), [&](auto p) {
            vec.push_back(p);
            });
        });

    return vec;
}


std::vector<glm::dvec3> CBronchoTree::getFullPath() {
    std::vector<glm::dvec3> vec;

    // iterate over all nodes in the tree
    for (auto node : m_bronchoTree)
    {
        //set points and colors
        std::for_each(node.m_vecPoints.begin(), node.m_vecPoints.end(), [&](auto p) {
            vec.push_back(p);
            });
    }

    return vec;
}


///// From root (node 1), prints the sequence in format 1 --> b --> c ... -->
///// iNode
///// @param iNode final node to reach it
//void CBronchoTree::printPath(const int& iNode) {
//    std::vector<int> path = getPathIndexTo(iNode);
//
//    for_each(path.begin(), path.end(),
//        [](int index) { cout << index << " --> "; });
//}
//
///// From root (node 1), this prints all points to reach until iNode
///// @param iNode destination node
//void CBronchoTree::printFullPathTo(const int& iNode) {
//    std::vector<int> path = getPathIndexTo(iNode);
//
//    for_each(path.begin(), path.end(), [=](int index) {
//        for_each(m_bronchoTree[index].m_vecPoints.begin(),
//            m_bronchoTree[index].m_vecPoints.end(),
//            [](auto p) { cout << p.x << " " << p.y << " " << p.z << endl; });
//        });
//}

//std::vector<glm::dvec3> CBronchoTree::getFullPathSortedTo(const int& iNode, std::map<int, int>& nodePositionMap) {
//    std::vector<glm::dvec3> vec;
//    std::vector<int> path = getPathTo(iNode);
//
//    int cnt = 0;
//
//    for_each(path.begin(), path.end(), [&](int index) {
//
//        bool isSorted = true;
//
//        if (m_bronchoTree[index].m_vecPoints.size() > 0) {
//            if (vec.size() == 0) {
//                isSorted = m_bronchoTree[index].m_vecPoints.at(0).z > m_bronchoTree[index].m_vecPoints.at(m_bronchoTree[index].m_vecPoints.size() - 1).z;
//            }
//            else {
//                isSorted = calcEuclideanDIstance(vec.back(), m_bronchoTree[index].m_vecPoints.at(0)) < calcEuclideanDIstance(vec.back(), m_bronchoTree[index].m_vecPoints.at(m_bronchoTree[index].m_vecPoints.size() - 1));
//            }
//
//            nodePositionMap[index] = cnt;
//        }
//
//        if (isSorted) {
//            for (int i = 0; i < m_bronchoTree[index].m_vecPoints.size(); i++) {
//                vec.push_back(m_bronchoTree[index].m_vecPoints.at(i));
//                cnt++;
//            }
//        }
//        else {
//            for (int i = m_bronchoTree[index].m_vecPoints.size() - 1; i >= 0; i--) {
//                vec.push_back(m_bronchoTree[index].m_vecPoints.at(i));
//                std::cout << m_bronchoTree[index].m_vecPoints.at(i).x << "-" << m_bronchoTree[index].m_vecPoints.at(i).y << "-" << m_bronchoTree[index].m_vecPoints.at(i).z << std::endl;
//
//                cnt++;
//            }
//        }
//        });
//
//
//
//
//    return vec;
//}

#pragma endregion

#pragma region EXPORT

// 0-based file
void CBronchoTree::writeTreeInFile(std::string strFilename) {
    ofstream myfile;
    myfile.open(strFilename);
    for (int iIndex = 1; iIndex < m_bronchoTree.size(); iIndex++) {
        string strOutputLine =
            std::to_string(m_bronchoTree[iIndex].m_point.x) + " " +
            std::to_string(m_bronchoTree[iIndex].m_point.y) + " " +
            std::to_string(m_bronchoTree[iIndex].m_point.z) + " ";
        strOutputLine += std::to_string((m_bronchoTree[iIndex].m_iParent - 1 == -2)
            ? -1
            : m_bronchoTree[iIndex].m_iParent - 1);

        // find children of node 9two children)
        int arrChildren[2]{ -1, -1 };
        int iTempIndex = 0;
        for (int k = 1; k < m_bronchoTree.size() && iTempIndex < 2; k++) {
            if (m_bronchoTree[k].m_iParent == iIndex)
                arrChildren[iTempIndex++] = k - 1;
        }

        // copy children into output string
        std::for_each(arrChildren, arrChildren + 2, [&strOutputLine](auto child) {
            strOutputLine += (" " + std::to_string(child));
            });

        myfile << strOutputLine << endl;
    }
    myfile.close();
}

/// Export distance of all node of tree into a txt file
void CBronchoTree::writeTreeNodeDistanceInFile(std::string strFilename) {

    auto fncCalc3dDistance = [](glm::dvec3 pos1, glm::dvec3 pos2) {
        return sqrt(pow(pos1.x - pos2.x, 2) +
            pow(pos1.y - pos2.y, 2) +
            pow(pos1.z - pos2.z, 2));
    };


    ofstream myfile;
    myfile.open(strFilename);
    for (int iIndex = 1; iIndex < m_bronchoTree.size(); iIndex++) {
        std::vector<int> path = getPathIndexTo(iIndex);

        glm::dvec3 lastP(-1, -1, -1);
        string strOutputLine = "";
        for (int pathIndex = 0; pathIndex < path.size(); pathIndex++) {
            if (m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() > 0) {
                bool isSorted = true;

                if (lastP.x != -1) {
                    glm::dvec3 currentPoint = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(0);
                    isSorted = fncCalc3dDistance(lastP, currentPoint) < fncCalc3dDistance(lastP, m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1));
                }
                else {
                    isSorted = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(0).z > m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1).z;
                }

                if (isSorted) {
                    if (lastP.x == -1) {
                        lastP = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(0);
                    }
                    for (int vecPointsIndex = 0; vecPointsIndex < m_bronchoTree[path.at(pathIndex)].m_vecPoints.size(); vecPointsIndex++) {
                        glm::dvec3 currentPoint = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(vecPointsIndex);
                        strOutputLine += "node " + std::to_string(path.at(pathIndex)) + ":" + std::to_string(fncCalc3dDistance(currentPoint, lastP)) + ";";
                        lastP = currentPoint;
                    }
                }
                else {
                    if (lastP.x == -1) {
                        lastP = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1);
                    }
                    for (int vecPointsIndex = m_bronchoTree[path.at(pathIndex)].m_vecPoints.size() - 1; vecPointsIndex >= 0; vecPointsIndex--) {
                        glm::dvec3 currentPoint = m_bronchoTree[path.at(pathIndex)].m_vecPoints.at(vecPointsIndex);
                        strOutputLine += "node " + std::to_string(path.at(pathIndex)) + ":" + std::to_string(fncCalc3dDistance(currentPoint, lastP)) + ";";
                        lastP = currentPoint;
                    }
                }

            }
        }

        myfile << strOutputLine << endl << endl << endl;
    }
    myfile.close();
}

/// Export distance of all node of tree into a txt file
void CBronchoTree::writeTreeNodeDistanceInFile_2(std::string strFilename) {

    auto fncCalc3dDistance = [](glm::dvec3 pos1, glm::dvec3 pos2) {
        return sqrt(pow(pos1.x - pos2.x, 2) +
            pow(pos1.y - pos2.y, 2) +
            pow(pos1.z - pos2.z, 2));
    };


    ofstream myfile;
    myfile.open(strFilename);
    for (int iIndex = 1; iIndex < m_bronchoTree.size(); iIndex++) {
        if (m_bronchoTree[iIndex].m_vecPoints.size() > 0) {
            string strOutputLine = "node " + std::to_string(iIndex) + ":" + std::to_string(fncCalc3dDistance(m_bronchoTree[iIndex].m_vecPoints.at(0), m_bronchoTree[iIndex].m_vecPoints.at(m_bronchoTree[iIndex].m_vecPoints.size() - 1))) + ";";
            myfile << strOutputLine << endl;
        }

    }
    myfile.close();
}

#pragma endregion
