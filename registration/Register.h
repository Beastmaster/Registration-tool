/* 
 * File:   Register.h
 * Author: zhangteng
 *
 * Created on 2014.12.11., PM. 7:39
 */

#ifndef REGISTER_H
#define	REGISTER_H

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCommand.h"
#include "itkImageDuplicator.h"
#include "itkNormalizeImageFilter.h"

#include "itkNormalizedCorrelationImageToImageMetric.h"

//template < class FixedImagePixelType = double , class MovingImagePixelType = double, class ThirdImagePixelType = double >
template < typename FixedImagePixelType = double , typename MovingImagePixelType = double, typename ThirdImagePixelType = double >
class Register {
public:
    typedef typename itk::Image< FixedImagePixelType, 3  > FixedImageType;
    typedef itk::Image< MovingImagePixelType, 3 > MovingImageType;
    typedef itk::Image< ThirdImagePixelType, 3 > ThirdImageType;
    typedef itk::Image< double, 3 > InternalImageType;
    typedef itk::VersorRigid3DTransform< double > TransformType;
    typedef itk::VersorRigid3DTransformOptimizer OptimizerType;
    typedef itk::MattesMutualInformationImageToImageMetric< InternalImageType, InternalImageType > MetricType;
    //typedef itk::NormalizedCorrelationImageToImageMetric< InternalImageType, InternalImageType > MetricType;
    typedef itk::LinearInterpolateImageFunction< InternalImageType, double > InterpolatorType;
    typedef itk::ImageRegistrationMethod< InternalImageType, InternalImageType > RegistrationType;
    
public:
    class CommandIterationUpdate : public itk::Command
    {
        public:
            typedef  CommandIterationUpdate   Self;
            typedef  itk::Command             Superclass;
            typedef itk::SmartPointer<Self>   Pointer;
            itkNewMacro( Self );
        protected:
            CommandIterationUpdate() {};
    public:
        typedef itk::VersorRigid3DTransformOptimizer OptimizerType;
        typedef   const OptimizerType *              OptimizerPointer;
        void Execute(itk::Object *caller, const itk::EventObject & event)
        {
            Execute( (const itk::Object *)caller, event);
        }
        void Execute(const itk::Object * object, const itk::EventObject & event)
        {
            OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );
            if( ! itk::IterationEvent().CheckEvent( &event ) )
            {
                return;
            }
            std::cout << optimizer->GetCurrentIteration() << "   ";
            std::cout << optimizer->GetValue() << "   ";
            std::cout << optimizer->GetCurrentPosition() << std::endl;
        }
    };
    
public:
    Register();
    Register(const Register& orig);
    virtual ~Register();
    
    virtual int SetFixedImage( typename FixedImageType::Pointer image );
    virtual int SetMovingImage(typename MovingImageType::Pointer image );
    
    virtual int GenerateTranformMatrix( void );
    
    virtual int GetRegisteredMovingImage( typename MovingImageType::Pointer &image );
    
    virtual int ApplyTransformToImage( typename ThirdImageType::Pointer input, typename ThirdImageType::Pointer &output );
    
	TransformType::Pointer GetTranformMatrix(void) {return m_transform;};
	void SetMaximumStepLength(double max) {this->max_step_length = max;};
	void SetMinimumStepLength(double min) {this->min_step_length = min;};
	void SetNumberOfIterations(itk::SizeValueType num) {this->num_iteration = num;};

protected:
    typename FixedImageType::Pointer m_fixedImage;
    typename MovingImageType::Pointer m_movingImage;
    typename InternalImageType::Pointer m_normalizedFixedImage;
    typename InternalImageType::Pointer m_normalizedMovingImage;
    
    TransformType::Pointer m_transform;
    bool m_registered;

	//add by qinshuo: iterate step length and number of iterations
	double min_step_length;
	double max_step_length;
	itk::SizeValueType num_iteration;
};

# include "Register.hpp"

#endif	/* REGISTER_H */

