#include "dialogDICOM.h"
#include "ui_dialogDICOM.h"
#include <QFileDialog>
#include <iostream>
using namespace std;

#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkDICOMReader.h>
#include <vtkIndent.h>
#include <vtkNew.h>

#include <QMessageBox>

CDialogDICOM::CDialogDICOM(QWidget *parent)
    : QDialog(parent), m_pUI(new Ui::dialogDICOM) {
  m_pUI->setupUi(this);
  m_pUI->tableWidget->setVisible(false);
  m_pUI->btnBack->setEnabled(false);
  m_pUI->btnNext->setEnabled(false);
  // setup for the displayed table
  QStringList labels;
  labels << "# Serie"
         << "Modality"
         << "Description"
         << "Patient"
         << "Dimensions";
  m_pUI->tableWidget->setHorizontalHeaderLabels(labels);
  // m_pUI->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  // m_pUI->tableWidget->horizontalHeader()->setStretchLastSection(true);
  m_bSecondWindow = false;
  m_iSelectedItem = -1;
}

CDialogDICOM::~CDialogDICOM() { 
    m_pUI; }


using namespace std;
//

#pragma comment(lib, "../lib/libmx.lib")
#pragma comment(lib, "../lib/libmat.lib")


/// Accomplish two functions:
/// 1) open a file dialog for DICOMDir selection,
/// 2) copy selected data to received from the parent window.
/// Invokes the accept event when the m_bSecondWindow is true.
void CDialogDICOM::openDICOMDir() {
  if (m_bSecondWindow) // 4. final step! when finish button is pressed
  {
    // copy information to pass to mainwindow
    m_metaData = m_DICOMdir->GetMetaDataForSeries(m_iSelectedItem);

    vtkSmartPointer<vtkStringArray> sortedFiles = m_DICOMdir->GetFileNamesForSeries(m_iSelectedItem);
    vtkNew<vtkDICOMReader> reader;
    reader->SetMemoryRowOrderToFileNative();
    reader->SetFileNames(sortedFiles);
    reader->Update();

    m_imgDICOM = vtkSmartPointer<vtkImageData>::New();
    auto spacing = reader->GetDataSpacing();
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
    //int spacing[3] = { 1, 1, 1 };
    //cout << "spacing: " << spacing[0] << endl;
    //cout << "spacing: " << spacing[1] << endl;
    //cout << "spacing: " << spacing[2] << endl;
    auto r = m_metaData->Get(DC::Rows);
    auto c = m_metaData->Get(DC::Columns);
    m_imgDICOM->SetDimensions(r.GetInt(0), c.GetInt(0), m_metaData->GetNumberOfInstances());




    m_imgDICOM->SetOrigin(0, 0, 0);
    m_imgDICOM->AllocateScalars(VTK_DOUBLE, 1);
    m_imgDICOM = reader->GetOutput();

    accept();
  } else // 1. to select the DICOMDIR file
  {
    QString qStrFilename = QFileDialog::getOpenFileName(this, "Open DICOMDIR", QDir::homePath(), "All files (*.*)");

    if (!qStrFilename.isNull()) {
      auto index = qStrFilename.lastIndexOf('/');
      QString qstrDir = qStrFilename.left(index).toUtf8();
      QString qstrFilename = qStrFilename.mid(index + 1).toUtf8();
      // check if DICOMDIR read is valid: DICOM file has to contain text DICM at
      // the offset 0x80, so that tags start at the offset 0x84 of file
      std::string strFileName = qStrFilename.toUtf8().constData();
      std::ifstream fileDICOM(strFileName); // qstring to std::string
      if (fileDICOM.is_open()) {
        fileDICOM.seekg(0x80); // C++ 17, hexadecimal constant
        std::string strTag;
        strTag.resize(4);
        fileDICOM.read(&strTag[0], 4);
        if (strTag.compare("DICM") == 0) // check if everything is ok
        {

            

          m_pUI->txtInput->setText(qstrFilename);
          m_pUI->lblPath->setText(qstrDir);
          m_pUI->btnNext->setEnabled(true);
        } else {
          std::string strMessage = "The file " + strFileName + " is not in DICOMDIR format.";
          QMessageBox::critical(this, "Error", strMessage.c_str());
        }
        fileDICOM.close();
      } else {
        std::string strMessage = "The file " + strFileName + " is not found.";
        QMessageBox::critical(this, "Error", strMessage.c_str());
      }
    } else {
      // if nothing is selected, then the Next button is disable
      m_pUI->btnNext->setEnabled(false);
    }
  }
}

