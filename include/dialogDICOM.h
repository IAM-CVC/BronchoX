#ifndef _DIALOG_DICOM_H_
#define _DIALOG_DICOM_H_

#include <vtkDICOMDirectory.h>
#include <vtkDICOMFileSorter.h>
#include <vtkDICOMItem.h>
#include <vtkDICOMMetaData.h>
#include <vtkDICOMTag.h>
#include <vtkDICOMValue.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>


#include "ui_dialogDICOM.h"
#include <QDialog>

namespace Ui {
	class CDialogDICOM;
}

/// Handle the link between the GUI and the logic part
class CDialogDICOM : public QDialog {
	Q_OBJECT

public:
	explicit CDialogDICOM(QWidget* parent = 0);
	~CDialogDICOM();

	inline vtkSmartPointer<vtkImageData> getImgDICOM() { return m_imgDICOM; }
	inline vtkSmartPointer<vtkDICOMMetaData> getMetaDICOM() { return m_metaData; }
	inline std::string getDICOMdir() { return m_DICOMdir->GetDirectoryName(); };
	inline int getDICOMSerieSelected() { return m_dicomSerie; };
	inline double* getSpacing() { return m_spacing; };

private slots:
	virtual void openDICOMDir();		//	Open dicom dialog
	virtual void next();			//	Open dicom dialog
	virtual void selectedItem(QTableWidgetItem*);	//	Open dicom dialog

private:
	Ui::dialogDICOM* m_pUI; // pointer to the window GUI
	bool m_bSecondWindow;
	bool m_showCompletTag = false;	//	flag to show complete information of tags in serie list window
	std::string m_strInfo;
	int m_iSelectedItem;	//	serie selected
	int m_dicomSerie;	//	serie selected
	double m_spacing[3];	//	3d spacing of medical images 
	vtkSmartPointer<vtkDICOMDirectory> m_DICOMdir;

	vtkSmartPointer<vtkDICOMMetaData> m_metaData;
	vtkSmartPointer<vtkImageData> m_imgDICOM;
	
	bool fillTable();
};

#endif // _DIALOG_DICOM_H_
