#include "ImageViewer2View.h"



//--------new slice view class: new view method------//
vtkimageview2_base::vtkimageview2_base(vtkRenderWindow* winx,char a)
{
	// init para
	this->Set_Direction(a);
	this->Set_Window(winx);
	this->slice_n = 0;

	this->dimensions = NULL;
	this->view_dirX = NULL;
	this->view_dirY = NULL;
	this->view_dirZ = NULL;

	//map vtkaction and qt signal
	this->m_Connections_mouse_back = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_back = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_back->Connect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelBackwardEvent,this,SLOT(on_scroll_mouse_back(vtkObject*)));
	this->m_Connections_mouse_forward = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_forward = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_forward->Connect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelForwardEvent,this,SLOT(on_scroll_mouse_forward(vtkObject*)));

	//renderer init
	new_render = vtkSmartPointer<vtkRenderer>::New();
	this->view_window->AddRenderer(this->new_render);
	//set default interact null
	vtkSmartPointer<interactor_style_viewer2> new_act_style = 
		vtkSmartPointer<interactor_style_viewer2>::New();
	this->view_window->GetInteractor()->SetInteractorStyle(new_act_style);

	//view2 init
	img_viewer2 = vtkSmartPointer<vtkImageViewer2>::New();
	img_viewer2->SetRenderWindow(this->view_window);
	img_viewer2->SetRenderer(new_render);
	switch(this->direction)
	{
	case 'a':
		{
			img_viewer2->SetSliceOrientationToXY();
			break;
		}
	case 'c':
		{
			img_viewer2->SetSliceOrientationToYZ();
			break;
		}
	case 's':
		{
			img_viewer2->SetSliceOrientationToXZ();
			break;
		}
	default:
		{
			img_viewer2->SetSliceOrientationToXY();
			break;
		}
	}
}
//destructor method: use vtksmartpointer so no need to delete
vtkimageview2_base::~vtkimageview2_base()
{
	m_Connections_mouse_back->Disconnect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelBackwardEvent,this,SLOT(on_scroll_mouse_back(vtkObject*)));
	m_Connections_mouse_forward->Disconnect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelForwardEvent,this,SLOT(on_scroll_mouse_forward(vtkObject*)));
	delete[] this->dimensions;
	delete[] this->view_dirX;
	delete[] this->view_dirY;
	delete[] this->view_dirZ;
}
//render the x th slice in a 3D image
int vtkimageview2_base::RenderView(int x)
{
	this->slice_n = x;
	img_viewer2->SetSlice(this->slice_n);
	this->img_viewer2->Render();
	this->Slice_Position = x;
	return x;
}
// private method: set view direction
void vtkimageview2_base::Set_Direction(char x)
{
	this->direction = x ;
	switch (x)
	{
	case 'a':
		{
			double axialX[3] = {1,0,0};
			double axialY[3] = {0,1,0};
			double axialZ[3] = {0,0,1};
			this->view_dirX = axialX;
			this->view_dirY = axialY;
			this->view_dirZ = axialZ;
			break;
		}
	case 'c':
		{
			double coronalX[3] = {1,0,0};
			double coronalY[3] = {0,0,-1};
			double coronalZ[3] = {0,1,0};
			this->view_dirX = coronalX;
			this->view_dirY = coronalY;
			this->view_dirZ = coronalZ;
			break;
		}
	case 's':
		{
			double sagittalX[3] = {0,1,0};
			double sagittalY[3] = {0,0,-1};
			double sagittalZ[3] = {-1,0,0};
			this->view_dirX = sagittalX;
			this->view_dirY = sagittalY;
			this->view_dirZ = sagittalZ;
			break;
		}
	default:
		{
			double xxx[3] = {0,1,0};
			double yyy[3] = {0,0,-1};
			double zzz[3] = {-1,0,0};
			this->view_dirX = xxx;
			this->view_dirY = yyy;
			this->view_dirZ = zzz;
			break;
		}
	}
}
// default method: add imag to view to view widget
void vtkimageview2_base::Set_View_Img(vtkSmartPointer<vtkImageData> img)
{
	this->img_to_view = img;
	this->img_viewer2->SetInput(img_to_view);
	this->dimensions = new int[3];
	this->img_to_view->GetDimensions(this->dimensions);
	std::cout<<"dimension is :"<<dimensions[0]<<dimensions[1]<<dimensions[2]<<std::endl;
}
//private method: add widget window to render
void vtkimageview2_base::Set_Window(vtkRenderWindow* win)
{
	this->view_window = vtkSmartPointer<vtkRenderWindow>::New();
	this->view_window = win;
}
//private method: calculate imge center of a 3D image
double* vtkimageview2_base::calculate_img_center(vtkSmartPointer<vtkImageData> img)
{
	double spacing[3];
	double origin[3];
	int extent[6];

	img->GetWholeExtent(extent);
	img->GetSpacing(spacing);
	img->GetOrigin(origin);

	double center[3];
	for (int i=0;i<3;i++)
	{
		center[i] = origin[i]+spacing[i]*0.5*(extent[i]+extent[i+1]);
	}
	//please ingore the warning: no return value used
	return center;
}
//slots: when mouse wheel scroll back, next slice of image show
void vtkimageview2_base::on_scroll_mouse_back(vtkObject* obj)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren = vtkRenderWindowInteractor::SafeDownCast(obj);

	std::cout<<"direct  "<<this->direction<<"  call mouse scroll  "<<this->slice_n<<std::endl;

	switch(this->direction)
	{
	case 'a':
		{
			this->slice_n++;
			if (this->slice_n>this->dimensions[2])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[2];
			}
			break;
		}
	case 'c':
		{
			this->slice_n++;
			if (this->slice_n>this->dimensions[0])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[0];
			}
			break;
		}
	case 's':
		{
			this->slice_n++;
			if (this->slice_n>this->dimensions[1])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[1];
			}
			break;
		}
	default:
		{
			this->slice_n++;
			if (this->slice_n>this->dimensions[0])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[0];
			}
			break;
		}
	}
	this->RenderView(slice_n);
}
void vtkimageview2_base::on_scroll_mouse_forward(vtkObject* obj)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren = vtkRenderWindowInteractor::SafeDownCast(obj);

	std::cout<<"direct  "<<this->direction<<"  call mouse scroll  "<<this->slice_n<<std::endl;

	switch(this->direction)
	{
	case 'a':
		{
			this->slice_n--;
			if (this->slice_n>this->dimensions[2])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[2];
			}
			break;
		}
	case 'c':
		{
			this->slice_n--;
			if (this->slice_n>this->dimensions[0])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[0];
			}
			break;
		}
	case 's':
		{
			this->slice_n--;
			if (this->slice_n>this->dimensions[1])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[1];
			}
			break;
		}
	default:
		{
			this->slice_n--;
			if (this->slice_n>this->dimensions[0])
			{
				this->slice_n = 0;
			}
			else if (this->slice_n<0)
			{
				this->slice_n = this->dimensions[0];
			}
			break;
		}
	}
	this->RenderView(slice_n);
}

//this line is badly need to inhert a new class
//vtkObjectFactory.h must include!
vtkStandardNewMacro(interactor_style_viewer2);








