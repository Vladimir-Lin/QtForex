#include <qtforex.h>

N::SymbolsQuotes:: SymbolsQuotes ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , TimeId        ( -1                          )
                 , TickTree      ( NULL                        )
{
  Configure ( ) ;
}

N::SymbolsQuotes::~SymbolsQuotes (void)
{
  if ( NotNull(TickTree) )                       {
    TickTree -> setEnabled ( "Deletion" , true ) ;
    TickTree = NULL                              ;
  }                                              ;
}

N::TickTreeItems * N::SymbolsQuotes::TickItems(void)
{
  return TickTree ;
}

void N::SymbolsQuotes::Configure(void)
{
  QString css = "QTreeView::item { "
                "background-color: #E1F9C7; "
                "border: 1px solid #E0E0E0; "
                "border-top-color: #E0E0E0; "
                "border-bottom-color: #F0F0F0; "
                "} "
                "QTreeView::item:selected:active { "
                "background-color: #DFDAAA; "
                "color: #007000; "
                "}"                                                   ;
  dockingPlace  = Qt::LeftDockWidgetArea                              ;
  /////////////////////////////////////////////////////////////////////
  setWindowIcon                ( QIcon(":/images/symbolquotes.png") ) ;
  setWindowTitle               ( tr("FOREX Quotes")                 ) ;
  setSuggestion                ( QSize ( 640 , 320 )                ) ;
  setDragDropMode              ( NoDragDrop                         ) ;
  setRootIsDecorated           ( false                              ) ;
  setAlternatingRowColors      ( true                               ) ;
  setSelectionMode             ( ExtendedSelection                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded              ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded              ) ;
  setColumnCount               ( 9                                  ) ;
  setStyleSheet                ( css                                ) ;
  /////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                               ) ;
  head -> setText              ( 0 , tr("Symbol"     )              ) ;
  head -> setText              ( 1 , tr("Bid"        )              ) ;
  head -> setText              ( 2 , tr("Ask"        )              ) ;
  head -> setText              ( 3 , tr("High"       )              ) ;
  head -> setText              ( 4 , tr("Low"        )              ) ;
  head -> setText              ( 5 , tr("Spread"     )              ) ;
  head -> setText              ( 6 , tr("Server Time")              ) ;
  head -> setText              ( 7 , tr("Local Time" )              ) ;
  head -> setText              ( 8 , tr("Ticks"      )              ) ;
  head -> setText              ( 9 , ""                             ) ;
  /////////////////////////////////////////////////////////////////////
  assignHeaderItems            ( head                               ) ;
  /////////////////////////////////////////////////////////////////////
  for (int i = 3 ; i <= 8 ; i++ )                                     {
    setColumnHidden            ( i , true                           ) ;
  }                                                                   ;
  setColumnWidth               ( 9 , 3                              ) ;
  /////////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                                  ) ;
  MountClicked                 ( 2                                  ) ;
  plan -> setFont              ( this                               ) ;
  /////////////////////////////////////////////////////////////////////
  LimitValues [ 100001  ] = 1                                         ;
  LimitValues [ 100002  ] = 1                                         ;
  LimitValues [ 100003  ] = 1                                         ;
  LimitValues [ 100004  ] = 1                                         ;
  LimitValues [ 100005  ] = 1                                         ;
  LimitValues [ 100006  ] = 1                                         ;
  LimitValues [ 100007  ] = 1                                         ;
  LimitValues [ 3121279 ] = 1                                         ;
  TickTree = new TickTreeItems ( this                               ) ;
  TimeId   = startTimer        ( 200                                ) ;
}

void N::SymbolsQuotes::timerEvent(QTimerEvent * e)
{
  QTreeWidget::timerEvent ( e )            ;
  if ( TimeId < 0                 ) return ;
  if ( TimeId != e -> timerId ( ) ) return ;
  Painting ( )                             ;
}

void N::SymbolsQuotes::Painting(void)
{
  if ( IsNull ( TickTree ) ) return                    ;
  if ( ! TickTree -> isEnabled ( "Ready"    ) ) return ;
  if ( ! TickTree -> isEnabled ( "Transmit" ) ) return ;
  TickTree -> Sync   ( this )                          ;
  TickTree -> Update (      )                          ;
  if ( 0 != LimitValues [ 3121279 ] ) emit AutoFit ( ) ;
}

bool N::SymbolsQuotes::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false              ) ;
  DisableAllActions (                                   ) ;
  AssignAction      ( Label        , windowTitle    ( ) ) ;
  LinkAction        ( Insert       , Insert         ( ) ) ;
  LinkAction        ( Delete       , Delete         ( ) ) ;
  LinkAction        ( Refresh      , Refresh        ( ) ) ;
  LinkAction        ( GoUp         , GoUp           ( ) ) ;
  LinkAction        ( GoDown       , GoDown         ( ) ) ;
  LinkAction        ( Load         , LoadSymbols    ( ) ) ;
  LinkAction        ( Save         , SaveSymbols    ( ) ) ;
  LinkAction        ( Import       , LoadStyleSheet ( ) ) ;
  LinkAction        ( Export       , SaveStyleSheet ( ) ) ;
  return true                                             ;
}

