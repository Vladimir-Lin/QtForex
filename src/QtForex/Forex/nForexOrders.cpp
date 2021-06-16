#include <qtforex.h>

N::ForexOrders:: ForexOrders ( QWidget * parent , Plan * p )
               : TreeDock    (           parent ,        p )
               , items       ( NULL                        )
               , TimeId      ( -1                          )
{
  Configure ( ) ;
}

N::ForexOrders::~ForexOrders (void)
{
  if ( NULL != items )                                    {
    items -> functions . setEnabled ( "Deletion" , true ) ;
  }                                                       ;
}

void N::ForexOrders::Configure(void)
{
  dockingPlace  = Qt::BottomDockWidgetArea                               ;
  ////////////////////////////////////////////////////////////////////////
  setWindowIcon                ( QIcon(":/images/buddy.png")           ) ;
  setWindowTitle               ( tr("FOREX Orders")                    ) ;
  setSuggestion                ( QSize ( 800 , 320 )                   ) ;
  setDragDropMode              ( NoDragDrop                            ) ;
  setRootIsDecorated           ( false                                 ) ;
  setAlternatingRowColors      ( true                                  ) ;
  setSelectionMode             ( ExtendedSelection                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded                 ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded                 ) ;
  setColumnCount               ( 18                                    ) ;
  ////////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                                  ) ;
  head -> setText              (  0 , tr("Order"     )                 ) ;
  head -> setText              (  1 , tr("Time"      )                 ) ;
  head -> setText              (  2 , tr("Type"      )                 ) ;
  head -> setText              (  3 , tr("Size"      )                 ) ;
  head -> setText              (  4 , tr("Symbol"    )                 ) ;
  head -> setText              (  5 , tr("Price"     )                 ) ;
  head -> setText              (  6 , tr("S/L"       )                 ) ;
  head -> setText              (  7 , tr("T/P"       )                 ) ;
  head -> setText              (  8 , tr("Time"      )                 ) ;
  head -> setText              (  9 , tr("Quote"     )                 ) ;
  head -> setText              ( 10 , tr("Duration"  )                 ) ;
  head -> setText              ( 11 , tr("Commission")                 ) ;
  head -> setText              ( 12 , tr("Taxes"     )                 ) ;
  head -> setText              ( 13 , tr("Swap"      )                 ) ;
  head -> setText              ( 14 , tr("Profit"    )                 ) ;
  head -> setText              ( 15 , tr("Comment"   )                 ) ;
  head -> setText              ( 16 , tr("Script"    )                 ) ;
  head -> setText              ( 17 , ""                               ) ;
  assignHeaderItems            ( head                                  ) ;
  for (int i=0;i<=16;i++) names [ i ] = head -> text ( i )               ;
  names [ 17 ] = tr("Pending")                                           ;
  setColumnHidden              (  8 , true                             ) ;
  setColumnHidden              ( 12 , true                             ) ;
  setColumnHidden              ( 13 , true                             ) ;
  setColumnHidden              ( 16 , true                             ) ;
  setColumnHidden              ( 17 , true                             ) ;
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
  LimitValues [ 110001 ] = 1                                             ;
  ////////////////////////////////////////////////////////////////////////
  TimeId   = startTimer        ( 1000                                  ) ;
}

bool N::ForexOrders::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  return true                                          ;
}

bool N::ForexOrders::FocusOut(void)
{
  return true ;
}

void N::ForexOrders::timerEvent(QTimerEvent * e)
{
  QTreeWidget::timerEvent ( e )                               ;
  if ( TimeId < 0                                    ) return ;
  if ( TimeId != e -> timerId ( )                    ) return ;
  if ( NULL   == items                               ) return ;
  if ( items -> count ( ) <= 0                       ) return ;
  if ( ! items -> functions . isEnabled ( "Render" ) ) return ;
  items -> functions . setEnabled ( "Render" , false )        ;
  start ( 90002 )                                             ;
}

void N::ForexOrders::run(int type,ThreadData * data)
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
          if ( first ) emit AutoFit ( )                          ;
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

bool N::ForexOrders::startup(void)
{
  return true ;
}

bool N::ForexOrders::Shutdown(void)
{
  return true ;
}

void N::ForexOrders::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Click ) ;
}

