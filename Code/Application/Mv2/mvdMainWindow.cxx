/*=========================================================================

  Program:   Monteverdi2
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mvdMainWindow.h"
#include "ui_mvdMainWindow.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.
#include <QtGui>

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)
#if defined( OTB_WRAP_QT ) && USE_OTB_APPS
# include "ApplicationsWrapper/mvdApplicationLauncher.h"
# include "ApplicationsWrapper/mvdApplicationsToolBoxController.h"
# include "ApplicationsWrapper/mvdOTBApplicationsModel.h"
# include "ApplicationsWrapper/mvdWrapperQtWidgetView.h"
#endif
//
// #include "Core/mvdDatabaseModel.h"
// #include "Core/mvdDatasetModel.h"
#include "Core/mvdQuicklookModel.h"
#include "Core/mvdVectorImageModel.h"
//
#include "Gui/mvdApplicationsToolBox.h"
#include "Gui/mvdColorSetupController.h"
#include "Gui/mvdColorSetupWidget.h"
#include "Gui/mvdColorDynamicsController.h"
#include "Gui/mvdColorDynamicsWidget.h"
// #include "Gui/mvdDatabaseBrowserController.h"
#if ENABLE_TREE_WIDGET_TEST
// #include "Gui/mvdDatabaseBrowserWidgetTest.h"
#else // ENABLE_TREE_WIDGET_TEST
// #include "Gui/mvdDatabaseBrowserWidget.h"
#endif // ENABLE_TREE_WIDGET_TEST
// #include "Gui/mvdDatasetPropertiesController.h"
// #include "Gui/mvdDatasetPropertiesWidget.h"
#include "Gui/mvdFilenameDragAndDropEventFilter.h"
#include "Gui/mvdHistogramController.h"
#include "Gui/mvdHistogramWidget.h"
#include "Gui/mvdImageViewManipulator.h"
#include "Gui/mvdImageViewRenderer.h"
#include "Gui/mvdImageViewWidget.h"
#include "Gui/mvdLayerStackController.h"
#include "Gui/mvdLayerStackWidget.h"
#if USE_PIXEL_DESCRIPTION
#  include "Gui/mvdPixelDescriptionWidget.h"
#endif // USE_PIXEL_DESCRIPTION
#include "Gui/mvdQuicklookViewManipulator.h"
#include "Gui/mvdQuicklookViewRenderer.h"
#include "Gui/mvdStatusBarWidget.h"
//
#include "mvdApplication.h"
#include "mvdPreferencesDialog.h"

namespace mvd
{

/*
  TRANSLATOR mvd::MainWindow

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CONSTANTS                                                                 */

#define REFERENCE_LAYER_COMBOBOX_NAME "referenceLayerComboBox"

/*****************************************************************************/
/* STATIC IMPLEMENTATION SECTION                                             */


/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*****************************************************************************/
MainWindow
::MainWindow( QWidget* parent, Qt::WindowFlags flags ) :
  I18nMainWindow( parent, flags ),
  m_UI( new mvd::Ui::MainWindow() ),
  m_ColorDynamicsDock( NULL ),
  m_ColorSetupDock( NULL ),
  // m_DatabaseBrowserDock( NULL ),
  // m_DatasetPropertiesDock(NULL),
  m_LayerStackDock( NULL ),
#if USE_PIXEL_DESCRIPTION
  m_PixelDescriptionDock(NULL),
#endif // USE_PIXEL_DESCRIPTION
  m_HistogramDock( NULL ),
#if defined( OTB_WRAP_QT ) && USE_OTB_APPS
  m_OtbApplicationsBrowserDock(NULL),
#endif
  m_ImageView( NULL ),
  m_QuicklookViewDock( NULL ),
  m_CentralTabWidget( NULL ),
  m_FilenameDragAndDropEventFilter( NULL )
{
  m_UI->setupUi( this );

  //
  // Event filters.
  m_FilenameDragAndDropEventFilter = new FilenameDragAndDropEventFilter( this );

  QObject::connect(
    m_FilenameDragAndDropEventFilter,
    SIGNAL( FilenameDropped( const QString& ) ),
    // to:
    this,
    SLOT( OnFilenameDropped( const QString& ) )
  );
}

/*****************************************************************************/
MainWindow
::~MainWindow()
{
  delete m_UI;
  m_UI = NULL;
}

/*****************************************************************************/
bool
MainWindow
::CheckGLCapabilities() const
{
  assert( m_ImageView!=NULL );
  assert( m_ImageView->GetRenderer()!=NULL );

  if( m_ImageView==NULL ||
      m_ImageView->GetRenderer()==NULL )
    return false;

  return m_ImageView->GetRenderer()->CheckGLCapabilities();
}

/*****************************************************************************/
void
MainWindow
::virtual_SetupUI()
{
  setObjectName( "mvd::MainWindow" );
  setWindowTitle( PROJECT_NAME );

  InitializeCentralWidget();

  InitializeDockWidgets();

  InitializeStatusBarWidgets();

  assert( m_UI!=NULL );

  {
  QComboBox * comboBox =
    new QComboBox( m_UI->m_RenderToolBar );

  assert( comboBox!=NULL );

  comboBox->setObjectName( "referenceLayerComboBox" );
  comboBox->setMinimumSize( QSize( 128, 0 ) );

  m_UI->m_RenderToolBar->addWidget( comboBox );
  }
}

