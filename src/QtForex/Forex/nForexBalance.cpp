#include <qtforex.h>

N::ForexBalance:: ForexBalance( QWidget * parent , Plan * p )
                : TableWidget (           parent ,        p )
                , items       ( NULL                        )
                , TimeId      ( -1                          )
{
  Configure ( ) ;
}

N::ForexBalance::~ForexBalance(void)
{
  if ( NULL != items )                                    {
    items -> functions . setEnabled ( "Deletion" , true ) ;
  }                                                       ;
}

QSize N::ForexBalance::sizeHint(void) const
{
  return QSize ( 640 , 400 ) ;
}

void N::ForexBalance::Configure(void)
{
  QStringList HS                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  HS << tr("Transaction")                                                    ;
  HS << tr("Time"       )                                                    ;
  HS << tr("Type"       )                                                    ;
  HS << tr("Commission" )                                                    ;
  HS << tr("Taxes"      )                                                    ;
  HS << tr("Swap"       )                                                    ;
  HS << tr("Amount"     )                                                    ;
  HS << tr("Comment"    )                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  setWindowTitle ( tr("FOREX Account Balance") )                             ;
  setColumnCount ( HS . count ( )              )                             ;
  ////////////////////////////////////////////////////////////////////////////
  plan->setFont   (     this )                                               ;
  for (int i=0;i<HS.count();i++)                                             {
    QTableWidgetItem * twi = new QTableWidgetItem ( )                        ;
    twi -> setText ( HS [ i ] )                                              ;
    setHorizontalHeaderItem ( i , twi )                                      ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  setColumnHidden ( 3 , true )                                               ;
  setColumnHidden ( 4 , true )                                               ;
  setColumnHidden ( 5 , true )                                               ;
  ////////////////////////////////////////////////////////////////////////////
  LimitValues [  90001 ] = 0                                                 ;
  LimitValues [  90002 ] = 0                                                 ;
  LimitValues [ 100001 ] = 1                                                 ;
  LimitValues [ 100002 ] = 1                                                 ;
  LimitValues [ 100003 ] = 1                                                 ;
  LimitValues [ 100004 ] = 1                                                 ;
  LimitValues [ 100005 ] = 1                                                 ;
  LimitValues [ 100006 ] = 1                                                 ;
  LimitValues [ 100007 ] = 1                                                 ;
  LimitValues [ 100008 ] = 1                                                 ;
  LimitValues [ 100009 ] = 1                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  TimeId   = startTimer        ( 1000                                      ) ;
}

bool N::ForexBalance::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  return true                                          ;
}

bool N::ForexBalance::FocusOut(void)
{
  return true ;
}

void N::ForexBalance::timerEvent(QTimerEvent * e)
{
  QTableWidget::timerEvent ( e )                              ;
  if ( TimeId < 0                                    ) return ;
  if ( TimeId != e -> timerId ( )                    ) return ;
  if ( NULL   == items                               ) return ;
  if ( items -> count ( ) <= 0                       ) return ;
  if ( ! items -> functions . isEnabled ( "Render" ) ) return ;
  items -> functions . setEnabled ( "Render" , false )        ;
  start ( 90002 )                                             ;
}

void N::ForexBalance::run(int type,ThreadData * data)
{
  switch ( type )                                                {
    case 90001                                                   :
      if ( 0 == LimitValues [ 90001 ] )                          {
        LimitValues [ 90001 ] = 1                                ;
        if ( NULL != items )                                     {
          bool first                                             ;
          first = items -> functions . isEnabled ( "First" )     ;
          if ( first )                                           {
            items -> functions . setEnabled ( "First" , false  ) ;
          }                                                      ;
          items -> Sync                     ( this             ) ;
          items -> Cleanup                  (                  ) ;
          if ( ! first )                                         {
            items -> functions . setEnabled ( "Render" , true  ) ;
          }                                                      ;
//          if ( first ) emit AutoFit ( )                          ;
        }                                                        ;
        LimitValues [ 90001 ] = 0                                ;
      }                                                          ;
    break                                                        ;
    case 90002                                                   :
      if ( 0 == LimitValues [ 90002 ] )                          {
        LimitValues [ 90002 ] = 1                                ;
        if ( NULL != items )                                     {
          items -> Render ( )                                    ;
          if ( items -> functions . isEnabled ( "First" ) )      {
            start ( 90001 )                                      ;
          }                                                      ;
        }                                                        ;
        LimitValues [ 90002 ] = 0                                ;
      }                                                          ;
    break                                                        ;
  }                                                              ;
}

bool N::ForexBalance::startup(void)
{
  return true ;
}

bool N::ForexBalance::Shutdown(void)
{
  return true ;
}

N::Finance::BalanceItems * N::ForexBalance::GetItems (void)
{
  return items ;
}

void N::ForexBalance::setBalanceItem(Finance::BalanceItems * it)
{
  items = it                                       ;
  nConnect ( items , SIGNAL ( Changed      ( ) )   ,
             this  , SLOT   ( ItemsChanged ( ) ) ) ;
}

void N::ForexBalance::ItemsChanged(void)
{
  bool first = items -> functions . isEnabled ( "First" ) ;
  if ( first )                                            {
    start ( 90002 )                                       ;
  } else                                                  {
    start ( 90001 )                                       ;
  }                                                       ;
}

void N::ForexBalance::LoadStyleSheet(void)
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

void N::ForexBalance::SaveStyleSheet(void)
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