void N::ForexOrders::doubleClicked(QTreeWidgetItem * item,int column)
{
  int order = item -> text ( 0 ) . toInt ( )        ;
  switch ( column )                                 {
    case  0                                         :
    case  1                                         :
    case  2                                         :
    case  3                                         :
    case  4                                         :
    case  5                                         :
    case  8                                         :
    case  9                                         :
    case 10                                         :
    case 11                                         :
    case 12                                         :
    case 13                                         :
    case 14                                         :
      if ( items -> orders . contains ( order ) )   {
        int L = items -> orders [ order ] -> login  ;
        emit ModifyOrder ( Identifier , L , order ) ;
      }                                             ;
    break                                           ;
    case  6                                         :
      if ( ( 1 == LimitValues [ 110001 ] )         &&
           items -> orders . contains ( order ) )   {
        Finance::Order * odr                        ;
        odr = items -> orders [order]               ;
        int D = odr -> digits                       ;
        int M = odr -> multiply                     ;
        double st = 1                               ;
        double sl                                   ;
        QDoubleSpinBox * sb                         ;
        sl  = odr -> sl                             ;
        st /= M                                     ;
        sb  = setDoubleSpin                         (
                item                                ,
                column                              ,
                0                                   ,
                100000.0                            ,
                SIGNAL ( editingFinished ( ) )      ,
                SLOT   ( profitsChanged  ( ) )    ) ;
        sb -> setDecimals   ( D  )                  ;
        sb -> setSingleStep ( st )                  ;
        if ( sl < 0.00001 )                         {
          int mp = odr -> multiply                  ;
          int pp = items -> PIPs   [order]          ;
          switch ( odr -> cmd )                     {
            case OP_BUY                             :
              pp  = odr -> mClosePrice() - pp       ;
              sl  = pp                              ;
              sl /= mp                              ;
            break                                   ;
            case OP_SELL                            :
              pp  = odr -> mClosePrice() + pp       ;
              sl  = pp                              ;
              sl /= mp                              ;
            break                                   ;
          }                                         ;
        }                                           ;
        sb -> setValue      ( sl )                  ;
      }                                             ;
    break                                           ;
    case  7                                         :
      if ( ( 1 == LimitValues [ 110001 ] )         &&
           items -> orders . contains ( order ) )   {
        Finance::Order * odr                        ;
        odr = items -> orders [order]               ;
        int D = odr -> digits                       ;
        int M = odr -> multiply                     ;
        double st = 1                               ;
        double tp                                   ;
        QDoubleSpinBox * sb                         ;
        tp  = odr -> tp                             ;
        st /= M                                     ;
        sb  = setDoubleSpin                         (
                item                                ,
                column                              ,
                0                                   ,
                100000.0                            ,
                SIGNAL ( editingFinished ( ) )      ,
                SLOT   ( profitsChanged  ( ) )    ) ;
        sb -> setDecimals   ( D  )                  ;
        sb -> setSingleStep ( st )                  ;
        if ( tp < 0.00001 )                         {
          int mp = odr -> multiply                  ;
          int pp = items -> PIPs   [order]          ;
          switch ( odr -> cmd )                     {
            case OP_BUY                             :
              pp  = odr -> mClosePrice() + pp       ;
              tp  = pp                              ;
              tp /= mp                              ;
            break                                   ;
            case OP_SELL                            :
              pp  = odr -> mClosePrice() - pp       ;
              tp  = pp                              ;
              tp /= mp                              ;
            break                                   ;
          }                                         ;
        }                                           ;
        sb -> setValue      ( tp )                  ;
      }                                             ;
    break                                           ;
    case 15                                         :
      if ( ( 1 == LimitValues [ 110001 ] )         &&
           items -> orders . contains ( order ) )   {
        QLineEdit * le                              ;
        le = setLineEdit                            (
               item                                 ,
               column                               ,
               SIGNAL ( editingFinished ( ) )       ,
               SLOT   ( commentChanged  ( ) )     ) ;
      }                                             ;
    break                                           ;
  }                                                 ;
}

N::Finance::OrderItems * N::ForexOrders::GetItems (void)
{
  return items ;
}

void N::ForexOrders::setOrderItem(Finance::OrderItems * it)
{
  items = it                                       ;
  nConnect ( items , SIGNAL ( Changed      ( ) )   ,
             this  , SLOT   ( ItemsChanged ( ) ) ) ;
}