/*****************************************************************************/
void
MainWindow
::virtual_ConnectUI()
{
  ConnectViewMenu();

  //
  // CHAIN CONTROLLERS.
  // Forward model update signals of color-setup controller...
  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( RgbChannelIndexChanged( RgbwChannel, int ) ),
    // to: ...color-dynamics controller model update signal.
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SLOT( OnRgbChannelIndexChanged( RgbwChannel, int ) )
  );
  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( GrayChannelIndexChanged( int ) ),
    // to: ...color-dynamics controller model update signal.
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SLOT( OnGrayChannelIndexChanged( int ) )
  );
  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( GrayscaleActivated( bool ) ),
    // to: ...color-dynamics controller model update signal.
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SLOT( OnGrayscaleActivated( bool ) )
  );

  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( RgbChannelIndexChanged( RgbwChannel, int ) ),
    // to: ...color-dynamics controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnRgbChannelIndexChanged( RgbwChannel, int ) )
  );
  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( GrayChannelIndexChanged( int ) ),
    // to: ...color-dynamics controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnGrayChannelIndexChanged( int ) )
  );
  QObject::connect(
    m_ColorSetupDock->findChild< AbstractModelController* >(),
    SIGNAL( GrayscaleActivated( bool ) ),
    // to: ...color-dynamics controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnGrayscaleActivated( bool ) )
  );

  QObject::connect(
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SIGNAL( LowIntensityChanged( RgbwChannel, double, bool ) ),
    // to: ...histogram controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnLowIntensityChanged( RgbwChannel, double, bool ) )
  );
  QObject::connect(
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SIGNAL( HighIntensityChanged( RgbwChannel, double, bool ) ),
    // to: ...histogram controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnHighIntensityChanged( RgbwChannel, double, bool ) )
  );
  QObject::connect(
    m_ColorDynamicsDock->findChild< AbstractModelController* >(),
    SIGNAL( HistogramRefreshed() ),
    // to: ...histogram controller model update signal.
    m_HistogramDock->findChild< AbstractModelController* >(),
    SLOT( OnHistogramRefreshed() )
  );

  //
  // OTB application support.
#if defined( OTB_WRAP_QT ) && USE_OTB_APPS
  //
  // Done here cause needed to be done once and only once.
  SetControllerModel(
    m_OtbApplicationsBrowserDock,
    Application::Instance()->GetOTBApplicationsModel()
  );

  //
  // need to get the ApplicationToolBox widget to setup connections.
  // a double click on the tree widget should trigger a signal connected
  // to this MainWindow slot. this slot will be in charge of getting the
  // widget of the application selected, and show it in the
  // MainWindow centralView.

  // # Step 1 : get the ApplicationToolBoxWidget
  ApplicationsToolBox* appWidget =
    qobject_cast< ApplicationsToolBox * >(
      m_OtbApplicationsBrowserDock->findChild< ApplicationsToolBox* >()
    );
  assert( appWidget!=NULL );

  // # Step 2 : setup connections
  QObject::connect(
    appWidget,
    SIGNAL( ApplicationToLaunchSelected(const QString &, const QString &) ),
    this,
    SLOT( OnApplicationToLaunchSelected(const QString &, const QString &) )
  );

  // # Step 3 : connect close slots
#endif

  //
  // close tabs handling
  QObject::connect(
    m_CentralTabWidget,
    SIGNAL( tabCloseRequested( int ) ),
    this,
    SLOT( OnTabCloseRequested( int ) )
  );


  //
  // Image views
  ConnectImageViews();
}

/*****************************************************************************/
void
MainWindow
::ConnectImageViews()
{
  assert( m_ImageView!=NULL );

  QObject::connect(
    this,
    SIGNAL( UserZoomIn() ),
    // to:
    m_ImageView,
    SLOT( ZoomIn() )
  );

  QObject::connect(
    this,
    SIGNAL( UserZoomOut() ),
    // to:
    m_ImageView,
    SLOT( ZoomOut() )
  );

  QObject::connect(
    this,
    SIGNAL( UserZoomExtent() ),
    m_ImageView,
    SLOT( ZoomToExtent() )
  );

  QObject::connect(
    this,
    SIGNAL( UserZoomFull() ),
    m_ImageView,
    SLOT( ZoomToFullResolution() )
  );

  //
  // Connect image-views for ROI-changed events.

  const AbstractImageViewManipulator* imageViewManipulator =
    m_ImageView->GetManipulator();

  assert( imageViewManipulator!=NULL );


  ImageViewWidget* quicklookView = GetQuicklookView();
  assert( quicklookView!=NULL );

  const AbstractImageViewManipulator* quicklookManipulator =
    quicklookView->GetManipulator();

  assert( quicklookManipulator!=NULL );


  QObject::connect(
    m_ImageView,
    SIGNAL(
      RoiChanged(
        const PointType&, const SizeType&, const SpacingType&, const PointType&
      )
    ),
    // to:
    quicklookManipulator,
    SLOT(
      OnRoiChanged(
        const PointType&, const SizeType&, const SpacingType&, const PointType&
      )
    )
  );

  QObject::connect(
    quicklookView,
    SIGNAL( CenterRoiRequested( const PointType& ) ),
    // to:
    imageViewManipulator,
    SLOT( CenterOn( const PointType& ) )
  );

  QObject::connect(
    quicklookManipulator,
    SIGNAL( RefreshViewRequested() ),
    // to:
    m_ImageView,
    SLOT( updateGL() )
  );
}

/*****************************************************************************/
void
MainWindow
::ConnectViewMenu()
{
  m_UI->menu_View->addAction( m_UI->m_ToolBar->toggleViewAction() );
  m_UI->menu_View->addAction( m_UI->m_RenderToolBar->toggleViewAction() );
  
  m_UI->menu_View->addSeparator();

  m_UI->menu_View->addAction( m_ColorSetupDock->toggleViewAction() );
  m_UI->menu_View->addAction( m_ColorDynamicsDock->toggleViewAction() );
  m_UI->menu_View->addAction( m_HistogramDock->toggleViewAction() );
  m_UI->menu_View->addAction( m_LayerStackDock->toggleViewAction() );

  m_UI->menu_View->addSeparator();

  m_UI->menu_View->addSeparator();

#if USE_PIXEL_DESCRIPTION
  m_UI->menu_View->addAction( m_PixelDescriptionDock->toggleViewAction() );
#endif // USE_PIXEL_DESCRIPTION

#if defined( OTB_WRAP_QT ) && USE_OTB_APPS
  m_UI->menu_View->addAction(
    m_OtbApplicationsBrowserDock->toggleViewAction() );
#endif
}