bool N::ForexBalance::AssignEngine(QString filename)
{
  if ( IsNull ( items ) ) return false                   ;
  QByteArray B                                           ;
  File::toByteArray ( filename , B )                     ;
  if ( B . size ( ) <= 0 ) return false                  ;
  QString script = QString::fromUtf8(B)                  ;
  if ( script . length ( ) <= 0 ) return false           ;
  ////////////////////////////////////////////////////////
  QScriptEngine * Engine = new QScriptEngine ( )         ;
  QScriptValue    func                                   ;
  QScriptValue    g                                      ;
  QScriptValue    src ( script )                         ;
  ////////////////////////////////////////////////////////
  plan  -> Scripts . AttachCreators ( *Engine          ) ;
  func   = Engine -> evaluate       ( script           ) ;
  g      = Engine -> globalObject   (                  ) ;
  g      . setProperty              ( "Original" , src ) ;
  ////////////////////////////////////////////////////////
  items -> setEngine                ( Engine           ) ;
  ////////////////////////////////////////////////////////
  return true                                            ;
}

bool N::ForexBalance::AssignEngine(void)
{
  if ( IsNull ( items ) ) return false             ;
  QString filename = GetPath ( )                   ;
  filename = QFileDialog::getOpenFileName          (
               this                                ,
               tr("Javascript for balance filter") ,
               filename                            ,
               tr("Javascript (*.js)")           ) ;
  if ( filename . length ( ) <= 0 ) return false   ;
  KeepPath ( filename )                            ;
  return AssignEngine ( filename )                 ;
}

void N::ForexBalance::KeepPath(QString path)
{
  QFileInfo F ( path )                     ;
  QString   p = F . absolutePath ( )       ;
  plan -> Variables [ "ForexBalance" ] = p ;
}

QString N::ForexBalance::GetPath(void)
{
  QString p ;
  if ( plan  -> Variables . contains ( "ForexBalance" ) )   {
    p = plan -> Variables [ "ForexBalance" ] . toString ( ) ;
  } else                                                    {
    p = plan -> Temporary ( "" )                            ;
  }                                                         ;
  return p                                                  ;
}

void N::ForexBalance::Export(void)
{
  QString filename = GetPath ( )             ;
  filename = QFileDialog::getSaveFileName    (
               this                          ,
               tr("Save report to file")     ,
               filename                      ,
               tr("Plain text (*.txt)")    ) ;
  if ( filename . length ( ) <= 0 ) return   ;
  KeepPath ( filename )                      ;
  ////////////////////////////////////////////
  QStrings    L                              ;
  QStrings    X                              ;
  QString     S                              ;
  QByteArray  B                              ;
  ////////////////////////////////////////////
  TableWidgetItems hr                        ;
  hr = horizontalHeaderItems ( )             ;
  X . clear ( )                              ;
  for (int j=0;j<8;j++)                      {
    if ( ! isColumnHidden ( j ) )            {
      X << hr [ j ] -> text ( )              ;
    }                                        ;
  }                                          ;
  L << X . join  ( "\t"         )            ;
  for (int i=0;i<rowCount();i++)             {
    X . clear ( )                            ;
    for (int j=0;j<8;j++)                    {
      if ( ! isColumnHidden ( j ) )          {
        QTableWidgetItem * ix                ;
        ix = item ( i , j )                  ;
        if ( NULL != ix )                    {
          X << ix -> text ( )                ;
        } else X << " "                      ;
      }                                      ;
    }                                        ;
    L << X . join( "\t"         )            ;
  }                                          ;
  ////////////////////////////////////////////
  S = L . join   ( "\r\n"       )            ;
  B = S . toUtf8 (              )            ;
  File::toFile   ( filename , B )            ;
  Alert          ( Done         )            ;
}

bool N::ForexBalance::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                                  ;
  QMenu          * mc                                                        ;
  QAction        * aa                                                        ;
  TableWidgetItems hr         = horizontalHeaderItems ( )                    ;
  QString          identifier = Variables [ "Identifier" ] .toString ( )     ;
  int              login      = Variables [ "Login"      ] .toInt    ( )     ;
  QString          name       = Variables [ "Name"       ] .toString ( )     ;
  QString          group      = Variables [ "Group"      ] .toString ( )     ;
  ////////////////////////////////////////////////////////////////////////////
  mm      . add     ( 101 , tr("Deposit" ) )                                 ;
  mm      . add     ( 102 , tr("Withdraw") )                                 ;
  mm      . addSeparator ( )                                                 ;
  mm      . add     ( 201 , tr("Export to file") )                           ;
  mc = mm . addMenu ( tr("Columns")        )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i=0;i<8;i++)                                                      {
    QString hs = hr [ i ] -> text ( )                                        ;
    mm . add ( mc                                                            ,
               800 + i                                                       ,
               hs                                                            ,
               true                                                          ,
               ! isColumnHidden ( i )                                      ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  aa = mm . exec ( )                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull        ( aa      ) ) return true                               ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 101                                                                 :
      emit Deposit  ( identifier , login , name , group )                    ;
    break                                                                    ;
    case 102                                                                 :
      emit Withdraw ( identifier , login , name , group )                    ;
    break                                                                    ;
    case 201                                                                 :
      Export ( )                                                             ;
    break                                                                    ;
    case 800                                                                 :
    case 801                                                                 :
    case 802                                                                 :
    case 803                                                                 :
    case 804                                                                 :
    case 805                                                                 :
    case 806                                                                 :
    case 807                                                                 :
      setColumnHidden ( mm [ aa ] - 800 , ! aa -> isChecked ( ) )            ;
    break                                                                    ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}
