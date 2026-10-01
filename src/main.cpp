#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <QFileDialog>
#include <qresource.h>
#include <qfontdatabase.h>
#include <qtextstream.h>

#include <vtkOpenGLRenderWindow.h>

#include "logmanager.h"
#include "mainview.h"

int main(int argc, char *argv[]) {
    bronchox::log::LogManager logger;
    logger.Initialize();

    BRONCHOX_INFO("BRONCHOX STATUS: STARTED");

    std::string configFile = "bronchox_config.json";


#pragma region CONSTRUCT_QAPPLICATION
    BRONCHOX_INFO("BRONCHOX STATUS: QAPP CREATED");
    QApplication app(argc, argv);
    BRONCHOX_INFO("BRONCHOX STATUS: LOAD QAPP CONFIG FILE (" + configFile + ")");
    std::ifstream filejson(configFile);
    if (!filejson.is_open()) {
        BRONCHOX_WARN("CAN NOT LOAD CONFIG FILE");
    }
    else {
        nlohmann::json m_config = nlohmann::json::parse(filejson);
        vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(
            0); // should be placed before the the creation of the QApplication
        QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());


        QApplication::setAttribute(
            Qt::AA_Use96Dpi); // if windows set the 150%, 200% the text size, there
                              // will be problems
        // QApplication app(argc, argv);

        // load resources
        std::string res = m_config["resource"];
        if (!QResource::registerResource(QString::fromStdString(res))) {
            BRONCHOX_WARN("Resource file was not loaded properly.");
        }

        // set stylesheet
        std::string style = m_config["style"];
        if (QFile f(QString::fromStdString(style)); !f.exists()) {
            BRONCHOX_WARN("Unable to set stylesheet, file .qss not found.");
        }
        else {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream stream(&f);
            app.setStyleSheet(stream.readAll());
        }

        // set font
        std::string font = m_config["font"];
        if (int idFont = QFontDatabase::addApplicationFont(QString::fromStdString(font)); idFont == -1) {
            BRONCHOX_WARN("Font segoeui not loaded.");
        }
        else {
            QString family = QFontDatabase::applicationFontFamilies(idFont).at(0);
            QFont myFont(family);
            myFont.setPointSize(8);
            QApplication::setFont(myFont);
        }
    }
#pragma endregion CONSTRUCT_QAPPLICATION


    std::unique_ptr<bronchox::ui::MainView> mainwindow; // manager class
    mainwindow = std::make_unique<bronchox::ui::MainView>();
    BRONCHOX_INFO("BRONCHOX STATUS: SHOW MAIN WINDOW");
    mainwindow->show();
    app.exec();



    BRONCHOX_INFO("BRONCHOX STATUS: STOP");
    logger.Shutdown();
    return 0;
}