/*****************************************************************************/
void
MainWindow
::ConnectStatusBar( AbstractLayerModel * model )
{
  assert( m_StatusBarWidget!=NULL );

  assert( m_ImageView!=NULL );

  QObject::connect(
    m_ImageView,
    SIGNAL( ScaleChanged( double, double ) ),
    // to:
    m_StatusBarWidget,
    SLOT( SetScale( double, double ) )
  );

  QObject::connect(
    m_StatusBarWidget,
    SIGNAL( ScaleChanged( double ) ),
    // to:
    m_ImageView->GetManipulator(),
    SLOT( ZoomTo( double ) )
  );

  QObject::connect(
    m_StatusBarWidget,
    SIGNAL( PixelIndexChanged( const IndexType& ) ),
    // to:
    m_ImageView,
    SLOT( CenterOn( const IndexType& ) )
  );

  if( model==NULL )
    return;

  // Access vector-image model.
  if( model->inherits( VectorImageModel::staticMetaObject.className() ) )
    {
    VectorImageModel * vectorImageModel =
      qobject_cast< VectorImageModel * >( model );

    assert( vectorImageModel==qobject_cast< VectorImageModel * >( model ) );

    //
    // send the physical point of the clicked point in screen
    // vectorImageModel is in charge of pixel information computation
    QObject::connect(
      m_ImageView,
      SIGNAL(
        PhysicalCursorPositionChanged(
          const PointType&,
          const DefaultImageType::PixelType&
        )
      ),
      // to:
      vectorImageModel,
      SLOT(
        OnPhysicalCursorPositionChanged(
          const PointType&,
          const DefaultImageType::PixelType&
        )
      )
    );

    QObject::connect(
      GetQuicklookView(),
      SIGNAL(
        PhysicalCursorPositionChanged(
          const PointType&,
          const DefaultImageType::PixelType&
        )
      ),
      // to:
      vectorImageModel,
      SLOT(
        OnPhysicalCursorPositionChanged(
          const PointType&,
          const DefaultImageType::PixelType&
        )
      )
    );

    //
    // Connect the status-bar widget to the vector-image corresponding
    // signal.
    QObject::connect(
      vectorImageModel,
      SIGNAL( CurrentIndexUpdated( const IndexType&, bool ) ),
      // to:
      m_StatusBarWidget,
      SLOT( SetPixelIndex( const IndexType&, bool ) )
    );

    QObject::connect(
      vectorImageModel,
      SIGNAL( CurrentRadioUpdated( const QString& ) ),
      // to:
      m_StatusBarWidget,
      SLOT( SetPixelRadiometryText( const QString& ) )
    );
    }
}

/*****************************************************************************/
#if USE_PIXEL_DESCRIPTION

void
MainWindow
::ConnectPixelDescriptionWidget( AbstractLayerModel * model )
{
  if( model==NULL ||
      !model->inherits( VectorImageModel::staticMetaObject.className() ) )
    return;

  // Access vector-image model.
  VectorImageModel * vectorImageModel =
    qobject_cast< VectorImageModel * >( model );

  assert( vectorImageModel==qobject_cast< VectorImageModel * >( model ) );
  assert( vectorImageModel!=NULL );

  // get the PixelDescription widget
  // TODO: Make a widget controller!
  PixelDescriptionWidget * pixelDescriptionWidget = 
    qobject_cast< PixelDescriptionWidget * >(
      m_PixelDescriptionDock->findChild< PixelDescriptionWidget * >()
    );

  assert( pixelDescriptionWidget!=NULL );

  QObject::connect(
    vectorImageModel,
    SIGNAL( CurrentPhysicalUpdated( const QStringList & ) ),
    // to:
    pixelDescriptionWidget,
    SLOT( OnCurrentPhysicalUpdated( const QStringList & ) )
  );

  QObject::connect(
    vectorImageModel,
    SIGNAL( CurrentGeographicUpdated( const QStringList & ) ),
    // to:
    pixelDescriptionWidget,
    SLOT( OnCurrentGeographicUpdated( const QStringList & ) )
  );

  QObject::connect(
    vectorImageModel,
    SIGNAL( CurrentPixelValueUpdated(const VectorImageType::PixelType &, 
                                     const QStringList & ) ),
    // to:
    pixelDescriptionWidget,
    SLOT( OnCurrentPixelValueUpdated(const VectorImageType::PixelType &, 
                                     const QStringList & ) )
  );
}

#endif // USE_PIXEL_DESCRIPTION

/*****************************************************************************/
#if USE_PIXEL_DESCRIPTION

void
MainWindow
::DisconnectPixelDescriptionWidget( const AbstractLayerModel * model )
{
  if( model==NULL ||
      !model->inherits( VectorImageModel::staticMetaObject.className() ) )
    return;

  // Access vector-image model.
  const VectorImageModel * vectorImageModel =
    qobject_cast< const VectorImageModel * >( model );

  assert( vectorImageModel==qobject_cast< const VectorImageModel * >( model ) );
  assert( vectorImageModel!=NULL );

  // get the PixelDescription widget
  // TODO: Make a widget controller!
  PixelDescriptionWidget * pixelDescriptionWidget = 
    qobject_cast< PixelDescriptionWidget * >(
      m_PixelDescriptionDock->findChild< PixelDescriptionWidget * >()
    );

  assert( pixelDescriptionWidget!=NULL );

  QObject::disconnect(
    vectorImageModel,
    SIGNAL( CurrentPhysicalUpdated( const QStringList & ) ),
    // to:
    pixelDescriptionWidget,
    SLOT( OnCurrentPhysicalUpdated( const QStringList & ) )
  );

  QObject::disconnect(
    vectorImageModel,
    SIGNAL( CurrentGeographicUpdated( const QStringList & ) ),
    // to:
    pixelDescriptionWidget,
    SLOT( OnCurrentGeographicUpdated( const QStringList & ) )
  );

  QObject::connect(
    vectorImageModel,
    SIGNAL(
      CurrentPixelValueUpdated(
        const VectorImageType::PixelType &,
        const QStringList &
      )
    ),
    // to:
    pixelDescriptionWidget,
    SLOT(
      OnCurrentPixelValueUpdated(
        const VectorImageType::PixelType &,
        const QStringList &
      )
    )
  );
}

#endif // USE_PIXEL_DESCRIPTION

