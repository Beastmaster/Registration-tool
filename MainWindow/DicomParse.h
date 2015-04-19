/******************************************
*Function: seek dicom image folder to read dicom headers
*
*Copyright: Mr Luo Jiaxi
*Changed by: Qin Shuo
*Date: 2015.3.30
*Additional: Write DICOM Series Function
******************************************/
#ifndef _DICOMPARSE_H_
#define _DICOMPARSE_H_

//use windows.h to call system function
//#include <windows.h>
#include <stdlib.h>
#include <vector>

#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
//read dcm file
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
//write dcm series
#include "itkMetaDataObject.h"
#include "itkImageSeriesWriter.h"
#include "itkNumericSeriesFileNames.h"
//read .nii and .hdr/img file
#include "itkNiftiImageIO.h"
//itk image file reader
#include <itkImageFileReader.h>
//itk image file writer
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
//convert vtk data format to itk data format
#include "itkVTKImageToImageFilter.h"
//convert itk data format to vtk data format
#include "itkImageToVTKImageFilter.h"
#include "gdcmUIDGenerator.h"
#include "itkShiftScaleImageFilter.h"
//cast pixel type
#include "itkCastImageFilter.h"

#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QList>
#include <QStringList>
#include <QFileInfoList>
#include <QDirIterator>
#include <QMessageBox>
#include <QFile>
#include <Qtextstream>
#include <QDialog>
#include <QProgressDialog>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageChangeInformation.h"


struct File_info_in_DicomParse
{
	QStringList name;
	QList<qint64> path_position;
	QStringList description;
	QStringList slice_number;
	QStringList temperal_number;
};


class DicomParseClass
{
public:
	typedef itk::GDCMSeriesFileNames					NamesGeneratorType;
	typedef float										PixelType;//typedef short      PixelType;
	typedef itk::Image< PixelType, 3 >					ImageType;
	typedef itk::ImageSeriesReader< ImageType >			ReaderType;
	typedef itk::GDCMImageIO							ImageIOType;
	//reader type (defined in DialogLoadImage.h)
	typedef itk::ImageSeriesReader< ImageType >			ReaderType;
	//writer type
	typedef itk::ImageFileWriter  < ImageType >			WriterType;
	//define IOs
	typedef itk::NiftiImageIO							NiftiIOType;
	typedef itk::GDCMImageIO							DicomIOType;
	//connector type
	typedef itk::ImageToVTKImageFilter<ImageType>		i2vConnectorType;
	typedef itk::VTKImageToImageFilter<ImageType>		v2iConnectorType;
	//dicom tags container type
	typedef itk::MetaDataDictionary						DictionaryType;
	//dicom tags are represented as strings
	typedef itk::MetaDataObject<std::string>			MetaDataStringType;
	typedef std::vector< std::string >					SeriesIdContainer;

	DicomParseClass();
	~DicomParseClass();

//select dir
void Seek_Dicom_Folder(QString m_strDir);
//sub function
void ParseParseParse(QString path, QString m_strDir);
//parse log file
void Load_File_from_log(QString log_name,
						std::vector<vtkSmartPointer<vtkImageData> >& container,
						std::vector<std::string>&);
//read the log and transform context into data
void Log2Container_inDicomParse(QString log_name, 
				   File_info_in_DicomParse* info, 
				   std::vector<vtkSmartPointer<vtkImageData> >& container,
				   int index,
				   std::vector<std::string>&);
void Log2Container_inDicomParse_Exception(QString log_name, 
								File_info_in_DicomParse* info, 
								std::vector<vtkSmartPointer<vtkImageData> >& container,
								int index,
								std::vector<std::string>&);

void Log2Container_inDicomParse_Stereo(QString log_name, 
										  File_info_in_DicomParse* info, 
										  std::vector<vtkSmartPointer<vtkImageData> >& container,
										  int index,
										  std::vector<std::string>&);

void Log2Container_inDicomParse_Stereo_Only(QString log_name, 
											File_info_in_DicomParse* info, 
											std::vector<vtkSmartPointer<vtkImageData> >& container,
											int index,
											std::vector<std::string>&);
//call dcm2nii function to convert dicom images to nii
static void Call_dcm2nii_func(std::string cmd_path,std::string folder_path);
static int Parse_GetDicomTag_InstanceNumber(std::string slice_name);
//Function: Convert vtkiamge to a dicom series
//IO:  input: vtkImageData
void WiteToDicomSeries(vtkSmartPointer<vtkImageData>,std::string dir,int index);
void WiteToDicomSeries(ImageType::Pointer,std::string dir,itk::ImageSeriesReader<ImageType>::DictionaryRawPointer&,ImageIOType::Pointer&);//DictionaryType);

void Set_Output_Dir(std::string dir) {this->output_dir = dir;};

void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);



//
std::string						output_dir;
std::vector<ReaderType::DictionaryRawPointer>   input_dict_container;
std::vector<DicomIOType::Pointer>				gdcmIO_container;
std::vector<std::string>						series_name_container;


private:
	SeriesIdContainer				dicom_seriesID_container;
	std::vector<DictionaryType>     dicom_directionary_container;
};



#endif