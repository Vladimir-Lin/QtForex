#include <qtforex.h>
#include "ui_SelectChart.h"

N::SelectChart:: SelectChart ( QWidget * parent , Plan * p )
               : Widget      (           parent ,        p )
               , ui          ( new Ui::SelectChart         )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SelectChart::~SelectChart(void)
{
  emit Leave ( this ) ;
  delete ui           ;
}

void N::SelectChart::Configure(void)
{
  ui -> setupUi ( this )                            ;
  ///////////////////////////////////////////////////
  PeriodNames [ Forex::Ticks ] = tr ( "Ticks" )     ;
  PeriodNames [ Forex::M1    ] = tr ( "M1"    )     ;
  PeriodNames [ Forex::M5    ] = tr ( "M5"    )     ;
  PeriodNames [ Forex::M15   ] = tr ( "M15"   )     ;
  PeriodNames [ Forex::M30   ] = tr ( "M30"   )     ;
  PeriodNames [ Forex::H1    ] = tr ( "H1"    )     ;
  PeriodNames [ Forex::H4    ] = tr ( "H4"    )     ;
  PeriodNames [ Forex::D1    ] = tr ( "D1"    )     ;
  PeriodNames [ Forex::W1    ] = tr ( "W1"    )     ;
  PeriodNames [ Forex::MN1   ] = tr ( "MN1"   )     ;
  ///////////////////////////////////////////////////
  nConnect ( this , SIGNAL ( EnableSymbol(bool) )   ,
             this , SLOT   ( SymbolCombo (bool) ) ) ;
}

void N::SelectChart::resizeEvent(QResizeEvent * e)
{
  QWidget::resizeEvent ( e ) ;
  Relocation           (   ) ;
}

void N::SelectChart::showEvent(QShowEvent * e)
{
  QWidget::showEvent   ( e ) ;
  Relocation           (   ) ;
}

bool N::SelectChart::Relocation(void)
{
  QSize R ( width ( ) - 20 , 24 ) ;
  ui -> Level    -> resize ( R )  ;
  ui -> Name     -> resize ( R )  ;
  ui -> Periods  -> resize ( R )  ;
  ui -> Reversal -> resize ( R )  ;
  ui -> Supplier -> resize ( R )  ;
  ui -> Symbols  -> resize ( R )  ;
  return true                     ;
}

void N::SelectChart::run(int T,ThreadData * d)
{
  nDropOut   ( ! IsContinue ( d ) ) ;
  ItemRunner ( T , d              ) ;
}

bool N::SelectChart::ItemRunner(int T,ThreadData * d)
{
  switch           ( T ) {
    case 10001           :
      startLoading (   ) ;
      Load         ( d ) ;
      stopLoading  (   ) ;
    return true          ;
    case 10002           :
      startLoading (   ) ;
      LoadSymbols  ( d ) ;
      stopLoading  (   ) ;
    return true          ;
  }                      ;
  return false           ;
}

