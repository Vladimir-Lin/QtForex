#include <qtforex.h>

N::ForexTraders:: ForexTraders ( QWidget * parent , Plan * p )
                : TreeDock     (           parent ,        p )
                , search       ( NULL                        )
                , items        ( NULL                        )
                , TimeId       ( -1                          )
{
  Configure ( ) ;
}

N::ForexTraders::~ForexTraders (void)
{
  if ( NULL != items )                                    {
    items -> functions . setEnabled ( "Deletion" , true ) ;
  }                                                       ;
}

void N::ForexTraders::Configure(void)
{
  dockingPlace  = Qt::BottomDockWidgetArea                               ;
  ////////////////////////////////////////////////////////////////////////
  setWindowIcon                ( QIcon(":/images/buddy.png")           ) ;
  setWindowTitle               ( tr("FOREX Traders")                   ) ;
  setSuggestion                ( QSize ( 800 , 320 )                   ) ;
  setDragDropMode              ( NoDragDrop                            ) ;
  setRootIsDecorated           ( false                                 ) ;
  setAlternatingRowColors      ( true                                  ) ;
  setSelectionMode             ( ExtendedSelection                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded                 ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded                 ) ;
  setColumnCount               ( 17                                    ) ;
  ////////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                                  ) ;
  head -> setText              (  0 , ""                               ) ;
  head -> setIcon              (  0 , QIcon(":/icons/join.png"       ) ) ;
  head -> setText              (  1 , ""                               ) ;
  head -> setIcon              (  1 , QIcon(":/images/bookmarks.png" ) ) ;
  head -> setText              (  2 , tr("Login"   )                   ) ;
  head -> setText              (  3 , tr("Leverage")                   ) ;
  head -> setText              (  4 , tr("Name"    )                   ) ;
  head -> setText              (  5 , tr("Balance" )                   ) ;
  head -> setText              (  6 , tr("Equity"  )                   ) ;
  head -> setText              (  7 , tr("Margin"  )                   ) ;
  head -> setText              (  8 , tr("Lots"    )                   ) ;
  head -> setText              (  9 , tr("IP"      )                   ) ;
  head -> setText              ( 10 , tr("Counter" )                   ) ;
  head -> setText              ( 11 , tr("Group"   )                   ) ;
  head -> setText              ( 12 , tr("Country" )                   ) ;
  head -> setText              ( 13 , tr("Phone"   )                   ) ;
  head -> setText              ( 14 , tr("E-mail"  )                   ) ;
  head -> setText              ( 15 , tr("Status"  )                   ) ;
  head -> setText              ( 16 , ""                               ) ;
  assignHeaderItems            ( head                                  ) ;
  ////////////////////////////////////////////////////////////////////////
  setColumnHidden              (  3 , true                             ) ;
  setColumnHidden              (  7 , true                             ) ;
  setColumnHidden              (  8 , true                             ) ;
  setColumnHidden              ( 10 , true                             ) ;
  setColumnHidden              ( 12 , true                             ) ;
  setColumnHidden              ( 13 , true                             ) ;
  setColumnHidden              ( 14 , true                             ) ;
  setColumnHidden              ( 15 , true                             ) ;
  ////////////////////////////////////////////////////////////////////////
  for (int i=2;i<=15;i++) names [ i ] = head -> text ( i )               ;
  names [  0 ] = tr("Enable" )                                           ;
  names [  1 ] = tr("Online" )                                           ;
  names [ 16 ] = tr("Pending")                                           ;
  ////////////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                                     ) ;
  MountClicked                 ( 2                                     ) ;
  plan->setFont                ( this                                  ) ;
  ////////////////////////////////////////////////////////////////////////
  LimitValues [  90001 ] = 0                                             ;
  LimitValues [  90002 ] = 0                                             ;
  LimitValues [ 100001 ] = 1                                             ;
  LimitValues [ 100002 ] = 1                                             ;
  LimitValues [ 100003 ] = 1                                             ;
  LimitValues [ 100004 ] = 1                                             ;
  LimitValues [ 100005 ] = 1                                             ;
  LimitValues [ 100006 ] = 1                                             ;
  LimitValues [ 100007 ] = 1                                             ;
  LimitValues [ 100008 ] = 1                                             ;
  LimitValues [ 100009 ] = 1                                             ;
  ////////////////////////////////////////////////////////////////////////
  TimeId   = startTimer        ( 1000                                  ) ;
}