/*****************************************************************************/
void
MainWindow
::DisconnectStatusBar( const AbstractLayerModel * model )
{
  assert( m_StatusBarWidget!=NULL );

  assert( m_ImageView!=NULL );

  QObject::disconnect(
    m_ImageView,
    SIGNAL( ScaleChanged( double, double ) ),
    // to:
    m_StatusBarWidget,
    SLOT( SetScale( double, double ) )
  );

  QObject::disconnect(
    m_StatusBarWidget,
    SIGNAL( ScaleChanged( double ) ),
    // to:
    m_ImageView->GetManipulator(),
    SLOT( ZoomTo( double ) )
  );

  QObject::disconnect(
    m_StatusBarWidget,
    SIGNAL( PixelIndexChanged( const IndexType& ) ),
    // to:
    m_ImageView,
    SLOT( CenterOn( const IndexType& ) )
  );

  if( model==NULL )
    return;

  if( model->inherits( VectorImageModel::staticMetaObject.className() ) )
    {
    // Access vector-image model.
    const VectorImageModel* vectorImageModel =
      qobject_cast< const VectorImageModel * >( model );

    assert(
      vectorImageModel==qobject_cast< const VectorImageModel * >( model )
    );

    //
    // send the physical point of the clicked point in screen
    // vectorImageModel is in charge of pixel information computation
    QObject::disconnect(
      m_ImageView,
      SIGNAL(
        PhysicalCursorPositionChanged( const PointType&,
                                       const DefaultImageType::PixelType& )
      ),
      // to:
      vectorImageModel,
      SLOT( OnPhysicalCursorPositionChanged( const PointType&,
                                             const DefaultImageType::PixelType& )
      )
    );

    QObject::disconnect(
      GetQuicklookView(),
      SIGNAL(
        PhysicalCursorPositionChanged( const PointType&,
                                       const DefaultImageType::PixelType& )
      ),
      // to:
      vectorImageModel,
      SLOT( OnPhysicalCursorPositionChanged( const PointType&,
                                             const DefaultImageType::PixelType& )
      )
    );

    //
    // disconnect the statusBar widget to the vectorImage corresponding
    // signal
    QObject::disconnect(
      vectorImageModel,
      SIGNAL( CurrentIndexUpdated( const IndexType&, bool ) ),
      m_StatusBarWidget,
      SLOT( SetPixelIndex( const IndexType&, bool ) )
      );

    QObject::disconnect(
      vectorImageModel,
      SIGNAL( CurrentRadioUpdated(const QString& ) ),
      m_StatusBarWidget,
      SLOT( SetPixelRadiometryText(const QString &) )
      );
    }
}

/*****************************************************************************/
void
MainWindow
::InitializeDockWidgets()
{
#define ENABLE_QTOOLBAR_TEST 0
#if ENABLE_QTOOLBAR_TEST
  // XP: QToolBar test.
  QDockWidget* dock =
    AddWidgetToDock(
      m_UI->m_ToolBar,
      "TOOL_BAR",
      tr( "Tool bar" ),
      Qt::TopDockWidgetArea
    );

  m_UI->menu_View->addAction( dock->toggleViewAction() );
#endif

  //
  // Left pane.

#if defined( OTB_WRAP_QT ) && USE_OTB_APPS
  // OTB-applications browser.
  assert( m_OtbApplicationsBrowserDock==NULL );
  m_OtbApplicationsBrowserDock =
    AddDockWidget
    < ApplicationsToolBox, ApplicationsToolBoxController, QDockWidget >
    ( "APPLICATIONS_BROWSER",
      tr( "OTB Applications browser" ),
      Qt::LeftDockWidgetArea );

  // tabifyDockWidget( m_DatasetPropertiesDock, m_OtbApplicationsBrowserDock );
#endif

  //
  // Right pane.

  // Quicklook-view dock-widget
  assert( m_QuicklookViewDock==NULL );
  assert( m_ImageView!=NULL );
  m_QuicklookViewDock = AddWidgetToDock(
    CreateQuicklookViewWidget( m_ImageView ),
    "QUICKLOOK_VIEW",
    tr( "Quicklook view" ),
    Qt::RightDockWidgetArea
  );

  // Layer-stack editor.
  assert( m_LayerStackDock==NULL );
  m_LayerStackDock = AddDockWidget<
    LayerStackWidget, LayerStackController, QDockWidget >(
      "LAYER_STACK",
      tr( "Layer stack" ),
      Qt::RightDockWidgetArea
    );

  tabifyDockWidget( m_QuicklookViewDock, m_LayerStackDock );

  // Histogram-view.
  assert( m_HistogramDock==NULL );
  m_HistogramDock =
    AddDockWidget
    < HistogramWidget, HistogramController, QDockWidget >
    ( "HISTOGRAM",
      tr( "Histogram" ),
      Qt::RightDockWidgetArea,
      false
    );

#if USE_PIXEL_DESCRIPTION

  // Pixel Description (no controller needed here / direct update of
  // the pixel description through signals from VectorImageModel)
  assert( m_PixelDescriptionDock==NULL );
  m_PixelDescriptionDock =
    AddDockWidget
    < PixelDescriptionWidget, QDockWidget >
    ( "CURRENT_PIXEL_DESCRIPTION",
      tr( "Pixel Description" ),
      Qt::RightDockWidgetArea
    );

  tabifyDockWidget( m_PixelDescriptionDock, m_HistogramDock );

#endif // USE_PIXEL_DESCRIPTION

  // Color-setup.
  assert( m_ColorSetupDock==NULL );
  m_ColorSetupDock =
    AddDockWidget
    < ColorSetupWidget, ColorSetupController, QDockWidget >
    ( "COLOR_SETUP",
      tr( "Color setup" ),
      Qt::RightDockWidgetArea
    );

  // Color-dynamics.
  assert( m_ColorDynamicsDock==NULL );
  m_ColorDynamicsDock =
    AddDockWidget
    < ColorDynamicsWidget, ColorDynamicsController, QDockWidget >
    ( "COLOR_DYNAMICS",
      tr( "Color dynamics" ),
      Qt::RightDockWidgetArea
    );

  // Tabify dock-widgets.
  tabifyDockWidget( m_ColorSetupDock, m_ColorDynamicsDock );
}

