#include "appmanager.h"

namespace bronchox::manager {

#pragma region CONSTRUCTOR_DESTRUCTOR

    AppManager::AppManager() {
        m_bDataLoaded = false;


        std::string strConfigFile = "./config.json";
        size_t pos;
        while ((pos = strConfigFile.find('/')) != std::string::npos) {
            strConfigFile.replace(pos, 1, "\\");
        }
        std::ifstream fileJson(strConfigFile);
        if (!fileJson.is_open()) {
            BRONCHOX_TRACE("BRONCHOX STATUS: CANNOT FOUND CONFIG FILE");
            m_matlabScriptPath = "";
        }
        else {
            nlohmann::json m_config = nlohmann::json::parse(fileJson);
            m_matlabScriptPath = m_config["matlab"]["script"];
        }
    }

    AppManager::~AppManager() {
        m_bDataLoaded = false;
        m_metaData = nullptr;
        m_imgDICOM = nullptr;
    }

#pragma endregion CONSTRUCTOR_DESTRUCTOR

    void AppManager::setDicomData(std::string dataDir, vtkSmartPointer<vtkDICOMMetaData>metaData, vtkSmartPointer<vtkImageData> imgDICOM, int dicomSerie) {
        m_bDataLoaded = true;
        m_dataDir = dataDir;
        m_metaData = metaData;
        m_imgDICOM = imgDICOM;
        m_dicomSerie = dicomSerie;

        m_dataOutputDir = dataDir + "/" + std::to_string(m_dicomSerie);

        fs::create_directories(m_dataOutputDir);
    }

    void AppManager::writePathNodeFile(double* pos) {
        ofstream myfile;
        myfile.open(m_dataOutputDir + "\\" + pathNodeTmpFile);
        myfile << pos[0] << endl;
        myfile << pos[1] << endl;
        myfile << pos[2] << endl;
        myfile.close();
    }

    double* AppManager::readPathNodeFile() {
        double pos[3];
        string line;
        ifstream myfile(m_dataOutputDir + "\\" + pathNodeTmpFile);
        if (myfile.is_open())
        {
            int cnt = 0;
            while (getline(myfile, line) || cnt <= 2)
            {

                //cout << line << endl;
                pos[cnt] = std::stod(line);
                cnt++;
            }
            myfile.close();
        }
        else {
            return nullptr;
        }
        return pos;
    }
}