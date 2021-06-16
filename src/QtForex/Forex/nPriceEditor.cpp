#include <qtforex.h>

N::PriceEditor:: PriceEditor ( QWidget * parent , Plan * p )
               : TableWidget (           parent ,        p )
               , level       ( 3                           )
               , period      ( Forex::M1                   )
               , reversal    ( false                       )
               , editing     ( false                       )
               , utc         ( false                       )
{
  Configure ( ) ;
}

N::PriceEditor::~PriceEditor(void)
{
}

void N::PriceEditor::Configure(void)
{
  setAccessibleName ( "N::PriceEditor"    )                     ;
  setObjectName     ( "N::PriceEditor"    )                     ;
  setSuggestion     ( QSize ( 640 , 480 ) )                     ;
  setEditTriggers   ( NoEditTriggers      )                     ;
  InstallMsgs       (                     )                     ;
  InstallIcons      (                     )                     ;
  plan -> setFont   ( this                )                     ;
  ///////////////////////////////////////////////////////////////
  PeriodNames  [ "TICKS"      ] = Forex::Ticks                  ;
  PeriodNames  [ "M1"         ] = Forex::M1                     ;
  PeriodNames  [ "M5"         ] = Forex::M5                     ;
  PeriodNames  [ "M15"        ] = Forex::M15                    ;
  PeriodNames  [ "M30"        ] = Forex::M30                    ;
  PeriodNames  [ "H1"         ] = Forex::H1                     ;
  PeriodNames  [ "H4"         ] = Forex::H4                     ;
  PeriodNames  [ "D1"         ] = Forex::D1                     ;
  PeriodNames  [ "W1"         ] = Forex::W1                     ;
  PeriodNames  [ "MN1"        ] = Forex::MN1                    ;
  ///////////////////////////////////////////////////////////////
  PeriodLimits [ Forex::Ticks ] = 86400 * 7                     ;
  PeriodLimits [ Forex::M1    ] = 86400 * 100                   ;
  PeriodLimits [ Forex::M5    ] = 86400 * 183                   ;
  PeriodLimits [ Forex::M15   ] = 86400 * 366                   ;
  PeriodLimits [ Forex::M30   ] = 86400 * 731                   ;
  PeriodLimits [ Forex::H1    ] = 86400 * 1461                  ;
  PeriodLimits [ Forex::H4    ] = 86400 * 2922                  ;
  PeriodLimits [ Forex::D1    ] = 86400 * 7310                  ;
  PeriodLimits [ Forex::W1    ] = 86400 * 7310                  ;
  PeriodLimits [ Forex::MN1   ] = 86400 * 7310                  ;
  ///////////////////////////////////////////////////////////////
  addConnector ( "Cell"    , SIGNAL ( cellChanged (int,int) )   ,
                             SLOT   ( dataChanged (int,int) ) ) ;
  addConnector ( "Ticks"   , SIGNAL ( LabelTicks  (       ) )   ,
                             SLOT   ( ShowTicks   (       ) ) ) ;
  addConnector ( "Periods" , SIGNAL ( LabelPeriod (       ) )   ,
                             SLOT   ( ShowPeriod  (       ) ) ) ;
  ///////////////////////////////////////////////////////////////
  onlyConnector ( "Cell"    )                                   ;
  onlyConnector ( "Ticks"   )                                   ;
  onlyConnector ( "Periods" )                                   ;
}

void N::PriceEditor::InstallMsgs(void)
{
  TicksLabels  . clear ( )        ;
  PeriodLabels . clear ( )        ;
  /////////////////////////////////
  TicksLabels  << tr ( "ID"     ) ;
  TicksLabels  << tr ( "Time"   ) ;
  TicksLabels  << tr ( "Bid"    ) ;
  TicksLabels  << tr ( "Volume" ) ;
  TicksLabels  << tr ( "Ask"    ) ;
  TicksLabels  << tr ( "Volume" ) ;
  /////////////////////////////////
  PeriodLabels << tr ( "ID"     ) ;
  PeriodLabels << tr ( "Time"   ) ;
  PeriodLabels << tr ( "Open"   ) ;
  PeriodLabels << tr ( "High"   ) ;
  PeriodLabels << tr ( "Low"    ) ;
  PeriodLabels << tr ( "Close"  ) ;
  PeriodLabels << tr ( "Volume" ) ;
}

void N::PriceEditor::InstallIcons(void)
{
  LocalIcons [ "Update" ] = QIcon ( ":/images/update.png" ) ;
}

bool N::PriceEditor::FocusIn(void)
{
  nKickOut          ( IsNull ( plan ) , false           ) ;
  DisableAllActions (                                   ) ;
  AssignAction      ( Label           , windowTitle ( ) ) ;
  /////////////////////////////////////////////////////////
  LinkAction        ( Refresh         , Refresh     ( ) ) ;
  LinkAction        ( PreviousPage    , PageUp      ( ) ) ;
  LinkAction        ( NextPage        , PageDown    ( ) ) ;
  LinkAction        ( Export          , Export      ( ) ) ;
  /////////////////////////////////////////////////////////
  return true                                             ;
}

void N::PriceEditor::ShowTicks(void)
{
  for (int i = 0 ; i < TicksLabels . count ( ) ; i++ ) {
    QTableWidgetItem * it = new QTableWidgetItem ( )   ;
    it -> setText           ( TicksLabels [ i ] )      ;
    setHorizontalHeaderItem ( i , it            )      ;
  }                                                    ;
}

void N::PriceEditor::ShowPeriod(void)
{
  for (int i = 0 ; i < PeriodLabels . count ( ) ; i++ ) {
    QTableWidgetItem * it = new QTableWidgetItem ( )    ;
    it -> setText           ( PeriodLabels [ i ] )      ;
    setHorizontalHeaderItem ( i , it             )      ;
  }                                                     ;
}

QString N::PriceEditor::VTS(int value)
{
  QString V = QString::number ( value ) ;
  int     L = V . length ( )            ;
  if ( L > digits )                     {
    V   . insert  ( L - digits , "."  ) ;
  } else                                {
    while ( V . length ( ) < digits )   {
      V . prepend ( "0"               ) ;
    }                                   ;
    V   . prepend ( "0."              ) ;
  }                                     ;
  return V                              ;
}