void N::ForexTraders::timerEvent(QTimerEvent * e)
{
  QTreeWidget::timerEvent ( e )                               ;
  if ( TimeId < 0                                    ) return ;
  if ( TimeId != e -> timerId ( )                    ) return ;
  if ( NULL   == items                               ) return ;
  if ( ! items -> functions . isEnabled ( "Render" ) ) return ;
  items -> functions . setEnabled ( "Render" , false )        ;
  start ( 90002 )                                             ;
}

bool N::ForexTraders::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Search       , Search      ( ) ) ;
  return true                                          ;
}

bool N::ForexTraders::FocusOut(void)
{
  return true ;
}

void N::ForexTraders::run(int type,ThreadData * data)
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
          if ( ! first )                                         {
            items -> functions . setEnabled ( "Render" , true  ) ;
          }                                                      ;
        }                                                        ;
        LimitValues [ 90001 ] = 0                                ;
        emit AutoFit ( )                                         ;
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

N::Finance::UserItems * N::ForexTraders::GetItems (void)
{
  return items ;
}

void N::ForexTraders::setUserItem(Finance::UserItems * it)
{
  items = it                                       ;
  nConnect ( items , SIGNAL ( Changed      ( ) )   ,
             this  , SLOT   ( ItemsChanged ( ) ) ) ;
}

bool N::ForexTraders::startup(void)
{
  return true ;
}

bool N::ForexTraders::Shutdown(void)
{
  return true ;
}

void N::ForexTraders::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Click ) ;
}

void N::ForexTraders::doubleClicked(QTreeWidgetItem * item,int column)
{
  int     login = item->text( 2).toInt()       ;
  QString group = item->text(11)               ;
  emit GetOrders ( Identifier ,login , group ) ;
}

void N::ForexTraders::ItemsChanged(void)
{
  bool first = items -> functions . isEnabled ( "First" ) ;
  if ( first )                                            {
    start ( 90002 )                                       ;
  } else                                                  {
    start ( 90001 )                                       ;
  }                                                       ;
}

void N::ForexTraders::LoadStyleSheet(void)
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

void N::ForexTraders::SaveStyleSheet(void)
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

bool N::ForexTraders::AssignLogins(void)
{
  QString filename = GetPath ( )                 ;
  filename = QFileDialog::getOpenFileName        (
               this                              ,
               tr("Load logins from file")       ,
               filename                          ,
               tr("Plain text (*.txt)")        ) ;
  if ( filename . length ( ) <= 0 ) return false ;
  KeepPath ( filename )                          ;
  QByteArray  B                                  ;
  QString     S                                  ;
  QStringList L                                  ;
  CUIDs       IDs                                ;
  File::toByteArray ( filename , B )             ;
  S = QString::fromUtf8(B)                       ;
  L = S . split ( "\n" )                         ;
  L = File::PurifyLines ( L )                    ;
  ////////////////////////////////////////////////
  foreach ( S ,  L ) IDs << S . toInt ( )        ;
  items -> ByLogins = IDs                        ;
  ////////////////////////////////////////////////
  Alert         ( Done )                         ;
  return true                                    ;
}

bool N::ForexTraders::AssignNames(void)
{
  QString filename = GetPath ( )                 ;
  filename = QFileDialog::getOpenFileName        (
               this                              ,
               tr("Load names from file")        ,
               filename                          ,
               tr("Plain text (*.txt)")        ) ;
  if ( filename . length ( ) <= 0 ) return false ;
  KeepPath ( filename )                          ;
  QByteArray  B                                  ;
  QString     S                                  ;
  QStringList L                                  ;
  File::toByteArray ( filename , B )             ;
  S = QString::fromUtf8(B)                       ;
  L = S . split ( "\n" )                         ;
  L = File::PurifyLines ( L )                    ;
  ////////////////////////////////////////////////
  items -> ByNames = L                           ;
  ////////////////////////////////////////////////
  Alert         ( Done )                         ;
  return true                                    ;
}