bool N::SelectChart::Load(ThreadData * d)
{
  ui -> Supplier -> blockSignals ( true  )                            ;
  ui -> Level    -> blockSignals ( true  )                            ;
  ui -> Symbols  -> blockSignals ( true  )                            ;
  ui -> Periods  -> blockSignals ( true  )                            ;
  ui -> Supplier -> clear        (       )                            ;
  ui -> Symbols  -> clear        (       )                            ;
  ui -> Periods  -> clear        (       )                            ;
  /////////////////////////////////////////////////////////////////////
  CUIDs IDs = PeriodNames . keys ( )                                  ;
  int   id                                                            ;
  foreach ( id , IDs )                                                {
    ui -> Periods -> addItem ( PeriodNames [ id ] , id )              ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  for (int i=0;i<26;i++)                                              {
    QString L = QString::number ( i )                                 ;
    ui -> Level -> addItem ( L , i )                                  ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  SUID puid = 0                                                       ;
  plan -> settings . beginGroup ( "Forex" )                           ;
  if ( plan -> settings . contains ( "Providers" ) )                  {
    puid = plan -> settings . value ( "Providers" ) . toULongLong ( ) ;
  }                                                                   ;
  plan -> settings . endGroup   (         )                           ;
  /////////////////////////////////////////////////////////////////////
  SqlConnection  SC ( plan -> sql )                                   ;
  FinanceManager FM ( plan        )                                   ;
  GroupItems     GI ( plan        )                                   ;
  if ( SC . open ( FunctionString ) )                                 {
    UUIDs U                                                           ;
    SUID  u                                                           ;
    GI . AutoMap    = true                                            ;
    GI . GroupTable = GI . LookTable                                  (
                        Types::Division                               ,
                        Types::Organization                           ,
                        Groups::Subordination                       ) ;
    U = GI . Subordination                                            (
          SC                                                          ,
          puid                                                        ,
          Types::Division                                             ,
          Types::Organization                                         ,
          Groups::Subordination                                       ,
          SC . OrderByAsc ( "position" )                            ) ;
    foreach ( u , U )                                                 {
      QString id = FM . FetchIdentifier ( SC , u )                    ;
      ui -> Supplier -> addItem ( id , u )                            ;
    }                                                                 ;
    SC . close  ( )                                                   ;
  }                                                                   ;
  SC   . remove ( )                                                   ;
  /////////////////////////////////////////////////////////////////////
  ui -> Periods  -> blockSignals ( false )                            ;
  ui -> Symbols  -> blockSignals ( false )                            ;
  ui -> Supplier -> blockSignals ( false )                            ;
  ui -> Level    -> blockSignals ( false )                            ;
  /////////////////////////////////////////////////////////////////////
  return true                                                         ;
}

bool N::SelectChart::LoadSymbols(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 3 ) return false                         ;
  QString supplier = d -> Arguments [ 0 ] . toString    ( )                  ;
  SUID    souid    = d -> Arguments [ 1 ] . toULongLong ( )                  ;
  int     level    = d -> Arguments [ 2 ] . toInt       ( )                  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( supplier . length ( ) <= 0 ) return false                             ;
  ui -> Symbols -> blockSignals ( true  )                                    ;
  ui -> Symbols -> clear        (       )                                    ;
  emit EnableSymbol             ( false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  SqlConnection  SC ( plan -> sql )                                          ;
  if ( SC . open ( FunctionString , "Normal" ) )                             {
    QStringList     Symbols                                                  ;
    Forex::Importer src                                                      ;
    src . identifier = supplier                                              ;
    src . Supplier   = souid                                                 ;
    src . Level      = level                                                 ;
    if ( src . GetSymbols ( SC                                               ,
                            PlanTable(ForexCommodities)                      ,
                            souid                                            ,
                            level                                            ,
                            Symbols                                      ) ) {
      QString n                                                              ;
      foreach ( n , Symbols ) ui -> Symbols -> addItem ( n )                 ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  emit EnableSymbol ( ui -> Symbols -> count ( ) > 0 )                       ;
  ui -> Symbols -> blockSignals ( false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::SelectChart::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::SelectChart::SymbolCombo(bool e)
{
  ui -> Symbols -> setEnabled ( e ) ;
}

void N::SelectChart::StateChanged(int)
{
  NameChanged ( "" ) ;
}

void N::SelectChart::NameChanged(QString name)
{
  bool enabling = true                           ;
  int  idx                                       ;
  name = ui -> Name -> text ( )                  ;
  if ( name . length ( ) <= 0 ) enabling = false ;
  idx = ui -> Level    -> currentIndex ( )       ;
  if ( idx < 0                ) enabling = false ;
  idx = ui -> Supplier -> currentIndex ( )       ;
  if ( idx < 0                ) enabling = false ;
  idx = ui -> Periods  -> currentIndex ( )       ;
  if ( idx < 0                ) enabling = false ;
  idx = ui -> Symbols  -> currentIndex ( )       ;
  if ( idx < 0                ) enabling = false ;
  ui -> Add -> setEnabled ( enabling )           ;
}

void N::SelectChart::Changed(int id)
{
  SUID ouid                                                                  ;
  int  level                                                                 ;
  int  idx                                                                   ;
  idx = ui -> Level      -> currentIndex ( )                                 ;
  if ( idx < 0 ) return                                                      ;
  level = ui -> Level    -> itemData ( idx , Qt::UserRole ) . toInt       () ;
  idx   = ui -> Supplier -> currentIndex ( )                                 ;
  if ( idx < 0 ) return                                                      ;
  ouid  = ui -> Supplier -> itemData ( idx , Qt::UserRole ) . toULongLong () ;
  QString name = ui -> Supplier -> currentText ( )                           ;
  VarArgs args                                                               ;
  args << name                                                               ;
  args << ouid                                                               ;
  args << level                                                              ;
  start       ( 10002 , args )                                               ;
  NameChanged ( ""           )                                               ;
}

QString N::SelectChart::ChartName(void)
{
  return ui -> Name -> text ( ) ;
}

bool N::SelectChart::isReversal(void)
{
  return ui -> Reversal -> isChecked   ( ) ;
}

QString N::SelectChart::Supplier(void)
{
  return ui -> Supplier -> currentText ( ) ;
}

int N::SelectChart::Level(void)
{
  int idx = ui -> Level -> currentIndex ( )                         ;
  if ( idx < 0 ) return -1                                          ;
  return ui -> Level -> itemData ( idx , Qt::UserRole ) . toInt ( ) ;
}

QString N::SelectChart::Symbol(void)
{
  return ui -> Symbols  -> currentText ( ) ;
}

int N::SelectChart::Period(void)
{
  int idx = ui -> Periods -> currentIndex ( )                         ;
  if ( idx < 0 ) return -1                                            ;
  return ui -> Periods -> itemData ( idx , Qt::UserRole ) . toInt ( ) ;
}

void N::SelectChart::Add(void)
{
  emit AddChart    ( this ) ;
}

void N::SelectChart::Cancel(void)
{
  emit CancelChart ( this ) ;
}
