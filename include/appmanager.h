#ifndef _APPMANAGER_H_
#define _APPMANAGER_H_

#include <vtkDICOMMetaData.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>

//#include "matlab/mat.h"
//#include "matlab/matrix.h"
//#include "matlab/mex.h"
//#include "matlab/engine.h"


#include "mat.h"
#include "mex.h"
#include "engine.h"

#include "logmanager.h"

#include "nlohmann/json.hpp"
namespace fs = std::filesystem;
using namespace std;

namespace bronchox {
    namespace manager {
        class AppManager {
        public:
            AppManager();
            ~AppManager();

            const std::string pathNodeTmpFile = "nodePath.txt";

            inline bool isDataLoaded() { return m_bDataLoaded; };
            inline std::string getDataDir() { return m_dataDir; };
            inline std::string getDataOutputDir() { return m_dataOutputDir; };
            inline std::string getDataOutputImgDir() { 
                fs::remove_all(m_dataOutputDir + "/img/");
                fs::create_directory(m_dataOutputDir + "/img/");
                return m_dataOutputDir + "/img/";
            };
            inline std::string getMatlabScriptPath() { return m_matlabScriptPath; };
             
            inline vtkSmartPointer<vtkDICOMMetaData> get_m_metaData() { return m_metaData; };
            inline vtkSmartPointer<vtkImageData> get_m_imgDICOM() { return m_imgDICOM; };
            inline int get_m_dicomSerie() { return m_dicomSerie; };

            void setDicomData(std::string dataDir, vtkSmartPointer<vtkDICOMMetaData>metaData, vtkSmartPointer<vtkImageData> imgDICOM, int dicomSerie);
            
            //  Write position of node selected in a file 
            void writePathNodeFile(double* pos);
            //  Read the position of node selected of the history file
            double* readPathNodeFile();

        private:
            bool m_bDataLoaded;  // true if data is correctly loaded.
            std::string m_dataDir;
            std::string m_dataOutputDir;
            std::string m_matlabScriptPath;
            int m_dicomSerie;
            vtkSmartPointer<vtkDICOMMetaData> m_metaData;
            vtkSmartPointer<vtkImageData> m_imgDICOM;

        };
    }
}

#endif // APPMANAGER