void N::ForexOrders::ItemsChanged(void)
{
  bool first = items -> functions . isEnabled ( "First" ) ;
  if ( first )                                            {
    start ( 90002 )                                       ;
  } else                                                  {
    start ( 90001 )                                       ;
  }                                                       ;
}

void N::ForexOrders::profitsChanged(void)
{
  if (IsNull(ItemEditing)) return                          ;
  QDoubleSpinBox * sb = Casting(QDoubleSpinBox,ItemWidget) ;
  if ( IsNull ( sb ) ) return                              ;
  int    order = ItemEditing -> text ( 0 ) . toInt ( )     ;
  int    ic    = ItemColumn                                ;
  double price = sb -> value ( )                           ;
  removeOldItem ( false , ic )                             ;
  //////////////////////////////////////////////////////////
  switch ( ic )                                            {
    case 6                                                 :
      emit ModifyProfits ( Identifier                      ,
                           items -> login                  ,
                           order                           ,
                           price                           ,
                           -1                            ) ;
    break                                                  ;
    case 7                                                 :
      emit ModifyProfits ( Identifier                      ,
                           items->login                    ,
                           order                           ,
                           -1                              ,
                           price                         ) ;
    break                                                  ;
  }                                                        ;
}

void N::ForexOrders::commentChanged(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget)          ;
  if ( IsNull ( le ) ) return                             ;
  int     order   = ItemEditing -> text ( 0 ) . toInt ( ) ;
  int     ic      = ItemColumn                            ;
  QString comment = le -> text ( )                        ;
  removeOldItem ( false , ic )                            ;
  emit ModifyComment ( Identifier                         ,
                       items -> login                     ,
                       order                              ,
                       comment                          ) ;
}

void N::ForexOrders::LoadStyleSheet(void)
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

void N::ForexOrders::SaveStyleSheet(void)
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

bool N::ForexOrders::AssignEngine(QString filename)
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

bool N::ForexOrders::AssignEngine(void)
{
  if ( IsNull ( items ) ) return false           ;
  QString filename = GetPath ( )                 ;
  filename = QFileDialog::getOpenFileName        (
               this                              ,
               tr("Javascript for order filter") ,
               filename                          ,
               tr("Javascript (*.js)")         ) ;
  if ( filename . length ( ) <= 0 ) return false ;
  KeepPath ( filename )                          ;
  return AssignEngine ( filename )               ;
}

void N::ForexOrders::KeepPath(QString path)
{
  QFileInfo F ( path )                    ;
  QString   p = F . absolutePath ( )      ;
  plan -> Variables [ "ForexOrders" ] = p ;
}

QString N::ForexOrders::GetPath(void)
{
  QString p ;
  if ( plan  -> Variables . contains ( "ForexOrders" ) )   {
    p = plan -> Variables [ "ForexOrders" ] . toString ( ) ;
  } else                                                   {
    p = plan -> Temporary ( "" )                           ;
  }                                                        ;
  return p                                                 ;
}

