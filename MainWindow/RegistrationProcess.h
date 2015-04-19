/*******************
* Re Construct of ZhangTeng's Register code
* Source frome ITK registration method
* By Qinshuo  
* Fool's Day__
*****************/

#ifndef _REGISTRATION_PROCESS_H_
#define _REGISTRATION_PROCESS_H_

#include "Register.h"

//convert vtk data format to itk data format
#include "itkVTKImageToImageFilter.h"
//convert itk data format to vtk data for
#include "itkImageToVTKImageFilter.h"

#include "itkimage.h"
#include "itkImageBase.h"

//for image resampling
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"


typedef itk::Image< float , 3 >             ImageTypex; //image type pixel:float;dimension:3
//connector type
typedef itk::ImageToVTKImageFilter<ImageTypex> i2vConnectorType;
typedef itk::VTKImageToImageFilter<ImageTypex> v2iConnectorType;

//
typedef vtkSmartPointer<vtkImageData> Reg_Image_Type;



//Reg_Image_Type Registration_Process(Reg_Image_Type ,Reg_Image_Type , std::string);
ImageTypex::Pointer Registration_Process(Reg_Image_Type fix, Reg_Image_Type src, std::string name);



#endif
