#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->init_Parameters();
	this->set_data_container(this->data_container);

	//axial_renderer->AddActor(axial_mask_Actor);
	//coronal_renderer->AddActor(coronal_mask_Actor);
	//sagittal_renderer->AddActor(sagittal_mask_Actor);

	connect(this->ui->show_Btn,SIGNAL(clicked()),this,SLOT(on_click_show()));
	connect(this->ui->show_Btn,SIGNAL(clicked()),this,SLOT(on_click_show3d()));
	connect(this->ui->img_load_Btn,SIGNAL(clicked()),this,SLOT(on_click_load()));
	connect(this->ui->del_file_Btn,SIGNAL(clicked()),this,SLOT(on_click_del_file()));
	//connect bold function=============
	//connect(this->ui->bold_Btn,SIGNAL(clicked()),this,SLOT(on_click_bold()));
	//slider bar
	connect(this->ui->view_vol_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_volume_move(int)));
	connect(this->ui->set_opacity_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_opacity_move(int)));
	connect(this->ui->strip_val_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_strip_val_move(int)));
	connect(this->ui->overlay_opacity_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_overlay_move(int)));

	//connect signal to enable atuo-scroll in info panel
	connect(this->ui->info_Panel,SIGNAL(cursorPositionChanged()),this,SLOT(info_Panel_Scroll()));

	//connect mask function Btns
	connect(this->ui->add_mask_Btn,SIGNAL(clicked()),this,SLOT(on_click_add_mask_file()));
	connect(this->ui->clr_mask_Btn,SIGNAL(clicked()),this,SLOT(on_click_del_mask()));
	connect(this->ui->mask_Btn,SIGNAL(clicked()),this,SLOT(on_click_mask()));
	//read dicom folder
	connect(this->ui->sel_folder_Btn,SIGNAL(clicked()),this,SLOT(on_click_sel_dicom()));
	//connect file list focus
	connect(this->ui->file_listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(on_file_list_focus_change(int)));

	//registration function
	connect(this->ui->register_Btn,SIGNAL(clicked()),this,SLOT(on_click_register()));
	connect(this->ui->add_refer_Btn,SIGNAL(clicked()),this,SLOT(on_click_add_refer()));
	connect(this->ui->add_src_Btn,SIGNAL(clicked()),this,SLOT(on_click_add_src()));
	connect(this->ui->bold_Btn,SIGNAL(clicked()),this,SLOT(on_clear_register()));

}

MainWindow::~MainWindow()
{
	if (this->view_axial!=NULL)
	{
		delete view_axial;
		delete view_cornoal;
		delete view_saggital;
	}
	if (this->view_coronal_reslice!=NULL)
	{
		delete view_axial_reslice;
		delete view_coronal_reslice;
		delete view_saggital_reslice;
	}

	//delete multi thread
	if (this->register_thread[0] != NULL)
	{
		delete this->register_thread[0] ;
	}

	//release dicom parse class handle
	delete dicom_parse_hd;
	dicom_parse_hd = NULL;

	delete ui;
}

void MainWindow::init_Parameters()
{

	view_saggital = NULL;
	view_cornoal  = NULL;
	view_axial    = NULL;
	
	view_axial_reslice = NULL;
	view_coronal_reslice = NULL;
	view_saggital_reslice = NULL;

	new_3d_view   = NULL;

	//actor for mask
	axial_mask_Actor   = NULL;
	coronal_mask_Actor = NULL;
	sagittal_mask_Actor= NULL;

	//init opacity slider bar
	this->ui->set_opacity_Slider->setRange(0,100);
	this->ui->set_opacity_Slider->setValue(100);
	//init strip value slider bar
	this->ui->strip_val_Slider->setRange(0,1500);
	this->ui->strip_val_Slider->setValue(500);
	//init overlay opacity slider bar
	this->ui->overlay_opacity_Slider->setRange(0,100);
	this->ui->overlay_opacity_Slider->setValue(100);

	//for multi thread
	this->register_thread[0] = NULL;

	//initial the dicom parse class
	dicom_parse_hd = NULL;
	dicom_parse_hd = new DicomParseClass;
}

