/* 
 * File:   Register.cpp
 * Author: zhangteng
 * 
 * Created on 2014.12.11., PM 7:39
 */

#include "Register.h"

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::Register() {
    m_registered = false;

	//init parameters
	this->max_step_length = 0.2000 ;
	this->min_step_length = 0.0010 ;
	this->num_iteration = 500 ;
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::~Register() {
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
int Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::SetFixedImage(typename FixedImageType::Pointer image)
{
    if( image.IsNull() )
    {
        std::cerr << "**Exception: input fixed image is null" << std::endl;
        return 1;
    }
    
    typedef itk::ImageDuplicator< FixedImageType > DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage( image );
    try
    {
        duplicator->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error  setting fixed image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    m_fixedImage = duplicator->GetOutput();
    m_registered = false;
    
    typedef itk::NormalizeImageFilter< FixedImageType, InternalImageType > NormalizeFilterType;
    typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
    normalizeFilter->SetInput( image );
    try
    {
        normalizeFilter->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error normalizing fixed image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    m_normalizedFixedImage = normalizeFilter->GetOutput();
    
    return 0;
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
int Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::SetMovingImage(typename MovingImageType::Pointer image)
{
    if( image.IsNull() )
    {
        std::cerr << "**Exception: input moving image is null" << std::endl;
        return 1;
    }
    
    typedef itk::ImageDuplicator< MovingImageType > DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage( image );
    try
    {
        duplicator->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error  setting moving image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    m_movingImage = duplicator->GetOutput();
        
    typedef itk::NormalizeImageFilter< MovingImageType, InternalImageType > NormalizeFilterType;
    typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
    normalizeFilter->SetInput( image );
    try
    {
        normalizeFilter->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error normalizing moving image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    m_normalizedMovingImage = normalizeFilter->GetOutput();
    
    m_registered = false;
    
    return 0;
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
int Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::GenerateTranformMatrix()
{
    typename MetricType::Pointer metric = MetricType::New();
    OptimizerType::Pointer optimizer = OptimizerType::New();
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    TransformType::Pointer transform = TransformType::New();
    typename RegistrationType::Pointer registration = RegistrationType::New();
    
    registration->SetMetric( metric );
    registration->SetOptimizer( optimizer );
    registration->SetInterpolator( interpolator );
    registration->SetTransform( transform );
    registration->SetFixedImage( m_normalizedFixedImage ) ;
    registration->SetMovingImage( m_normalizedMovingImage );
    
    /*typedef TransformType::VersorType VersorType;
    typedef VersorType::VectorType VectorType;    
    VersorType     rotation;
    VectorType     axis;
    axis[0] = 0.0;
    axis[1] = 0.0;
    axis[2] = 1.0;
    const double angle = 0;
    rotation.Set(  axis, angle  );
    transform->SetRotation( rotation );*/
    
    transform->SetIdentity();
    
    registration->SetInitialTransformParameters( transform->GetParameters() );
    
    typedef OptimizerType::ScalesType OptimizerScalesType;
    OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
    const double translationScale = 1.0/1000.0;
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;
    optimizerScales[5] = translationScale;
    optimizer->SetScales( optimizerScales );
    
    optimizer->SetMaximumStepLength( this->max_step_length );//0.2000 
    optimizer->SetMinimumStepLength( this->min_step_length );//0.0010
    optimizer->SetNumberOfIterations( this->num_iteration );//500
    
    typename CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    optimizer->AddObserver( itk::IterationEvent(), observer );
    try
    {
        registration->Update();
        std::cout << "Optimizer stop condition"
                << registration->GetOptimizer()->GetStopConditionDescription()
                << std::endl;
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error registering" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    
    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();
    
    const double versorX = finalParameters[0];
    const double versorY = finalParameters[1];
    const double versorZ = finalParameters[2];
    const double finalTranslationX = finalParameters[3];
    const double finalTranslationY = finalParameters[4];
    const double finalTranslationZ = finalParameters[5];
    const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
    const double bestValue = optimizer->GetValue();
    std::cout << std::endl << std::endl
            << "Result = " << std::endl
            << "versorX = " << versorX << std::endl
            << "versorY = " << versorY << std::endl
            << "versorZ = " << versorZ << std::endl
            << "Translation X = " << finalTranslationX << std::endl
            << "Translation Y = " << finalTranslationY << std::endl
            << "Translation Z = " << finalTranslationZ << std::endl
            << "Iterations = " << numberOfIterations << std::endl
            << "Metric value = " << bestValue << std::endl;
    
    transform->SetParameters( finalParameters );
    TransformType::MatrixType matrix = transform->GetMatrix();
    TransformType::OffsetType offset = transform->GetOffset();
    std::cout << "Matrix = " << std::endl << matrix << std::endl;
    std::cout << "Offset = " << std::endl << offset << std::endl;
    
    if( m_transform.IsNull() )
        m_transform = TransformType::New();
    m_transform->SetCenter( transform->GetCenter() );
    m_transform->SetParameters( finalParameters );
    m_transform->SetFixedParameters( transform->GetFixedParameters() );
    
    m_registered = true;
    return 0;
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
int Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::GetRegisteredMovingImage(typename MovingImageType::Pointer& image)
{
    if( !m_registered && this->GenerateTranformMatrix() != 0 )
        return 1;
    
    if( m_movingImage.IsNull() )
    {
        std::cerr << "**Error: moving image information is lost" << std::endl;
        return 1;
    }
    
    typedef itk::ResampleImageFilter< MovingImageType, MovingImageType > ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform( m_transform );
    resampler->SetInput( m_movingImage );
    resampler->SetSize( m_movingImage->GetLargestPossibleRegion().GetSize() );
    //resampler->SetOutputParametersFromImage( m_movingImage );
    resampler->SetOutputDirection( m_movingImage->GetDirection() );
    resampler->SetOutputOrigin( m_movingImage->GetOrigin() );
    resampler->SetOutputSpacing( m_movingImage->GetSpacing() );
    resampler->SetDefaultPixelValue( 0.0 );
        
    try
    {
        resampler->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error geting registered moving image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    image = resampler->GetOutput();
    
    return 0;
}

template < typename FixedImagePixelType, typename MovingImagePixelType, typename ThirdImagePixelType >
int Register<FixedImagePixelType, MovingImagePixelType, ThirdImagePixelType>::ApplyTransformToImage(typename ThirdImageType::Pointer input, typename ThirdImageType::Pointer& output)
{
    if( !m_registered && this->GenerateTranformMatrix() != 0 )
        return 1;
    
    if( input.IsNull() )
    {
        std::cerr << "**Error: input is NULL" << std::endl;
        return 1;
    }
    
    if( m_fixedImage.IsNull() )
    {
        std::cerr << "**Error: fixed image information is lost" << std::endl;
        return 1;
    }
    
    //typedef itk::ResampleImageFilter< ThirdImageType, FixedImageType > ResampleFilterType;
    typedef itk::ResampleImageFilter< ThirdImageType, ThirdImageType > ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform( m_transform );
    resampler->SetInput( input );
    resampler->SetSize( m_fixedImage->GetLargestPossibleRegion().GetSize() );
    //resampler->SetOutputParametersFromImage( m_fixedImage );
    resampler->SetOutputOrigin(  m_fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( m_fixedImage->GetSpacing() );
    resampler->SetOutputDirection( m_fixedImage->GetDirection() );
    
    /*resampler->SetSize( input->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputParametersFromImage( input );*/
    resampler->SetDefaultPixelValue( 0 );
    
    try
    {
        resampler->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error resampling image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
      
    /*typedef itk::CastImageFilter< FixedImageType, ThirdImageType > CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput( resampler->GetOutput() );
    
    try
    {
        caster->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error geting registered moving image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    output = caster->GetOutput();*/
    output = resampler->GetOutput();
    /*typedef itk::ImageDuplicator< ThirdImageType > DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage( resampler->GetOutput() );
    try
    {
        duplicator->Update();
    }
    catch( itk::ExceptionObject &err )
    {
        std::cerr << "**Error duplicating image" << std::endl;
        std::cerr << err << std::endl;
        return 1;
    }
    output =  duplicator->GetOutput();*/
    
    return 0;
}