/*****************************************************************************/
void
MainWindow
::InitializeCentralWidget()
{
  // Create central tab-widget for multi-view support.
  assert( m_CentralTabWidget==NULL );
  m_CentralTabWidget = new QTabWidget( this );

  // Customize it.
  m_CentralTabWidget->setTabsClosable( true );

  // Set-it up as central widget.
  setCentralWidget( m_CentralTabWidget );

  //
  // access to the quicklook tabBar to remove the close button
  QTabBar* tabBar = m_CentralTabWidget->findChild< QTabBar* >();

  // Initialize image-view.
  assert( m_ImageView==NULL );
  m_ImageView = CreateImageViewWidget();

  // Add first tab: image-view.
  int index = m_CentralTabWidget->addTab(
    m_ImageView,
    tr( "Image view" )
  );

  tabBar->setTabButton( index, QTabBar::RightSide, 0);
  tabBar->setTabButton( index, QTabBar::LeftSide, 0);
}

/*****************************************************************************/
void
MainWindow
::InitializeStatusBarWidgets()
{
  m_StatusBarWidget = new StatusBarWidget(statusBar());
  statusBar()->addPermanentWidget(m_StatusBarWidget, 1);
}

/*****************************************************************************/
ImageViewWidget*
MainWindow
::CreateImageViewWidget( QGLWidget* sharedGlWidget )
{
  ImageViewRenderer* renderer =
    new ImageViewRenderer( this );

  ImageViewManipulator* manipulator =
    new ImageViewManipulator(
#if USE_VIEW_SETTINGS_SIDE_EFFECT
      renderer->GetViewSettings(),
#endif // USE_VIEW_SETTINGS_SIDE_EFFECT
      this
    );

  ImageViewWidget* imageView = new ImageViewWidget(
    manipulator, // (will be reparented.)
    renderer, // (will be reparented.)
    this,
    sharedGlWidget
  );

  imageView->setMinimumWidth( 256 );

  return imageView;
}

/*****************************************************************************/
ImageViewWidget*
MainWindow
::CreateQuicklookViewWidget( QGLWidget* sharedGlWidget )
{
  QuicklookViewRenderer* renderer =
    new QuicklookViewRenderer( this );

  QuicklookViewManipulator* manipulator =
    new QuicklookViewManipulator(
#if USE_VIEW_SETTINGS_SIDE_EFFECT
      renderer->GetViewSettings(),
#endif // USE_VIEW_SETTINGS_SIDE_EFFECT
      this
    );

  ImageViewWidget* quicklookView = new ImageViewWidget(
    manipulator, // (will be reparented.)
    renderer, // (will be reparented.)
    this,
    sharedGlWidget
  );

  quicklookView->setMinimumSize(  64,  64 );
  quicklookView->setMaximumSize( 512, 512 );
  quicklookView->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

  return quicklookView;
}

/*****************************************************************************/
void
MainWindow
::ImportImage( const QString& filename, bool forceCreate )
{
  VectorImageModel * imageModel = ImportImage( filename, -1, -1 );
  // assert( imageModel );

  if( imageModel==NULL )
    return;

  assert( Application::Instance() );
  assert(
    Application::Instance()->GetModel()==
    Application::Instance()->GetModel< StackedLayerModel >()
  );

  StackedLayerModel* stackedLayerModel =
    Application::Instance()->GetModel< StackedLayerModel >();

  assert( stackedLayerModel!=NULL );

  stackedLayerModel->Add( imageModel );

  imageModel->setParent( stackedLayerModel );

  stackedLayerModel->SetCurrent( imageModel );

  if( !stackedLayerModel->HasReference() )
    stackedLayerModel->SetReference( imageModel  );
}

/*****************************************************************************/
void
MainWindow
::closeEvent( QCloseEvent* event )
{
  I18nMainWindow::closeEvent( event );
}

/*****************************************************************************/
void
MainWindow
::virtual_InitializeUI()
{
  assert( Application::Instance() );

  Application::Instance()->SetModel( new StackedLayerModel() ); 
}

/*****************************************************************************/
void
MainWindow
::ConnectReferenceLayerComboBox( StackedLayerModel * model )
{
  assert( model!=NULL );

  QComboBox * comboBox =
    m_UI->m_RenderToolBar->findChild< QComboBox * >(
      REFERENCE_LAYER_COMBOBOX_NAME
    );

  assert( comboBox!=NULL );

  QObject::connect(
    comboBox,
    SIGNAL( currentIndexChanged( int ) ),
    // to:
    this,
    SLOT( OnReferenceLayerCurrentIndexChanged( int ) )
  );

  QObject::connect(
    model,
    SIGNAL( ContentChanged() ),
    // to:
    this,
    SLOT( RefreshReferenceLayerComboBox() )
  );

  QObject::connect(
    model,
    SIGNAL( OrderChanged() ),
    // to:
    this,
    SLOT( RefreshReferenceLayerComboBox() )
  );

  QObject::connect(
    model,
    SIGNAL( ReferenceChanged( size_t ) ),
    // to:
    this,
    SLOT( OnReferenceLayerChanged( size_t ) )
  );
}

/*****************************************************************************/
void
MainWindow
::SetupReferenceLayerComboBox( StackedLayerModel * model )
{
  QComboBox * comboBox =
    m_UI->m_RenderToolBar->findChild< QComboBox * >(
      REFERENCE_LAYER_COMBOBOX_NAME
    );

  assert( comboBox!=NULL );


  bool signalsBlocked = comboBox->blockSignals( true );
  {
  comboBox->clear();

  comboBox->addItem( "None" );
  }
  comboBox->blockSignals( signalsBlocked );


  if( model==NULL )
    return;


  for( StackedLayerModel::SizeType i=0;
       i<model->GetCount();
       ++i )
    {
    AbstractLayerModel * layer = model->At( i );
    assert( layer!=NULL );

    if( layer->inherits( VectorImageModel::staticMetaObject.className() ) )
      {
      const VectorImageModel * vectorImageModel =
        qobject_cast< const VectorImageModel * >( layer );

      assert( vectorImageModel!=NULL );

      comboBox->addItem(
        QString( "%1: %2" )
        .arg( comboBox->count() - 1 )
        .arg( QFileInfo( vectorImageModel->GetFilename() ).fileName() )
      );
      }
    else
      qDebug() << "Unhandled AbstractLayerModel subclass.";
    }


#if 1
  comboBox->setCurrentIndex(
    model->GetReferenceIndex()>=model->GetCount()
    ? 0 // comboBox->count() - 1
    : model->GetReferenceIndex() + 1
  );
#endif


  comboBox->setEnabled( model->GetCount()>0 );
}