void MainWindow::on_click_load()
{
	disconnect(this->ui->view_vol_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_volume_move(int)));
	this->file_name_list = 
		QFileDialog::getOpenFileNames(this,
		tr("open log file"),"./",tr("(*)"));
	if (file_name_list.isEmpty()){return;}

	for (int i=0;i<file_name_list.size();i++)
	{
		this->file_name = file_name_list[i];
		std::string filenamexx = this->file_name.toStdString();
		print_Info("Load Image:  ",file_name);

		Image_Convert_Base* img_reader = new Image_Convert_Base;
		img_reader->SetFileName(filenamexx);

		img_view_base_Type temp_img;
		temp_img.first = filenamexx;
		temp_img.second	= vtkSmartPointer<vtkImageData>::New();
		if (img_reader->GetOutput() == NULL)
		{
			delete img_reader;
			return;
		}
		vtkSmartPointer<vtkImageCast> caster = 
			vtkSmartPointer<vtkImageCast>::New();
		

#if VTK_MAJOR_VERSION <= 5
		caster->SetInput(img_reader->GetOutput());
#else
		caster->SetInputData(img_reader->GetOutput());
#endif

		caster->SetOutputScalarTypeToFloat();
		caster->Update();
		temp_img.second->DeepCopy( caster->GetOutput());
		//temp_img.second = xx_temp;
		delete img_reader;

		print_Info("Image Scalar Number: ",QString::number(temp_img.second->GetNumberOfScalarComponents()));

		int* dim = new int[3];
		temp_img.second->GetDimensions(dim);
		print_Info("Dimension: ",QString::number(*dim),QString::number(dim[1]),QString::number(dim[2]));

		double* ori = new double[3];
		temp_img.second->GetOrigin(ori);
		print_Info("Origin: ",QString::number(*ori),QString::number(ori[1]),QString::number(ori[2]));
		int* ex = new int [6];
		temp_img.second->GetExtent(ex);
		print_Info("extent: ",QString::number(*ex),QString::number(ex[1]),QString::number(ex[2]));
		print_Info("extent: ",QString::number(ex[3]),QString::number(ex[4]),QString::number(ex[5]));
		
		this->img_to_view = temp_img;
		this->data_container.push_back(temp_img);

		//create new file list item and add to file list
		QListWidgetItem* new_item =
			new QListWidgetItem(temp_img.first.c_str(),this->ui->file_listWidget);
		this->ui->file_listWidget->addItem(new_item);
	}

	//for (int i;i<this->data_container.size();i++)
	//{
	//	vtkSmartPointer<vtkImageData> test_temp = 
	//		vtkSmartPointer<vtkImageData>::New();
	//	test_temp->DeepCopy(this->data_container[i].second);
	//}

	this->set_slider_volume_range(this->data_container.size());

	connect(this->ui->view_vol_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_volume_move(int)));
}



