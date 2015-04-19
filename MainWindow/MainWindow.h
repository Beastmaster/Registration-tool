/*****************************
*By	Qin Shuo
*HUST  Graduation Project MainWindow
*Date: 2015.3.10
******************************/
#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QMainWindow>
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <functional>
#include <algorithm>

//qt open file_dialog
#include <QFileDialog>
//messagebox
#include <QMessageBox>
#include <QWheelEvent>
//qt string class
#include <QString>
#include <QStringList>
//enable multi thread!
#include <QThread>

//change vtk interact handle
#include "vtkInteractorStyleImage.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"

//qt + vtk include
#include "QVTKWidget.h"
//qt + vtk connect signal and slot
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
//vtk include files
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"//cannot use SetInput in color_map if not include
#include "vtkImageProperty.h"
#include <vtkImageMapper3D.h>
#include "vtkMetaImageReader.h"
#include "vtkDICOMImageReader.h"

//imageviewer2base class
#include "ImageViewer2View.h"

// my class
#include "ResliceView.h"

//include my image convert class
#include "ImageConvert.h"

//include my construct
#include "MarchingCube_construct.h"

//
#include "DicomParse.h"
#include "RegistrationProcess.h"



//----------typedefs for itk IOs-------------//
//pointer type : float
//typedef itk::Point< double,3 >				PointType;//cooridnate and spacing
//image type: float, 3 dimensions (defined in DialogLoadImage.h)
typedef itk::Image< float , 3 >             ImageTypex; //image type pixel:float;dimension:3
//image type for atlas 
typedef itk::Image<short, 3>					AtlasImageType;
typedef itk::Image<unsigned char, 3>						AtlasLabelType;
//reader type: atlas
typedef itk::ImageFileReader<AtlasImageType> AtlasReaderType;
typedef itk::ImageFileReader<AtlasLabelType> LabelReaderType;
//reader type (defined in DialogLoadImage.h)
typedef itk::ImageSeriesReader< ImageTypex > ReaderType_b;
//writer type
typedef itk::ImageFileWriter  < ImageTypex > WriterType_b;
//define IOs
typedef itk::NiftiImageIO					 NiftiIOType;
typedef itk::GDCMImageIO					 DicomIOType;
//connector type
typedef itk::ImageToVTKImageFilter<ImageTypex> i2vConnectorType;
typedef itk::VTKImageToImageFilter<ImageTypex> v2iConnectorType;
//dicom tags container type
typedef itk::MetaDataDictionary				 DictionaryType;
//dicom tags are represented as strings
typedef itk::MetaDataObject<std::string>	 MetaDataStringType;

struct File_info
{
	QStringList name;
	QList<qint64> path_position;
	QStringList description;
	QStringList slice_number;
	QStringList temperal_number;
};





class vtkimageview2_base;
class multi_thread ;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	friend class multi_thread;
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	std::vector<std::pair<std::string, vtkSmartPointer<vtkImageData> > > 
		GetDataContainer() {return this->data_container;};
//define slots
private slots:
	//load images
	void on_click_load();
	void on_click_show();
	void on_click_show3d();
	void on_click_add_mask_file();
	void on_click_mask();
	void on_click_del_mask();
	void on_click_sel_dicom();
	void on_click_del_file();
	void on_click_add_refer();
	void on_click_add_src();
	void on_click_register();
	void on_clear_register();
	void on_slider_volume_move(int);
	void on_slider_opacity_move(int);
	void on_slider_strip_val_move(int);
	void on_slider_overlay_move(int);
	void info_Panel_Scroll();
	void mouse_Wheel_move(QWheelEvent *e);
	void init_Parameters();
	void on_file_list_focus_change(int row);

private:
	//GUI
	Ui::MainWindow	*ui;

	//file names
	QString file_name;
	QStringList file_name_list;

	//first: name; second: vtkImageData
	typedef std::pair<std::string, vtkSmartPointer<vtkImageData> > img_view_base_Type;
	img_view_base_Type img_to_view;
	img_view_base_Type mask_img;
	img_view_base_Type reference_img;
	std::vector<img_view_base_Type > data_container;
	std::vector<img_view_base_Type > register_container;


	//pravite methods
	double* calculate_img_center(vtkSmartPointer<vtkImageData>);
	void set_slider_volume_range(int);
	void set_data_container(std::vector<img_view_base_Type > );
	void refresh_view();
	void print_Info(QString,QString); 
	void print_Info(QString,QString,QString,QString);

	//qt vtk views
	vtkimageview2_base* view_axial;
	vtkimageview2_base* view_cornoal;
	vtkimageview2_base* view_saggital;


	//
	reslice_view_base* view_axial_reslice;
	reslice_view_base* view_coronal_reslice;
	reslice_view_base* view_saggital_reslice;

	//marching cube class base
	construct_base* new_3d_view;

	//vtk image views
	vtkSmartPointer<vtkRenderer> axial_renderer;
	vtkSmartPointer<vtkRenderer> coronal_renderer;
	vtkSmartPointer<vtkRenderer> sagittal_renderer;

	//actor for mask
	vtkSmartPointer<vtkImageActor> axial_mask_Actor;
	vtkSmartPointer<vtkImageActor> coronal_mask_Actor;
	vtkSmartPointer<vtkImageActor> sagittal_mask_Actor;

	//multi thread!!!
	multi_thread* register_thread[1];

	//containers to write dicom series
	DicomParseClass* dicom_parse_hd;
};



class multi_thread : public QThread
{
	Q_OBJECT
public:
	multi_thread(MainWindow*);
	~multi_thread();
	void run();
protected:
	MainWindow* main_win;
};





#endif