/*****************************************************************************/
void
MainWindow
::DisconnectReferenceLayerComboBox( StackedLayerModel * model )
{
  QComboBox * comboBox =
    m_UI->m_RenderToolBar->findChild< QComboBox * >(
      REFERENCE_LAYER_COMBOBOX_NAME
    );

  assert( comboBox!=NULL );

  QObject::disconnect(
    comboBox,
    SIGNAL( currentIndexChanged( int ) ),
    // to:
    this,
    SLOT( OnReferenceLayerCurrentIndexChanged( int ) )
  );

  if( model!=NULL )
    {
    QObject::disconnect(
      model,
      SIGNAL( ContentChanged() ),
      // to:
      this,
      SLOT( RefreshReferenceLayerComboBox() )
    );

    QObject::disconnect(
      model,
      SIGNAL( OrderChanged() ),
      // to:
      this,
      SLOT( RefreshReferenceLayerComboBox() )
    );

    QObject::disconnect(
      model,
      SIGNAL( ReferenceChanged( size_t ) ),
      // to:
      this,
      SLOT( OnReferenceLayerChanged( size_t ) )
    );
    }

  comboBox->clear();
  comboBox->setEnabled( false );
}

/*****************************************************************************/
/* SLOTS                                                                     */
/*****************************************************************************/
void
MainWindow
::on_action_ImportImage_triggered()
{
  //
  // Select filename.

  QString filename(
    I18nMainWindow::GetOpenFileName( this, tr( "Open file..." ) )
  );

  if( filename.isNull() )
    return;

  //
  // Import the image
  ImportImage( filename, true );
}

/*****************************************************************************/
void
MainWindow
::on_action_Preferences_triggered()
{
  PreferencesDialog prefDialog( this );

  prefDialog.exec();
}

/*****************************************************************************/
void
MainWindow
::on_action_ZoomIn_triggered()
{
  emit UserZoomIn();
}

/*****************************************************************************/
void
MainWindow
::on_action_ZoomOut_triggered()
{
  emit UserZoomOut();
}

/*****************************************************************************/
void
MainWindow
::on_action_ZoomExtent_triggered()
{
  emit UserZoomExtent();
}

/*****************************************************************************/
void
MainWindow
::on_action_ZoomFull_triggered()
{
  emit UserZoomFull();
}

/*****************************************************************************/
void
MainWindow
::OnAboutToChangeModel( const AbstractModel * model )
{
  qDebug() << this << "::OnAboutToChangeModel(" << model << ")";


  // Force to disconnect previously selected layer-model before
  // stacked-layer model is disconnected.
  //
  // If there was no previously set layer-model, this will cause GUI
  // views to be disabled.
  //
  // N.B.: This will cause UI controllers to disable widgets.
  OnAboutToChangeSelectedLayerModel( StackedLayerModel::KeyType() );

  // Assign stacked-layer model controller.
  SetControllerModel( m_LayerStackDock, NULL );


  assert( Application::Instance() );
  assert( Application::Instance()->GetModel()==
          Application::Instance()->GetModel< StackedLayerModel >() );

  StackedLayerModel * stackedLayerModel =
    Application::Instance()->GetModel< StackedLayerModel >();

  DisconnectReferenceLayerComboBox( stackedLayerModel );

  // Exit, if there were no previously set database model.
  if( stackedLayerModel==NULL )
    return;

  // Disonnect stacked-layer model from main-window when selected
  // layer-model is about to change.
  QObject::disconnect(
    stackedLayerModel,
    SIGNAL(
      AboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & )
    ),
    // from:
    this,
    SLOT(
      OnAboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & )
    )
  );

  // Disconnect stacked-layer model to main-window when selected
  // layer-model has been changed.
  QObject::disconnect(
    stackedLayerModel,
    SIGNAL( SelectedLayerModelChanged( const StackedLayerModel::KeyType & ) ),
    // from:
    this,
    SLOT( OnSelectedLayerModelChanged( const StackedLayerModel:KeyType & ) )
  );
}

/*****************************************************************************/
void
MainWindow
::OnModelChanged( AbstractModel * model )
{
  qDebug() << this << "::OnModelChanged(" << model << ")";

  assert( model==qobject_cast< StackedLayerModel * >( model ) );

  StackedLayerModel * stackedLayerModel =
    qobject_cast< StackedLayerModel * >( model );

  SetupReferenceLayerComboBox( stackedLayerModel );
  ConnectReferenceLayerComboBox( stackedLayerModel );

  // Assign stacked-layer model controller.
  SetControllerModel( m_LayerStackDock, model );

  m_ImageView->SetLayerStack( stackedLayerModel );

  assert( GetQuicklookView()!=NULL );
  GetQuicklookView()->SetLayerStack( stackedLayerModel );

  if( stackedLayerModel==NULL )
    return;


  // Connect stacked-layer model to main-window when selected layer-model
  // is about to change.
  QObject::connect(
    stackedLayerModel,
    SIGNAL(
      AboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & )
    ),
    // to:
    this,
    SLOT(
      OnAboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & )
    )
  );

  // Connect stacked-layer -model to main-window when selected layer-model
  // has been changed.
  QObject::connect(
    stackedLayerModel,
    SIGNAL( SelectedLayerModelChanged( const StackedLayerModel::KeyType & ) ),
    // to:
    this,
    SLOT( OnSelectedLayerModelChanged( const StackedLayerModel::KeyType & ) )
  );

  // Force to connect selected layer-model after stacked-layer model
  // is connected.
  //
  // N.B.: This will cause UI controllers to disable widgets.
  OnSelectedLayerModelChanged( stackedLayerModel->GetCurrentKey() );
}