void N::PriceEditor::setCellValue(int row,int column,int value,bool editable)
{
  QTableWidgetItem * it = new QTableWidgetItem (                      ) ;
  QString            v  = VTS                  ( value                ) ;
  Qt::ItemFlags      FF = Qt::ItemIsSelectable | Qt::ItemIsEnabled      ;
  if ( editable ) FF |= Qt::ItemIsEditable                              ;
  it -> setText                                ( v                    ) ;
  it -> setTextAlignment                       ( Qt::AlignRight         |
                                                 Qt::AlignVCenter     ) ;
  it -> setFlags                               ( FF                   ) ;
  it -> setData                                ( Qt::UserRole , value ) ;
  setItem                                      ( row , column , it    ) ;
}

void N::PriceEditor::setCellValue(int row,int column,TUID value,bool editable)
{
  QTableWidgetItem * it = new QTableWidgetItem (                      ) ;
  QString            v  = QString::number      ( value                ) ;
  Qt::ItemFlags      FF = Qt::ItemIsSelectable | Qt::ItemIsEnabled      ;
  if ( editable ) FF |= Qt::ItemIsEditable                              ;
  it -> setText                                ( v                    ) ;
  it -> setTextAlignment                       ( Qt::AlignRight         |
                                                 Qt::AlignVCenter     ) ;
  it -> setFlags                               ( FF                   ) ;
  it -> setData                                ( Qt::UserRole , value ) ;
  setItem                                      ( row , column , it    ) ;
}

void N::PriceEditor::setCellTime(int row,int column,TUID value,bool editable)
{
  QTableWidgetItem * it = new QTableWidgetItem  (                          ) ;
  time_t             tt = (time_t)              ( value / 1000             ) ;
  int                ms = (int)                 ( value % 1000             ) ;
  QDateTime          TT = QDateTime::fromTime_t ( tt                       ) ;
  Qt::ItemFlags      FF = Qt::ItemIsSelectable | Qt::ItemIsEnabled           ;
  QString            v                                                       ;
  if ( editable ) FF |= Qt::ItemIsEditable                                   ;
  TT  = TT . addMSecs                           ( ms                       ) ;
  if ( utc ) TT = TT . toUTC                    (                          ) ;
  v   = TT . toString                           ( "yyyy/MM/dd hh:mm:ss.zzz") ;
  it -> setText                                 ( v                        ) ;
  it -> setTextAlignment                        ( Qt::AlignLeft              |
                                                  Qt::AlignVCenter         ) ;
  it -> setFlags                                ( FF                       ) ;
  it -> setData                                 ( Qt::UserRole , value     ) ;
  setItem                                       ( row , column , it        ) ;
}

bool N::PriceEditor::UpdateMonetary(void)
{
  bool correct = false                                                  ;
  SqlConnection SC       ( plan -> sql                                ) ;
  if                     ( SC . open ( FunctionString )               ) {
    monetary . load      ( SC , PlanTable(Currency)                   ) ;
    monetary . loadName  ( SC , PlanTable(Names) , plan -> LanguageId ) ;
    monetary . loadPairs ( SC , PlanTable(CurrencyPairs)              ) ;
    SC . close           (                                            ) ;
  }                                                                     ;
  SC   . remove          (                                            ) ;
  return correct                                                        ;
}

void N::PriceEditor::run(int T,ThreadData * d)
{
  ItemRunner ( T , d ) ;
}

bool N::PriceEditor::ItemRunner(int T,ThreadData * d)
{
  switch           ( T ) {
    case 10001           :
      startLoading (   ) ;
      Load         ( d ) ;
      stopLoading  (   ) ;
    return true          ;
    case 10002           :
      startLoading (   ) ;
      LoadTicks    ( d ) ;
      stopLoading  (   ) ;
    return true          ;
    case 10003           :
      startLoading (   ) ;
      LoadPeriod   ( d ) ;
      stopLoading  (   ) ;
    return true          ;
    case 10004           :
      startLoading (   ) ;
      Update       ( d ) ;
      stopLoading  (   ) ;
    return true          ;
    case 10011           :
      startLoading (   ) ;
      GetMonetary  ( d ) ;
      stopLoading  (   ) ;
    return true          ;
  }                      ;
  return false           ;
}