/// Accomplish two functions: 1) show the table (and associated buttons) and 2)
/// save the selected ttem to the finish button.
void CDialogDICOM::next() {
  if (m_bSecondWindow) // 3. Study was selected already, shows the description
  {
    m_pUI->lblTitle->setText(tr("Study's info"));
    m_pUI->lblSubTitle->setVisible(true);
    m_pUI->btnBack->setVisible(false);
    m_pUI->btnNext->setVisible(false);
    m_pUI->btnCancel->setVisible(false);
    m_pUI->tableWidget->setVisible(false);
    // use the browse button and change labas as the finish button
    m_pUI->btnBrowse->setVisible(true);
    m_pUI->btnBrowse->setGeometry(430, 230, 70, 25);
    m_pUI->btnBrowse->setText(tr("Finish"));
    m_pUI->lblSubTitle->setText(QString::fromStdString(m_strInfo));

    // extract info of selected item from widget (extracting index)
    auto item = m_pUI->tableWidget->selectedItems().at(0);
    m_iSelectedItem = item->row();
    m_dicomSerie = item->data(0).toInt();
  } else // 2. selection window: fill the table to present to user
  {
    m_pUI->btnBrowse->setVisible(false);
    m_pUI->lblPath->setVisible(false);
    m_pUI->lblSubTitle->setVisible(false);
    m_pUI->txtInput->setVisible(false);
    m_pUI->lblTitle->setText(tr("Select DICOM serie to open"));
    m_pUI->btnNext->setEnabled(false);
    m_pUI->btnBack->setEnabled(true);
    m_pUI->tableWidget->setVisible(true);
    if (fillTable())
      m_bSecondWindow = true; // to able the next window
  }
}

/// Function which fill table with patient's data (using vtkDICOMDirectory)
/// @return true is exists some studies and some series for the content in
/// lblPath
bool CDialogDICOM::fillTable() {
  // Use vtkDICOMDirectory to scan a directory with MRI images
  string strFolder = m_pUI->lblPath->text().toStdString();
  m_DICOMdir = vtkSmartPointer<vtkDICOMDirectory>::New();
  m_DICOMdir->SetDirectoryName(strFolder.c_str());
  m_DICOMdir->RequirePixelDataOn();
  m_DICOMdir->Update();
  m_strInfo.clear(); // clear description

  int iNStudies = m_DICOMdir->GetNumberOfStudies();
  if (iNStudies > 0) {
    // fill description string (using the first study
    auto study = m_DICOMdir->GetStudyRecord(0);
    m_strInfo += study.Get(DC::StudyInstanceUID).AsString();
    m_strInfo += "\n";
    m_strInfo += study.Get(DC::StudyDescription).AsString();
    m_strInfo += "\n";
    m_strInfo += study.Get(DC::StudyID).AsString();
    m_strInfo += "\n";
    m_strInfo += study.Get(DC::StudyDate).AsString();
    m_strInfo += "\n";
    m_strInfo += study.Get(DC::StudyComments).AsString();
    m_strInfo += "\n";

    // function to center the item to insert
    auto fcnCenterItem = [](QString text) -> QTableWidgetItem * {
      QTableWidgetItem *item = new QTableWidgetItem(text);
      item->setTextAlignment(Qt::AlignCenter);
      return item;
    };

    for (int i = 0; i < iNStudies; i++) {
      // cout << "Study #" << i << endl;
      // get number of series availables
      int iNSeries = m_DICOMdir->GetNumberOfSeries();
      if (iNSeries <= 0)
        return false;

      for (int k = 0; k < iNSeries; k++) {
          vtkDICOMMetaData* meta;
          vtkNew<vtkDICOMReader> reader;
          if (m_showCompletTag) {
              vtkSmartPointer<vtkStringArray> sortedFiles = m_DICOMdir->GetFileNamesForSeries(k);
              reader->SetMemoryRowOrderToFileNative();
              reader->SetFileNames(sortedFiles);
              reader->UpdateInformation();
              meta = reader->GetMetaData();
          }
          else {
              meta = m_DICOMdir->GetMetaDataForSeries(k);
          }


        std::string strDim; // dimension as width x height x depth
        strDim = meta->Get(DC::Rows).AsString() + " x " + meta->Get(DC::Columns).AsString() + " x " + std::to_string(meta->GetNumberOfInstances());

        int iRow = m_pUI->tableWidget->rowCount(); // get the lastest inserted row
        m_pUI->tableWidget->insertRow(iRow);
        m_pUI->tableWidget->setItem(iRow, 0,fcnCenterItem(QString::fromStdString(meta->Get(DC::SeriesNumber).AsString())));
        m_pUI->tableWidget->setItem(iRow, 1,fcnCenterItem(QString::fromStdString(meta->Get(DC::Modality).AsString())));
        m_pUI->tableWidget->setItem(iRow, 2,new QTableWidgetItem(QString::fromStdString(meta->Get(DC::SeriesDescription).AsString())));
        m_pUI->tableWidget->setItem(iRow, 3,new QTableWidgetItem(QString::fromStdString(meta->Get(DC::PatientName).AsString())));
        m_pUI->tableWidget->setItem(iRow, 4, new QTableWidgetItem(QString::fromStdString(strDim)));
      }
      m_pUI->tableWidget
          ->resizeColumnsToContents(); // to fit columns into the content
    }
  } else
    return false; // no studies
  return true;
}

/// Enable/disable the Next button, only when item was selected
/// @param item represented for the signal event, the selected item of the table
void CDialogDICOM::selectedItem(QTableWidgetItem *item) {
  if (item->isSelected())
    m_pUI->btnNext->setEnabled(true);
  else
    m_pUI->btnNext->setEnabled(false);
}