void N::ForexTraders::SaveLogins(void)
{
  QString filename = GetPath ( )           ;
  filename = QFileDialog::getSaveFileName  (
               this                        ,
               tr("Export logins to file") ,
               filename                    ,
               tr("Plain text (*.txt)")  ) ;
  if ( filename . length ( ) <= 0 ) return ;
  KeepPath ( filename )                    ;
  //////////////////////////////////////////
  QStringList       L                      ;
  QString           S                      ;
  QByteArray        B                      ;
  QTreeWidgetItem * it                     ;
  //////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)  {
    it = topLevelItem ( i )                ;
    L << it -> text   ( 2 )                ;
  }                                        ;
  //////////////////////////////////////////
  S = L . join   ( "\r\n"       )          ;
  B = S . toUtf8 (              )          ;
  File::toFile   ( filename , B )          ;
  Alert          ( Done         )          ;
}

void N::ForexTraders::SaveNames(void)
{
  QString filename = GetPath ( )           ;
  filename = QFileDialog::getSaveFileName  (
               this                        ,
               tr("Export names to file")  ,
               filename                    ,
               tr("Plain text (*.txt)")  ) ;
  if ( filename . length ( ) <= 0 ) return ;
  KeepPath ( filename )                    ;
  //////////////////////////////////////////
  QStringList       L                      ;
  QString           S                      ;
  QByteArray        B                      ;
  QTreeWidgetItem * it                     ;
  //////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)  {
    it = topLevelItem ( i )                ;
    L << it -> text   ( 4 )                ;
  }                                        ;
  //////////////////////////////////////////
  S = L . join   ( "\r\n"       )          ;
  B = S . toUtf8 (              )          ;
  File::toFile   ( filename , B )          ;
  Alert          ( Done         )          ;
}

void N::ForexTraders::AssignMode(QString mode)
{
  if ( IsNull(items) ) return                              ;
  if ( "All" == mode )                                     {
    items -> functions . setEnabled ( "All"    , true  )   ;
    items -> functions . setEnabled ( "Online" , false )   ;
    items -> functions . setEnabled ( "Group"  , false )   ;
    items -> functions . setEnabled ( "Logins" , false )   ;
    items -> functions . setEnabled ( "Names"  , false )   ;
    items -> functions . setEnabled ( "Script" , false )   ;
    ItemsChanged ( )                                       ;
    return                                                 ;
  }                                                        ;
  if ( "Online" == mode )                                  {
    items -> functions . setEnabled ( "All"    , false )   ;
    items -> functions . setEnabled ( "Online" , true  )   ;
    items -> functions . setEnabled ( "Group"  , false )   ;
    items -> functions . setEnabled ( "Logins" , false )   ;
    items -> functions . setEnabled ( "Names"  , false )   ;
    items -> functions . setEnabled ( "Script" , false )   ;
    ItemsChanged ( )                                       ;
    return                                                 ;
  }                                                        ;
  if ( "Group" == mode )                                   {
    items -> functions . setEnabled ( "All"    , false )   ;
    items -> functions . setEnabled ( "Online" , false )   ;
    items -> functions . setEnabled ( "Group"  , true  )   ;
    items -> functions . setEnabled ( "Logins" , false )   ;
    items -> functions . setEnabled ( "Names"  , false )   ;
    items -> functions . setEnabled ( "Script" , false )   ;
    ItemsChanged ( )                                       ;
    return                                                 ;
  }                                                        ;
  if ( "Logins" == mode )                                  {
    items -> functions . setEnabled ( "All"    , false )   ;
    items -> functions . setEnabled ( "Online" , false )   ;
    items -> functions . setEnabled ( "Group"  , false )   ;
    items -> functions . setEnabled ( "Logins" , true  )   ;
    items -> functions . setEnabled ( "Names"  , false )   ;
    items -> functions . setEnabled ( "Script" , false )   ;
    ItemsChanged ( )                                       ;
    return                                                 ;
  }                                                        ;
  if ( "Names" == mode )                                   {
    items -> functions . setEnabled ( "All"    , false )   ;
    items -> functions . setEnabled ( "Online" , false )   ;
    items -> functions . setEnabled ( "Group"  , false )   ;
    items -> functions . setEnabled ( "Logins" , false )   ;
    items -> functions . setEnabled ( "Names"  , true  )   ;
    items -> functions . setEnabled ( "Script" , false )   ;
    ItemsChanged ( )                                       ;
    return                                                 ;
  }                                                        ;
  if ( "Script" == mode )                                  {
    if ( items -> hasEngine ( ) )                          {
      items -> functions . setEnabled ( "All"    , false ) ;
      items -> functions . setEnabled ( "Online" , false ) ;
      items -> functions . setEnabled ( "Group"  , false ) ;
      items -> functions . setEnabled ( "Logins" , false ) ;
      items -> functions . setEnabled ( "Names"  , false ) ;
      items -> functions . setEnabled ( "Script" , true  ) ;
      ItemsChanged ( )                                     ;
    }                                                      ;
    return                                                 ;
  }                                                        ;
}