/*****************************************************************************/
void
MainWindow
::OnAboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & key )
{
  // qDebug()
  //   << this
  //   << "::OnAboutToChangeSelectedDatasetModel(" << FromStdString( key ) << ")";

  //
  // CONTROLLERS.
  //

  //
  // Unset model from controllers.
  //
  // N.B.: This step must be done *before* disconnecting signals &
  // slots between model(s) and view(s).
  //
  // See also, ::OnSelectedLayerModel() changed.

  // Unset image-model from color-dynamics controller.
  SetControllerModel( m_ColorDynamicsDock, NULL );

  // Unset image-model from color-setup controller.
  SetControllerModel( m_ColorSetupDock, NULL );

  // Unset histogram-model from histogram controller.
  SetControllerModel( m_HistogramDock, NULL );

  // Unset stacked-layer model from stacked-layer controller.
  // SetControllerModel( m_StackedLayerDock, NULL );

  //
  // VIEWS.
  //

  //
  // MODEL(s).
  //
  assert( Application::Instance() );
  assert( Application::Instance()->GetModel()==
          Application::Instance()->GetModel< StackedLayerModel >() );

  const StackedLayerModel * stackedLayerModel =
    Application::Instance()->GetModel< StackedLayerModel >();

  if( !stackedLayerModel )
    return;

  const AbstractLayerModel * layerModel = stackedLayerModel->GetCurrent();

  if( !layerModel )
    return;

  if( layerModel->inherits( VectorImageModel::staticMetaObject.className() ) )
    {
    // Disconnect previously selected image-model from view.
    QObject::disconnect(
      layerModel,
      SIGNAL( SettingsUpdated() ),
      // from:
      m_ImageView,
      SLOT( updateGL()  )
    );

    // Disconnect previously selected quicklook-model from view.
    // TODO: Remove quicklook temporary hack by better design.
    QObject::disconnect(
      layerModel,
      SIGNAL( SettingsUpdated() ),
      // from:
      m_QuicklookViewDock->widget(),
      SLOT( updateGL()  )
    );
    }

  else
    {
    assert( false && "Unhandled AbstractLayerModel derived-type." );
    }

  DisconnectStatusBar( layerModel );

#if USE_PIXEL_DESCRIPTION
  DisconnectPixelDescriptionWidget( layerModel );
#endif // USE_PIXEL_DESCRIPTION
}

/*****************************************************************************/
void
MainWindow
::OnSelectedLayerModelChanged( const StackedLayerModel::KeyType & key )
{
  // qDebug()
  //   << this << "::OnLayerModelChanged( " << FromStdString( key ) << ")";

  assert( Application::Instance() );
  assert( Application::Instance()->GetModel()==
          Application::Instance()->GetModel< StackedLayerModel >() );

  StackedLayerModel * stackedLayerModel =
    Application::Instance()->GetModel< StackedLayerModel >();

  assert( stackedLayerModel==Application::Instance()->GetModel() );

  if( stackedLayerModel==NULL )
    return;

  //
  // VIEWS.
  //

  //
  // MODEL(s).
  //

  AbstractLayerModel * layerModel = stackedLayerModel->GetCurrent();

  if( !layerModel )
    return;

  if( layerModel->inherits( VectorImageModel::staticMetaObject.className() ) )
    {
    //
    // SAT: Using m_TabWidget->index( 0 ) or m_ImageView is equivalent
    // since Qt may use signal & slot names to connect (see MOC .cxx
    // files). Thus, using m_ImageView saves one indirection call.
    QObject::connect(
      layerModel,
      SIGNAL( SettingsUpdated() ),
      // to:
      m_ImageView,
      SLOT( updateGL()  )
    );

    QObject::connect(
      layerModel,
      SIGNAL( SettingsUpdated() ),
      // to:
      m_QuicklookViewDock->widget(),
      SLOT( updateGL()  )
    );

    VectorImageModel * imageModel =
      qobject_cast< VectorImageModel * >( layerModel );

    assert( imageModel!=NULL );

    setWindowTitle(
      QString( PROJECT_NAME " - %1" )
      .arg( QFileInfo( imageModel->GetFilename() ).fileName() )
    );
    }
  else
    {
    assert( false && "Unhandled AbstractLayerModel derived-type." );
    }

  ConnectStatusBar( layerModel );

#if USE_PIXEL_DESCRIPTION
  ConnectPixelDescriptionWidget( layerModel );
#endif // USE_PIXEL_DESCRIPTION

  //
  // CONTROLLERS.
  //

  //
  // Connect image-model controllers.
  //
  // N.B.: This step *must* be done after signals and slots between
  // model(s) and view(s) have been connected (because when model is
  // assigned to controller, widgets/view are reset and emit
  // refreshing signals).
  //
  // See also: OnAboutToChangeLayerModel().

  // Assign dataset-model to dataset-properties controller.

  // Assign image-model to color-dynamics controller.
  SetControllerModel( m_ColorDynamicsDock, layerModel );

  // Assign image-model to color-setup controller.
  SetControllerModel( m_ColorSetupDock, layerModel );

  // Assign histogram-model to histogram controller.
  SetControllerModel( m_HistogramDock, layerModel );

  //
  // TOOLBAR.
  //
  m_UI->action_ZoomIn->setEnabled( layerModel!=NULL );
  m_UI->action_ZoomOut->setEnabled( layerModel!=NULL );
  m_UI->action_ZoomExtent->setEnabled( layerModel!=NULL );
  m_UI->action_ZoomFull->setEnabled( layerModel!=NULL );
}