void MainWindow::on_click_show()
{
	if (this->img_to_view.first.empty())
	{
		return;
	}

	if (view_axial != NULL)
	{
		delete view_axial;
		delete view_cornoal;
		delete view_saggital;
	}
	if (view_axial_reslice != NULL)
	{
		delete view_axial_reslice;
		delete view_coronal_reslice;
		delete view_saggital_reslice;
	}
	view_saggital_reslice = new reslice_view_base(this->ui->sagittal_view_widget->GetRenderWindow(),'s');
	view_coronal_reslice  = new reslice_view_base(this->ui->coronal_view_widget->GetRenderWindow(),'c');
	view_axial_reslice    = new reslice_view_base(this->ui->axial_view_widget->GetRenderWindow(),'a');

	////add mask first
	view_coronal_reslice->Set_View_Img(this->img_to_view.second);
	//view_cornoal_reslice->Set_Mask_Img(this->mask_img.second);
	view_coronal_reslice->RenderView();
	view_coronal_reslice->Set_View_Img(this->img_to_view.second);
	//view_cornoal_reslice->Set_Mask_Img(this->mask_img.second);
	view_coronal_reslice->RenderView();
	view_saggital_reslice->Set_View_Img(this->img_to_view.second);
//	view_saggital_reslice->Set_Mask_Img(this->mask_img.second);
	view_saggital_reslice->RenderView();
	view_axial_reslice->Set_View_Img(this->img_to_view.second);
//	view_axial_reslice->Set_Mask_Img(this->mask_img.second);
	view_axial_reslice->RenderView();
}

void MainWindow::on_click_show3d()
{
	//check if enable re-construct
	if (!this->ui->en_cst_Check->isChecked())
	{
		return;
	}

	if (this->img_to_view.first.empty())
	{
		return;
	}
	if (new_3d_view != NULL)
	{
		delete new_3d_view;
	}
	new_3d_view = new construct_base(this->ui->ster_3d_view_widget->GetRenderWindow());
	new_3d_view->Set_Input_Img(this->img_to_view.second);
	new_3d_view->Re_Construct();
}

void MainWindow::refresh_view()
{
	//refresh 2d views
	if (view_axial !=NULL)
	{
		view_axial->Set_View_Img(this->img_to_view.second);
		view_axial->RenderView(10);
		view_cornoal->Set_View_Img(this->img_to_view.second);
		view_cornoal->RenderView(13);
		view_saggital->Set_View_Img(this->img_to_view.second);
		view_saggital->RenderView(10);
	}
	if (view_axial_reslice!=NULL)
	{
		view_axial_reslice->Set_View_Img(this->img_to_view.second);
		view_axial_reslice->RenderView();
		view_coronal_reslice->Set_View_Img(this->img_to_view.second);
		view_coronal_reslice->RenderView();
		view_saggital_reslice->Set_View_Img(this->img_to_view.second);
		view_saggital_reslice->RenderView();
	}
	
	
	//refresh 3d view
	if (! this->ui->en_cst_Check->isChecked())
	{
		return;
	}
	new_3d_view->Set_Input_Img(this->img_to_view.second);
	new_3d_view->Re_Construct();
}

void MainWindow::on_click_add_mask_file()
{
	print_Info("add a ","mask file");
	this->file_name = 
		QFileDialog::getOpenFileName(this,
		tr("open log file"),"./",tr("(*)"));
	if (file_name.isEmpty()){return;}

	std::string filenamexx = this->file_name.toStdString();
	print_Info("Load Mask Image:  ",file_name);

	Image_Convert_Base* img_reader = new Image_Convert_Base;
	img_reader->SetFileName(filenamexx);

	img_view_base_Type temp_img;
	temp_img.first = filenamexx;
	temp_img.second = img_reader->GetOutput();

	//reslice mask image to adjust 
	vtkSmartPointer<vtkImageReslice> resample =
		vtkSmartPointer<vtkImageReslice>::New();
#if VTK_MAJOR_VERSION <= 5
	resample->SetInput(temp_img.second);
#else
	resample->SetInputData(temp_img.second);
#endif
	resample->SetOutputSpacing(this->img_to_view.second->GetSpacing());
	resample->SetOutputExtent(this->img_to_view.second->GetExtent());
	resample->SetInterpolationModeToLinear();
	resample->Update();

	this->mask_img = temp_img;

	print_Info("Load Mask Image:  ",
		QString::number(*temp_img.second->GetDimensions()),
		QString::number(*temp_img.second->GetDimensions()+1),
		QString::number(*temp_img.second->GetDimensions()+2));
}
void MainWindow::on_click_mask()
{	
	if (this->view_axial_reslice == NULL)
	{
		print_Info("ERROR ","SHOW first!");
		return;
	}
	print_Info("show ","mask");

	if (this->mask_img.first.empty())
	{
		print_Info("Please add a ","mask");
		return;
	}

	view_saggital_reslice->Set_Mask_Img(this->mask_img.second);
	view_coronal_reslice->Set_Mask_Img(this->mask_img.second);
	view_axial_reslice->Set_Mask_Img(this->mask_img.second);

	view_saggital_reslice->RenderView();
	view_coronal_reslice->RenderView();
	view_axial_reslice->RenderView();
}
void MainWindow::on_click_del_mask()
{
	print_Info("del a ","mask");
	if (mask_img.first.empty())
	{
		print_Info("No Mask ","to del");
		return;
	}

	//remove from view
	this->view_axial_reslice->RemoveMask();
	this->view_axial_reslice->RenderView();
	this->view_coronal_reslice->RemoveMask();
	this->view_coronal_reslice->RenderView();
	this->view_saggital_reslice->RemoveMask();
	this->view_saggital_reslice->RenderView();

	//delete mask from data container
	this->mask_img.first.clear();
	this->mask_img.second = NULL;
}

