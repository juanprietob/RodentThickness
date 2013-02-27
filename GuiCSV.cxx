#include "GuiCSV.h"
/*Qt classes*/
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
/*std classes*/
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <csignal>
#include <cstdlib> // for getenv() and exit()
/*itk classes*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include <itksys/SystemTools.hxx> // for FindProgram() and GetFilenamePath()
/* Values of the C variables:
F_OK= 0
W_OK= 2
R_OK= 4
X_OK= 1
*/
mode_t ITKmode_F_OK = 0;
mode_t ITKmode_W_OK = 2;
mode_t ITKmode_R_OK = 4;
mode_t ITKmode_X_OK = 1;
GuiCSV::GuiCSV(std::string dataset,std::string configfile,std::string PathBms,bool noGUI,std::string WorkDir,std::string commandRan) : QMainWindow()
{
setupUi(this);

m_ErrorDetectedInConstructor=false;
RScriptButton->setEnabled(false);
RScriptPathButton->setEnabled(false);
concatToColumnsPythonScriptButton->setEnabled(false);
vtkPointAttributesPythonScriptButton->setEnabled(false);
RScriptResetButton->setEnabled(false);
RScriptPathResetButton->setEnabled(false);
concatToColumnsPythonScriptResetButton->setEnabled(false);
vtkPointAttributesPythonScriptResetButton->setEnabled(false);



m_scriptrunner = new ScriptRunner;
m_statisticalpartrunner = new StatisticalpartRunner;
m_noGUI=noGUI;
m_ScriptRunning=false;
m_statistic=0;
m_CSVseparator = QString("	");
m_ParamSaved=1;
if(!m_noGUI)
{
QObject::connect(loaddataButton, SIGNAL(clicked()), this, SLOT(ReadCSVSlot()));
QObject::connect(savedatabutton, SIGNAL(clicked()), this, SLOT(SaveCSVDatasetBrowseSlot()));
QObject::connect(OutputFolderButton, SIGNAL(clicked()), this, SLOT(OpenOutputBrowseWindow()));
QObject::connect(ComputeButton, SIGNAL(clicked()), this, SLOT(Compute()));
QObject::connect(tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(FullTableWidget(int,int)));
QObject::connect(AddRowButton, SIGNAL(clicked()), this, SLOT(OpenAddRow()));
QObject::connect(DeleteRowButton, SIGNAL(clicked()),this, SLOT(RemoveSelectedRow()));
QObject::connect(DefaultButton, SIGNAL(clicked()), this, SLOT(ConfigDefaultSlot()));
QObject::connect(actionRead_Me, SIGNAL(triggered()), this, SLOT(ReadMe()));
QObject::connect(actionExit, SIGNAL(triggered()), this, SLOT(ExitProgram()));
QObject::connect(actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(LoadConfigSlot()));
QObject::connect(actionSave_Configuration, SIGNAL(triggered()), this, SLOT(SaveConfigSlot()));
QObject::connect(StatisticBox, SIGNAL(stateChanged(int)), this, SLOT(WidgetHasChangedParam()));

// Browse software path Buttons 
	QSignalMapper *SoftButtonMapper = new QSignalMapper();
	QObject::connect(SoftButtonMapper, SIGNAL(mapped(int)), this, SLOT( BrowseSoft(int) ));

	QObject::connect(measureThicknessFilterButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(measureThicknessFilterButton,1);
	QObject::connect(GenParaMeshCLPButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(GenParaMeshCLPButton,2);
	QObject::connect(ParaToSPHARMMeshCLPButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(ParaToSPHARMMeshCLPButton,3);
	QObject::connect(MeshPointIntensitySamplingButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(MeshPointIntensitySamplingButton,4);
	QObject::connect(ShapeWorksRunButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(ShapeWorksRunButton,5);
	QObject::connect(ImageMathButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(ImageMathButton,6);
	QObject::connect(SegPostprocessButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(SegPostprocessButton,7);
	QObject::connect(ShapeWorksPythonScriptButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(ShapeWorksPythonScriptButton,8);
	QObject::connect(concatToColumnsPythonScriptButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(concatToColumnsPythonScriptButton,9);
	QObject::connect(vtkPointAttributesPythonScriptButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(vtkPointAttributesPythonScriptButton,10);
	QObject::connect(PathFilesBmsButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(PathFilesBmsButton,11);
	QObject::connect(RScriptButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(RScriptButton,12);
	QObject::connect(PythonPathButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(PythonPathButton,13);
	QObject::connect(RScriptPathButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(RScriptPathButton,14);
	QObject::connect(ShapeWorksGroomButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(ShapeWorksGroomButton,15);
	QObject::connect(BinaryToDistanceMapButton, SIGNAL(clicked()), SoftButtonMapper, SLOT(map()));
	SoftButtonMapper->setMapping(BinaryToDistanceMapButton,16);
/* Reset software path Buttons */
	QSignalMapper *ResetSoftButtonMapper = new QSignalMapper();
	QObject::connect(ResetSoftButtonMapper, SIGNAL(mapped(int)), this, SLOT( ResetSoft(int) ));

	QObject::connect(measureThicknessFilterResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(measureThicknessFilterResetButton,1);
	QObject::connect(GenParaMeshResetCLPButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(GenParaMeshResetCLPButton,2);
	QObject::connect(ParaToSPHARMMeshCLPResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(ParaToSPHARMMeshCLPResetButton,3);
	QObject::connect(MeshPointIntensitySamplingResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(MeshPointIntensitySamplingResetButton,4);
	QObject::connect(ShapeWorksRunResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(ShapeWorksRunResetButton,5);
	QObject::connect(ImageMathResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(ImageMathResetButton,6);
	QObject::connect(SegPostprocessResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(SegPostprocessResetButton,7);
	QObject::connect(PythonPathResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(PythonPathResetButton,8);
	QObject::connect(RScriptPathResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(RScriptPathResetButton,9);
	QObject::connect(ShapeWorksGroomResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(ShapeWorksGroomResetButton,10);
	QObject::connect(BinaryToDistanceMapResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(BinaryToDistanceMapResetButton,11);
	QObject::connect(ShapeWorksPythonScriptResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(ShapeWorksPythonScriptResetButton,12);
	QObject::connect(concatToColumnsPythonScriptResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(concatToColumnsPythonScriptResetButton,13);
	QObject::connect(vtkPointAttributesPythonScriptResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(vtkPointAttributesPythonScriptResetButton,14);
	QObject::connect(RScriptResetButton, SIGNAL(clicked()), ResetSoftButtonMapper, SLOT(map()));
	ResetSoftButtonMapper->setMapping(RScriptResetButton,15);
} //no gui


/* SET the soft config */
// set the path to the executable directory for FindProgram
std::string RTExecutablePath= itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(commandRan).c_str() ); // get the place where the running executable is
if(RTExecutablePath=="") RTExecutablePath= itksys::SystemTools::GetCurrentWorkingDirectory(); // If called by itself ($ RodentThickness) = either in the PATH or in the current directory : will be found either way by find_program
m_FindProgramRTExecDirVec.push_back(RTExecutablePath); // FindProgram will search in the executable directory too

if( configfile.empty() )  ConfigDefault();

// Look for the config file in the executable directory
std::string SoftConfigPath= RTExecutablePath + "/RodentThicknessConfigfile.bms";
if( itksys::SystemTools::GetPermissions(SoftConfigPath.c_str(),ITKmode_F_OK) ) if( LoadConfig(QString( SoftConfigPath.c_str() )) == -1 ) m_ErrorDetectedInConstructor=true; // if file exists

// Look for the config file in the current directory
std::string CurrentPath = itksys::SystemTools::GetRealPath( itksys::SystemTools::GetCurrentWorkingDirectory().c_str() ); //GetRealPath() to remove symlinks
SoftConfigPath = CurrentPath + "/RodentThicknessConfigfile.bms";
if( itksys::SystemTools::GetPermissions( SoftConfigPath.c_str() , ITKmode_F_OK) ) if( LoadConfig(QString( SoftConfigPath.c_str() )) == -1 ) m_ErrorDetectedInConstructor=true; // if file exists

	 

	if( !dataset.empty() ) 
	{
		if( ReadCSV( QString(dataset.c_str())) == -1 ) m_ErrorDetectedInConstructor=true;
	}
	else if(m_noGUI) // no parameters and nogui => not possible
	{
		std::cout<<"| Please give a dataset file"<<std::endl; // command line display
		m_ErrorDetectedInConstructor=true;
	} 		

	if( !configfile.empty() ) 
	{
		if( LoadConfig( QString(configfile.c_str())) == -1 ) m_ErrorDetectedInConstructor=true;
	}
	else if(m_noGUI) // no parameters and nogui => not possible
	{
		std::cout<<"| Please give a parameter file"<<std::endl; // command line display
		m_ErrorDetectedInConstructor=true;
	}
	if( !WorkDir.empty() ) 
	{
		OutputFolderPath->setText(QString(WorkDir.c_str()));
	}
	else if(m_noGUI) // no parameters and nogui => not possible
	{
		std::cout<<"| Please give an output directory"<<std::endl; // command line display
		m_ErrorDetectedInConstructor=true;
	}
	if( !PathBms.empty() ) 
	{
		PathFilesBmsPath->setText(QString(PathBms.c_str()));
	}
	else if(m_noGUI) // no parameters and nogui => not possible
	{
		std::cout<<"| Please give apath for bms file"<<std::endl; // command line display
		m_ErrorDetectedInConstructor=true;
	}
 	

	std::string notFound;
	if(measureThicknessFilterPath->text().isEmpty()) notFound = notFound + "> measureThicknessFilter\n";
	if(GenParaMeshCLPPath->text().isEmpty()) notFound = notFound + "> GenParaMeshCLP\n";
	if(ParaToSPHARMMeshCLPPath->text().isEmpty()) notFound = notFound + "> ParaToSPHARMMeshCLP\n";
	if(MeshPointIntensitySamplingPath->text().isEmpty()) notFound = notFound + "> MeshPointIntensitySampling\n";
	if(ShapeWorksRunPath->text().isEmpty()) notFound = notFound + "> ShapeWorksRun\n";
	if(ImageMathPath->text().isEmpty()) notFound = notFound + "> ImageMath\n";
	if(SegPostprocessPath->text().isEmpty()) notFound = notFound + "> SegPostprocessCLP\n";
	if(ShapeWorksPythonScriptPath->text().isEmpty()) notFound = notFound + "> shapeworks.py\n";
	if(concatToColumnsPythonScriptPath->text().isEmpty()) notFound = notFound + "> concatToColumns.py\n";
	if(vtkPointAttributesPythonScriptPath->text().isEmpty()) notFound = notFound + "> vtkPointAttributes.py\n";
	if(RScriptRodentThicknessPath->text().isEmpty()) notFound = notFound + "> rodentThicknessStats.r\n";
	if(PythonPath->text().isEmpty()) notFound = notFound + "> python\n";
	if(RScriptPath->text().isEmpty()) notFound = notFound + "> Rscript\n";
	if(ShapeWorksGroomPath->text().isEmpty()) notFound = notFound + "> ShapeWorksGroom\n";
	if(BinaryToDistanceMapPath->text().isEmpty()) notFound = notFound + "> BinaryToDistanceMap\n";
	
	if( !notFound.empty() )
	{
		if(!m_noGUI)
		{
			std::string text = "The following programs have not been found.\nPlease enter the path manually or open a configuration file:\n" + notFound;
			QMessageBox::warning(this, "Program missing", QString(text.c_str()) );
		}
		else std::cout<<"| The following programs have not been found. Please give a configuration file or modify it or enter the path manually in the GUI:\n"<< notFound <<std::endl;
	}
}

  /////////////////////////////////////////
 //                EXIT                 //
/////////////////////////////////////////

void GuiCSV::ExitProgram() /*SLOT*/
{
	std::cout<<"| End of the program"<<std::endl; // command line display
	delete m_statisticalpartrunner;
	delete m_scriptrunner;
	
	qApp->quit(); //end of Application: close the main window
}

void GuiCSV::closeEvent(QCloseEvent* event)
{
	int ret = QMessageBox::question(this,"Quit","Dou you want quit ?",QMessageBox::No | QMessageBox::Yes);
	if (ret == QMessageBox::No) 
	{
		event->ignore();
		return;
	}	
			
	event->accept();
}
  /////////////////////////////////////////
 //                ROWS                //
/////////////////////////////////////////

void GuiCSV::OpenAddRow() /*SLOT*/
{
	
	int ItemRow;
	ItemRow = tableWidget->row( tableWidget->selectedItems().at(0) );
	
	if(ItemRow<0)
	{
	ItemRow=tableWidget->rowCount();
	
	tableWidget->QTableWidget::insertRow(ItemRow);
	}
	else tableWidget->QTableWidget::insertRow(ItemRow+1);
	
}

void GuiCSV::RemoveSelectedRow() /*SLOT*/
{

	int ItemRow;
	ItemRow = tableWidget->row( tableWidget->selectedItems().at(0) );
	if(ItemRow<0)
	{
	ItemRow=tableWidget->rowCount();
	tableWidget->QTableWidget::removeRow(ItemRow-1);
	}
	else tableWidget->QTableWidget::removeRow(ItemRow);
	
	

}
  /////////////////////////////////////////
 //               DATASET               //
/////////////////////////////////////////
void GuiCSV::FullTableWidget(int row, int column)
{	
	if (column==1)
	{
	QString item=QFileDialog::getOpenFileName(this, "Open image", QString(), "NRRD Images (*.nrrd *.nhdr)");
	QTableWidgetItem * Itemcell=new QTableWidgetItem( tr("%1").arg( item ) );
	
	tableWidget->setItem(row,column,Itemcell);
	}
	

}
void GuiCSV::ReadCSVSlot() /*SLOT*/
{	
	QString CSVBrowse=QFileDialog::getOpenFileName(this, "Open CSV File", QString(), ".csv Files (*.csv)");

	if(!CSVBrowse.isEmpty())
	{
		ReadCSV(CSVBrowse);
	}
	
}

int GuiCSV::ReadCSV(QString CSVfile)
{	
	if(!CSVfile.isEmpty())
	{
		if( itksys::SystemTools::GetPermissions(CSVfile.toStdString().c_str(), ITKmode_F_OK) ) // Test if the csv file exists => unistd::access() returns 0 if F(file)_OK
		{
			QFile file(CSVfile);

			if (file.open(QFile::ReadOnly))
			{
				std::cout<<"| Loading csv file \'"<< CSVfile.toStdString() <<"\'..."; // command line display
				int i=0;
				QTextStream stream(&file);
				while(!stream.atEnd()) //read all the lines
				{
					
						QString line = stream.readLine();
						QStringList list = line.split(m_CSVseparator);
					if(tableWidget->rowCount()<=i) tableWidget->QTableWidget::insertRow(i);
					
					if(i==0)
					{
						
						if(list.at(0) != "subjId" || list.at(1) != "labelMapInput" || list.at(2) != "group")
						{
							qDebug( "Could not open csv file, first line is not correct");						
							return -1;
						}
					}else
					{
						QTableWidgetItem * Itemcell=new QTableWidgetItem( tr("%1").arg( list.at(0)) );
						tableWidget->setItem(i-1,0,Itemcell);
						QTableWidgetItem * Itemcell1=new QTableWidgetItem( tr("%1").arg( list.at(1)) );
						tableWidget->setItem(i-1,1,Itemcell1);
						QTableWidgetItem * Itemcell2=new QTableWidgetItem( tr("%1").arg( list.at(2)) );
						tableWidget->setItem(i-1,2,Itemcell2);
						
					}
					i++;
				}
				
				std::cout<<"DONE"<<std::endl; // command line display
			} 
			else
			{
				
				qDebug( "Could not open csv file");
				return -1;
			}

		}
		else std::cout<<"| The given CSV file does not exist"<<std::endl; // command line display
	}
	int ItemRow;
	ItemRow=tableWidget->rowCount();
	tableWidget->QTableWidget::removeRow(ItemRow-1);
	return 0;
}
void GuiCSV::SaveCSVDatasetBrowseSlot()
{
	if(tableWidget->rowCount()<1)
	{
		QMessageBox::critical(this, "No Cases", "Please give at least one case");
	}
	else if(checkdataset()==1)
	{
		 QMessageBox::critical(this, "Item Empty", "Please enter all the item for subject and Image");
	}
	else if(checkdataset()==2)
	{
		 QMessageBox::critical(this, "Item Empty", "Please enter all the item for subject and Image and Group");
	}
	else	
	{	
		QString CSVBrowseName = QFileDialog::getSaveFileName(this, tr("Save Dataset"),"/dataset.csv",tr("CSV File (*.csv)"));
		if(!CSVBrowseName.isEmpty())
		{	
			if(SaveCSVDatasetBrowse(CSVBrowseName)==0) QMessageBox::information(this, "Saving succesful", "Dataset has been succesfully saved at" + CSVBrowseName);
		}
	}
}
int GuiCSV::SaveCSVDatasetBrowse(QString CSVBrowseName) /*SLOT*/
{

		QFile file(CSVBrowseName);
		if ( file.open( IO_WriteOnly | IO_Translate ) )
		{
			std::cout<<"| Generating Dataset csv file..."; // command line display
	
			QTextStream stream( &file );
			stream << QString("subjId") << m_CSVseparator << QString("labelMapInput") << m_CSVseparator << QString("group")<< endl;
		
			for(int i=0; i < tableWidget->rowCount();i++) 
			{	
			
				stream << tableWidget->item(i,0)->text()<< m_CSVseparator << tableWidget->item(i,1)->text()<< m_CSVseparator << tableWidget->item(i,2)->text()<< endl;
			
			}
			std::cout<<"DONE"<<std::endl; // command line display
			return 0;			
		}
		else
		{
			qDebug( "Could not create file");	
			return -1 ;
		}
}

  /////////////////////////////////////////
 //         SOFT CONFIGURATION          //
/////////////////////////////////////////

void GuiCSV::LoadConfigSlot() /*SLOT*/
{
	QString ConfigBrowse=QFileDialog::getOpenFileName(this, "Open Configuration File", QString(), ".bms Files (*.bms)");

	if(!ConfigBrowse.isEmpty())
	{
		LoadConfig(ConfigBrowse);
	}	
}

int GuiCSV::LoadConfig(QString configFile) // returns -1 if fails, otherwise 0
{
	if( access(configFile.toStdString().c_str(), F_OK) == 0 ) // Test if the config file exists => unistd::access() returns 0 if F(file)_OK
	{
		std::cout<<"| Loading Configuration file \'"<< configFile.toStdString() <<"\'..."; // command line display

		std::string notFound;

		///get the values from file
		QFile file(configFile);
		if (file.open(QFile::ReadOnly))
		{	
			QTextStream stream(&file);

			QString line = stream.readLine();
			int n=line.size();
			line.remove(n-1,1);
			QStringList list = line.split(" ");
			
			if(!list.at(0).contains(QString("Set(measureThicknessFilterPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) measureThicknessFilterPath->setText(list.at(1));
			else if(measureThicknessFilterPath->text().isEmpty()) notFound = notFound + "> measureThicknessFilter\n";	

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(GenParaMeshCLPPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) GenParaMeshCLPPath->setText(list.at(1));
			else if(GenParaMeshCLPPath->text().isEmpty()) notFound = notFound + "> GenParaMeshCLP\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(ParaToSPHARMMeshCLPPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) ParaToSPHARMMeshCLPPath->setText(list.at(1));
			else if(ParaToSPHARMMeshCLPPath->text().isEmpty()) notFound = notFound + "> ParaToSPHARMMeshCLP\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(MeshPointIntensitySamplingPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) MeshPointIntensitySamplingPath->setText(list.at(1));
			else if(MeshPointIntensitySamplingPath->text().isEmpty()) notFound = notFound + "> MeshPointIntensitySampling\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(ShapeWorksRunPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) ShapeWorksRunPath->setText(list.at(1));
			else if(ShapeWorksRunPath->text().isEmpty()) notFound = notFound + "> ShapeWorksRun\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(ShapeWorksGroomPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) ShapeWorksGroomPath->setText(list.at(1));
			else if(ShapeWorksGroomPath->text().isEmpty()) notFound = notFound + "> ShapeWorksGroom\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(ImageMathPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) ImageMathPath->setText(list.at(1));
			else if(ImageMathPath->text().isEmpty()) notFound = notFound + "> ImageMath\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(SegPostprocessPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) SegPostprocessPath->setText(list.at(1));
			else if(SegPostprocessPath->text().isEmpty()) notFound = notFound + "> SegPostProcessCLP\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(ShapeWorksPythonScriptPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) ShapeWorksPythonScriptPath->setText(list.at(1));
			else if(ShapeWorksPythonScriptPath->text().isEmpty()) notFound = notFound + "> PythonScript\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(PythonPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) PythonPath->setText(list.at(1));
			else if(PythonPath->text().isEmpty()) notFound = notFound + "> python\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(BinaryToDistanceMapPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) BinaryToDistanceMapPath->setText(list.at(1));
			else if(BinaryToDistanceMapPath->text().isEmpty()) notFound = notFound + "> BinaryToDistanceMap\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(concatToColumnsPythonScriptPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) concatToColumnsPythonScriptPath->setText(list.at(1));
			else if(concatToColumnsPythonScriptPath->text().isEmpty()) notFound = notFound + "> concatToColumnsPythonScript\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(vtkPointAttributesPythonScriptPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) vtkPointAttributesPythonScriptPath->setText(list.at(1));
			else if(vtkPointAttributesPythonScriptPath->text().isEmpty()) notFound = notFound + "> vtkPointAttributesPythonScript\n";

			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(RScriptRodentThicknessPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) RScriptRodentThicknessPath->setText(list.at(1));
			else if(RScriptRodentThicknessPath->text().isEmpty()) notFound = notFound + "> rodentThicknessStats.r\n";
			
			
			line = stream.readLine();
			n=line.size();
			line.remove(n-1,1);
			list = line.split(" ");
			if(!list.at(0).contains(QString("Set(RScriptPath")))
			{
				if(!m_noGUI)
				{
					QMessageBox::critical(this, "Corrupt File", "This config file is corrupted");
					std::cout<<"FAILED"<<std::endl; // command line display
				}
				else std::cout<<"FAILED"<<std::endl<<"| This config file is corrupted"<<std::endl;
				return -1;
				
			}
			if(!list.at(1).isEmpty()) RScriptPath->setText(list.at(1));
			else if(RScriptPath->text().isEmpty()) notFound = notFound + "> Rscript\n";

			std::cout<<"DONE"<<std::endl; // command line display
			if( !notFound.empty() )
			{
				if(!m_noGUI)
				{
					std::string text = "The following programs are missing.\nPlease enter the path manually:\n" + notFound;
					QMessageBox::warning(this, "Program missing", QString(text.c_str()) );
				}
				else
				{
					std::cout<<"| The following programs have not been found. Please give a configuration file or modify it or enter the path manually in the GUI:\n"<< notFound <<std::endl;
					return -1;
				}
			}
		}
		else qDebug( "Could not open file");
	}
	else std::cout<<"| The given configuration file does not exist"<<std::endl; // command line display

	return 0;
}
void GuiCSV::SaveConfigSlot()
{
	QString ConfigBrowseName=QFileDialog::getSaveFileName(this, tr("Save Configuration File"),"RodentThicknessconfigfile.bms",tr("Bms File (*.bms)"));
	if(!ConfigBrowseName.isEmpty())
	{	
	SaveConfig(ConfigBrowseName);
	QMessageBox::information(this, "Saving succesful", "ConfigFile have been succesfully saved at" + ConfigBrowseName);
	}
}
void GuiCSV::SaveConfig(QString ConfigBrowseName) /*SLOT*/
{
	if(!ConfigBrowseName.isEmpty())
	{
/* getting the values and generating the config file */

		QFile file(ConfigBrowseName);
		if ( file.open( IO_WriteOnly | IO_Translate ) )
		{
			std::cout<<"| Generating config file..."; // command line display

			QTextStream stream( &file );

			
			stream << "Set(measureThicknessFilterPath " << measureThicknessFilterPath->text() <<")"<< endl;
			stream << "Set(GenParaMeshCLPPath " << GenParaMeshCLPPath->text() <<")"<< endl;
			stream << "Set(ParaToSPHARMMeshCLPPath " << ParaToSPHARMMeshCLPPath->text() <<")"<< endl;
			stream << "Set(MeshPointIntensitySamplingPath " << MeshPointIntensitySamplingPath->text() <<")"<< endl;
			stream << "Set(ShapeWorksRunPath " << ShapeWorksRunPath->text() <<")"<< endl;
			stream << "Set(ShapeWorksGroomPath " << ShapeWorksGroomPath->text() <<")"<< endl;
			stream << "Set(ImageMathPath " << ImageMathPath->text() <<")"<< endl;
			stream << "Set(SegPostprocessPath " << SegPostprocessPath->text() <<")"<< endl;
			stream << "Set(ShapeWorksPythonScriptPath " << ShapeWorksPythonScriptPath->text() <<")"<< endl;
			stream << "Set(PythonPath " << PythonPath->text() <<")"<< endl;
			stream << "Set(BinaryToDistanceMapPath " << BinaryToDistanceMapPath->text() <<")"<< endl;
			if(concatToColumnsPythonScriptPath->text().isEmpty()) stream << "Set(concatToColumnsPythonScriptPath nopath)"<< endl;
			else stream << "Set(concatToColumnsPythonScriptPath " << concatToColumnsPythonScriptPath->text() <<")"<< endl;
			if(vtkPointAttributesPythonScriptPath->text().isEmpty()) stream << "Set(vtkPointAttributesPythonScriptPath nopath)"<< endl;			
			else stream << "Set(vtkPointAttributesPythonScriptPath " << vtkPointAttributesPythonScriptPath->text() <<")"<< endl;
			if(RScriptRodentThicknessPath->text().isEmpty()) stream << "Set(RScriptRodentThicknessPath nopath)"<< endl;			
			else stream << "Set(RScriptRodentThicknessPath " << RScriptRodentThicknessPath->text() <<")"<< endl;
			stream << "Set(PythonPath " << PythonPath->text() <<")"<< endl;
			if(RScriptPath->text().isEmpty()) stream << "Set(RScriptPath nopath)"<< endl;
			else stream << "Set(RScriptPath " << RScriptPath->text() <<")"<< endl;
			std::cout<<"DONE"<<std::endl; // command line display
		}
		else 
		{
			std::cout<<"FAILED"<<std::endl; // command line display
			qDebug( "Could not create config file");
		}
	}
}
void GuiCSV::ConfigDefaultSlot() /*SLOT*/
{	
		ConfigDefault();
		if( !m_notFound.empty() )
		{
			if(!m_noGUI)
			{
				std::string text = "The following programs have not been found.\nPlease enter the path manually or open a configuration file:\n" + m_notFound;
				QMessageBox::warning(this, "Program missing", QString(text.c_str()) );
			}
			else std::cout<<"| The following programs have not been found. Please give a configuration file or modify it or enter the path manually in the GUI:\n"<< m_notFound <<std::endl;
		}
}
void GuiCSV::ConfigDefault() 
{
std::cout<<"| Searching the softwares..."; // command line display

	std::string program;
	std ::string notFound;

	program = itksys::SystemTools::FindProgram("measureThicknessFilter",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(measureThicknessFilterPath->text().isEmpty()) notFound = notFound + "> measureThicknessFilter\n"; }
	else measureThicknessFilterPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("GenParaMeshCLP",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(GenParaMeshCLPPath->text().isEmpty()) notFound = notFound + "> GenParaMeshCLP\n"; }
	else GenParaMeshCLPPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("ParaToSPHARMMeshCLP",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(ParaToSPHARMMeshCLPPath->text().isEmpty()) notFound = notFound + "> ParaToSPHARMMeshCLP\n"; }
	else ParaToSPHARMMeshCLPPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("MeshPointsIntensitySampling",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(MeshPointIntensitySamplingPath->text().isEmpty()) notFound = notFound + "> MeshPointsIntensitySampling\n"; }
	else MeshPointIntensitySamplingPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("ShapeWorksRun",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(ShapeWorksRunPath->text().isEmpty()) notFound = notFound + "> ShapeWorksRun\n"; }
	else ShapeWorksRunPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("ShapeWorksGroom",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(ShapeWorksGroomPath->text().isEmpty()) notFound = notFound + "> ShapeWorksGroom\n"; }
	else ShapeWorksGroomPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("ImageMath",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(ImageMathPath->text().isEmpty()) notFound = notFound + "> ImageMath\n"; }
	else ImageMathPath->setText(QString(program.c_str()));
	
	program = itksys::SystemTools::FindProgram("SegPostProcessCLP",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(SegPostprocessPath->text().isEmpty()) notFound = notFound + "> SegPostProcessCLP\n"; }
	else SegPostprocessPath->setText(QString(program.c_str()));
	
	program = itksys::SystemTools::FindFile("shapeworks.py",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(ShapeWorksPythonScriptPath->text().isEmpty()) notFound = notFound + ">shapeworks.py\n"; }
	else ShapeWorksPythonScriptPath->setText(QString(program.c_str()));

	program = itksys::SystemTools::FindProgram("python",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(PythonPath->text().isEmpty()) notFound = notFound + ">python\n"; }
	else PythonPath->setText(QString(program.c_str()));
	
	program = itksys::SystemTools::FindProgram("BinaryToDistanceMap",m_FindProgramRTExecDirVec);
	if(program.empty()) { if(BinaryToDistanceMapPath->text().isEmpty()) notFound = notFound + ">BinaryToDistanceMap\n"; }
	else BinaryToDistanceMapPath->setText(QString(program.c_str()));
	


	if(m_statistic==1)
	{
		program = itksys::SystemTools::FindFile("concatToColumns.py",m_FindProgramRTExecDirVec);
		if(program.empty()) { if(concatToColumnsPythonScriptPath->text().isEmpty()) notFound = notFound + "> concatToColumns.py\n"; }
		else concatToColumnsPythonScriptPath->setText(QString(program.c_str()));

		program = itksys::SystemTools::FindFile("vtkPointAttributes.py",m_FindProgramRTExecDirVec);
		if(program.empty()) { if(vtkPointAttributesPythonScriptPath->text().isEmpty()) notFound = notFound + "> vtkPointAttributes.py\n"; }
		else vtkPointAttributesPythonScriptPath->setText(QString(program.c_str()));

		program = itksys::SystemTools::FindFile("rodentThicknessStats.r",m_FindProgramRTExecDirVec);
		if(program.empty()) { if(RScriptRodentThicknessPath->text().isEmpty()) notFound = notFound + "> rodentThicknessStats.r\n"; }
		else RScriptRodentThicknessPath->setText(QString(program.c_str()));
		
		program = itksys::SystemTools::FindProgram("Rscript",m_FindProgramRTExecDirVec);
		if(program.empty()) { if(RScriptPath->text().isEmpty()) notFound = notFound + "> Rscript\n"; }
		else RScriptPath->setText(QString(program.c_str()));
	}
	std::cout<<"DONE"<<std::endl; // command line display
	
	m_notFound=notFound;
	
}

void GuiCSV::BrowseSoft(int soft)  /*SLOT*/ //softwares: 
{
	QString SoftBrowse;
	
	if(soft ==11 ) SoftBrowse = QFileDialog::getExistingDirectory(this);
	else if(soft > 7 && soft < 11) SoftBrowse = QFileDialog::getOpenFileName(this, "Open Python Script", QString(), "Python Script (*.py)");
	else if(soft == 12) SoftBrowse = QFileDialog::getOpenFileName(this, "Open R Script", QString(), "R Script (*.r)");
	else SoftBrowse = QFileDialog::getOpenFileName(this, "Open Software", QString(), "Executable Files (*)");
	
	
	if(!SoftBrowse.isEmpty())
	{
		switch (soft)
		{
		case 1: measureThicknessFilterPath->setText(SoftBrowse);
			break;
		case 2: GenParaMeshCLPPath->setText(SoftBrowse);
			break;
		case 3: ParaToSPHARMMeshCLPPath->setText(SoftBrowse);
			break;
		case 4: MeshPointIntensitySamplingPath->setText(SoftBrowse);
			break;
		case 5: ShapeWorksRunPath->setText(SoftBrowse);
			break;
		case 6: ImageMathPath->setText(SoftBrowse);
			break;
		case 7: SegPostprocessPath->setText(SoftBrowse);
			break;
		case 8: ShapeWorksPythonScriptPath->setText(SoftBrowse);
			break;
		case 9: concatToColumnsPythonScriptPath->setText(SoftBrowse);
			break;
		case 10: vtkPointAttributesPythonScriptPath->setText(SoftBrowse);
			break;
		case 11: PathFilesBmsPath->setText(SoftBrowse);
			break;
		case 12: RScriptRodentThicknessPath->setText(SoftBrowse);
			break;
		case 13: PythonPath->setText(SoftBrowse);
			break;
		case 14: RScriptPath->setText(SoftBrowse);
			break;
		case 15: ShapeWorksGroomPath->setText(SoftBrowse);
			break;
		case 16: BinaryToDistanceMapPath->setText(SoftBrowse);
			break;
		}
	}
}

void GuiCSV::ResetSoft(int softindex) /*SLOT*/ //softwares: 
{
	std::string soft;

	switch (softindex)
	{
	case 1: soft="measureThicknessFilter";
		break;
	case 2: soft="GenParaMeshCLP";
		break;
	case 3: soft="ParaToSPHARMMeshCLP";
		break;
	case 4: soft="MeshPointsIntensitySampling";
		break;
	case 5:	soft="ShapeWorksRun";
		break;
	case 6: soft="ImageMath";
		break;
	case 7: soft="SegPostProcessCLP";
		break;
	case 8: soft="python";
		break;
	case 9: soft="Rscript";
		break;
	case 10:soft="ShapeWorksGroom";
		break;
	case 11:soft="BinaryToDistanceMap";
		break;
	case 12:soft="shapeworks.py";
		break;
	case 13:soft="concatToColumns.py";
		break;
	case 14:soft="vtkPointAttributes.py";
		break;
	case 15:soft="rodentthickness.r";
		break;
	}

	std::cout<<"| Searching the software \'"<< soft <<"\'..."; // command line display
	std::string program;
	if(softindex<12) program = itksys::SystemTools::FindProgram(soft.c_str(),m_FindProgramRTExecDirVec);
	else program = itksys::SystemTools::FindFile(soft.c_str(),m_FindProgramRTExecDirVec);
	
	
	if(program.empty()) 
	{
		std::string text = "The program or file  \'" + soft + "\' is missing.\nPlease enter the path manually.\n";
		QMessageBox::warning(this, "Program missing", QString(text.c_str()) );
	}
	else 
	{
		if(softindex==1) measureThicknessFilterPath->setText(QString(program.c_str()));
		else if(softindex==2) GenParaMeshCLPPath->setText(QString(program.c_str()));
		else if(softindex==3) ParaToSPHARMMeshCLPPath->setText(QString(program.c_str()));
		else if(softindex==4) MeshPointIntensitySamplingPath->setText(QString(program.c_str()));
		else if(softindex==5) ShapeWorksRunPath->setText(QString(program.c_str()));
		else if(softindex==6) ImageMathPath->setText(QString(program.c_str()));
		else if(softindex==7) SegPostprocessPath->setText(QString(program.c_str()));
		else if(softindex==8) PythonPath->setText(QString(program.c_str()));
		else if(softindex==9) RScriptPath->setText(QString(program.c_str()));
		else if(softindex==10) ShapeWorksGroomPath->setText(QString(program.c_str()));
		else if(softindex==11) BinaryToDistanceMapPath->setText(QString(program.c_str()));
		else if(softindex==12) ShapeWorksPythonScriptPath->setText(QString(program.c_str()));
		else if(softindex==13) concatToColumnsPythonScriptPath->setText(QString(program.c_str()));
		else if(softindex==14) vtkPointAttributesPythonScriptPath->setText(QString(program.c_str()));
		else if(softindex==15) RScriptRodentThicknessPath->setText(QString(program.c_str()));
		
	}

	std::cout<<"DONE"<<std::endl; // command line display
	
}
  /////////////////////////////////////////
 //               OUTPUT                //
/////////////////////////////////////////

void GuiCSV::OpenOutputBrowseWindow() /*SLOT*/
{
	QString OutputBrowse=QFileDialog::getExistingDirectory(this);
	if(!OutputBrowse.isEmpty())
	{
		OutputFolderPath->setText(OutputBrowse);
	}
	
}
  /////////////////////////////////////////
 // READ ME //
/////////////////////////////////////////

void GuiCSV::ReadMe() /*SLOT*/ /////to UPDATE
{

QDialog *dlg = new QDialog(this);
dlg->setWindowTitle ("Read Me");

std::string info = "RodentThickness\n===============\n\nA tool to measure cortical thickness of rodent brain\n\nThese Softwares need to be installed before executing the tool :\n- ImageMath\n- measureThicknessFilter\n- GenParaMeshCLP\n- ParaToSPHARMMeshCLP\n- ShapeWorksRun\n- ShapeWorksGroom\n- SegPostProcessCLP\n- BinaryToDistanceMap\n- MeshPointsIntensitysampling\n\nThe program use : \n- python2.6 or python2.7 with vtk installed\n- Rscript\n\n For any question, suggestion or remark, please contact mjacquem@unc.edu";
QLabel *InfoLabel = new QLabel (info.c_str(), this);
QVBoxLayout *VLayout = new QVBoxLayout();
VLayout->addWidget(InfoLabel);

dlg->setLayout(VLayout);

dlg->setVisible(!dlg->isVisible()); // display the window
}


  /////////////////////////////////////////
 //         WIDGET CHANGE SLOT          //
/////////////////////////////////////////

void GuiCSV::WidgetHasChangedParam() /*SLOT*/ //called when any widget is changed
{
	if(StatisticBox->isChecked()) 
	{
		m_statistic=1;
		RScriptButton->setEnabled(true);
		RScriptPathButton->setEnabled(true);
		concatToColumnsPythonScriptButton->setEnabled(true);
		vtkPointAttributesPythonScriptButton->setEnabled(true);
		RScriptResetButton->setEnabled(true);
		RScriptPathResetButton->setEnabled(true);
		concatToColumnsPythonScriptResetButton->setEnabled(true);
		vtkPointAttributesPythonScriptResetButton->setEnabled(true);
	}
	else 
	{
		m_statistic=0;
		RScriptButton->setEnabled(false);
		RScriptPathButton->setEnabled(false);
		concatToColumnsPythonScriptButton->setEnabled(false);
		vtkPointAttributesPythonScriptButton->setEnabled(false);
		RScriptResetButton->setEnabled(false);
		RScriptPathResetButton->setEnabled(false);
		concatToColumnsPythonScriptResetButton->setEnabled(false);
		vtkPointAttributesPythonScriptResetButton->setEnabled(false);
	}
}
  /////////////////////////////////////////
 //           CHECK IMAGE OK            //
/////////////////////////////////////////

int GuiCSV::checkImage(std::string Image) // returns 1 if not an image, 2 if not a dti, otherwise 0
{
	typedef itk::Image < double , 4 > ImageType; //itk type for image
	typedef itk::ImageFileReader <ImageType> ReaderType; //itk reader class to open an image
	ReaderType::Pointer reader=ReaderType::New();
	ImageType::RegionType region;

	reader->SetFileName( Image );

	try{
		reader->UpdateOutputInformation();
	}
	catch(itk::ExceptionObject & err)
	{
		return 1; // file is not an image
	}
	return 0;
}
int GuiCSV::checkdataset()
{	
	int m=0,n=0;
	for(int i=0; i < tableWidget->rowCount();i++) 
	{
		QTableWidgetItem* item = tableWidget->item(i,0);
		QTableWidgetItem* item1 = tableWidget->item(i,1);
 		QTableWidgetItem* item2 = tableWidget->item(i,2);
		  if (!item || item->text().isEmpty())
		    {
		    	m=1;
		    }
		 if (!item1 || item1->text().isEmpty())
		    {
		    	m=1;
		    }
 		
		if((StatisticBox->isChecked() && item2->text().isEmpty()) || (StatisticBox->isChecked() && !item2))
		{
			n=1;
		}
		else
		{
			if (!item2 || item2->text().isEmpty())
		    	{
		    		QString item="A";
				QTableWidgetItem * Itemcell=new QTableWidgetItem( tr("%1").arg( item ) );
				tableWidget->setItem(i,2,Itemcell);
		    	}
		}

		
	}
	int mn=m+n;
	return mn;	
}
  /////////////////////////////////////////
 //           MAIN FUNCTIONS            //
/////////////////////////////////////////

int GuiCSV::Compute() /*SLOT*/
{
	if( m_ErrorDetectedInConstructor && m_noGUI )
	{
		ExitProgram();
		return -1;
	}
	else
	{
		if(tableWidget->rowCount()<1)
		{
			if(!m_noGUI) QMessageBox::critical(this, "No Cases", "Please give at least one case");
			else std::cout<<"| No Cases: Please give at least one case"<<std::endl;
		}
		else if(tableWidget->rowCount()<6 && m_statistic==1) 
		{
			if(!m_noGUI) QMessageBox::critical(this, "No Cases", "Please give more six cases for statistical");
			else std::cout<<"| No Cases: Please give more six cases for statistical"<<std::endl;
		}
		else // OK Case
		{
			if(OutputFolderPath->text().isEmpty())
			{
				QMessageBox::critical(this, "No Output Folder", "Please give an output folder");
			}
			else // OK Output
			{
				if(LaunchScriptWriter()==-1) 
				{
					if(m_noGUI) ExitProgram(); // no possibility to change options because no GUI so QUIT
					return -1;
				}	
				return LaunchScriptRunner();	
			} // else of if(OutputFolderLineEdit->text().isEmpty())

		} // else of if[Case]
	
	if(m_noGUI) ExitProgram(); 
	return -1;
	}
			
}
int GuiCSV::LaunchScriptWriter()
{	
	if(checkdataset()==1)
	{
	 	QMessageBox::critical(this, "Item Empty", "Please enter all the item for subject and Image");
		return -1;
	}		
	if(checkdataset()==2)
	{
	 	QMessageBox::critical(this, "Item Empty", "Please enter all the item for subject and Image and Group");
		return -1;
	}		

	int ExitCode=0;
	std::string program;

	for(int i=0; i <tableWidget->rowCount() ;i++) 
	{	
		std::string CurrentCase = tableWidget->item(i,1)->text().toStdString();//.substr( tableWidget->item(i,2)->text().split(":").at(0).size()+2 );
		
		if( ! itksys::SystemTools::GetPermissions(CurrentCase.c_str(), ITKmode_F_OK)) // Test if the case files exist => unistd::access() returns 0 if F(file)_OK
		{
			std::string text = "This file does not exist :\n" + CurrentCase;
			QMessageBox::critical(this, "Case does not exist", QString(text.c_str()) );
			return -1;
		}
		int checkIm = checkImage(CurrentCase); // returns 1 if not an image, 2 if not a dti, otherwise 
		
		if( checkIm == 1 ) // returns 1 if not an image, 2 if not a dti, otherwise 0
		{
			std::string text = "This file is not an image :\n" + CurrentCase;
			QMessageBox::critical(this, "No image", QString(text.c_str()) );
			return -1;
		}
	}
	
/* Output */
	m_OutputPath=OutputFolderPath->text();// ! itksys::SystemTools::GetPermissions(m_OutputPath.toStdString().c_str(), ITKmode_F_OK)
	if( ! itksys::SystemTools::GetPermissions(m_OutputPath.toStdString().c_str(), ITKmode_F_OK) ) // Test if the folder exists => unistd::access() returns 0 if F(file)_OK
	{
		QMessageBox::critical(this, "No Output Folder", "The output folder does not exist");
		return -1;
	}
	if( ! itksys::SystemTools::GetPermissions(m_OutputPath.toStdString().c_str(), ITKmode_W_OK) ) // Test if the program can write in the output folder => unistd::access() returns 0 if W(write)_OK
	{
		QMessageBox::critical(this, "Output Folder Unwritable", "Please give an output folder authorized in reading");
		return -1;
	}

	if(! itksys::SystemTools::GetPermissions((m_OutputPath.toStdString() + "/RodentThickness").c_str(), ITKmode_F_OK)) // Test if the main folder does not exists => unistd::access() returns 0 if F(file)_OK
	{
		std::cout<<"| Creating Main directory..."<<std::endl; // command line display
		QProcess * mkdirMainProcess = new QProcess;
		program = "mkdir " + m_OutputPath.toStdString() + "/RodentThickness"; //  Creates the directory
		std::cout<<"| $ " << program << std::endl;
		ExitCode = mkdirMainProcess->execute( program.c_str() );
	}

	if( ! itksys::SystemTools::GetPermissions((m_OutputPath.toStdString() + "/RodentThickness/Output").c_str(), ITKmode_F_OK) ) // Test if the script folder does not exists => unistd::access() returns 0 if F(file)_OK
	{
		std::cout<<"| Creating output directory..."<<std::endl; // command line display
		QProcess * mkdirScriptProcess = new QProcess;
		program = "mkdir " + m_OutputPath.toStdString() + "/RodentThickness/Output";
		std::cout<<"| $ " << program << std::endl;
		ExitCode = mkdirScriptProcess->execute( program.c_str() );
	}
	if(m_statistic==1)
	{
		if( ! itksys::SystemTools::GetPermissions((m_OutputPath.toStdString() + "/RodentThickness/Statistical").c_str(), ITKmode_F_OK) ) // Test if the script folder does not exists => unistd::access() returns 0 if F(file)_OK
		{
			std::cout<<"| Creating Statistical directory..."<<std::endl; // command line display
			QProcess * mkdirScriptProcess = new QProcess;
			program = "mkdir " + m_OutputPath.toStdString() + "/RodentThickness/Statistical";
			std::cout<<"| $ " << program << std::endl;
			ExitCode = mkdirScriptProcess->execute( program.c_str() );
		}
	}
	if( ! itksys::SystemTools::GetPermissions((m_OutputPath.toStdString() + "/RodentThickness/Script").c_str(), ITKmode_F_OK) ) // Test if the script folder does not exists => unistd::access() returns 0 if F(file)_OK
	{
		std::cout<<"| Creating Script directory..."<<std::endl; // command line display
		QProcess * mkdirScriptProcess = new QProcess;
		program = "mkdir " + m_OutputPath.toStdString() + "/RodentThickness/Script";
		std::cout<<"| $ " << program << std::endl;
		ExitCode = mkdirScriptProcess->execute( program.c_str() );
	}
	m_FilesBmsPath=PathFilesBmsPath->text();
	if(! itksys::SystemTools::GetPermissions(m_FilesBmsPath.toStdString().c_str(), ITKmode_F_OK) ) // Test if the folder exists => unistd::access() returns 0 if F(file)_OK
	{
		QMessageBox::critical(this, "No Bms Folder", "The bms folder does not exist");
		return -1;
	}
/* ComboBox*/
	
	if(ExitCode!=0) return -1;
/* Software paths */
/* Checking if all the programs have been given */
	if(measureThicknessFilterPath->text().isEmpty() || GenParaMeshCLPPath->text().isEmpty() || ParaToSPHARMMeshCLPPath->text().isEmpty() || MeshPointIntensitySamplingPath->text().isEmpty() || ShapeWorksRunPath->text().isEmpty() || ShapeWorksGroomPath->text().isEmpty() || ImageMathPath->text().isEmpty() || SegPostprocessPath->text().isEmpty() || ShapeWorksPythonScriptPath->text().isEmpty() || concatToColumnsPythonScriptPath->text().isEmpty() || vtkPointAttributesPythonScriptPath->text().isEmpty()|| PythonPath->text().isEmpty()|| RScriptPath->text().isEmpty()|| RScriptRodentThicknessPath->text().isEmpty()|| BinaryToDistanceMapPath->text().isEmpty()) // if any path is missing => check in the config file and in the PATH
	{

		std::string programPath;
		std::string notFound;

		if(measureThicknessFilterPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("measureThicknessFilter",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> measureThicknessFilter\n";
			else measureThicknessFilterPath->setText(QString(programPath.c_str()));
		}
		if(GenParaMeshCLPPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("GenParaMeshCLP",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> GenParaMeshCLP\n";
			else GenParaMeshCLPPath->setText(QString(programPath.c_str()));
		}
		if(ParaToSPHARMMeshCLPPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("ParaToSPHARMMeshCLP",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> ParaToSPHARMMeshCLP\n";
			else ParaToSPHARMMeshCLPPath->setText(QString(programPath.c_str()));
		}
		if(MeshPointIntensitySamplingPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("MeshPointsIntensitySampling",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> MeshPointsIntensitySampling\n";
			else MeshPointIntensitySamplingPath->setText(QString(programPath.c_str()));
		}
		if(ShapeWorksRunPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("ShapeWorksRun",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> ShapeWorksRun\n";
			else ShapeWorksRunPath->setText(QString(programPath.c_str()));
		}
		if(ShapeWorksGroomPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("ShapeWorksGroom",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> ShapeWorksGroom\n";
			else ShapeWorksGroomPath->setText(QString(programPath.c_str()));
		}
		if(ImageMathPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("ImageMath",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> ImageMath\n";
			else ImageMathPath->setText(QString(programPath.c_str()));
		}
		if(SegPostprocessPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("SegPostprocessCLP",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> SegPostprocessCLP\n";
			else SegPostprocessPath->setText(QString(programPath.c_str()));
		}
		if(ShapeWorksPythonScriptPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindFile("shapeworks.py",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> shapeworks.py\n";
			else ShapeWorksPythonScriptPath->setText(QString(programPath.c_str()));
		}
		if(PythonPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("python",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> python\n";
			else PythonPath->setText(QString(programPath.c_str()));
		}
		if(BinaryToDistanceMapPath->text().isEmpty())
		{
			programPath = itksys::SystemTools::FindProgram("BinaryToDistanceMap",m_FindProgramRTExecDirVec);
			if(programPath.empty()) notFound = notFound + "> BinaryToDistanceMap\n";
			else BinaryToDistanceMapPath->setText(QString(programPath.c_str()));
		}
		if(m_statistic==1)
		{	if(concatToColumnsPythonScriptPath->text().isEmpty())
			{
				programPath = itksys::SystemTools::FindFile("concatToColumns.py",m_FindProgramRTExecDirVec);
				if(programPath.empty()) notFound = notFound + "> concatToColumns.py\n";
				else concatToColumnsPythonScriptPath->setText(QString(programPath.c_str()));
			}
			if(vtkPointAttributesPythonScriptPath->text().isEmpty())
			{
				programPath = itksys::SystemTools::FindFile("vtkPointAttributes.py",m_FindProgramRTExecDirVec);
				if(programPath.empty()) notFound = notFound + "> vtkPointAttributes.py\n";
				else vtkPointAttributesPythonScriptPath->setText(QString(programPath.c_str()));
			}
			if(RScriptRodentThicknessPath->text().isEmpty())
			{
				programPath = itksys::SystemTools::FindFile("rodentThicknessStats.r",m_FindProgramRTExecDirVec);
				if(programPath.empty()) notFound = notFound + "> rodentThicknessStats.r\n";
				else RScriptRodentThicknessPath->setText(QString(programPath.c_str()));
			}
			if(RScriptPath->text().isEmpty())
			{
				programPath = itksys::SystemTools::FindProgram("Rscript",m_FindProgramRTExecDirVec);
				if(programPath.empty()) notFound = notFound + "> Rscript\n";
				else RScriptPath->setText(QString(programPath.c_str()));
			}
		}
		if( !notFound.empty() )
		{
			std::string text = "The following programs or files are missing.\nPlease enter the path manually:\n" + notFound;
			QMessageBox::critical(this, "Program missing", QString(text.c_str()) );
			return -1;
		}
	}

/* Checking if the given files are executable */
	if(! itksys::SystemTools::GetPermissions(measureThicknessFilterPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + measureThicknessFilterPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(GenParaMeshCLPPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + GenParaMeshCLPPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(ParaToSPHARMMeshCLPPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + ParaToSPHARMMeshCLPPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(MeshPointIntensitySamplingPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + MeshPointIntensitySamplingPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(ShapeWorksRunPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + ShapeWorksRunPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(ShapeWorksGroomPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + ShapeWorksGroomPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(ImageMathPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + ImageMathPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(SegPostprocessPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + SegPostprocessPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(ShapeWorksPythonScriptPath->text().toStdString().c_str(), ITKmode_F_OK) )
	{
			std::string text = "The file \'" + ShapeWorksPythonScriptPath->text().toStdString() + "\' is not a file";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(PythonPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + PythonPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(! itksys::SystemTools::GetPermissions(BinaryToDistanceMapPath->text().toStdString().c_str(), ITKmode_X_OK) )
	{
			std::string text = "The file \'" + BinaryToDistanceMapPath->text().toStdString() + "\' is not executable";
			QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
			return -1;
	}
	if(m_statistic==1)
	{
		if(! itksys::SystemTools::GetPermissions(concatToColumnsPythonScriptPath->text().toStdString().c_str(), ITKmode_F_OK) )
		{
				std::string text = "The file \'" + concatToColumnsPythonScriptPath->text().toStdString() + "\' is not a file";
				QMessageBox::critical(this, "No File", QString(text.c_str()) );
				return -1;
		}
		if(! itksys::SystemTools::GetPermissions(vtkPointAttributesPythonScriptPath->text().toStdString().c_str(), ITKmode_F_OK) )
		{
				std::string text = "The file \'" + vtkPointAttributesPythonScriptPath->text().toStdString() + "\' is not a file";
				QMessageBox::critical(this, "No File", QString(text.c_str()) );
				return -1;
		}
		if(! itksys::SystemTools::GetPermissions(RScriptRodentThicknessPath->text().toStdString().c_str(), ITKmode_F_OK) )
		{
				std::string text = "The file \'" + RScriptRodentThicknessPath->text().toStdString() + "\' is not a file";
				QMessageBox::critical(this, "No File", QString(text.c_str()) );
				return -1;
		}
		if(! itksys::SystemTools::GetPermissions(RScriptPath->text().toStdString().c_str(), ITKmode_X_OK) )
		{
				std::string text = "The file \'" + ShapeWorksPythonScriptPath->text().toStdString() + "\' is not executable";
				QMessageBox::critical(this, "Non executable File", QString(text.c_str()) );
				return -1;
		}
	}

	m_outconfig=m_OutputPath + QString("/RodentThickness/RodentThicknessconfigfile.bms");
	m_outdataset=m_OutputPath + QString("/RodentThickness/dataset.csv");
	m_StatisticaldatasetPath=m_OutputPath + QString("/RodentThickness/outputdataset.csv");
	SaveConfig(m_outconfig);
	SaveCSVDatasetBrowse(m_outdataset);


return 0;
}
int GuiCSV :: LaunchScriptRunner()
{
	ComputeButton->setEnabled(false);
	m_ScriptRunning=true;
	if(m_statistic==0)
	{
		if(m_scriptrunner->RunBatchmakeScript(m_outdataset.toStdString(),m_outconfig.toStdString(),m_FilesBmsPath.toStdString(),m_OutputPath.toStdString())==0) 
		{	
			RunningCompleted();	
			return 0;
		}
		else 
		{
			RunningFailed();
			return -1 ;
		}
	}
	else
	{
		if(m_scriptrunner->RunBatchmakeScript(m_outdataset.toStdString(),m_outconfig.toStdString(),m_FilesBmsPath.toStdString(), m_OutputPath.toStdString())==0) 
		{	
		       if(m_statisticalpartrunner->RunBatchmakeStatisticalScript(m_StatisticaldatasetPath.toStdString(),m_outconfig.toStdString(),m_FilesBmsPath.toStdString(), m_OutputPath.toStdString())==0)
			{	RunningCompleted();	
				return 0;
			}
			else
			{
				RunningFailed();
				return -1 ;
			}
		}
		else 
		{
			RunningFailed();
			return -1 ;
		}

	}
}
void GuiCSV::RunningCompleted()
{

ComputeButton->setEnabled(true);
m_ScriptRunning=false;
//ScriptRunningDisplayQLabel->setText("Running Completed ");

if(!m_noGUI) QMessageBox::information(this, "Running Completed", "Running Completed !");
std::cout<<"| Running Completed !"<<std::endl; // command line display
}
void GuiCSV::RunningFailed()
{

ComputeButton->setEnabled(true);
m_ScriptRunning=false;
//ScriptRunningDisplayQLabel->setText("Running Failed ");

if(!m_noGUI) QMessageBox::information(this, "Running Failed", "Running Failed...");
std::cout<<"| Running Failed..."<<std::endl; // command line display
}