bool N::ForexTraders::AssignEngine(QString filename)
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

bool N::ForexTraders::AssignEngine(void)
{
  if ( IsNull ( items ) ) return false             ;
  QString filename = GetPath ( )                   ;
  filename = QFileDialog::getOpenFileName          (
               this                                ,
               tr("Javascript for account filter") ,
               filename                            ,
               tr("Javascript (*.js)")           ) ;
  if ( filename . length ( ) <= 0 ) return false   ;
  KeepPath ( filename )                            ;
  return AssignEngine ( filename )                 ;
}

void N::ForexTraders::KeepPath(QString path)
{
  QFileInfo F ( path )                     ;
  QString   p = F . absolutePath ( )       ;
  plan -> Variables [ "ForexTraders" ] = p ;
}

QString N::ForexTraders::GetPath(void)
{
  QString p ;
  if ( plan  -> Variables . contains ( "ForexTraders" ) )   {
    p = plan -> Variables [ "ForexTraders" ] . toString ( ) ;
  } else                                                    {
    p = plan -> Temporary ( "" )                            ;
  }                                                         ;
  return p                                                  ;
}

void N::ForexTraders::Details(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()             ;
  QString group = item->text(11)                     ;
  QString name  = item->text( 4)                     ;
  emit Details ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Join(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()          ;
  QString group = item->text(11)                  ;
  QString name  = item->text( 4)                  ;
  emit Join ( Identifier , login , name , group ) ;
}

void N::ForexTraders::History(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()             ;
  QString group = item->text(11)                     ;
  QString name  = item->text( 4)                     ;
  emit History ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Balance(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()             ;
  QString group = item->text(11)                     ;
  QString name  = item->text( 4)                     ;
  emit Balance ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Credit(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()            ;
  QString group = item->text(11)                    ;
  QString name  = item->text( 4)                    ;
  emit Credit ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Deposit(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()             ;
  QString group = item->text(11)                     ;
  QString name  = item->text( 4)                     ;
  emit Deposit ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Withdraw(QTreeWidgetItem * item)
{
  int     login = item->text( 2).toInt()              ;
  QString group = item->text(11)                      ;
  QString name  = item->text( 4)                      ;
  emit Withdraw ( Identifier , login , name , group ) ;
}

void N::ForexTraders::Hide(QTreeWidgetItem * it)
{
  if ( IsNull ( it ) ) return              ;
  int index = indexOfTopLevelItem ( it )   ;
  if ( index < 0     ) return              ;
  int login = it -> text ( 2 ) . toInt ( ) ;
  takeTopLevelItem ( index )               ;
  items -> visible [ login ] = false       ;
}

void N::ForexTraders::Delete(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( IsNull ( it ) ) return            ;
  Hide ( it )                            ;
}

void N::ForexTraders::Search(void)
{
  if ( NULL != search )                                          {
    search -> setFocus ( Qt::TabFocusReason )                    ;
    Alert              ( Action             )                    ;
    return                                                       ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  QString m = tr("Search keyword")                               ;
  QSize   s = plan -> status -> size ( )                         ;
  QSize   w = QSize ( s . width ( ) / 6 , s . height ( ) - 2 )   ;
  Font    f = plan -> fonts [ Fonts::TreeView ]                  ;
  QFont   x = f . toQFont ( )                                    ;
  if (  w . width ( ) > 240 ) w . setWidth ( 240 )               ;
  x      . setPixelSize                  ( w . height ( ) - 2  ) ;
  search = new LineEdit                  ( plan->status , plan ) ;
  search -> setMinimumSize               ( w                   ) ;
  search -> setMaximumSize               ( w                   ) ;
  search -> setFont                      ( x                   ) ;
  search -> setPlaceholderText           ( m                   ) ;
  m       = tr("Search keyword in FOREX Traders")                ;
  search -> setToolTip                   ( m                   ) ;
  plan   -> status -> addPermanentWidget ( search              ) ;
  nConnect ( search , SIGNAL ( returnPressed  ( ) )              ,
             this   , SLOT   ( keywordPressed ( ) )            ) ;
  search -> setFocus                     ( Qt::TabFocusReason  ) ;
  Alert                                  ( Action              ) ;
}

void N::ForexTraders::keywordPressed(void)
{
  if ( IsNull ( search ) ) return              ;
  //////////////////////////////////////////////
  QTreeWidgetItem * it    = currentItem    ( ) ;
  QString           t     = search -> text ( ) ;
  int               index = 0                  ;
  bool              found = false              ;
  QString           z     = ""                 ;
  //////////////////////////////////////////////
  if ( NotNull ( it ) )                        {
    index = indexOfTopLevelItem ( it )         ;
  }                                            ;
  //////////////////////////////////////////////
  while ( ( t.length() > 0                 )  &&
          ( ! found                        )  &&
          ( index < topLevelItemCount()    ) ) {
    it = topLevelItem ( index )                ;
    for (int i=0;i<columnCount();i++)          {
      z = it -> text ( i )                     ;
      if ( z . contains ( t ) ) found = true   ;
    }                                          ;
    if ( ! found ) index++                     ;
  }                                            ;
  //////////////////////////////////////////////
  if ( found )                                 {
    it = topLevelItem ( index )                ;
    scrollToItem      ( it    )                ;
    setCurrentItem    ( it    )                ;
    Alert             ( Done  )                ;
  } else                                       {
    Alert             ( Error )                ;
  }                                            ;
  //////////////////////////////////////////////
  search -> deleteLater ( )                    ;
  search  = NULL                               ;
}

bool N::ForexTraders::Menu(QPoint pos)
{
  if ( 0 == LimitValues [ 100001 ] ) return false              ;
  nScopedMenu ( mm , this )                                    ;
  QAction         * aa                                         ;
  QTreeWidgetItem * it  = itemAt ( pos )                       ;
  int               cc  = currentColumn ( )                    ;
  QMenu           * mc                                         ;
  QMenu           * mg                                         ;
  QMenu           * ms                                         ;
  QMenu           * ma                                         ;
  QMenu           * mf                                         ;
  QMenu           * mt                                         ;
  QMenu           * mo                                         ;
  QLineEdit       * groupname = new QLineEdit ( )              ;
  QLineEdit       * jfunc     = new QLineEdit ( )              ;
  QString           m                                          ;
  //////////////////////////////////////////////////////////////
  m = tr("Account group name")                                 ;
  groupname -> setPlaceholderText ( m )                        ;
  groupname -> setToolTip         ( m )                        ;
  m = tr("Javascript function name")                           ;
  jfunc -> setPlaceholderText     ( m )                        ;
  jfunc -> setToolTip             ( m )                        ;
  if ( items -> Variables . contains ( "Group" ) )             {
    m = items -> Variables [ "Group" ] . toString ( )          ;
    groupname -> setText ( m )                                 ;
  }                                                            ;
  if ( items -> Variables . contains ( "Java"  ) )             {
    m = items -> Variables [ "Java"  ] . toString ( )          ;
    jfunc -> setText ( m )                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( ( NULL != it ) && ( 0 != LimitValues [ 100002 ] ) )     {
    aa  = mm . add     ( 0 , it -> text(2)        )            ;
    aa -> setEnabled   ( false                    )            ;
    mm  . addSeparator (                          )            ;
    mm  . add          ( 101 , tr("Trade orders") )            ;
    mm  . add          ( 102 , tr("Join active traders") )     ;
    mo  = mm . addMenu ( tr("Account details")    )            ;
    mm  . add          ( mo , 107 , tr("New account"   ) )     ;
    mm  . addSeparator ( mo                              )     ;
    mm  . add          ( mo , 108 , tr("Details"       ) )     ;
    mm  . add          ( mo , 111 , tr("History orders") )     ;
    mm  . add          ( mo , 112 , tr("Balance"       ) )     ;
    mm  . add          ( mo , 113 , tr("Credit"        ) )     ;
    mm  . add          ( mo , 114 , tr("Associate with people") ) ;
    mm  . addSeparator (                          )            ;
    mm  . add          ( 109 , tr("Search"      ) )            ;
    mm  . add          ( 110 , tr("Hide"        ) )            ;
    if ( 0 != LimitValues [ 100009 ] )                         {
      mt = mm . addMenu ( tr("Transactions")      )            ;
      mm . add ( mt , 701 , tr("Deposit" ) )                   ;
      mm . add ( mt , 702 , tr("Withdraw") )                   ;
    }                                                          ;
    mm  . addSeparator (                          )            ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( ( NULL != items ) && ( 0 != LimitValues [ 100003 ] ) )  {
    mg = mm . addMenu ( tr("Catalog")               )          ;
    mm . add ( mg , 501 , tr("All accounts"     )   )          ;
    mm . add ( mg , 502 , tr("Online accounts"  )   )          ;
    mm . addSeparator ( mg                          )          ;
    mm . add ( mg , 506 , tr("Login lists"      )   )          ;
    mm . add ( mg , 507 , tr("Assign logins"    )   )          ;
    mm . addSeparator ( mg                          )          ;
    mm . add ( mg , 508 , tr("Name lists"       )   )          ;
    mm . add ( mg , 509 , tr("Assign names"     )   )          ;
    mm . addSeparator ( mg                          )          ;
    mm . add ( mg , 503 , tr("Group"            )   )          ;
    mm . add ( mg , 511 , groupname                 )          ;
    mm . addSeparator ( mg                          )          ;
    if ( items -> hasEngine ( ) )                              {
      mm . add ( mg , 504 , tr("Javascript"     )   )          ;
    }                                                          ;
    mm . add ( mg , 505 , tr("Assign Javascript")   )          ;
    mm . add ( mg , 512 , jfunc                     )          ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100004 ] )                           {
    mf   = mm . addMenu ( tr("Configurations") )               ;
    mm   . add ( mf , 411 , tr("Load stylesheet"  ) )          ;
    if ( styleSheet ( ) . length ( ) > 0 )                     {
      mm . add ( mf , 412 , tr("Save stylesheet"  ) )          ;
    }                                                          ;
    mm   . add ( mf , 413 , tr("Assign stylesheet") )          ;
    mm   . addSeparator ( mf )                                 ;
    mm   . add ( mf , 421 , tr("Export login lists") )         ;
    mm   . add ( mf , 422 , tr("Export name lists" ) )         ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100005 ] )                           {
    mc = mm . addMenu ( tr("Columns"    )           )          ;
    mm . add ( mc , 901 , tr("Resize columns")      )          ;
    mm . add ( mc , 902 , tr("All columns"   )      )          ;
    mm . addSeparator ( mc                          )          ;
    for (int i=0;i<columnCount();i++)                          {
      mm . add ( mc                                            ,
                 30000 + i                                     ,
                 names [ i ]                                   ,
                 true                                          ,
                 ! isColumnHidden ( i )                      ) ;
    }                                                          ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100006 ] )                           {
    ms = mm . addMenu ( tr("Sorting"    )                    ) ;
    SortingMenu ( mm , ms   )                                  ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100007 ] )                           {
    ma = mm . addMenu ( tr("Adjustments")                    ) ;
    AdjustMenu  ( mm , ma   )                                  ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100008 ] )                           {
    DockingMenu ( mm        )                                  ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  aa = mm . exec ( )                                           ;
  //////////////////////////////////////////////////////////////
  m = groupname -> text ( )                                    ;
  if ( m . length ( ) > 0 )                                    {
    items -> Variables [ "Group" ] = m                         ;
  } else                                                       {
    items -> Variables . remove ( "Group" )                    ;
  }                                                            ;
  m = jfunc     -> text ( )                                    ;
  if ( m . length ( ) > 0 )                                    {
    items -> Variables [ "Java" ] = m                          ;
  } else                                                       {
    items -> Variables . remove ( "Java" )                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( IsNull        ( aa      ) ) return true                 ;
  if ( RunSorting    ( mm , aa ) ) return true                 ;
  if ( RunAdjustment ( mm , aa ) ) return true                 ;
  if ( RunDocking    ( mm , aa ) ) return true                 ;
  //////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                         {
    case 101                                                   :
      doubleClicked    ( it , 2                )               ;
    break                                                      ;
    case 102                                                   :
      Join             ( it                    )               ;
    break                                                      ;
    case 107                                                   :
      emit New         ( Identifier            )               ;
    break                                                      ;
    case 108                                                   :
      Details          ( it                    )               ;
    break                                                      ;
    case 109                                                   :
      Search           (                       )               ;
    break                                                      ;
    case 110                                                   :
      Hide             ( it                    )               ;
    break                                                      ;
    case 111                                                   :
      History          ( it                    )               ;
    break                                                      ;
    case 112                                                   :
      Balance          ( it                    )               ;
    break                                                      ;
    case 113                                                   :
      Credit           ( it                    )               ;
    break                                                      ;
    case 114                                                   :
      Associate        ( this                  )               ;
    break                                                      ;
    case 411                                                   :
      LoadStyleSheet   (                       )               ;
    break                                                      ;
    case 412                                                   :
      SaveStyleSheet   (                       )               ;
    break                                                      ;
    case 413                                                   :
      assignStyleSheet ( windowTitle () , this )               ;
    break                                                      ;
    case 421                                                   :
      SaveLogins       (                       )               ;
    break                                                      ;
    case 422                                                   :
      SaveNames        (                       )               ;
    break                                                      ;
    case 501                                                   :
      AssignMode ( "All" )                                     ;
    break                                                      ;
    case 502                                                   :
      AssignMode ( "Online" )                                  ;
    break                                                      ;
    case 503                                                   :
      if ( items -> Variables . contains ( "Group" ) )         {
        AssignMode ( "Group" )                                 ;
      }                                                        ;
    break                                                      ;
    case 504                                                   :
      if ( NotNull(items) )                                    {
        if ( ! items -> hasEngine ( ) )                        {
          if ( AssignEngine ( ) )                              {
            AssignMode ( "Script" )                            ;
          }                                                    ;
        } else                                                 {
          AssignMode ( "Script" )                              ;
        }                                                      ;
      }                                                        ;
    break                                                      ;
    case 505                                                   :
      if ( AssignEngine ( ) )                                  {
        if ( items -> Variables . contains ( "Java" ) )        {
          AssignMode ( "Script" )                              ;
        }                                                      ;
      }                                                        ;
    break                                                      ;
    case 506                                                   :
      AssignMode ( "Logins" )                                  ;
    break                                                      ;
    case 507                                                   :
      if ( AssignLogins ( ) ) AssignMode ( "Logins" )          ;
    break                                                      ;
    case 508                                                   :
      AssignMode ( "Names" )                                   ;
    break                                                      ;
    case 509                                                   :
      if ( AssignNames ( ) ) AssignMode ( "Names" )            ;
    break                                                      ;
    case 701                                                   :
      Deposit          ( it                    )               ;
    break                                                      ;
    case 702                                                   :
      Withdraw         ( it                    )               ;
    break                                                      ;
    case 901                                                   :
      emit AutoFit ( )                                         ;
    break                                                      ;
    case 902                                                   :
      for (int i=0;i<columnCount();i++)                        {
        setColumnHidden ( i , false )                          ;
      }                                                        ;
      emit AutoFit ( )                                         ;
    break                                                      ;
    case 30000                                                 :
    case 30001                                                 :
    case 30002                                                 :
    case 30003                                                 :
    case 30004                                                 :
    case 30005                                                 :
    case 30006                                                 :
    case 30007                                                 :
    case 30008                                                 :
    case 30009                                                 :
    case 30010                                                 :
    case 30011                                                 :
    case 30012                                                 :
    case 30013                                                 :
    case 30014                                                 :
    case 30015                                                 :
    case 30016                                                 :
      setColumnHidden ( mm [ aa ] - 30000                      ,
                        ! aa->isChecked()                    ) ;
    break                                                      ;
    default                                                    :
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