bool N::SymbolsQuotes::FocusOut(void)
{
  return true ;
}

void N::SymbolsQuotes::singleClicked (QTreeWidgetItem*,int)
{
  Alert ( Action ) ;
}

void N::SymbolsQuotes::doubleClicked (QTreeWidgetItem * item,int)
{
  Insert ( item ) ;
}

bool N::SymbolsQuotes::Shutdown(void)
{
  return true ;
}

void N::SymbolsQuotes::Insert(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( NULL == it ) return               ;
  Insert ( it )                          ;
}

void N::SymbolsQuotes::Delete(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( NULL == it ) return               ;
  Delete ( it )                          ;
}

void N::SymbolsQuotes::Insert(QTreeWidgetItem * item)
{
  if ( NULL == item ) return                                     ;
  QString identifier = Variables [ "Identifier" ] . toString ( ) ;
  emit NewOrder ( identifier , item )                            ;
}

void N::SymbolsQuotes::Delete(QTreeWidgetItem * item)
{
  if ( NULL == item ) return                                     ;
  int index                                                      ;
  index = indexOfTopLevelItem ( item )                           ;
  if ( index < 0 ) return                                        ;
  QString s = item -> text ( 0 )                                 ;
  takeTopLevelItem ( index )                                     ;
  if ( NotNull(TickTree) )                                       {
    TickTree -> Hide ( s )                                       ;
  }                                                              ;
  QString identifier = Variables [ "Identifier" ] . toString ( ) ;
  emit Remove ( identifier , item )                              ;
  ////////////////////////////////////////////////////////////////
  QTreeWidgetItem * it                                           ;
  it = topLevelItem ( index )                                    ;
  if ( NULL == it ) return                                       ;
  setCurrentItem ( it )                                          ;
}

void N::SymbolsQuotes::ChartWindow(QTreeWidgetItem * item)
{
  if ( NULL == item ) return                                     ;
  QString identifier = Variables [ "Identifier" ] . toString ( ) ;
  emit Chart ( identifier , item )                               ;
}

void N::SymbolsQuotes::TickChart(QTreeWidgetItem * item)
{
  if ( NULL == item ) return                                     ;
  QString identifier = Variables [ "Identifier" ] . toString ( ) ;
  emit Tick ( identifier , item )                                ;
}

void N::SymbolsQuotes::DepthOfMarket(QTreeWidgetItem * item)
{
  if ( NULL == item ) return                                     ;
  QString identifier = Variables [ "Identifier" ] . toString ( ) ;
  emit Market ( identifier , item )                              ;
}

void N::SymbolsQuotes::Refresh(void)
{
  Painting ( ) ;
}

void N::SymbolsQuotes::GoUp(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( NULL == it ) return               ;
  int index                              ;
  index = indexOfTopLevelItem ( it )     ;
  if ( index < 0 ) return                ;
  takeTopLevelItem   ( index           ) ;
  index --                               ;
  insertTopLevelItem ( index , it      ) ;
  setCurrentItem     ( it              ) ;
  Alert              ( Action          ) ;
}

void N::SymbolsQuotes::GoDown(void)
{
  QTreeWidgetItem * it = currentItem ( )               ;
  if ( NULL == it ) return                             ;
  int index                                            ;
  index = indexOfTopLevelItem ( it )                   ;
  if ( index < 0 ) return                              ;
  if ( ( index + 1 ) >= topLevelItemCount ( ) ) return ;
  takeTopLevelItem   ( index      )                    ;
  index ++                                             ;
  insertTopLevelItem ( index , it )                    ;
  setCurrentItem     ( it         )                    ;
  Alert              ( Action     )                    ;
}

void N::SymbolsQuotes::AllSymbols    (void)
{
  if ( IsNull(TickTree) ) return ;
  TickTree -> Full ( this )      ;
}

void N::SymbolsQuotes::LoadSymbols(void)
{
  QString filename = GetPath ( )            ;
  filename = QFileDialog::getOpenFileName   (
               this                         ,
               tr("Load symbols from file") ,
               filename                     ,
               tr("Plain text (*.txt)")   ) ;
  if ( filename . length ( ) <= 0 ) return  ;
  KeepPath ( filename )                     ;
  QByteArray  B                             ;
  QString     S                             ;
  QStringList L                             ;
  File::toByteArray ( filename , B )        ;
  S = QString::fromUtf8(B)                  ;
  L = S.split("\n")                         ;
  L = File::PurifyLines ( L )               ;
  if ( L . count ( ) <= 0 ) return          ;
  TickTree -> BySymbols ( this , L )        ;
  Alert ( Done )                            ;
}