void MainWindow::on_click_sel_dicom()
{
	QString dicom_dir = QFileDialog::getExistingDirectory(this, tr("Open Dicom Directory"),
		tr("C:/Users/user/Desktop"),QFileDialog::ShowDirsOnly);

	if (dicom_dir.isEmpty())  return;

	//create a new folder to hold converted .nii file
	//create a folder
	QString output_dir = dicom_dir;
	output_dir.append(tr("-out"));
	QDir *temp = new QDir;
	bool exist = temp->exists(output_dir);
	if(exist)
		;
	else
	{
		bool ok = temp->mkdir(output_dir);
		if( ok )
			print_Info("Folder Created: ",output_dir);
	}
	delete temp;

	print_Info("Begin Parsing Dicom files!"," Please Wait...");

	std::string dcm2nii_dir = "dcm2nii.exe";//"C:/Users/USER/Desktop/MRIcron/dcm2nii.exe";
	if (this->ui->use_dcm2nii_check->isChecked())
	{
		DicomParseClass::Call_dcm2nii_func(dcm2nii_dir,dicom_dir.toStdString());
	}
	else
	{
		dicom_parse_hd->Seek_Dicom_Folder(dicom_dir);
		QString dicom_list = dicom_dir;
		dicom_list.append("\\DicomSortList.txt");
		std::cout<<"Reading"<<dicom_list.toStdString()<<std::endl;
		std::vector< vtkSmartPointer<vtkImageData> > temp_container;
		std::vector< std::string >                   temp_name_container;
		dicom_parse_hd->Load_File_from_log(dicom_list,temp_container,temp_name_container);
		
		//put data to global container
		for (int i = 0;i<temp_name_container.size();i++)
		{
			img_view_base_Type pair_temp;
			pair_temp.second = vtkSmartPointer<vtkImageData>::New();
			pair_temp.first  = temp_name_container[i];
			//add dicom dir!!
			pair_temp.first.insert(0,"\\");
			pair_temp.first.insert(0,dicom_dir.toStdString());
			pair_temp.second->DeepCopy(temp_container[i]);
			this->data_container.push_back(pair_temp);
			this->img_to_view = pair_temp;
			//add list item to listview
			QListWidgetItem* new_item =
				new QListWidgetItem(pair_temp.first.c_str(),this->ui->file_listWidget);
			this->ui->file_listWidget->addItem(new_item);

			//write for test
			//save .nii files for test
			v2iConnectorType::Pointer connector = v2iConnectorType::New();
			connector->SetInput(pair_temp.second);
			try
			{
				connector->Update();
			}
			catch (itk::ExceptionObject& e)
			{
				std::cout<<"connect itk vtk error"<<std::endl;
				std::cerr<<e;
			}

			WriterType_b::Pointer nii_writer_parse = 
				WriterType_b::New();
			nii_writer_parse->SetInput(connector->GetOutput());
			std::string name1 = pair_temp.first;
			if (name1.compare(name1.size()-4,4,".nii") == 0)
			{}//do nothing
			else
			{
				name1.append(".nii");
			}
			nii_writer_parse->SetFileName(name1);
			//nii_writer_parse->Update();
			nii_writer_parse->Write();
			//write to dicom
			//dicom_parse_hd->WiteToDicomSeries(pair_temp.second,dicom_dir.toStdString(),i);
		}

		int xxx = this->data_container.size();
		//update volume select slide bar!!disconnect first or will be error
		disconnect(this->ui->view_vol_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_volume_move(int)));
		this->set_slider_volume_range(xxx);
		connect(this->ui->view_vol_Slider,SIGNAL(valueChanged(int)),this,SLOT(on_slider_volume_move(int)));
		this->on_click_show();
		print_Info("Pare Dicom files"," Done!");
	}
}