bool N::ForexOrders::Menu(QPoint pos)
{
  if ( 0 == LimitValues [ 100001 ] ) return false                            ;
  nScopedMenu ( mm , this )                                                  ;
  QAction         * aa                                                       ;
  QTreeWidgetItem * it  = itemAt ( pos )                                     ;
  int               cc  = currentColumn ( )                                  ;
  QMenu           * mc                                                       ;
  QMenu           * ms                                                       ;
  QMenu           * ma                                                       ;
  QMenu           * mf                                                       ;
  QMenu           * mg                                                       ;
  QLineEdit       * jfunc = new QLineEdit ( )                                ;
  QSpinBox        * pips  = new QSpinBox  ( )                                ;
  QString           m                                                        ;
  ////////////////////////////////////////////////////////////////////////////
  m = tr("Javascript function name")                                         ;
  jfunc -> setPlaceholderText     ( m        )                               ;
  jfunc -> setToolTip             ( m        )                               ;
  m = tr("Trace pips for stop loss and take profit")                         ;
  pips  -> setToolTip             ( m        )                               ;
  pips  -> setValue               ( 0        )                               ;
  pips  -> setRange               ( 0 , 1000 )                               ;
  if ( items -> variables . contains ( "Java"  ) )                           {
    m = items -> variables [ "Java"  ] . toString ( )                        ;
    jfunc -> setText ( m )                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NULL != it )                                                          {
    int O = it -> text ( 0 ) . toInt ( )                                     ;
    if ( items -> PIPs       . contains ( O ) )                              {
      pips -> setValue ( items -> PIPs [ O ] )                               ;
    }                                                                        ;
    if ( items -> jsFunction . contains ( O ) )                              {
      jfunc -> setText ( items -> jsFunction [ O ] )                         ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( NULL != it ) && ( 0 != LimitValues [ 100002 ] ) )                   {
    int O = it -> text ( 0 ) . toInt ( )                                     ;
    int L = 0                                                                ;
    if ( items -> orders . contains ( O ) )                                  {
      L = items -> orders [ O ] -> login                                     ;
    }                                                                        ;
    if ( L > 0 )                                                             {
      m   = tr("%1 / %2 (%3)").arg(O).arg(it->text(4)).arg(L)                ;
      aa  = mm . add     ( 0 , m )                                           ;
      aa -> setEnabled   ( false )                                           ;
      mm  . addSeparator (       )                                           ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( NULL != items ) && ( items -> login > 0 ) )                         {
    mm   . add ( 101 , tr("New order"  ) )                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( NULL != it ) && ( 0 != LimitValues [ 100002 ] ) )                   {
    int orderId = it -> text ( 0 ) . toInt ( )                               ;
    m   = tr("Close order")                                                  ;
    m  += QString(" %1").arg(orderId)                                        ;
    mm  . add          ( 102 , m )                                           ;
    m   = tr("Modify or delete order")                                       ;
    m  += QString(" %1").arg(orderId)                                        ;
    mm  . add          ( 103 , m )                                           ;
  }                                                                          ;
  if ( ( topLevelItemCount ( ) > 0 ) && ( 0 != LimitValues [ 100002 ] ) )    {
    m   = tr("Close all orders in %1").arg(items->login)                     ;
    mm  . add          ( 104 , m )                                           ;
  }                                                                          ;
  mm . addSeparator ( )                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( NULL != items ) && ( 0 != LimitValues [ 100003 ] ) )                {
    bool tracetp = false                                                     ;
    if ( NULL != it )                                                        {
      int O = it -> text ( 0 ) . toInt ( )                                   ;
      if ( items -> PIPs . contains ( O ) && ( items -> PIPs [ O ] > 0 ) )   {
        tracetp = true                                                       ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    mg = mm . addMenu ( tr("Expert advisors")       )                        ;
    mm . add ( mg , 501 , tr("Display order duration") )                     ;
    mm . addSeparator ( mg                          )                        ;
    mm . add ( mg , 502 , tr("Trace sl/tp") , true , tracetp )               ;
    mm . add ( mg , 511 , pips                      )                        ;
    mm . addSeparator ( mg                          )                        ;
    if ( items -> hasEngine ( ) )                                            {
      mm . add ( mg , 504 , tr("Javascript"     )   )                        ;
    }                                                                        ;
    mm . add ( mg , 505 , tr("Assign Javascript")   )                        ;
    mm . add ( mg , 512 , jfunc                     )                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100004 ] )                                         {
    mf   = mm . addMenu ( tr("Configurations") )                             ;
    mm   . add ( mf , 411 , tr("Load stylesheet"  ) )                        ;
    if ( styleSheet ( ) . length ( ) > 0 )                                   {
      mm . add ( mf , 412 , tr("Save stylesheet"  ) )                        ;
    }                                                                        ;
    mm   . add ( mf , 413 , tr("Assign stylesheet") )                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100005 ] )                                         {
    mc = mm . addMenu ( tr("Columns"    )           )                        ;
    mm . add ( mc , 901 , tr("Resize columns")      )                        ;
    mm . add ( mc , 902 , tr("All columns"   )      )                        ;
    mm . addSeparator ( mc                          )                        ;
    for (int i=0;i<columnCount();i++)                                        {
      mm . add ( mc                                                          ,
                 30000 + i                                                   ,
                 names [ i ]                                                 ,
                 true                                                        ,
                 ! isColumnHidden ( i )                                    ) ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100006 ] )                                         {
    ms = mm . addMenu ( tr("Sorting"    )                                  ) ;
    SortingMenu ( mm , ms   )                                                ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100007 ] )                                         {
    ma = mm . addMenu ( tr("Adjustments")                                  ) ;
    AdjustMenu  ( mm , ma   )                                                ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( 0 != LimitValues [ 100008 ] )                                         {
    DockingMenu ( mm        )                                                ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  aa = mm . exec ( )                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( it ) )                                                      {
    int O = it -> text ( 0 ) . toInt ( )                                     ;
    if ( pips -> value ( ) > 0 )                                             {
      items -> PIPs [ O ] = pips -> value ( )                                ;
    } else                                                                   {
      items -> PIPs . remove ( O )                                           ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  m = jfunc -> text ( )                                                      ;
  if ( NotNull ( it ) )                                                      {
    int O = it -> text ( 0 ) . toInt ( )                                     ;
    if ( m . length ( ) > 0 )                                                {
      items -> jsFunction [ O ] = m                                          ;
    } else                                                                   {
      items -> jsFunction . remove ( O )                                     ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull        (      aa ) ) return true                               ;
  if ( RunAdjustment ( mm , aa ) ) return true                               ;
  if ( RunSorting    ( mm , aa ) ) return true                               ;
  if ( RunDocking    ( mm , aa ) ) return true                               ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 101                                                                 :
      emit NewOrder ( Identifier , items -> login )                          ;
    break                                                                    ;
    case 102                                                                 :
      if ( items -> orders . contains ( it->text(0).toInt() ) )              {
        int O = it    -> text ( 0 ) . toInt ( )                              ;
        int L = items -> orders [ O ] -> login                               ;
        emit CloseOrder  ( Identifier , L , O )                              ;
      }                                                                      ;
    break                                                                    ;
    case 103                                                                 :
      if ( items -> orders . contains ( it->text(0).toInt() ) )              {
        int O = it    -> text ( 0 ) . toInt ( )                              ;
        int L = items -> orders [ O ] -> login                               ;
        emit ModifyOrder ( Identifier , L , O )                              ;
      }                                                                      ;
    break                                                                    ;
    case 104                                                                 :
      emit CloseAllOrders ( Identifier , items -> login )                    ;
    break                                                                    ;
    case 411                                                                 :
      LoadStyleSheet    (                                                  ) ;
    break                                                                    ;
    case 412                                                                 :
      SaveStyleSheet    (                                                  ) ;
    break                                                                    ;
    case 413                                                                 :
      assignStyleSheet  ( windowTitle () , this                            ) ;
    break                                                                    ;
    case 501                                                                 :
      if ( items -> orders . contains ( it->text(0).toInt() ) )              {
        int O = it    -> text ( 0 ) . toInt ( )                              ;
        int L = items -> orders [ O ] -> login                               ;
        emit DisplayOrder ( Identifier , L , O )                             ;
      }                                                                      ;
    break                                                                    ;
    case 504                                                                 :
      if ( NotNull(items) )                                                  {
        if ( ! items -> hasEngine ( ) ) AssignEngine ( )                     ;
      }                                                                      ;
    break                                                                    ;
    case 505                                                                 :
      if ( AssignEngine ( ) )                                                {
        if ( items -> variables . contains ( "Java" ) )                      {
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case 901                                                                 :
      emit AutoFit ( )                                                       ;
    break                                                                    ;
    case 902                                                                 :
      for (int i=0;i<columnCount();i++) setColumnHidden ( i , false )        ;
      emit AutoFit ( )                                                       ;
    break                                                                    ;
    case 30000                                                               :
    case 30001                                                               :
    case 30002                                                               :
    case 30003                                                               :
    case 30004                                                               :
    case 30005                                                               :
    case 30006                                                               :
    case 30007                                                               :
    case 30008                                                               :
    case 30009                                                               :
    case 30010                                                               :
    case 30011                                                               :
    case 30012                                                               :
    case 30013                                                               :
    case 30014                                                               :
    case 30015                                                               :
    case 30016                                                               :
    case 30017                                                               :
      setColumnHidden ( mm [ aa ] - 30000 , ! aa->isChecked()              ) ;
      emit AutoFit ( )                                                       ;
    break                                                                    ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}
