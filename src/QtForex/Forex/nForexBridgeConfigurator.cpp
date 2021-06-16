#include <qtforex.h>
#include "ui_nForexBridgeConfigurator.h"

N::ForexBridgeConfigurator:: ForexBridgeConfigurator ( QWidget * parent , Plan * p      )
                           : Widget                  (           parent ,        p      )
                           , ui                      ( new Ui::nForexBridgeConfigurator )
                           , Bridges                 ( NULL                             )
                           , connector               ( NULL                             )
                           , dealer                  ( NULL                             )
                           , conf                    ( NULL                             )
{
  Configure ( ) ;
}

N::ForexBridgeConfigurator::~ForexBridgeConfigurator(void)
{
  delete ui ;
}

void N::ForexBridgeConfigurator::resizeEvent(QResizeEvent * e)
{
  QWidget::resizeEvent ( e ) ;
  Relocation           (   ) ;
}

void N::ForexBridgeConfigurator::Configure(void)
{
  ui -> setupUi ( this )                   ;
  Dir = plan -> Path ( Directory::Users  ) ;
  Dir = Dir . absoluteFilePath ( "Forex" ) ;
}

void N::ForexBridgeConfigurator::Relocation(void)
{
  int w = width ( ) - 20                ;
  int h                                 ;
  int x                                 ;
  ///////////////////////////////////////
  h   = ui -> Line -> height (        ) ;
  ui -> Line    -> resize    ( w ,  h ) ;
  h   = height ( ) - 110                ;
  ui -> Stacked -> resize    ( w ,  h ) ;
  ///////////////////////////////////////
  w   = width  ( ) -  40                ;
  w  /= 3                               ;
  ui -> Name       -> resize ( w , 24 ) ;
  ui -> Identifier -> resize ( w , 24 ) ;
  ui -> Modules    -> resize ( w , 24 ) ;
  ///////////////////////////////////////
  x   = 20 + w                          ;
  ui -> Identifier -> move   ( x , 60 ) ;
  x  += 10 + w                          ;
  ui -> Modules    -> move   ( x , 60 ) ;
}

void N::ForexBridgeConfigurator::setDirectory(QDir dir)
{
  Dir = dir ;
}

void N::ForexBridgeConfigurator::List(void)
{
  ui -> Modules -> clear ( )                                                 ;
  if ( NULL == Bridges ) return                                              ;
  ////////////////////////////////////////////////////////////////////////////
  NAMEs Nx                                                                   ;
  for (int i = 0 ; i < Bridges -> ForexConnectors . count ( ) ; i++ )        {
    Forex::ConnectorGui * gui = Bridges -> ForexConnectors [ i ]             ;
    if ( NULL != gui )                                                       {
      Nx [ gui -> type ( ) ] = gui -> name ( )                               ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  UUIDs U = Nx . keys ( )                                                    ;
  SUID  u                                                                    ;
  qSort ( U . begin ( ), U . end ( ) )                                       ;
  foreach ( u , U ) ui -> Modules -> addItem ( Nx [ u ] , u )                ;
}

void N::ForexBridgeConfigurator::ModuleChanged(int index)
{
}

void N::ForexBridgeConfigurator::TextChanged(QString text)
{ Q_UNUSED ( text )                                   ;
  QString name       = ui -> Name       -> text ( )   ;
  QString identifier = ui -> Identifier -> text ( )   ;
  bool    enabled    = true                           ;
  if ( name       . length ( ) <= 0 ) enabled = false ;
  if ( identifier . length ( ) <= 0 ) enabled = false ;
  ui -> Configure -> setEnabled ( enabled )           ;
  if ( NULL != dealer )                               {
    dealer -> Name = name                             ;
  }                                                   ;
}

void N::ForexBridgeConfigurator::Add(void)
{
  if ( NULL == dealer ) return            ;
  ui      -> Add -> setEnabled ( false  ) ;
  Bridges -> add               ( dealer ) ;
  Bridges -> UpdateSettings    ( Dir    ) ;
  emit Update                  (        ) ;
  Cancel                       (        ) ;
}

void N::ForexBridgeConfigurator::Configurator(void)
{
  int     index      = ui -> Modules    -> currentIndex ( )      ;
  QString name       = ui -> Name       -> text         ( )      ;
  QString identifier = ui -> Identifier -> text         ( )      ;
  QString ini        = ""                                        ;
  if ( index                   <  0 ) return                     ;
  if ( name       . length ( ) <= 0 ) return                     ;
  if ( identifier . length ( ) <= 0 ) return                     ;
  ini = QString ( "%1.ini" ) . arg ( identifier )                ;
  ini = Dir . absoluteFilePath     ( ini        )                ;
  if ( index >= Bridges -> ForexConnectors . count ( ) ) return  ;
  index = ui -> Modules -> itemData ( index ) . toInt ( )        ;
  if ( index <  0                                      ) return  ;
  connector = Bridges -> Connector ( index )                     ;
  if ( NULL == connector                               ) return  ;
  dealer    = connector -> dealer       ( NULL                 ) ;
  if ( NULL == dealer                                  ) return  ;
  conf      = connector -> Configurator ( this , plan , dealer ) ;
  if ( NULL == conf                                    ) return  ;
  if ( ui -> Stacked -> count ( ) > 0 )                          {
    for (int i = 0 ; i < ui -> Stacked -> count ( ) ; i++ )      {
      QWidget * w = ui -> Stacked -> widget ( i )                ;
      ui -> Stacked -> removeWidget ( w )                        ;
    }                                                            ;
  }                                                              ;
  ui     -> Stacked    -> addWidget        ( conf  )             ;
  ui     -> Stacked    -> setCurrentWidget ( conf  )             ;
  ui     -> Add        -> setEnabled       ( true  )             ;
  ui     -> Configure  -> setEnabled       ( false )             ;
  ui     -> Modules    -> setEnabled       ( false )             ;
  ui     -> Identifier -> setEnabled       ( false )             ;
  ui     -> Modules    -> setEnabled       ( false )             ;
  dealer -> Name        = name                                   ;
  dealer -> Variables [ "Identifier" ] = identifier              ;
  dealer -> Variables [ "Settings"   ] = ini                     ;
}

void N::ForexBridgeConfigurator::Cancel(void)
{
  QMdiSubWindow * msw = Casting(QMdiSubWindow,parent()) ;
  if ( NotNull ( msw ) )                                {
    msw -> deleteLater ( )                              ;
  } else                                                {
    deleteLater ( )                                     ;
  }                                                     ;
}
