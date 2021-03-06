/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mvdQtWidgetView.h"

/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// System includes (sorted by alphabetic order)
#include <functional>

//
// OTB includes (sorted by alphabetic order)
#include "otbWrapperApplicationHtmlDocGenerator.h"
#include "otbWrapperOutputFilenameParameter.h"
#include "otbWrapperOutputVectorDataParameter.h"
#include "otbWrapperQtWidgetSimpleProgressReport.h"

//
// Monteverdi includes (sorted by alphabetic order)
#include "mvdAbstractLayerModel.h"
#include "mvdFilenameInterface.h"
#include "mvdI18nCoreApplication.h"
#include "mvdStackedLayerModel.h"
#include "mvdQtWidgetParameterInitializers.h"

namespace mvd
{
namespace Wrapper
{
/*
  TRANSLATOR mvd::Wrapper::QtWidgetView

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* INTERNAL TYPES                                                            */

class KeyLayerAccumulator :
    public std::unary_function< StackedLayerModel::ConstIterator::value_type,
				void >
{
public:
  typedef
  std::unary_function< StackedLayerModel::ConstIterator::value_type, void >
    SuperType;

  typedef std::list< SuperType::argument_type > KeyLayerPairList;

  KeyLayerAccumulator( const std::string & filename,
		       KeyLayerPairList & klp ) :
    m_KeyLayerPairs( klp ),
    m_Filename( FromStdString( filename ) ),
    m_Count( 0 )
  {
  }

  void
  operator () ( const SuperType::argument_type & pair )
  {
    const FilenameInterface * interface =
      dynamic_cast< const FilenameInterface * >( pair.second );

    if( interface!=NULL &&
	m_Filename.compare( interface->GetFilename() )==0 )
      {
      qDebug() << m_Filename << "==" << interface->GetFilename();

      m_KeyLayerPairs.push_back( pair );

      ++ m_Count;
      }
  }

  std::size_t
  GetCount() const
  {
    return m_Count;
  }


public:
  KeyLayerPairList & m_KeyLayerPairs;


private:
  QString m_Filename;
  std::size_t m_Count;
};

/*****************************************************************************/
/* CONSTANTS                                                                 */

char const * const
QtWidgetView
::OBJECT_NAME = "mvd::Wrapper::QtWidgetView";

/*****************************************************************************/
/* STATIC IMPLEMENTATION SECTION                                             */


/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */
/*****************************************************************************/
QtWidgetView
::QtWidgetView( const otb::Wrapper::Application::Pointer & otbApp,
		QWidget* p,
		Qt::WindowFlags flags ) :
  otb::Wrapper::QtWidgetView( otbApp, p, flags )
{
  setObjectName( QtWidgetView::OBJECT_NAME );

  m_IconPathDone = std::string("<img src=\":/icons/done\" width=\"16\" height=\"16\" />");
  m_IconPathFailed = std::string("<img src=\":/icons/failed\" width=\"16\" height=\"16\" />");

  //
  // need to be connected to the end of a process
  QObject::connect(
    GetModel(),
    SIGNAL( SetProgressReportDone( int ) ),
    // to:
    this,
    SLOT ( OnApplicationExecutionDone( int ) )
    );
}

/*******************************************************************************/
QtWidgetView
::~QtWidgetView()
{
}

/*******************************************************************************/
QWidget*
QtWidgetView
::CreateInputWidgets()
{
  QWidget * widget = otb::Wrapper::QtWidgetView::CreateInputWidgets();

  otb::Wrapper::QtWidgetParameterBase *paramWidget =
    widget->findChild<otb::Wrapper::QtWidgetParameterBase*>();

  SetupParameterWidgets(paramWidget);

  return widget;
}

/*******************************************************************************/
void
QtWidgetView
::SetupParameterWidgets( QWidget * widget )
{
  assert( widget!=NULL );

  SetupWidget( widget, InputFilenameInitializer() );
  //SetupWidget( widget, InputFilenameListInitializer() );
  SetupWidget( widget, InputImageInitializer() );
  //SetupWidget( widget, InputImageListInitializer() );
  SetupWidget( widget, ComplexInputImageInitializer() );
  SetupWidget( widget, InputProcessXMLInitializer() );
  SetupWidget( widget, InputVectorDataInitializer() );
  //SetupWidget( widget, InputVectorDataListInitializer() );
  SetupWidget( widget, ParameterListInitializer() );
#if defined( OTB_DEBUG )
  SetupWidget( widget, ToolTipInitializer() );
#endif

  SetupWidget( widget, OutputFilenameInitializer() );
  SetupWidget( widget, OutputProcessXMLInitializer() );

  SetupWidget(
    widget,
    OutputImageInitializer( GetModel()->GetApplication()->GetName() )
  );
  SetupWidget(
    widget,
    ComplexOutputImageInitializer( GetModel()->GetApplication()->GetName() )
  );

  SetupWidget( widget, OutputVectorDataInitializer() );
}

/*******************************************************************************/
void
QtWidgetView
::SetupFileSelectionWidget( QWidget * widget )
{
  assert( widget!=NULL );
  assert(
    qobject_cast< FileSelectionInitializer::argument_type >( widget )!=NULL
  );

  FileSelectionInitializer initialize;

  initialize( qobject_cast< FileSelectionInitializer::argument_type >( widget ) );
}

/*******************************************************************************/
/* SLOTS                                                                       */
/*******************************************************************************/
void
QtWidgetView
::OnExecButtonClicked()
{
  if ( !IsRunning() )
    {
    assert( GetModel()!=NULL );
    assert( GetModel()->GetApplication()!=NULL );
  
  
    assert( I18nCoreApplication::Instance()!=NULL );
  
    //
    // Get layer-stack, if any.
    StackedLayerModel * layerStack =
      I18nCoreApplication::Instance()->GetModel< StackedLayerModel >();
  
    otb::Wrapper::Application::Pointer otbApp( GetModel()->GetApplication() );
  
    //
    // Check output parameters of OTB-application.
    StringVector paramKeys( otbApp->GetParametersKeys() );
    QStringList filenames1;
  
    KeyLayerAccumulator::KeyLayerPairList layers;
    QStringList filenames2;
  
    for( StringVector::const_iterator it( paramKeys.begin() );
         it!=paramKeys.end();
         ++it )
      {
      if( otbApp->IsParameterEnabled( *it, true ) &&
          otbApp->HasValue( *it ) )
        {
        otb::Wrapper::Parameter::Pointer param( otbApp->GetParameterByKey( *it ) );
        assert( !param.IsNull() );
  
        // qDebug()
        // 	<< it->c_str() << ": type" << otbApp->GetParameterType( *it );
  
        // const char* filename = NULL;
        std::string filename;
  
        switch( otbApp->GetParameterType( *it ) )
    {
    case otb::Wrapper::ParameterType_OutputFilename:
      filename =
        otb::DynamicCast< otb::Wrapper::OutputFilenameParameter >( param )
        ->GetValue();
      break;
    //
    // FILENAME.
    //
    // IMAGE.
    case otb::Wrapper::ParameterType_OutputImage:
      filename =
        otb::DynamicCast< otb::Wrapper::OutputImageParameter >( param )
        ->GetFileName();
      break;
    //
    // VECTOR-DATA.
    case otb::Wrapper::ParameterType_OutputVectorData:
      filename =
        otb::DynamicCast< otb::Wrapper::OutputVectorDataParameter >( param )
        ->GetFileName();
      break;
    //
    // COMPLEX IMAGE.
    case otb::Wrapper::ParameterType_ComplexOutputImage:
      filename =
        otb::DynamicCast< otb::Wrapper::ComplexOutputImageParameter >( param )
        ->GetFileName();
      break;
    //
    // NONE.
    default:
      break;
    }
  
        if( QFileInfo( filename.c_str() ).exists() )
    filenames1.push_back( filename.c_str() );
  
        if( layerStack!=NULL )
    {
    KeyLayerAccumulator accumulator(
      std::for_each(
        layerStack->Begin(),
        layerStack->End(), KeyLayerAccumulator( filename, layers )
      )
    );
  
    if( accumulator.GetCount()>0 )
      filenames2.push_back( filename.c_str() );
    }
        }
      }
  
    {
    QString message;
  
    if( filenames1.size()==1 )
      {
      // qDebug()
      //   << it->c_str() << ":" << QString( filename.c_str() );
  
      message =
        tr( "Are you sure you want to overwrite file '%1'?" )
        .arg( filenames1.front() );
      }
    else if( filenames1.size()>1 )
      {
      message =
        tr( "Following files will be overwritten. Are you sure you want to continue?\n- %1" )
        .arg( filenames1.join( "\n- " ) );
      }
  
    if( !message.isEmpty() )
      {
      QMessageBox::StandardButton button =
        QMessageBox::question(
    this,
    PROJECT_NAME,
    message,
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No
        );
  
      if( button==QMessageBox::No )
        return;
      }
    }
  
    {
    QString message;
  
    if( filenames2.size()==1 )
      {
      // qDebug()
      //   << it->c_str() << ":" << QString( filename.c_str() );
  
      message =
        tr( "File '%1' is being viewed in " PROJECT_NAME " and will be concurrently overwritten by running this %2. File will be removed from layer-stack before running %2 and reloaded after.\n\nDo you want to continue?" )
        .arg( filenames2.front() )
        .arg( otbApp->GetDocName() );
      }
    else if( filenames2.size()>1 )
      {
      message =
        tr( "Following files are being viewed in " PROJECT_NAME " and will be concurrently overwritter by running %2. Files will be removed from layer-stack before running %2. Do you want to continue?\n- %1" )
        .arg( filenames2.join( "\n- " ) )
        .arg( otbApp->GetDocName() );
      }
  
    if( !message.isEmpty() )
      {
      QMessageBox::StandardButton button =
        QMessageBox::question(
    this,
    PROJECT_NAME,
    message,
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No
        );
  
      if( button==QMessageBox::No )
        return;
  
      while( !layers.empty() )
        {
        layerStack->Delete( layers.front().first );
  
        layers.pop_front();
        }
      }
    }
  }

  otb::Wrapper::QtWidgetView::OnExecButtonClicked();
}

/*******************************************************************************/
void
QtWidgetView
::OnExceptionRaised( QString what  )
{
  qWarning() << what;

#if defined( OTB_DEBUG )
  QMessageBox::warning(
    this,
    PROJECT_NAME,
    what,
    QMessageBox::Ok
  );
#endif
}

/*******************************************************************************/
void
QtWidgetView
::OnApplicationExecutionDone( int status )
{
  otb::Wrapper::Application::Pointer otbApp( GetModel()->GetApplication() );

  if( status!=0 )
    {
    QMessageBox::information(
      this,
      PROJECT_NAME,
      tr( "'%1' has failed with return status %2.\n"
	  "Please refer to '%1' documentation and check log tab."
      )
      .arg( otbApp->GetName() )
      .arg( status ),
      QMessageBox::Ok
    );

    emit ExecutionDone( status );

    return;
    }

  /*
  // Removed as per MVDX-259.
  QMessageBox::information(
    this,
    PROJECT_NAME,
    tr( "'%1' has succeeded.\n"
	"Result(s) will be imported as dataset(s).\n")
    .arg( otbApp->GetName() ),
    QMessageBox::Ok
  );
  */

  CountType count = 0;

  //
  // detect if this application has outputImageParameter. emit
  // the output filenames if any
  StringVector paramList( otbApp->GetParametersKeys( true ) );

  // iterate on the application parameters
  for ( StringVector::const_iterator it( paramList.begin() );
	it!=paramList.end();
	++it )
    {
    // parameter key
    const std::string& key = *it;

    // get a valid outputParameter
    if( otbApp->GetParameterType( key )
	==otb::Wrapper::ParameterType_OutputImage && 
	otbApp->IsParameterEnabled( key, true ) &&
	otbApp->HasValue( key ) )
      {
      // get the parameter
      otb::Wrapper::Parameter* param = otbApp->GetParameterByKey( key );

      // try to cast it 
      otb::Wrapper::OutputImageParameter* outputParam = 
	dynamic_cast< otb::Wrapper::OutputImageParameter* >( param );

      // emit the output image filename selected
      if( outputParam!=NULL )
	{
	QFileInfo fileInfo( outputParam->GetFileName() );

	/* U N S A F E
	// BUGFIX: Mantis-750
	//
	// If output image-exists, it's sure that it has been output
	// from the OTB-application process because overwritten
	// files are first deleted (see OnExecButtonClicked()).
	if( fileInfo.exists() )
	{
	*/
	++ count;

	emit OTBApplicationOutputImageChanged(
	  QString( otbApp->GetName() ),
	  QFile::decodeName( outputParam->GetFileName() )
	);
	/*
	}
	*/
	}
      }
    }

  emit ExecutionDone( status );
}

} // end of namespace Wrapper
} // end of namespace mvd
