#ifndef  __MARCHINGCUBE_CONSTRUCT__
#define __MARCHINGCUBE_CONSTRUCT__

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "vtkSmartPointer.h"
#include "vtkMarchingCubes.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageViewer2.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"


class construct_base
{

public:
	construct_base(vtkSmartPointer<vtkRenderWindow> win);
	construct_base();
	~construct_base();

	//public functions
	void Set_Input_Img(vtkSmartPointer<vtkImageData>);
	void Re_Construct();
	void Set_View_Window(vtkSmartPointer<vtkRenderWindow>);
	void Set_Opicity(double);
	void Set_Cube_Value(double); 

	//public parameters
	double opicity;
	double value_cube;

private:

	//private data
	vtkSmartPointer<vtkImageData> img_to_Construct;
	vtkSmartPointer<vtkRenderer> new_render;
	vtkSmartPointer<vtkRenderWindow> view_window;
	vtkSmartPointer<vtkMarchingCubes> marchingCubes;
	vtkSmartPointer<vtkActor> actor;
};




#endif