//delete file in file list
void MainWindow::on_click_del_file()
{
	int cur_row = this->ui->file_listWidget->currentRow();

	//1. first: disconnect signal and slot first
	disconnect(this->ui->file_listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(on_file_list_focus_change(int)));

	//2. delete item
	if (this->ui->file_listWidget->takeItem(cur_row) == 0)
	{
		return;
	}

	//3. remove data
	this->data_container.erase(this->data_container.begin()+cur_row);

	//4. update volume number sliader bar
	this->set_slider_volume_range(this->data_container.size());

	//last: connect back the signal and slots
	connect(this->ui->file_listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(on_file_list_focus_change(int)));
}


void MainWindow::on_slider_volume_move(int posxx)
{
	// strange bug: no on_click_show() called error will occur
	if (this->view_axial_reslice == NULL)
	{
		return;
	}

	int pos = this->ui->view_vol_Slider->value();
	std::cout<<"position is : "<<pos<<std::endl;
	print_Info("Position is : ", QString::number(pos));

	if (this->data_container.empty())
	{
		return;
	}
	else
	{
		if (pos<1)
		{
			return;
		}
		this->img_to_view = this->data_container[pos-1];
		print_Info("Current image is: ",img_to_view.first.data());
		this->refresh_view();
	}
}

void MainWindow::on_slider_overlay_move(int)
{
	// strange bug: no on_click_show() called error will occur
	if (this->view_axial_reslice == NULL)
	{
		return;
	}

	int pos = this->ui->overlay_opacity_Slider->value();
	double opicity = double(pos)/100;
	this->ui->overlay_opacity_Label->setText(QString::number(opicity));

	if (this->mask_img.first.empty())
	{
		return;
	}
	else
	{
		if (pos<1)
		{
			return;
		}
		this->view_axial_reslice->SetMaskOpacity(opicity);
		this->view_coronal_reslice->SetMaskOpacity(opicity);
		this->view_saggital_reslice->SetMaskOpacity(opicity);
		this->view_axial_reslice->RenderView();
		this->view_coronal_reslice->RenderView();
		this->view_saggital_reslice->RenderView();
	}
}

void MainWindow::on_slider_opacity_move(int posxx)
{
	int pos = this->ui->set_opacity_Slider->value();
	
	double opacity = double(pos)/100;
	//print_Info("Opacity is : ", QString::number(opacity));
	this->ui->label_opacity->setText(QString::number(opacity));

	if (this->new_3d_view == NULL)   {return;}

	this->new_3d_view->Set_Opicity(opacity);
	this->ui->ster_3d_view_widget->GetRenderWindow()->Render();
}
void MainWindow::on_slider_strip_val_move(int)
{
	int pos = this->ui->strip_val_Slider->value();
	double strip_val = pos;
	//print_Info("strip value is : ", );
	this->ui->label_strip_value->setText(QString::number(strip_val));

	if (this->new_3d_view == NULL)   {return;}

	this->new_3d_view->Set_Cube_Value(strip_val);
	this->new_3d_view->Re_Construct();
	this->ui->ster_3d_view_widget->GetRenderWindow()->Render();
}