void N::PriceEditor::GetMonetary(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d ) )                                       ;
  ///////////////////////////////////////////////////////////////////////
  DataManipulator * manipulator = NULL                                  ;
  if ( plan -> Manipulators . contains ( "FOREX" ) )                    {
    manipulator = plan -> Manipulators [ "FOREX" ]                      ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  if ( IsNull ( manipulator ) )                                         {
    UpdateMonetary ( )                                                  ;
  } else                                                                {
    if ( manipulator -> Contains ( "Forex" , "ISO4217" ) )              {
      char * m = (char *) manipulator -> Memory ( "Forex" , "ISO4217" ) ;
      int    s =          manipulator -> Size   ( "Forex" , "ISO4217" ) ;
      if ( manipulator -> isCreated ( "Forex" , "ISO4217" ) )           {
        ::memset                 ( m , s , 0                          ) ;
        UpdateMonetary ( )                                              ;
        monetary . ToByteArray   ( (void *) m , s                     ) ;
      } else                                                            {
        monetary . FromByteArray ( (void *) m , s                     ) ;
      }                                                                 ;
    } else                                                              {
      UpdateMonetary ( )                                                ;
    }                                                                   ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  nDropOut ( d -> Arguments . count ( ) <= 0  )                         ;
  start    ( d -> Arguments [ 0 ] . toInt ( ) )                         ;
}

void N::PriceEditor::TickValues(SqlConnection & SC,SqlConnection & XC,int row)
{ Q_UNUSED               ( SC                       ) ;
  TUID id    = XC . Tuid ( 0                        ) ;
  TUID quote = XC . Tuid ( 1                        ) ;
  int  bid   = XC . Int  ( 2                        ) ;
  int  ask   = XC . Int  ( 3                        ) ;
  TUID bidv  = XC . Tuid ( 4                        ) ;
  TUID askv  = XC . Tuid ( 5                        ) ;
  /////////////////////////////////////////////////////
  setCellValue           ( row , 0 , id     , false ) ;
  setCellTime            ( row , 1 , quote  , false ) ;
  setCellValue           ( row , 2 , bid    , true  ) ;
  setCellValue           ( row , 3 , bidv   , true  ) ;
  setCellValue           ( row , 4 , ask    , true  ) ;
  setCellValue           ( row , 5 , askv   , true  ) ;
}

void N::PriceEditor::PeriodValues(SqlConnection & SC,SqlConnection & XC,int row)
{ Q_UNUSED                ( SC                       ) ;
  TUID id     = XC . Tuid ( 0                        ) ;
  TUID quote  = XC . Tuid ( 1                        ) ;
  int  open   = XC . Int  ( 2                        ) ;
  int  high   = XC . Int  ( 3                        ) ;
  int  low    = XC . Int  ( 4                        ) ;
  int  close  = XC . Int  ( 5                        ) ;
  TUID volume = XC . Tuid ( 6                        ) ;
  //////////////////////////////////////////////////////
  setCellValue            ( row , 0 , id     , false ) ;
  setCellTime             ( row , 1 , quote  , false ) ;
  setCellValue            ( row , 2 , open   , true  ) ;
  setCellValue            ( row , 3 , high   , true  ) ;
  setCellValue            ( row , 4 , low    , true  ) ;
  setCellValue            ( row , 5 , close  , true  ) ;
  setCellValue            ( row , 6 , volume , true  ) ;
}

void N::PriceEditor::LoadTicks(ThreadData * d)
{
  nDropOut            ( ! IsContinue ( d )      )                            ;
  undoConnector       ( "Cell"                  )                            ;
  assignColumns       ( TicksLabels . count ( ) )                            ;
  assignRows          ( 0                       )                            ;
  emit LabelTicks     (                         )                            ;
  ////////////////////////////////////////////////////////////////////////////
  emit assignEnabling ( false                   )                            ;
  while               ( isEnabled ( )           )                            {
    Time::msleep      ( 10                      )                            ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Forex::Importer imp                                                        ;
  SqlConnection   SC ( plan -> sql )                                         ;
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    SUID    provider = 0                                                     ;
    bool    correct  = false                                                 ;
    //////////////////////////////////////////////////////////////////////////
    provider = imp . FetchProvider ( SC                                      ,
                                     PlanTable(Identifiers)                  ,
                                     supplier                              ) ;
    //////////////////////////////////////////////////////////////////////////
    if ( provider > 0 )                                                      {
      QStringList ss                                                         ;
      correct = imp . GetSymbols                                             (
                  SC                                                         ,
                  PlanTable(ForexCommodities)                                ,
                  provider                                                   ,
                  level                                                      ,
                  ss                                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        correct = ss . contains ( symbol )                                   ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        imp . identifier = supplier                                          ;
        imp . symbol     = original                                          ;
        imp . direction  = reversal                                          ;
        correct = imp . Get                                                  (
                    SC                                                       ,
                    PlanTable(ForexCommodities)                              ,
                    provider                                                 ,
                    level                                                    ,
                    symbol                                                 ) ;
        if ( correct )                                                       {
          correct = imp . GetConnexion                                       (
                      SC                                                     ,
                      PlanTable(SqlConnections)                            ) ;
          if ( correct )                                                     {
            SqlConnection XC ( imp . Connexion )                             ;
            if ( XC . open ( FunctionString ) )                              {
              QDateTime st    = range . L                                    ;
              QDateTime tt    = range . R                                    ;
              QString   order = XC . OrderByAsc ( "`quotetime`" )            ;
              QString   items                                                ;
              QString   where                                                ;
              QString   table                                                ;
              TUID      stime = st . toTime_t ( )                            ;
              TUID      etime = tt . toTime_t ( )                            ;
              QString   title                                                ;
              ////////////////////////////////////////////////////////////////
              title = QString ( "%1 (%2)/%3 [ %4 , %5 ]"                     )
                      . arg   ( supplier                                     )
                      . arg   ( base                                         )
                      . arg   ( imp . PeriodName ( period )                  )
                      . arg   ( st . toString ( "yyyy/MM/dd hh:mm:ss" )      )
                      . arg   ( tt . toString ( "yyyy/MM/dd hh:mm:ss" )    ) ;
              emit assignWindowTitle ( title )                               ;
              ////////////////////////////////////////////////////////////////
              items  = "`id`,`quotetime`,`bidprice`,`askprice`,`bidvolume`,`askvolume`" ;
              st     = QDateTime::fromTime_t ( stime , Qt::UTC )             ;
              tt     = QDateTime::fromTime_t ( etime , Qt::UTC )             ;
              stime  = st . toTime_t ( )                                     ;
              etime  = tt . toTime_t ( )                                     ;
              stime *= 1000                                                  ;
              etime *= 1000                                                  ;
              where  = QString ( "where ( `quotetime` >= %1 ) and (`quotetime` <= %2)" )
                       . arg   ( stime                                       )
                       . arg   ( etime                                     ) ;
              imp . symbol     = base                                        ;
              imp . setPeriod  ( period                                    ) ;
              digits = imp . Digits                                          ;
              if ( reversal ) digits = imp . Full - imp . Digits             ;
              ////////////////////////////////////////////////////////////////
              ProgressReporter pr                                            ;
              pr . total = ( etime - stime ) / 1000                          ;
              pr . Progress      ( tr("Load %1 ticks").arg(symbol)           ,
                                   tr("%v/%m seconds")                     ) ;
              pr . Start         (                                         ) ;
              pr . ProgressReady ( 500                                     ) ;
              pr . setRange      ( 0 , pr . total                          ) ;
              pr . setFrequency  ( tr("%1.%2 ticks/s"),tr("%1.%2 s/tick")  ) ;
              do                                                             {
                stime  = st . toTime_t ( )                                   ;
                stime *= 1000                                                ;
                table  = imp . QuoteTableName ( stime )                      ;
                Q      = XC  . sql . SelectFrom                              (
                           items                                             ,
                           table                                             ,
                           where                                             ,
                           order                                           ) ;
                pr . ProgressText ( table )                                  ;
                if ( XC . Query ( Q ) )                                      {
                  int cnt = XC . q . size ( )                                ;
                  if ( cnt > 0 )                                             {
                    int row = rowCount ( )                                   ;
                    assignRows ( row + cnt )                                 ;
                    while ( ( row + cnt ) > rowCount ( ) ) Time::skip(10)    ;
                    while ( XC . Next ( ) )                                  {
                      qint64 tq = XC . Tuid ( 1 )                            ;
                      pr . value = ( tq - stime ) / 1000                     ;
                      TickValues ( SC , XC , row )                           ;
                      row++                                                  ;
                    }                                                        ;
                  }                                                          ;
                }                                                            ;
                st = imp . NextBlock ( period , st , tt )                    ;
              } while ( IsContinue ( d ) && ( st <= tt ) && pr . running )   ;
              pr . Finish ( )                                                ;
              ////////////////////////////////////////////////////////////////
              if ( rowCount ( ) > 0 )                                        {
                Alert ( Done  )                                              ;
              } else                                                         {
                Alert ( Error )                                              ;
              }                                                              ;
              ////////////////////////////////////////////////////////////////
              XC . close  ( )                                                ;
            }                                                                ;
            XC   . remove ( )                                                ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    if ( ! correct ) Alert ( Error )                                         ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  emit assignEnabling ( true                             )                   ;
  emit ColumnsFit     (                                  )                   ;
  Time::msleep        ( 1000                             )                   ;
  onlyConnector       ( "Cell"                           )                   ;
  setEditTriggers     ( QAbstractItemView::DoubleClicked )                   ;
}

void N::PriceEditor::LoadPeriod(ThreadData * d)
{
  nDropOut            ( ! IsContinue ( d )       )                           ;
  undoConnector       ( "Cell"                   )                           ;
  assignColumns       ( PeriodLabels . count ( ) )                           ;
  assignRows          ( 0                        )                           ;
  emit LabelPeriod    (                          )                           ;
  ////////////////////////////////////////////////////////////////////////////
  emit assignEnabling ( false                    )                           ;
  while               ( isEnabled ( )            )                           {
    Time::msleep      ( 10                       )                           ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Forex::Importer imp                                                        ;
  SqlConnection   SC ( plan -> sql )                                         ;
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    SUID    provider = 0                                                     ;
    bool    correct  = false                                                 ;
    //////////////////////////////////////////////////////////////////////////
    provider = imp . FetchProvider ( SC                                      ,
                                     PlanTable(Identifiers)                  ,
                                     supplier                              ) ;
    //////////////////////////////////////////////////////////////////////////
    if ( provider > 0 )                                                      {
      QStringList ss                                                         ;
      correct = imp . GetSymbols                                             (
                  SC                                                         ,
                  PlanTable(ForexCommodities)                                ,
                  provider                                                   ,
                  level                                                      ,
                  ss                                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        correct = ss . contains ( symbol )                                   ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        imp . identifier = supplier                                          ;
        imp . symbol     = original                                          ;
        imp . direction  = reversal                                          ;
        correct = imp . Get                                                  (
                    SC                                                       ,
                    PlanTable(ForexCommodities)                              ,
                    provider                                                 ,
                    level                                                    ,
                    symbol                                                 ) ;
        if ( correct )                                                       {
          correct = imp . GetConnexion                                       (
                      SC                                                     ,
                      PlanTable(SqlConnections)                            ) ;
          if ( correct )                                                     {
            SqlConnection XC ( imp . Connexion )                             ;
            if ( XC . open ( FunctionString ) )                              {
              QDateTime st    = range . L                                    ;
              QDateTime tt    = range . R                                    ;
              QString   order = XC . OrderByAsc ( "`quote`" )                ;
              QString   items                                                ;
              QString   where                                                ;
              QString   table                                                ;
              TUID      stime = st . toTime_t ( )                            ;
              TUID      etime = tt . toTime_t ( )                            ;
              QString   title                                                ;
              ////////////////////////////////////////////////////////////////
              title = QString ( "%1 (%2)/%3 [ %4 , %5 ]"                     )
                      . arg   ( supplier                                     )
                      . arg   ( base                                         )
                      . arg   ( imp . PeriodName ( period )                  )
                      . arg   ( st . toString ( "yyyy/MM/dd hh:mm:ss" )      )
                      . arg   ( tt . toString ( "yyyy/MM/dd hh:mm:ss" )    ) ;
              emit assignWindowTitle ( title )                               ;
              ////////////////////////////////////////////////////////////////
              items  = "`id`,`quote`,`open`,`high`,`low`,`close`,`volume`"   ;
              st     = QDateTime::fromTime_t ( stime , Qt::UTC )             ;
              tt     = QDateTime::fromTime_t ( etime , Qt::UTC )             ;
              stime  = st . toTime_t ( )                                     ;
              etime  = tt . toTime_t ( )                                     ;
              stime *= 1000                                                  ;
              etime *= 1000                                                  ;
              where  = QString ("where ( `quote` >= %1 ) and (`quote` <= %2)")
                       . arg   ( stime                                       )
                       . arg   ( etime                                     ) ;
              imp . symbol     = base                                        ;
              imp . setPeriod  ( period                                    ) ;
              digits = imp . Digits                                          ;
              if ( reversal ) digits = imp . Full - imp . Digits             ;
              ////////////////////////////////////////////////////////////////
              ProgressReporter pr                                            ;
              pr . total = ( etime - stime ) / 1000                          ;
              pr . Progress      ( tr("Load quotes %1").arg(symbol)          ,
                                   tr("%v/%m seconds")                     ) ;
              pr . Start         (                                         ) ;
              pr . ProgressReady ( 500                                     ) ;
              pr . setRange      ( 0 , pr . total                          ) ;
              pr . setFrequency  ( tr("%1.%2 quotes/s"),tr("%1.%2 s/quote")) ;
              do                                                             {
                stime  = st . toTime_t ( )                                   ;
                stime *= 1000                                                ;
                table  = imp . QuoteTableName ( stime )                      ;
                Q      = XC  . sql . SelectFrom                              (
                           items                                             ,
                           table                                             ,
                           where                                             ,
                           order                                           ) ;
                pr . ProgressText ( table )                                  ;
                if ( XC . Query ( Q ) )                                      {
                  int cnt = XC . q . size ( )                                ;
                  if ( cnt > 0 )                                             {
                    int row = rowCount ( )                                   ;
                    assignRows ( row + cnt )                                 ;
                    while ( ( row + cnt ) > rowCount ( ) ) Time::skip(10)    ;
                    while ( XC . Next ( ) )                                  {
                      qint64 tq = XC . Tuid ( 1 )                            ;
                      pr . value = ( tq - stime ) / 1000                     ;
                      PeriodValues ( SC , XC , row )                         ;
                      row++                                                  ;
                    }                                                        ;
                  }                                                          ;
                }                                                            ;
                st = imp . NextBlock ( period , st , tt )                    ;
              } while ( IsContinue ( d ) && ( st <= tt ) && pr . running )   ;
              pr . Finish ( )                                                ;
              ////////////////////////////////////////////////////////////////
              if ( rowCount ( ) > 0 )                                        {
                Alert ( Done  )                                              ;
              } else                                                         {
                Alert ( Error )                                              ;
              }                                                              ;
              ////////////////////////////////////////////////////////////////
              XC . close  ( )                                                ;
            }                                                                ;
            XC   . remove ( )                                                ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    if ( ! correct ) Alert ( Error )                                         ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  emit assignEnabling ( true                             )                   ;
  emit ColumnsFit     (                                  )                   ;
  Time::msleep        ( 1000                             )                   ;
  onlyConnector       ( "Cell"                           )                   ;
  setEditTriggers     ( QAbstractItemView::DoubleClicked )                   ;
}

void N::PriceEditor::Load(ThreadData * d)
{
  nDropOut     ( ! IsContinue ( d )        ) ;
  TUID dt = PeriodLimits [ period ]          ;
  TUID xt = range . L . secsTo ( range . R ) ;
  if           ( xt > dt                   ) {
    range . R = range . L                    ;
    range . R = range . R . addSecs ( dt )   ;
  }                                          ;
  if           ( Forex::Ticks == period    ) {
    LoadTicks  ( d                         ) ;
  } else                                     {
    LoadPeriod ( d                         ) ;
  }                                          ;
}

void N::PriceEditor::UpdateTick(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d ) )                                            ;
  ////////////////////////////////////////////////////////////////////////////
  if ( reversal                       ) return                               ;
  if ( d -> Arguments . count ( ) < 2 ) return                               ;
  int r = d -> Arguments [ 0 ] . toInt ( )                                   ;
  if ( r < 0                          ) return                               ;
  ////////////////////////////////////////////////////////////////////////////
  QTableWidgetItem * twi                                                     ;
  TUID               id                                                      ;
  TUID               quote                                                   ;
  int                bid                                                     ;
  int                ask                                                     ;
  TUID               bidv                                                    ;
  TUID               askv                                                    ;
  int                dbid                                                    ;
  int                dask                                                    ;
  TUID               dbidv                                                   ;
  TUID               daskv                                                   ;
  bool               changed = false                                         ;
  int                m       = 1                                             ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; i < digits ; i++ ) m *= 10                                ;
  ////////////////////////////////////////////////////////////////////////////
  twi   = item ( r , 0 )                                                     ;
  id    = twi -> data ( Qt::UserRole ) . toLongLong ( )                      ;
  twi   = item ( r , 1 )                                                     ;
  quote = twi -> data ( Qt::UserRole ) . toLongLong ( )                      ;
  twi   = item ( r , 2 )                                                     ;
  bid   = twi -> data ( Qt::UserRole ) . toInt      ( )                      ;
  twi   = item ( r , 3 )                                                     ;
  bidv  = twi -> data ( Qt::UserRole ) . toLongLong ( )                      ;
  twi   = item ( r , 4 )                                                     ;
  ask   = twi -> data ( Qt::UserRole ) . toInt      ( )                      ;
  twi   = item ( r , 5 )                                                     ;
  askv  = twi -> data ( Qt::UserRole ) . toLongLong ( )                      ;
  ////////////////////////////////////////////////////////////////////////////
  twi   = item ( r , 2 )                                                     ;
  dbid  = twi -> text ( ) . toDouble   ( ) * m                               ;
  twi   = item ( r , 3 )                                                     ;
  dbidv = twi -> text ( ) . toLongLong ( )                                   ;
  twi   = item ( r , 4 )                                                     ;
  dask  = twi -> text ( ) . toDouble   ( ) * m                               ;
  twi   = item ( r , 5 )                                                     ;
  daskv = twi -> text ( ) . toLongLong ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( dbid  != bid  ) changed = true                                        ;
  if ( dask  != ask  ) changed = true                                        ;
  if ( dbidv != bidv ) changed = true                                        ;
  if ( daskv != askv ) changed = true                                        ;
  if ( ! changed     ) return                                                ;
  ////////////////////////////////////////////////////////////////////////////
  Forex::Importer imp                                                        ;
  SqlConnection   SC ( plan -> sql )                                         ;
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    SUID    provider = 0                                                     ;
    bool    correct  = false                                                 ;
    //////////////////////////////////////////////////////////////////////////
    provider = imp . FetchProvider ( SC                                      ,
                                     PlanTable(Identifiers)                  ,
                                     supplier                              ) ;
    //////////////////////////////////////////////////////////////////////////
    if ( provider > 0 )                                                      {
      QStringList ss                                                         ;
      correct = imp . GetSymbols                                             (
                  SC                                                         ,
                  PlanTable(ForexCommodities)                                ,
                  provider                                                   ,
                  level                                                      ,
                  ss                                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        correct = ss . contains ( symbol )                                   ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        imp . identifier = supplier                                          ;
        imp . symbol     = original                                          ;
        imp . direction  = reversal                                          ;
        correct = imp . Get                                                  (
                    SC                                                       ,
                    PlanTable(ForexCommodities)                              ,
                    provider                                                 ,
                    level                                                    ,
                    symbol                                                 ) ;
        if ( correct )                                                       {
          correct = imp . GetConnexion                                       (
                      SC                                                     ,
                      PlanTable(SqlConnections)                            ) ;
          if ( correct )                                                     {
            SqlConnection XC ( imp . Connexion )                             ;
            if ( XC . open ( FunctionString ) )                              {
              ////////////////////////////////////////////////////////////////
              QString table                                                  ;
              ////////////////////////////////////////////////////////////////
              imp . symbol     = base                                        ;
              imp . setPeriod      ( period               )                  ;
              ////////////////////////////////////////////////////////////////
              table = imp . QuoteTableName ( quote )                         ;
              Q = XC . sql . Update                                          (
                    table                                                    ,
                    XC . sql . Where ( 1 , "id" )                            ,
                    4                                                        ,
                    "bidprice"                                               ,
                    "askprice"                                               ,
                    "bidvolume"                                              ,
                    "askvolume"                                            ) ;
              XC . Prepare         ( Q                    )                  ;
              XC . Bind            ( "id"        , id     )                  ;
              XC . Bind            ( "bidprice"  , dbid   )                  ;
              XC . Bind            ( "askprice"  , dask   )                  ;
              XC . Bind            ( "bidvolume" , dbidv  )                  ;
              XC . Bind            ( "askvolume" , daskv  )                  ;
              if                   ( XC . Exec ( )        )                  {
                QString em                                                   ;
                twi  = item        ( r            , 1     )                  ;
                em   = twi -> text (                      )                  ;
                em   = tr("%1 updated") . arg ( em )                         ;
                twi  = item        ( r            , 2     )                  ;
                twi -> setData     ( Qt::UserRole , dbid  )                  ;
                twi  = item        ( r            , 3     )                  ;
                twi -> setData     ( Qt::UserRole , dbidv )                  ;
                twi  = item        ( r            , 4     )                  ;
                twi -> setData     ( Qt::UserRole , dask  )                  ;
                twi  = item        ( r            , 5     )                  ;
                twi -> setData     ( Qt::UserRole , daskv )                  ;
                alert              ( "Done"       , em    )                  ;
              } else                                                         {
                QString em                                                   ;
                em = XC . q . lastError ( ) . text ( )                       ;
                alert              ( "Error"      , em    )                  ;
              }                                                              ;
              ////////////////////////////////////////////////////////////////
              XC . close           (                      )                  ;
            }                                                                ;
            XC   . remove          (                      )                  ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    if ( ! correct ) Alert ( Error )                                         ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
}

void N::PriceEditor::UpdatePeriod(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d ) )                                            ;
  ////////////////////////////////////////////////////////////////////////////
  if ( d -> Arguments . count ( ) < 2 ) return                               ;
  int r = d -> Arguments [ 0 ] . toInt ( )                                   ;
  if ( r < 0                          ) return                               ;
  ////////////////////////////////////////////////////////////////////////////
  QTableWidgetItem * twi                                                     ;
  TUID               id                                                      ;
  TUID               quote                                                   ;
  int                popen                                                   ;
  int                phigh                                                   ;
  int                plow                                                    ;
  int                pclose                                                  ;
  TUID               pvolume                                                 ;
  int                dopen                                                   ;
  int                dhigh                                                   ;
  int                dlow                                                    ;
  int                dclose                                                  ;
  TUID               dvolume                                                 ;
  bool               changed = false                                         ;
  int                m       = 1                                             ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; i < digits ; i++ ) m *= 10                                ;
  ////////////////////////////////////////////////////////////////////////////
  twi     = item ( r , 0 )                                                   ;
  id      = twi -> data ( Qt::UserRole ) . toLongLong ( )                    ;
  twi     = item ( r , 1 )                                                   ;
  quote   = twi -> data ( Qt::UserRole ) . toLongLong ( )                    ;
  twi     = item ( r , 2 )                                                   ;
  popen   = twi -> data ( Qt::UserRole ) . toInt      ( )                    ;
  twi     = item ( r , 3 )                                                   ;
  phigh   = twi -> data ( Qt::UserRole ) . toInt      ( )                    ;
  twi     = item ( r , 4 )                                                   ;
  plow    = twi -> data ( Qt::UserRole ) . toInt      ( )                    ;
  twi     = item ( r , 5 )                                                   ;
  pclose  = twi -> data ( Qt::UserRole ) . toInt      ( )                    ;
  twi     = item ( r , 6 )                                                   ;
  pvolume = twi -> data ( Qt::UserRole ) . toLongLong ( )                    ;
  ////////////////////////////////////////////////////////////////////////////
  twi     = item ( r , 2 )                                                   ;
  dopen   = twi -> text ( ) . toDouble   ( ) * m                             ;
  twi     = item ( r , 3 )                                                   ;
  dhigh   = twi -> text ( ) . toDouble   ( ) * m                             ;
  twi     = item ( r , 4 )                                                   ;
  dlow    = twi -> text ( ) . toDouble   ( ) * m                             ;
  twi     = item ( r , 5 )                                                   ;
  dclose  = twi -> text ( ) . toDouble   ( ) * m                             ;
  twi     = item ( r , 6 )                                                   ;
  dvolume = twi -> text ( ) . toLongLong ( )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( popen   != dopen   ) changed = true                                   ;
  if ( phigh   != dhigh   ) changed = true                                   ;
  if ( plow    != dlow    ) changed = true                                   ;
  if ( pclose  != dclose  ) changed = true                                   ;
  if ( pvolume != dvolume ) changed = true                                   ;
  if ( ! changed          ) return                                           ;
  ////////////////////////////////////////////////////////////////////////////
  Forex::Importer imp                                                        ;
  SqlConnection   SC ( plan -> sql )                                         ;
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    SUID    provider = 0                                                     ;
    bool    correct  = false                                                 ;
    //////////////////////////////////////////////////////////////////////////
    provider = imp . FetchProvider ( SC                                      ,
                                     PlanTable(Identifiers)                  ,
                                     supplier                              ) ;
    //////////////////////////////////////////////////////////////////////////
    if ( provider > 0 )                                                      {
      QStringList ss                                                         ;
      correct = imp . GetSymbols                                             (
                  SC                                                         ,
                  PlanTable(ForexCommodities)                                ,
                  provider                                                   ,
                  level                                                      ,
                  ss                                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        correct = ss . contains ( symbol )                                   ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( correct )                                                         {
        imp . identifier = supplier                                          ;
        imp . symbol     = original                                          ;
        imp . direction  = reversal                                          ;
        correct = imp . Get                                                  (
                    SC                                                       ,
                    PlanTable(ForexCommodities)                              ,
                    provider                                                 ,
                    level                                                    ,
                    symbol                                                 ) ;
        if ( correct )                                                       {
          correct = imp . GetConnexion                                       (
                      SC                                                     ,
                      PlanTable(SqlConnections)                            ) ;
          if ( correct )                                                     {
            SqlConnection XC ( imp . Connexion )                             ;
            if ( XC . open ( FunctionString ) )                              {
              QString table                                                  ;
              ////////////////////////////////////////////////////////////////
              imp . symbol     = base                                        ;
              imp . setPeriod      ( period               )                  ;
              ////////////////////////////////////////////////////////////////
              table = imp . QuoteTableName ( quote )                         ;
              Q = XC . sql . Update                                          (
                    table                                                    ,
                    XC . sql . Where ( 1 , "id" )                            ,
                    5                                                        ,
                    "open"                                                   ,
                    "high"                                                   ,
                    "low"                                                    ,
                    "close"                                                  ,
                    "volume"                                               ) ;
              XC . Prepare         ( Q                      )                ;
              XC . Bind            ( "id"     , id          )                ;
              XC . Bind            ( "open"   , dopen       )                ;
              XC . Bind            ( "high"   , dhigh       )                ;
              XC . Bind            ( "low"    , dlow        )                ;
              XC . Bind            ( "close"  , dclose      )                ;
              XC . Bind            ( "volume" , dvolume     )                ;
              if                   ( XC . Exec ( )          )                {
                QString em                                                   ;
                twi  = item        ( r            , 1       )                ;
                em   = twi -> text (                        )                ;
                em   = tr("%1 updated") . arg ( em )                         ;
                twi  = item        ( r            , 2       )                ;
                twi -> setData     ( Qt::UserRole , dopen   )                ;
                twi  = item        ( r            , 3       )                ;
                twi -> setData     ( Qt::UserRole , dhigh   )                ;
                twi  = item        ( r            , 4       )                ;
                twi -> setData     ( Qt::UserRole , dlow    )                ;
                twi  = item        ( r            , 5       )                ;
                twi -> setData     ( Qt::UserRole , dclose  )                ;
                twi  = item        ( r            , 6       )                ;
                twi -> setData     ( Qt::UserRole , dvolume )                ;
                alert              ( "Done"       , em      )                ;
              } else                                                         {
                QString em                                                   ;
                em = XC . q . lastError ( ) . text ( )                       ;
                alert              ( "Error"      , em    )                  ;
              }                                                              ;
              ////////////////////////////////////////////////////////////////
              XC . close  ( )                                                ;
            }                                                                ;
            XC   . remove ( )                                                ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    if ( ! correct ) Alert ( Error )                                         ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
}

void N::PriceEditor::Update(ThreadData * d)
{
  nDropOut       ( ! IsContinue ( d )     ) ;
  nDropOut       ( ! editing              ) ;
  if             ( Forex::Ticks == period ) {
    UpdateTick   ( d                      ) ;
  } else                                    {
    UpdatePeriod ( d                      ) ;
  }                                         ;
}

bool N::PriceEditor::startup(void)
{
  VarArgs args                           ;
  args << 10001                          ;
  plan -> processEvents (              ) ;
  clear                 (              ) ;
  start                 ( 10011 , args ) ;
  return true                            ;
}

bool N::PriceEditor::startup(VarArgs args)
{
  QString    SUPPLIER = args [ 0 ] . toString ( )                  ;
  int        LEVEL    = args [ 1 ] . toInt    ( )                  ;
  QString    SYMBOL   = args [ 2 ] . toString ( )                  ;
  QString    PERIOD   = args [ 3 ] . toString ( )                  ;
  QString    r        = args [ 4 ] . toString ( )                  ;
  bool       reversal = ( "reversal" == r )                        ;
  QDateTime  fromTime                                              ;
  QDateTime  toTime   = nTimeNow                                   ;
  //////////////////////////////////////////////////////////////////
  if ( args . count ( ) > 6 )                                      {
    QString t = args [ 6 ] . toString ( )                          ;
    toTime   = QDateTime::fromString ( t , "yyyy/MM/dd-hh:mm:ss" ) ;
  }                                                                ;
  if ( args . count ( ) > 5 )                                      {
    QString t = args [ 5 ] . toString ( )                          ;
    fromTime = QDateTime::fromString ( t , "yyyy/MM/dd-hh:mm:ss" ) ;
  } else                                                           {
    fromTime = toTime                                              ;
    toTime   = toTime . addSecs ( -86400 )                         ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  setSupplier    ( SUPPLIER , LEVEL  )                             ;
  setReversal    ( reversal          )                             ;
  setPeriod      ( PERIOD            )                             ;
  setSymbol      ( SYMBOL            )                             ;
  setRange       ( fromTime , toTime )                             ;
  return startup (                   )                             ;
}

void N::PriceEditor::Refresh(void)
{
  clear (       ) ;
  start ( 10001 ) ;
}

void N::PriceEditor::PageUp(void)
{
  TUID dt   = range . L . secsTo  ( range . R ) ;
  range . L = range . L . addSecs ( -dt       ) ;
  range . R = range . R . addSecs ( -dt       ) ;
  clear                           (           ) ;
  start                           ( 10001     ) ;
}

void N::PriceEditor::PageDown(void)
{
  TUID dt   = range . L . secsTo  ( range . R ) ;
  range . L = range . L . addSecs ( dt        ) ;
  range . R = range . R . addSecs ( dt        ) ;
  clear                           (           ) ;
  start                           ( 10001     ) ;
}

void N::PriceEditor::setSupplier(QString s,int l)
{
  supplier = s ;
  level    = l ;
}

void N::PriceEditor::setPeriod(QString ps)
{
  period = Forex::M1                   ;
  ps     = ps . toUpper ( )            ;
  if ( PeriodNames . contains ( ps ) ) {
    period = PeriodNames [ ps ]        ;
  }                                    ;
}

void N::PriceEditor::setPeriod(int p)
{
  period = p ;
}

void N::PriceEditor::setReversal(bool r)
{
  reversal = r ;
}

void N::PriceEditor::setRange(QDateTime from,QDateTime to)
{
  range . L = from ;
  range . R = to   ;
}

void N::PriceEditor::setSymbol(QString s)
{
  QString f = s . mid ( 0 , 3 )           ;
  QString e = s . mid ( 3 , 3 )           ;
  symbol   = s                            ;
  original = f + "/" + e                  ;
  if ( reversal )                         {
    base   = e + "/" + f                  ;
  } else                                  {
    base   = f + "/" + e                  ;
  }                                       ;
  base = base . replace ( "CNH" , "CNY" ) ;
}

void N::PriceEditor::dataChanged(int r, int c)
{
  if ( ! editing ) return ;
  VarArgs args            ;
  args << r               ;
  args << c               ;
  start ( 10004 , args )  ;
}

QString N::PriceEditor::fromTick(int r)
{
  QTableWidgetItem * twi   ;
  QString     L            ;
  QStringList S            ;
  QStringList X            ;
  //////////////////////////
  for (int i=1;i<6;i++)    {
    twi = item ( r , i )   ;
    if ( NULL != twi )     {
      S << twi -> text ( ) ;
    } else                 {
      S << ""              ;
    }                      ;
  }                        ;
  //////////////////////////
  X << S [ 0 ]             ;
  X << S [ 1 ]             ;
  X << S [ 2 ]             ;
  X << S [ 3 ]             ;
  X << S [ 4 ]             ;
  //////////////////////////
  L = X . join ( "," )     ;
  return L                 ;
}

QString N::PriceEditor::fromPeriod(int r)
{
  QTableWidgetItem * twi   ;
  QString     L            ;
  QStringList S            ;
  QStringList X            ;
  //////////////////////////
  for (int i=1;i<7;i++)    {
    twi = item ( r , i )   ;
    if ( NULL != twi )     {
      S << twi -> text ( ) ;
    } else                 {
      S << ""              ;
    }                      ;
  }                        ;
  //////////////////////////
  X << S [ 0 ]             ;
  X << S [ 1 ]             ;
  X << S [ 2 ]             ;
  X << S [ 3 ]             ;
  X << S [ 4 ]             ;
  X << S [ 5 ]             ;
  //////////////////////////
  L = X . join ( "," )     ;
  return L                 ;
}

void N::PriceEditor::Export(void)
{
  QString path = plan -> Temporary ( "" )                       ;
  if ( plan -> Variables . contains ( "PriceEditor" ) )         {
    path = plan -> Variables [ "PriceEditor" ] . toString ( )   ;
  }                                                             ;
  path = QFileDialog::getSaveFileName                           (
           this                                                 ,
           tr("Export CSV")                                     ,
           path                                                 ,
           tr("CSV (*.csv)")                                  ) ;
  if ( path . length ( ) <= 0 ) return                          ;
  ///////////////////////////////////////////////////////////////
  QFileInfo FXI ( path )                                        ;
  QString   filename = FXI . absoluteDir ( ) . absolutePath ( ) ;
  plan -> Variables [ "PriceEditor" ] = filename                ;
  filename = path                                               ;
  ///////////////////////////////////////////////////////////////
  if             ( Forex::Ticks == period )                     {
    ExportTicks  ( filename               )                     ;
  } else                                                        {
    ExportPeriod ( filename               )                     ;
  }                                                             ;
}

bool N::PriceEditor::ExportTicks(QString filename)
{
  nKickOut            ( ( rowCount ( ) <= 0 ) , false )                      ;
  QFile F             ( filename                      )                      ;
  if ( ! F . open     ( QIODevice::WriteOnly          ) ) return false       ;
  emit assignEnabling ( false                         )                      ;
  while               ( isEnabled ( )                 ) Time::skip ( 10 )    ;
  ////////////////////////////////////////////////////////////////////////////
  ProgressReporter pr                                                        ;
  pr . total = rowCount ( )                                                  ;
  pr . Progress      ( tr("Export %1 to CSV").arg(base)                      ,
                       tr("%v/%m rows")                                    ) ;
  pr . Start         (                                                     ) ;
  pr . ProgressReady ( 500                                                 ) ;
  pr . setRange      ( 0 , pr . total                                      ) ;
  pr . setFrequency  ( tr("%1.%2 rows/s") , tr("%1.%2 s/row")              ) ;
  for (int i = 0 ; i < rowCount ( ) ; i++ )                                  {
    QString    L = fromTick   ( i                  )                         ;
    QByteArray B                                                             ;
    if                        ( L . length ( ) > 0 )                         {
      L += "\r\n"                                                            ;
      B  = L . toUtf8         (                    )                         ;
      F  . write              ( B                  )                         ;
    }                                                                        ;
    ++ pr                                                                    ;
    plan -> processEvents     (                    )                         ;
  }                                                                          ;
  pr . Finish         (                               )                      ;
  ////////////////////////////////////////////////////////////////////////////
  F . close           (                               )                      ;
  emit assignEnabling ( true                          )                      ;
  return true                                                                ;
}

bool N::PriceEditor::ExportPeriod(QString filename)
{
  nKickOut            ( ( rowCount ( ) <= 0 ) , false )                      ;
  QFile F             ( filename                      )                      ;
  if ( ! F . open     ( QIODevice::WriteOnly          ) ) return false       ;
  emit assignEnabling ( false                         )                      ;
  while               ( isEnabled ( )                 ) Time::skip ( 10 )    ;
  ////////////////////////////////////////////////////////////////////////////
  ProgressReporter pr                                                        ;
  pr . total = rowCount ( )                                                  ;
  pr . Progress      ( tr("Export %1 to CSV").arg(base)                      ,
                       tr("%v/%m rows")                                    ) ;
  pr . Start         (                                                     ) ;
  pr . ProgressReady ( 500                                                 ) ;
  pr . setRange      ( 0 , pr . total                                      ) ;
  pr . setFrequency  ( tr("%1.%2 rows/s") , tr("%1.%2 s/row")              ) ;
  for (int i = 0 ; i < rowCount ( ) ; i++ )                                  {
    QString    L = fromPeriod ( i                  )                         ;
    QByteArray B                                                             ;
    if                        ( L . length ( ) > 0 )                         {
      L += "\r\n"                                                            ;
      B  = L . toUtf8         (                    )                         ;
      F  . write              ( B                  )                         ;
    }                                                                        ;
    ++ pr                                                                    ;
    plan -> processEvents     (                    )                         ;
  }                                                                          ;
  pr . Finish         (                               )                      ;
  ////////////////////////////////////////////////////////////////////////////
  F . close           (                               )                      ;
  emit assignEnabling ( true                          )                      ;
  return true                                                                ;
}

bool N::PriceEditor::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                            ;
  QAction       * aa                                   ;
  QDateTimeEdit * st = new QDateTimeEdit ( )           ;
  QDateTimeEdit * et = new QDateTimeEdit ( )           ;
  //////////////////////////////////////////////////////
  st -> setDisplayFormat     ( "yyyy/MM/dd hh:mm:ss" ) ;
  et -> setDisplayFormat     ( "yyyy/MM/dd hh:mm:ss" ) ;
  st -> setDateTime          ( range . L             ) ;
  et -> setDateTime          ( range . R             ) ;
  //////////////////////////////////////////////////////
  mm . add                   ( 101                     ,
                               LocalIcons["Update"]    ,
                               tr("Refresh")         ) ;
  mm . add                   ( 102 , tr("Export CSV")) ;
  mm . addSeparator          (                       ) ;
  mm . add                   ( 201 , tr("Previous")  ) ;
  mm . add                   ( 202 , tr("Next"    )  ) ;
  //////////////////////////////////////////////////////
  mm . addSeparator          (                       ) ;
  mm . add                   ( 901 , st              ) ;
  mm . add                   ( 902 , et              ) ;
  //////////////////////////////////////////////////////
  mm . addSeparator          (                       ) ;
  mm . add                   ( 301                     ,
                               tr("Editable")          ,
                               true                    ,
                               editing               ) ;
  mm . add                   ( 302                     ,
                               tr("UTC")               ,
                               true                    ,
                               utc                   ) ;
  //////////////////////////////////////////////////////
  mm . setFont               ( plan                  ) ;
  aa = mm . exec             (                       ) ;
  range . L = st -> dateTime (                       ) ;
  range . R = et -> dateTime (                       ) ;
  if                         ( IsNull ( aa )         ) {
    return true                                        ;
  }                                                    ;
  //////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                 {
    case 101                                           :
      Refresh  ( )                                     ;
    break                                              ;
    case 102                                           :
      Export   ( )                                     ;
    break                                              ;
    case 201                                           :
      PageUp   ( )                                     ;
    break                                              ;
    case 202                                           :
      PageDown ( )                                     ;
    break                                              ;
    case 301                                           :
      editing = aa -> isChecked ( )                    ;
    break                                              ;
    case 302                                           :
      utc     = aa -> isChecked ( )                    ;
    break                                              ;
  }                                                    ;
  //////////////////////////////////////////////////////
  return true                                          ;
}
