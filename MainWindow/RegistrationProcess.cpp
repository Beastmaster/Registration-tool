#include "RegistrationProcess.h"


ImageTypex::Pointer Registration_Process(Reg_Image_Type fix, Reg_Image_Type src, std::string name)
//Reg_Image_Type Registration_Process(Reg_Image_Type fix, Reg_Image_Type src, std::string name) 
{
	//1. connect fixed image to ITK data type
	vtkSmartPointer<vtkImageData> temp_fixed_image = 
		vtkSmartPointer<vtkImageData>::New();
	temp_fixed_image->DeepCopy(fix);
	v2iConnectorType::Pointer v2iconnector_pre_temp = v2iConnectorType::New();
	v2iconnector_pre_temp->SetInput(fix);
	try
	{
		v2iconnector_pre_temp->Update();
	}
	catch( itk::ExceptionObject &err )
	{
		std::cerr << "**Error converting vtk type to itk type" << std::endl;
		std::cerr << err << std::endl;
		return NULL;
	}
	ImageTypex::Pointer itk_temp_fixed_image = v2iconnector_pre_temp->GetOutput();

	//2. connect source image to ITK data type
	vtkSmartPointer<vtkImageData> temp_src_image = 
		vtkSmartPointer<vtkImageData>::New();
	temp_src_image->DeepCopy(src);
	v2iConnectorType::Pointer v2iconnector_pre_temp1 = v2iConnectorType::New();
	v2iconnector_pre_temp1->SetInput(temp_src_image);
	try
	{
		v2iconnector_pre_temp1->Update();
	}
	catch( itk::ExceptionObject &err )
	{
		std::cerr << "**Error converting vtk type to itk type" << std::endl;
		std::cerr << err << std::endl;
		return NULL;
	}
	ImageTypex::Pointer itk_temp_src_image = v2iconnector_pre_temp1->GetOutput();

	//3. resample images 
	typedef itk::ResampleImageFilter<ImageTypex,ImageTypex> ResampleFilterType;
	ResampleFilterType::Pointer resample_filter = ResampleFilterType::New();
	typedef itk::AffineTransform<double,3> TransformType;
	TransformType::Pointer transform = TransformType::New();
	resample_filter->SetTransform(transform);
	typedef itk::NearestNeighborInterpolateImageFunction<ImageTypex,double> InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();
	resample_filter->SetInterpolator(interpolator);
	resample_filter->SetDefaultPixelValue(0);

	ImageTypex::SpacingType src_spacing = 	itk_temp_src_image->GetSpacing();
	ImageTypex::PointType   src_origin  =  itk_temp_src_image->GetOrigin();
	ImageTypex::SizeType    src_size    =  itk_temp_src_image->GetLargestPossibleRegion().GetSize();
	ImageTypex::DirectionType direction;
	direction.SetIdentity();

	resample_filter->SetOutputDirection(direction);
	resample_filter->SetOutputSpacing(src_spacing);
	resample_filter->SetOutputOrigin(src_origin);
	resample_filter->SetSize(src_size);
	resample_filter->SetInput(itk_temp_fixed_image);
	try
	{
		resample_filter->Update();
	}
	catch( itk::ExceptionObject &err )
	{
		std::cerr << "**Error Resampling!!**" << std::endl;
		std::cerr << err << std::endl;
		return NULL;
	}
	ImageTypex::Pointer resampled_itk_temp_fixed_image = resample_filter->GetOutput();
	typedef itk::ImageFileWriter  < ImageTypex > WriterType_reg;
	WriterType_reg::Pointer nii_writerx = 
		WriterType_reg::New();
	nii_writerx->SetInput(resampled_itk_temp_fixed_image);
	std::string new_name = name;
	new_name.insert(new_name.find_last_of("."),"-rspd");
	nii_writerx->SetFileName(new_name);
	//nii_writerx->Update();


	//4. run register process
	ImageTypex::Pointer  registered_img= ImageTypex::New();
	typedef Register< float, float, float > RegisterType_process;
	RegisterType_process * reg_hd = new RegisterType_process;    
	reg_hd->SetFixedImage( resampled_itk_temp_fixed_image );/////////////////////////////
	reg_hd->SetMovingImage( itk_temp_src_image );
	reg_hd->GenerateTranformMatrix();    
	reg_hd->GetRegisteredMovingImage( registered_img );
	delete reg_hd;

	//5. save registered image
	typedef itk::ImageFileWriter  < ImageTypex > WriterType_reg;
	WriterType_reg::Pointer nii_writer = 
		WriterType_reg::New();
	nii_writer->SetInput(registered_img);
	std::string name2 = name;
	name2.insert(name2.find_last_of("."),"-reg");
	nii_writer->SetFileName(name2);
	nii_writer->Update();


	//5. convert itk image to vtk image
	i2vConnectorType::Pointer i2vconnector_lag_temp = i2vConnectorType::New();
	i2vconnector_lag_temp->SetInput(registered_img);
	try
	{
		i2vconnector_lag_temp->Update();
	}
	catch( itk::ExceptionObject &err )
	{
		std::cerr << "**Error converting ITK type to VTK type" << std::endl;
		std::cerr << err << std::endl;
		return NULL;
	}

	//return i2vconnector_lag_temp->GetOutput();
	return registered_img;
}