void MainWindow::mouse_Wheel_move(QWheelEvent *e)
{
	int numDegree = e->delta()/8;
	int numSteps = numDegree/15;
	std::cout<<numDegree<<std::endl;
	print_Info("axial slice is: ",QString::number(this->view_axial->Slice_Position));
	print_Info("cornoal slice is: ",QString::number(this->view_cornoal->Slice_Position));
	print_Info("saggital slice is: ",QString::number(this->view_saggital->Slice_Position));
}

//information scroll in info panel
void MainWindow::info_Panel_Scroll()
{
	QTextCursor text_cursor =  ui->info_Panel->textCursor();
	text_cursor.movePosition(QTextCursor::End);
	ui->info_Panel->setTextCursor(text_cursor);
}
void MainWindow::print_Info(QString in,QString x)
{
	QString temp = in;
	temp.append(x);
	temp.append("\n");
	ui->info_Panel->insertPlainText(temp);
}
void MainWindow::print_Info(QString in,QString x,QString y,QString z)
{
	QString temp = in;
	temp.append(x);
	temp.append(y);
	temp.append(z);
	temp.append("\n");
	ui->info_Panel->insertPlainText(temp);
}
//basic method to calculate center of the image
double* MainWindow::calculate_img_center(vtkSmartPointer<vtkImageData> img)
{
	double spacing[3];
	double origin[3];
	
#if VTK_MAJOR_VERSION <= 5
	int extent[6];
	img->GetWholeExtent(extent);
#else
	int* extent;
	extent = img->GetExtent();
#endif
	img->GetSpacing(spacing);
	img->GetOrigin(origin);

	double center[3];
	for (int i=0;i<3;i++)
	{
		center[i] = origin[i]+spacing[i]*0.5*(extent[i]+extent[i+1]);
	}

	return center;
}
void MainWindow::set_slider_volume_range(int num)
{
	this->ui->view_vol_Slider->setMinimum(1);
	this->ui->view_vol_Slider->setMaximum(num);

	//set display number
	this->ui->min_vol_Lab->setText("1");
	this->ui->max_vol_Lab->setText(QString::number(this->data_container.size()));
}
void MainWindow::set_data_container(std::vector<img_view_base_Type >  da_con)
{
	if (da_con.empty())
	{
		return;
	}
	else
	{
		this->data_container = da_con;
	}
}


void MainWindow::on_file_list_focus_change(int row)
{
	if (this->view_axial_reslice == NULL)
	{
		return;
	}
	std::cout<<"row is "<<row<<std::endl;
	this->data_container.size();
	this->img_to_view = this->data_container[row];
	this->ui->view_vol_Slider->setValue(row+1);
	this->refresh_view();
}

void MainWindow::on_click_add_refer()
{
	int index = this->ui->file_listWidget->currentRow();

	if ( index < 0)
	{
		return;
	}
	//if there is no item, then create a item
	if (this->ui->reg_refer_List->takeItem(0) == 0)
	{
		QListWidgetItem* new_item =
			new QListWidgetItem(data_container[index].first.c_str(),this->ui->reg_refer_List);
		this->ui->reg_refer_List->addItem(new_item);
		this->reference_img.first = data_container[index].first;
		this->reference_img.second = vtkSmartPointer<vtkImageData>::New();
		this->reference_img.second->DeepCopy(data_container[index].second);
	}
	//if there is a item, change its name
	else
	{
		QListWidgetItem* new_item =
		new QListWidgetItem(data_container[index].first.c_str(),this->ui->reg_refer_List);
		this->ui->reg_refer_List->addItem(new_item);
		this->reference_img.first = data_container[index].first;
		this->reference_img.second->DeepCopy(data_container[index].second);
	}
}