void N::SymbolsQuotes::SaveSymbols(void)
{
  QString filename = GetPath ( ) ;
  filename = QFileDialog::getSaveFileName   (
               this                         ,
               tr("Save symbols to file")   ,
               filename                     ,
               tr("Plain text (*.txt)")   ) ;
  if ( filename . length ( ) <= 0 ) return  ;
  KeepPath ( filename )                     ;
  QStringList L                             ;
  QString     S                             ;
  QByteArray  B                             ;
  for (int i=0;i<topLevelItemCount();i++)   {
    QTreeWidgetItem * it                    ;
    it = topLevelItem ( i )                 ;
    L << it -> text ( 0 )                   ;
  }                                         ;
  S = L . join   ( "\r\n" )                 ;
  B = S . toUtf8 (        )                 ;
  File::toFile ( filename , B )             ;
  Alert ( Done )                            ;
}

void N::SymbolsQuotes::LoadStyleSheet(void)
{
  QString filename = GetPath ( )               ;
  filename = QFileDialog::getOpenFileName      (
               this                            ,
               tr("Load stylesheet from file") ,
               filename                        ,
               tr("Style Sheet (*.css)")     ) ;
  if ( filename . length ( ) <= 0 ) return     ;
  KeepPath ( filename )                        ;
  QByteArray  B                                ;
  QString     S                                ;
  File::toByteArray ( filename , B )           ;
  S = QString::fromUtf8(B)                     ;
  setStyleSheet ( S    )                       ;
  Alert         ( Done )                       ;
}

void N::SymbolsQuotes::SaveStyleSheet(void)
{
  QString filename = GetPath ( )             ;
  filename = QFileDialog::getSaveFileName    (
               this                          ,
               tr("Save stylesheet to file") ,
               filename                      ,
               tr("Style Sheet (*.css)")   ) ;
  if ( filename . length ( ) <= 0 ) return   ;
  KeepPath ( filename )                      ;
  QString     S                              ;
  QByteArray  B                              ;
  S = styleSheet (              )            ;
  B = S . toUtf8 (              )            ;
  File::toFile   ( filename , B )            ;
  Alert          ( Done         )            ;
}

void N::SymbolsQuotes::KeepPath(QString path)
{
  QFileInfo F ( path )                      ;
  QString   p = F . absolutePath ( )        ;
  plan -> Variables [ "SymbolsQuotes" ] = p ;
}

QString N::SymbolsQuotes::GetPath(void)
{
  QString p ;
  if ( plan  -> Variables . contains ( "SymbolsQuotes" ) )   {
    p = plan -> Variables [ "SymbolsQuotes" ] . toString ( ) ;
  } else                                                     {
    p = plan -> Temporary ( "" )                             ;
  }                                                          ;
  return p                                                   ;
}

void N::SymbolsQuotes::run(int Type,ThreadData * data)
{
}