/*****************************************************************************/
void
MainWindow
::OnApplicationToLaunchSelected( const QString& appName,
				 const QString& docName)
{
#if defined( OTB_WRAP_QT ) && USE_OTB_APPS

  assert( Application::ConstInstance()!=NULL );
  assert( Application::ConstInstance()->GetOTBApplicationsModel()!=NULL );
  assert(
    Application::ConstInstance()->GetOTBApplicationsModel()->GetLauncher()!=NULL
  );

  Wrapper::QtWidgetView* appWidget =
    Application::ConstInstance()
    ->GetOTBApplicationsModel()
    ->GetLauncher()
    ->NewOtbApplicationWidget( appName );

  assert( appWidget!=NULL );

  //
  // add the application in a tab
  // TODO : check if this application is already opened ???
  int tabIndex = m_CentralTabWidget->addTab(
    appWidget, QIcon( ":/icons/process" ), docName );

  // no checking needed here, if index is not available nothing is
  // done. Focus on the newly added tab
  m_CentralTabWidget->setCurrentIndex( tabIndex );

  //
  // connections. not using m_CentralTabWidget->currentWidget() leads
  // to a wrong connection!!!!
  QObject::connect(
    m_CentralTabWidget->currentWidget(),
    SIGNAL( OTBApplicationOutputImageChanged( const QString&,
					      const QString& ) ),
    // to:
    this,
    SLOT( OnOTBApplicationOutputImageChanged( const QString&,
					      const QString& ) )
    );

  QObject::connect(
    appWidget,
    SIGNAL( ExecutionDone( int ) ),
    // to:
    this,
    SLOT( OnExecutionDone( int ) )
  );

  //
  // on quit widget signal, close its tab
  QObject::connect(
    m_CentralTabWidget->currentWidget(),
    SIGNAL( QuitSignal() ),
    // to:
    this,
    SLOT( OnTabCloseRequested() )
    );

#endif

}

/*****************************************************************************/
void
MainWindow
::OnTabCloseRequested()
{
  // get current tab index
  int currentIndex = m_CentralTabWidget->currentIndex();

  // close tab and delete its widget
  OnTabCloseRequested( currentIndex );
}

/*****************************************************************************/
void
MainWindow
::OnTabCloseRequested( int index )
{
  assert( index >= 1 );

  QWidget* appWidget = m_CentralTabWidget->widget( index );
  assert( appWidget!=NULL );

#if defined( OTB_WRAP_QT ) && USE_OTB_APPS

  assert( appWidget==qobject_cast< Wrapper::QtWidgetView* >( appWidget ) );
  Wrapper::QtWidgetView* appWidgetView =
    qobject_cast< Wrapper::QtWidgetView* >( appWidget );
  assert( appWidgetView!=NULL );

  if( !appWidgetView->IsClosable() )
    {
    QMessageBox::warning(
      this,
      tr( "Monteverdi2 - Warning!" ),
      tr( "Tab cannot be closed while OTB application is running." )
    );
   
    return;
    }

#endif

  m_CentralTabWidget->removeTab( index );

  delete appWidget;
  appWidget = NULL;
}

/*****************************************************************************/
void
MainWindow
::OnOTBApplicationOutputImageChanged( const QString& appName,
				      const QString & outfname )
{
  //
  // If this slot is called, it means that an application has finished
  // its process and has an output image parameter. The 'outfname' in
  // parameters is the output filename. This slot may launch or not,
  // depending on the app settings, the import of the 'outfname' to the
  // catalog database.

  // import the result image into the database
  ImportImage( outfname, true );
}

/*****************************************************************************/
void
MainWindow
::OnExecutionDone( int status )
{
  if( status<0 )
    return;
}

/*****************************************************************************/
void
MainWindow
::OnFilenameDropped( const QString& filename )
{
  ImportImage( filename, true );
}

/*****************************************************************************/
void
MainWindow
::OnReferenceLayerChanged( size_t index )
{
  // qDebug() << this << "::OnReferenceLayerChanged(" << index << ")";

  //
  // Access widget.
  QComboBox * comboBox =
    m_UI->m_RenderToolBar->findChild< QComboBox * >(
      REFERENCE_LAYER_COMBOBOX_NAME
    );

  assert( comboBox!=NULL );

  //
  // Access model.
  assert( I18nCoreApplication::Instance()!=NULL );
  assert( I18nCoreApplication::Instance()->GetModel()==
          I18nCoreApplication::Instance()->GetModel< StackedLayerModel >() );

  StackedLayerModel * model = 
    I18nCoreApplication::Instance()->GetModel< StackedLayerModel >();

  assert( model!=NULL );

  //
  // Update widget from model.
  comboBox->setCurrentIndex(
    model->GetReferenceIndex()>=model->GetCount()
    ? 0 // comboBox->count() - 1
    : model->GetReferenceIndex() + 1
  );
}

/*****************************************************************************/
void
MainWindow
::OnReferenceLayerCurrentIndexChanged( int index )
{
  // qDebug() << this << "::OnReferenceLayerCurrentIndexChanged(" << index << ")";

  //
  // Access widget.
  QComboBox * comboBox =
    m_UI->m_RenderToolBar->findChild< QComboBox * >(
      REFERENCE_LAYER_COMBOBOX_NAME
    );

  assert( comboBox!=NULL );

  //
  // Access model.
  assert( I18nCoreApplication::Instance()!=NULL );
  assert( I18nCoreApplication::Instance()->GetModel()==
          I18nCoreApplication::Instance()->GetModel< StackedLayerModel >() );

  StackedLayerModel * model = 
    I18nCoreApplication::Instance()->GetModel< StackedLayerModel >();

  assert( model!=NULL );


  //
  // Update model.
  model->SetReference(
    index<=0 // index>=comboBox->count() - 1
    ? StackedLayerModel::NIL_INDEX
    : comboBox->currentIndex() - 1
  );
}

/*****************************************************************************/
void
MainWindow
::RefreshReferenceLayerComboBox()
{
  // qDebug() << this << "::RefreshReferenceLayerComboBox()";

  assert( I18nCoreApplication::Instance()!=NULL );
  assert( I18nCoreApplication::Instance()->GetModel()==
          I18nCoreApplication::Instance()->GetModel< StackedLayerModel >() );

  SetupReferenceLayerComboBox(
    I18nCoreApplication::Instance()->GetModel< StackedLayerModel >()
  );
}

} // end namespace 'mvd'