void MainWindow::on_click_add_src()
{
	int index = this->ui->file_listWidget->currentRow();

	if ( index < 0)
	{
		return;
	}
	QListWidgetItem* new_item =
		new QListWidgetItem(data_container[index].first.c_str(),this->ui->reg_src_List);
	this->ui->reg_src_List->addItem(new_item);

	this->register_container.push_back(data_container[index]);
}
void MainWindow::on_clear_register()
{
	this->ui->reg_refer_List->clear();
	this->ui->reg_src_List->clear();
	this->register_container.clear();
}

void MainWindow::on_click_register()
{
	if (register_container.size() == 0)
	{
		return;
	}
	if (this->reference_img.first.empty())
	{
		return;
	}

	print_Info("Registering Process","  Running");
	//start register process
	if (register_thread[0] != NULL)
	{
		delete register_thread[0];
	}
	register_thread[0] = new multi_thread(this);
	register_thread[0]->start();

	////enable some buttons
	//this->ui->bold_Btn->setEnabled(true);//name clear(temp)
	//this->ui->register_Btn->setEnabled(true);
	//this->ui->add_src_Btn->setEnabled(true);
	//this->ui->add_refer_Btn->setEnabled(true);
}










multi_thread::multi_thread(MainWindow* win)
{
	this->main_win = win;
}

multi_thread::~multi_thread()
{

}

void multi_thread::run()
{
	//start register process
	this->main_win->print_Info("Registering Process","  Running");
	for (int i=0;i<this->main_win->register_container.size();i++)
	{
		std::string file_name =this->main_win->register_container[i].first;
		if (file_name.compare(file_name.size()-4,4,".nii") == 0)
		{	
		}
		else
		{
			file_name.append(".nii");
		}
		auto reg_temp = Registration_Process(this->main_win->reference_img.second,
			this->main_win->register_container[i].second,file_name);

		//save to dicom series
		//find dicom source; if no dicom file source->escape
		std::string full_name_temp = this->main_win->register_container[i].first;
		unsigned found = full_name_temp.find_last_of("/\\");
		std::string path_temp = full_name_temp.substr(0,found);
		//seek to top-folder
		std::string dir = path_temp;
		//create a folder
		QString output_dir = dir.c_str();
		output_dir.append(tr("-reg"));
		QDir *temp = new QDir;
		bool exist = temp->exists(output_dir);
		if(exist)
			;
		else
		{
			bool ok = temp->mkdir(output_dir);
			if( ok )
				main_win->print_Info("Folder Created: ",output_dir);
			else
				return;
		}
		delete temp;
		//get name (without path)
		std::string name_temp = full_name_temp.substr(found+1);


		if (main_win->dicom_parse_hd->series_name_container.empty())
		{
			;
		}
		else
		{
			auto it = std::find (main_win->dicom_parse_hd->series_name_container.begin(),
				main_win->dicom_parse_hd->series_name_container.end(),
				name_temp);
				//main_win->register_container[i].first);

			if (it != main_win->dicom_parse_hd->series_name_container.end())
			//find dicom source; if there is dicom file source, use the same gdcmIO to generate dicom series
			{
				//get position
				auto ser_pos = it - main_win->dicom_parse_hd->series_name_container.begin();
				main_win->dicom_parse_hd->WiteToDicomSeries(reg_temp,
					output_dir.toStdString(),
					main_win->dicom_parse_hd->input_dict_container[ser_pos],
					main_win->dicom_parse_hd->gdcmIO_container[ser_pos]);
			}
			else
			{
				main_win->print_Info("No dicom file source, ", " no writing to dicom files");
			}
		}
	}
	main_win->print_Info("Registering Process"," Done!!");
}