bool N::SymbolsQuotes::Menu(QPoint pos)
{
  if ( 0 == LimitValues [ 100001 ] ) return false                ;
  nScopedMenu ( mm , this )                                      ;
  QAction         * aa                                           ;
  QTreeWidgetItem * hr    = headerItem    (     )                ;
  QTreeWidgetItem * it    = itemAt        ( pos )                ;
  int               cc    = currentColumn (     )                ;
  QMenu           * mc    = NULL                                 ;
  QMenu           * md    = NULL                                 ;
  QMenu           * mt    = NULL                                 ;
  QMenu           * mf    = NULL                                 ;
  QLineEdit       * le    = new QLineEdit (     )                ;
  QString           title = windowTitle   (     )                ;
  QString           identifier                                   ;
  identifier = Variables [ "Identifier" ] . toString ( )         ;
  ////////////////////////////////////////////////////////////////
  if ( ( NULL != it ) && ( 0 != LimitValues [ 100002 ] ) )       {
    aa  = mm . add ( 0 , it -> text ( 0 ) )                      ;
    aa -> setEnabled   ( false )                                 ;
    mm  . addSeparator (       )                                 ;
    if ( 0 != LimitValues [ 100003 ] )                           {
      mt = mm . addMenu ( tr("Trade") )                          ;
      mm . add ( mt , 501 , tr("Sell at market") )               ;
      mm . add ( mt , 502 , tr("Buy at market" ) )               ;
    }                                                            ;
    mm  . add ( 101 , tr("New Order"      ) )                    ;
    mm  . add ( 102 , tr("Chart Window"   ) )                    ;
    mm  . add ( 103 , tr("Tick Chart"     ) )                    ;
    mm  . add ( 104 , tr("Depth Of Market") )                    ;
    mm  . add ( 105 , tr("Hide"           ) )                    ;
    mm  . addSeparator ( )                                       ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100004 ] )                             {
    mm    . add ( 201 , tr("All symbols"    ) )                  ;
    //////////////////////////////////////////////////////////////
    mf   = mm . addMenu ( tr("Configurations") )                 ;
    mm   . add ( mf , 401 , tr("Load symbols"     ) )            ;
    mm   . add ( mf , 402 , tr("Save symbols"     ) )            ;
    mm   . addSeparator ( mf )                                   ;
    mm   . add ( mf , 411 , tr("Load stylesheet"  ) )            ;
    mm   . add ( mf , 412 , tr("Save stylesheet"  ) )            ;
    mm   . add ( mf , 413 , tr("Assign stylesheet") )            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100005 ] )                             {
    mc = mm . addMenu ( tr("Columns") )                          ;
    mm   . add ( mc                                              ,
                 901                                             ,
                 tr("Sorting")                                   ,
                 true                                            ,
                 isSortingEnabled()                            ) ;
    mm .add ( mc                                                 ,
              902                                                ,
              tr("Resize columns")                               ,
              true                                               ,
              ( 0 != LimitValues [ 3121279 ] )                 ) ;
    mm . add ( mc , 903 , tr("All columns") )                    ;
    mm . addSeparator ( mc )                                     ;
    for (int i=0;i<columnCount();i++)                            {
      QString cn = hr->text(i)                                   ;
      if ( cn . length ( ) <= 0 ) cn = tr("Direction")           ;
      aa = mm . add ( mc                                         ,
                      30000 + i                                  ,
                      cn                                         ,
                      true                                       ,
                      ! isColumnHidden ( i )                   ) ;
    }                                                            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100006 ] )                             {
    md = mm . addMenu ( tr("Display") )                          ;
    le -> setText ( title )                                      ;
    mm . add ( md , 31001 , le )                                 ;
    mm . add ( md                                                ,
               301                                               ,
               tr("Show trend")                                  ,
               true                                              ,
               TickTree->isEnabled("Trend")                    ) ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100007 ] )                             {
    mm . addSeparator ( )                                        ;
    DockingMenu ( mm )                                           ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  aa = mm . exec (                )                              ;
  if ( title != le -> text ( ) )                                 {
    setWindowTitle    ( le -> text ( ) )                         ;
    emit titleChanged ( this           )                         ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( IsNull     ( aa      ) ) return true                      ;
  if ( RunDocking ( mm , aa ) ) return true                      ;
  ////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                           {
    case 101                                                     :
      Insert             ( it                    )               ;
    break                                                        ;
    case 102                                                     :
      ChartWindow        ( it                    )               ;
    break                                                        ;
    case 103                                                     :
      TickChart          ( it                    )               ;
    break                                                        ;
    case 104                                                     :
      DepthOfMarket      ( it                    )               ;
    break                                                        ;
    case 105                                                     :
      Delete             ( it                    )               ;
    break                                                        ;
    case 201                                                     :
      AllSymbols         (                       )               ;
    break                                                        ;
    case 301                                                     :
      TickTree -> setEnabled ( "Trend" , aa -> isChecked ( )  )  ;
    break                                                        ;
    case 401                                                     :
      LoadSymbols        (                       )               ;
    break                                                        ;
    case 402                                                     :
      SaveSymbols        (                       )               ;
    break                                                        ;
    case 411                                                     :
      LoadStyleSheet     (                       )               ;
    break                                                        ;
    case 412                                                     :
      SaveStyleSheet     (                       )               ;
    break                                                        ;
    case 413                                                     :
      assignStyleSheet   ( windowTitle () , this )               ;
    break                                                        ;
    case 501                                                     :
      emit Sell          ( identifier     , it   )               ;
    break                                                        ;
    case 502                                                     :
      emit Buy           ( identifier     , it   )               ;
    break                                                        ;
    case 901                                                     :
      setSortingEnabled  ( aa -> isChecked ( )   )               ;
    break                                                        ;
    case 902                                                     :
      LimitValues [ 3121279 ] = aa->isChecked() ? 1 : 0          ;
    break                                                        ;
    case 903                                                     :
      for (int i=0;i<columnCount();i++)                          {
        setColumnHidden ( i , false )                            ;
      }                                                          ;
    break                                                        ;
    case 30000                                                   :
    case 30001                                                   :
    case 30002                                                   :
    case 30003                                                   :
    case 30004                                                   :
    case 30005                                                   :
    case 30006                                                   :
    case 30007                                                   :
    case 30008                                                   :
    case 30009                                                   :
      setColumnHidden ( mm [ aa ] - 30000 , ! aa->isChecked() )  ;
    break                                                        ;
    default                                                      :
    break                                                        ;
  }                                                              ;
  return true                                                    ;
}
