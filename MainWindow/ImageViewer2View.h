/******************************************
*This base class is not use in this project
*It is created for test 
*Use ImageReslice Instead
*This base class use vtkImageViewer2
*******************************************/

#ifndef _ImageViewer2View__H_
#define _ImageViewer2View__H_

//for visualize
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkImageViewer2.h"
#include "vtkImageActor.h"
#include "vtkSmartPointer.h"
#include "vtkInteractorStyleImage.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"

//vtk reslice
#include "vtkImageReslice.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageMapToColors.h"

//qt + vtk include
#include "QVTKWidget.h"
//qt + vtk connect signal and slot
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkImageData.h"
#include "vtkPointData.h"//cannot use SetInput in color_map if not include
#include "vtkImageProperty.h"

class vtkimageview2_base : QObject
{
	Q_OBJECT
public:
	explicit vtkimageview2_base(QWidget *parent = 0);
	vtkimageview2_base(vtkRenderWindow*,char);
	~vtkimageview2_base();

	void SetSlice(int x) {this->slice_n = x;};
	void Set_View_Img(vtkSmartPointer<vtkImageData>);
	int RenderView(int x);
	int Slice_Position;

	vtkSmartPointer<vtkRenderer> new_render;
	vtkSmartPointer<vtkImageViewer2> img_viewer2;
	vtkSmartPointer<vtkRenderWindow> view_window;

	public slots:
		void on_scroll_mouse_back(vtkObject*);
		void on_scroll_mouse_forward(vtkObject*);
private:
	double* view_dirX;
	double* view_dirY;
	double* view_dirZ;


	vtkSmartPointer<vtkImageData> img_to_view;
	//vtkSmartPointer<vtkImageReslice> reslice;
	//vtkSmartPointer<vtkWindowLevelLookupTable> lookup_table;
	//vtkSmartPointer<vtkImageMapToColors> color_map;

	char direction;
	int  slice_n;
	int* dimensions;
	void Set_Direction(char);
	double* calculate_img_center(vtkSmartPointer<vtkImageData>);
	void Set_Window(vtkRenderWindow*);

	//qt slot connect
	vtkSmartPointer<vtkEventQtSlotConnect> m_Connections_mouse_back;
	vtkSmartPointer<vtkEventQtSlotConnect> m_Connections_mouse_forward;
};

class interactor_style_viewer2 : public vtkInteractorStyleImage
{
public:
	static interactor_style_viewer2* New();
	vtkTypeMacro(interactor_style_viewer2, vtkInteractorStyleImage);

	void OnMouseWheelBackward() {};
	void OnMouseWheelForward() {};
};


#endif












