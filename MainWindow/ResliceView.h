#ifndef  __RESLICEVIEW_H_
#define  __RESLICEVIEW_H_

#include <QApplication>
//qt + vtk include
#include "QVTKWidget.h"
//qt + vtk connect signal and slot
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
//vtk reslice
#include "vtkImageReslice.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
//change vtk interact handle
#include "vtkInteractorStyleImage.h"
#include "vtkObject.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"
//get value range
#include "vtkFloatArray.h"
#include "vtkPointData.h"
//blend two images
#include "vtkImageBlend.h"
#include "vtkImageMapToWindowLevelColors.h"
//pick pixel in image view
#include "vtkPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkMath.h"

//vtk slider calss
#include "vtkSliderWidget.h"
#include "vtkSliderRepresentation2D.h"
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>

class reslice_interactor_style;
class vtkSliderCallback;


class reslice_view_base : QObject
{
	Q_OBJECT
	friend class vtkSliderCallback;
public:
	explicit reslice_view_base(QWidget *parent = 0);
	reslice_view_base(vtkRenderWindow*,char);
	~reslice_view_base();

	void SetSlice(int x) {this->slice_n = x;};//useless
	void SetMaskOpacity(double op) {mask_actor->SetOpacity(op);};
	void Set_View_Img(vtkSmartPointer<vtkImageData>);
	void Set_Mask_Img(vtkSmartPointer<vtkImageData>);
	void RenderView();
	void RemoveMask();
	int Slice_Position;//useless

	public slots:
		void on_scroll_mouse_back(vtkObject*);
		void on_scroll_mouse_forward(vtkObject*);
		void on_click_mouse_lft(vtkObject*);
private:
	double view_dirX[3];
	double view_dirY[3];
	double view_dirZ[3];
	double center[3];

	double spacing[3];
	double origin[3];
	int    extent_m[6];

	vtkSmartPointer<vtkImageData> img_to_view;
	vtkSmartPointer<vtkImageData> img_to_mask;
	vtkSmartPointer<vtkImageReslice> reslice;
	vtkSmartPointer<vtkImageReslice> mask_reslice;
	vtkSmartPointer<vtkWindowLevelLookupTable> lookup_table;
	vtkSmartPointer<vtkImageMapToColors> color_map;
	//two actors
	vtkSmartPointer<vtkImageActor>					 actor;
	vtkSmartPointer<vtkImageActor>					 mask_actor;
	vtkSmartPointer<vtkImageMapToWindowLevelColors>  WindowLevel1;
	vtkSmartPointer<vtkImageMapToWindowLevelColors>  WindowLevel2;
	vtkSmartPointer<vtkRenderer>				     new_render;
	vtkSmartPointer<vtkRenderWindow>				 view_window;
	vtkSmartPointer<vtkRenderWindowInteractor>       Interactor;
	vtkSmartPointer<reslice_interactor_style>        InteractorStyle;

	char direction;
	int  slice_n;
	int* dimensions;
	void Set_Direction(char);

	double* calculate_img_center(vtkSmartPointer<vtkImageData>);
	void Set_Window(vtkRenderWindow*);
	void InstallPipeline();

	//qt slot connect
	vtkSmartPointer<vtkEventQtSlotConnect> m_Connections_mouse_back;
	vtkSmartPointer<vtkEventQtSlotConnect> m_Connections_mouse_forward;
	vtkSmartPointer<vtkEventQtSlotConnect> m_Connections_mouse_lft_click;

	//vtk slider
	void SetUpSlider(vtkRenderWindowInteractor*);
};





class reslice_interactor_style : public vtkInteractorStyleImage
{
public:
	static reslice_interactor_style* New();
	vtkTypeMacro(reslice_interactor_style, vtkInteractorStyleImage);

	void OnMouseWheelBackward() {};
	void OnMouseWheelForward() {};
	void OnLeftButtonDown() {};
};



#endif









