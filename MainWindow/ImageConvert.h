#ifndef __IMAGE_CONVERT_H__
#define __IMAGE_CONVERT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>

//use itk to convert 2D slices to 3D volume
#include <itkImage.h>
//itk smart pointer
#include <itkSmartPointer.h>
//itk image file reader
#include <itkImageFileReader.h>
//itk image file writer
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
//convert vtk data format to itk data format
#include "itkVTKImageToImageFilter.h"
//convert itk data format to vtk data format
#include "itkImageToVTKImageFilter.h"
//read dcm file
#include "itkGDCMImageIO.h"
//read .nii and .hdr/img file
#include "itkNiftiImageIO.h"
//
//#include ""
//itk cast image to different type
#include "itkCastImageFilter.h"
//deep copy
#include "itkImageRegionIterator.h"
#include "vtkSmartPointer.h"

//
#include "vtkImageData.h"
#include "vtkMetaImageReader.h"
#include "vtkDICOMImageReader.h"
//cast image to same scalartype
#include "vtkImageCast.h"

typedef itk::Image< float , 3 >             ImageTypex; //image type pixel:float;dimension:3

//define IOs
typedef itk::NiftiImageIO					 NiftiIOType;
typedef itk::GDCMImageIO					 DicomIOType;

//connector type
typedef itk::ImageToVTKImageFilter<ImageTypex> i2vConnectorType;
typedef itk::VTKImageToImageFilter<ImageTypex> v2iConnectorType;

//reader type (defined in DialogLoadImage.h)
//typedef itk::ImageSeriesReader< ImageTypex > ReaderType_b;
typedef itk::ImageFileReader< ImageTypex > ReaderType_Convert;
//writer type
typedef itk::ImageFileWriter  < ImageTypex > WriterType_Convert;


class Image_Convert_Base
{
public:
	Image_Convert_Base();
	~Image_Convert_Base();

	void SetFileName(std::string);
	vtkSmartPointer<vtkImageData> GetOutput();
	static ImageTypex::Pointer GetITKPointer(std::string);
private:
	void Get_Name_Suffix();
	std::string file_name;
	std::string file_suffix;
};




#endif