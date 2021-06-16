#include <qtforex.h>

N::Finance::OrderItems:: OrderItems ( QObject * parent )
                       : QObject    (           parent )
                       , Orders     (                  )
                       , engine     ( NULL             )
{
  functions . setEnabled ( "Trading"     , true  ) ;
  functions . setEnabled ( "Buy"         , true  ) ;
  functions . setEnabled ( "Sell"        , true  ) ;
  functions . setEnabled ( "Buy Limit"   , true  ) ;
  functions . setEnabled ( "Sell Limit"  , true  ) ;
  functions . setEnabled ( "Buy Stop"    , true  ) ;
  functions . setEnabled ( "Sell Stop"   , true  ) ;
  functions . setEnabled ( "Balance"     , false ) ;
  functions . setEnabled ( "Credit"      , false ) ;
  functions . setEnabled ( "DeleteItems" , false ) ;
  functions . setEnabled ( "Render"      , false ) ;
  functions . setEnabled ( "First"       , true  ) ;
  variables [ "SL" ]  = QColor ( 240 , 192 , 192 ) ;
  variables [ "TP" ]  = QColor ( 192 , 240 , 192 ) ;
  nConnect ( this , SIGNAL ( EmitChanged ( ) )     ,
             this , SLOT   ( SendChanged ( ) )   ) ;
}

N::Finance::OrderItems::~OrderItems (void)
{
}

int N::Finance::OrderItems::StopsLevel(QString s)
{
  QString k = QString("PIPs[%1]").arg(s)        ;
  if ( ! variables . contains ( k ) ) return 10 ;
  return variables [ k ] . toInt ( )            ;
}

#ifdef QT_QTMTAPI_LIB

bool N::Finance::OrderItems::isCorrect(TradeRecord & trade)
{
  if ( 0             == login ) return false           ;
  if ( trade . login != login ) return false           ;
  if ( functions . isEnabled ( "Buy"               ) ) {
    if ( OP_BUY        == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Sell"              ) ) {
    if ( OP_SELL       == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Buy Limit"         ) ) {
    if ( OP_BUY_LIMIT  == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Sell Limit"        ) ) {
    if ( OP_SELL_LIMIT == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Buy Stop"          ) ) {
    if ( OP_BUY_STOP   == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Sell Stop"         ) ) {
    if ( OP_SELL_STOP  == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Balance"           ) ) {
    if ( OP_BALANCE    == trade . cmd ) return true    ;
  }                                                    ;
  if ( functions . isEnabled ( "Credit"            ) ) {
    if ( OP_CREDIT     == trade . cmd ) return true    ;
  }                                                    ;
  return false                                         ;
}

N::Finance::Orders & N::Finance::OrderItems::assign(int total,TradeRecord * trade)
{
  if ( functions . isEnabled("Trading") )   {
    return Orders::assign ( total , trade ) ;
  }                                         ;
  ///////////////////////////////////////////
  CUIDs   keys = orders . keys ( )          ;
  CUIDs   mews                              ;
  int     id                                ;
  ///////////////////////////////////////////
  if ( total <= 0 )                         {
    foreach ( id , keys )                   {
      enabled [ id ] = 0                    ;
    }                                       ;
    return (*this)                          ;
  }                                         ;
  ///////////////////////////////////////////
  for (int i=0;i<total;i++)                 {
    if ( isCorrect ( trade [ i ] ) )        {
      id = trade [ i ] . order              ;
      assure ( id )                         ;
      (*orders[id]) = trade [ i ]           ;
      mews << id                            ;
      enabled [ id ] = 1                    ;
    }                                       ;
  }                                         ;
  ///////////////////////////////////////////
  foreach ( id , keys )                     {
    if ( ! mews . contains ( id ) )         {
      enabled [ id ] = 0                    ;
    }                                       ;
  }                                         ;
  ///////////////////////////////////////////
  return (*this)                            ;
}

#endif

int N::Finance::OrderItems::take(int order)
{
  if ( items . contains ( order ) )        {
    QTreeWidgetItem * it = items [ order ] ;
    items . remove ( order )               ;
    if ( orderItems . contains ( it ) )    {
      orderItems . remove ( it )           ;
    }                                      ;
  }                                        ;
  if ( visible.contains(order))            {
    visible . remove ( order )             ;
  }                                        ;
  if ( isJava.contains(order))             {
    isJava . remove ( order )              ;
  }                                        ;
  if ( jsFunction . contains ( order ) )   {
    jsFunction . remove ( order )          ;
  }                                        ;
  if ( PIPs . contains ( order ) )         {
    PIPs . remove ( order )                ;
  }                                        ;
  if ( enabled . contains ( order ) )      {
    enabled . remove ( order )             ;
  }                                        ;
  return Orders::take ( order )            ;
}

void N::Finance::OrderItems::operate(void)
{
  functions . setEnabled ( "Updated" , true ) ;
  emit EmitChanged ( )                        ;
}

bool N::Finance::OrderItems::hasEngine(void)
{
  return NotNull ( engine ) ;
}

QScriptEngine * N::Finance::OrderItems::Engine (void)
{
  return engine ;
}

void N::Finance::OrderItems::setEngine(QScriptEngine * e)
{
  if ( NULL != engine ) delete engine ;
  engine = e                          ;
}

void N::Finance::OrderItems::assure(int order)
{
  if ( orders . contains ( order ) ) return            ;
  if ( items  . contains ( order ) ) return            ;
  QTreeWidgetItem * it                                 ;
  int align = Qt::AlignRight | Qt::AlignVCenter        ;
  it                 = new QTreeWidgetItem  ( )        ;
  orders  [ order ]  = new Order            ( )        ;
  items   [ order ]  = it                              ;
  enabled [ order ]  = 0                               ;
  items   [ order ] -> setTextAlignment (  0 , align ) ;
  items   [ order ] -> setTextAlignment (  1 , align ) ;
  items   [ order ] -> setTextAlignment (  3 , align ) ;
  items   [ order ] -> setTextAlignment (  5 , align ) ;
  items   [ order ] -> setTextAlignment (  6 , align ) ;
  items   [ order ] -> setTextAlignment (  7 , align ) ;
  items   [ order ] -> setTextAlignment (  8 , align ) ;
  items   [ order ] -> setTextAlignment (  9 , align ) ;
  items   [ order ] -> setTextAlignment ( 10 , align ) ;
  items   [ order ] -> setTextAlignment ( 11 , align ) ;
  items   [ order ] -> setTextAlignment ( 12 , align ) ;
  items   [ order ] -> setTextAlignment ( 13 , align ) ;
  items   [ order ] -> setTextAlignment ( 14 , align ) ;
}

QString N::Finance::OrderItems::TradeString (int cmd)
{
  switch ( cmd )            {
    case OP_BUY             :
    return tr("Buy")        ;
    case OP_SELL            :
    return tr("Sell")       ;
    case OP_BUY_LIMIT       :
    return tr("Buy Limit")  ;
    case OP_SELL_LIMIT      :
    return tr("Sell Limit") ;
    case OP_BUY_STOP        :
    return tr("Buy Stop")   ;
    case OP_SELL_STOP       :
    return tr("Sell Stop")  ;
    case OP_BALANCE         :
    return tr("Balance")    ;
    case OP_CREDIT          :
    return tr("Credit")     ;
  }                         ;
  return ""                 ;
}

QString N::Finance::OrderItems::Lots(int volume)
{
  int     l = volume / 100             ;
  int     m = volume % 100             ;
  QString s                            ;
  s = QString::number ( l )            ;
  s . append ( "."                   ) ;
  s . append ( QString::number(m/10) ) ;
  s . append ( QString::number(m%10) ) ;
  return s                             ;
}

QString N::Finance::OrderItems::MoneyString(double v)
{
  QString sign = ""                    ;
  qint64  m                            ;
  qint64  d                            ;
  QString s                            ;
  if ( v < 0 )                         {
    sign = "-"                         ;
    v    = -v                          ;
  }                                    ;
  m = (qint64) ( v * 100 )             ;
  d = m / 100                          ;
  m = m % 100                          ;
  s = QString::number ( d )            ;
  s . append ( "."                   ) ;
  s . append ( QString::number(m/10) ) ;
  s . append ( QString::number(m%10) ) ;
  if ( sign . length ( ) > 0 )         {
    s . prepend ( sign )               ;
  }                                    ;
  return s                             ;
}

void N::Finance::OrderItems::toItem(int order)
{
  if ( ! orders . contains ( order ) ) return                       ;
  if ( ! items  . contains ( order ) ) return                       ;
  ///////////////////////////////////////////////////////////////////
  Order           * u         = orders    [ order      ]            ;
  QTreeWidgetItem * it        = items     [ order      ]            ;
  int               tz        = variables [ "TimeZone" ] . toInt () ;
  time_t            ot_t      = u -> OpenTime  . toTime_t ( ) - tz  ;
  time_t            ct_t      = u -> CloseTime . toTime_t ( )       ;
  if ( ct_t > 0 ) ct_t -= tz                                        ;
  if ( ct_t < 0 ) ct_t  = 0                                         ;
  if ( ! PIPs . contains ( order ) )                                {
    PIPs [ order ] = StopsLevel ( u -> symbol )                     ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  QDateTime OpenTime  = QDateTime::fromTime_t ( ot_t )              ;
  QDateTime CloseTime = QDateTime::fromTime_t ( ct_t )              ;
  QString   symbol    = u -> symbol                                 ;
  time_t    ct        = CloseTime.toTime_t ( )                      ;
  QString   CT        = ""                                          ;
  QString   duration  = ""                                          ;
  QString   js        = ""                                          ;
  qint64    dt        = OpenTime . secsTo ( nTimeNow )              ;
  qint64    days      = dt / 86400                                  ;
  QTime     dT ( 0 , 0 , 0 , 0 )                                    ;
  QString   DT                                                      ;
  ///////////////////////////////////////////////////////////////////
  if ( ! functions . isEnabled ( "Trading" ) )                      {
    dt   = OpenTime . secsTo ( CloseTime )                          ;
    days = dt / 86400                                               ;
  }                                                                 ;
  if ( 0 != ct ) CT   = CloseTime.toString( "yyyy.MM.dd hh:mm:ss" ) ;
  if ( jsFunction . contains ( order ) ) js = jsFunction [ order ]  ;
  if ( dt < 0 ) dt = 0                                              ;
  dt       = dt % 86400                                             ;
  dT       = dT . addSecs ( dt )                                    ;
  DT       = dT.toString("hh:mm:ss")                                ;
  if ( days > 0 ) duration = tr("%1 days %2").arg(days).arg(DT)     ;
             else duration = DT                                     ;
  ///////////////////////////////////////////////////////////////////
  it -> setData (  2 , Qt::UserRole , u -> cmd                    ) ;
  ///////////////////////////////////////////////////////////////////
  it -> setText (  0 , QString::number  ( order           )       ) ;
  it -> setText (  1 , OpenTime . toString("yyyy.MM.dd hh:mm:ss") ) ;
  it -> setText (  2 , TradeString      ( u -> cmd        )       ) ;
  it -> setText (  3 , u -> LotString   (                 )       ) ;
  it -> setText (  4 , symbol                                     ) ;
  it -> setText (  5 , u -> PriceString ( u -> OpenPrice  )       ) ;
  it -> setText (  6 , u -> PriceString ( u -> sl         )       ) ;
  it -> setText (  7 , u -> PriceString ( u -> tp         )       ) ;
  it -> setText (  8 , CT                                         ) ;
  it -> setText (  9 , u -> PriceString ( u -> ClosePrice )       ) ;
  it -> setText ( 10 , duration                                   ) ;
  it -> setText ( 11 , MoneyString      ( u -> commission )       ) ;
  it -> setText ( 12 , MoneyString      ( u -> taxes      )       ) ;
  it -> setText ( 13 , MoneyString      ( u -> storage    )       ) ;
  it -> setText ( 14 , MoneyString      ( u -> profit     )       ) ;
  it -> setText ( 15 , u -> comment                               ) ;
  it -> setText ( 16 , js                                         ) ;
  ///////////////////////////////////////////////////////////////////
  QBrush brush = QBrush ( QColor ( 0 , 0 , 0 ) )                    ;
  if ( u -> profit > 0 )                                            {
    brush = QBrush ( QColor (   0 , 0 , 255 ) )                     ;
  } else
  if ( u -> profit < 0 )                                            {
    brush = QBrush ( QColor ( 255 , 0 ,   0 ) )                     ;
  }                                                                 ;
  it -> setForeground (  5 , brush )                                ;
  it -> setForeground (  9 , brush )                                ;
  it -> setForeground ( 14 , brush )                                ;
  ///////////////////////////////////////////////////////////////////
  if ( functions . isEnabled ( "Trading" ) )                        {
    QColor slc = variables [ "SL" ] . value <QColor> ( )            ;
    QColor tpc = variables [ "TP" ] . value <QColor> ( )            ;
    QBrush dbb = it -> background ( 0     )                         ;
    QBrush slb = QBrush           ( slc   )                         ;
    QBrush tpb = QBrush           ( tpc   )                         ;
    int    pip = PIPs             [ order ]                         ;
    int    lip = u -> mClosePrice ( ) + pip                         ;
    int    hip = u -> mClosePrice ( ) - pip                         ;
    it -> setBackground ( 6 , dbb )                                 ;
    it -> setBackground ( 7 , dbb )                                 ;
    switch ( u -> cmd )                                             {
      case OP_BUY                                                   :
        if ( u -> mSL ( ) > 0 )                                     {
          if ( u -> mSL () >= hip ) it -> setBackground ( 6 , slb ) ;
        }                                                           ;
        if ( u -> mTP ( ) > 0 )                                     {
          if ( u -> mTP () <= lip ) it -> setBackground ( 7 , tpb ) ;
        }                                                           ;
      break                                                         ;
      case OP_SELL                                                  :
        if ( u -> mSL ( ) > 0 )                                     {
          if ( u -> mSL () <= lip ) it -> setBackground ( 6 , slb ) ;
        }                                                           ;
        if ( u -> mTP ( ) > 0 )                                     {
          if ( u -> mTP () >= hip ) it -> setBackground ( 7 , tpb ) ;
        }                                                           ;
      break                                                         ;
      case OP_BUY_LIMIT                                             :
      break                                                         ;
      case OP_SELL_LIMIT                                            :
      break                                                         ;
      case OP_BUY_STOP                                              :
      break                                                         ;
      case OP_SELL_STOP                                             :
      break                                                         ;
    }                                                               ;
  }                                                                 ;
}

void N::Finance::OrderItems::SendChanged (void)
{
  emit Changed ( ) ;
}

void N::Finance::OrderItems::Sync(QTreeWidget * tree)
{
  CUIDs             L = orders . keys ( )        ;
  int               order                        ;
  TreeWidgetItems   twi                          ;
  TreeWidgetItems   rwi                          ;
  QTreeWidgetItem * it                           ;
  foreach ( order , L )                          {
    it = items [ order ]                         ;
    if ( 0 == enabled [ order ] )                {
      if (   orderItems . contains ( it ) )      {
        rwi << it                                ;
        orderItems . remove ( it )               ;
        visible [ order ] = false                ;
      }                                          ;
    } else                                       {
      if ( ! orderItems . contains ( it ) )      {
        orderItems [ it    ] = order             ;
        visible    [ order ] = true              ;
        twi << it                                ;
      } else                                     {
        if ( ! visible [ order ] )               {
          twi << it                              ;
        }                                        ;
        visible [ order ] = true                 ;
      }                                          ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  remove ( tree , rwi )                          ;
  add    ( tree , twi )                          ;
}

void N::Finance::OrderItems::add(QTreeWidget * tree,TreeWidgetItems it)
{
  if ( NULL == tree        ) return             ;
  if ( it . count ( ) > 0 )                     {
    TreeWidgetItems x = it                      ;
    TreeWidgetItems y                           ;
    while ( x . count ( ) > 0 )                 {
      while ( ( x . count ( ) >  0 )           &&
              ( y . count ( ) < 50 )          ) {
        y << x . first (   )                    ;
        x . takeAt     ( 0 )                    ;
      }                                         ;
      if ( y . count ( ) > 0 )                  {
        tree -> addTopLevelItems ( y )          ;
        qApp -> processEvents    (   )          ;
        y . clear ( )                           ;
      }                                         ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  int     total = tree -> topLevelItemCount ( ) ;
  QString s     = tr("%1 orders").arg(total)    ;
  tree -> setToolTip ( s )                      ;
}

void N::Finance::OrderItems::remove(QTreeWidget * tree,TreeWidgetItems it)
{
  if ( NULL == tree        ) return             ;
  if ( it . count ( ) <= 0 ) return             ;
  ///////////////////////////////////////////////
  int               index                       ;
  QTreeWidgetItem * itx                         ;
  for (int i=0;i<it.count();i++)                {
    itx = it [ i ]                              ;
    index = tree -> indexOfTopLevelItem ( itx ) ;
    if ( index >= 0 )                           {
      tree -> takeTopLevelItem ( index )        ;
    }                                           ;
    // delete itx ;
    if ( 99 == ( i % 100 ) )                    {
      qApp -> processEvents ( )                 ;
    }                                           ;
  }                                             ;
}

void N::Finance::OrderItems::Cleanup(void)
{
  CUIDs keys = orders . keys ( )    ;
  CUIDs ds                          ;
  int   id                          ;
  ///////////////////////////////////
  foreach ( id , keys )             {
    if ( 0 == enabled [ id ] )      {
      ds << id                      ;
    }                               ;
  }                                 ;
  ///////////////////////////////////
  if ( ds . count ( ) <= 0 ) return ;
  ///////////////////////////////////
  foreach ( id , ds ) take ( id )   ;
}

void N::Finance::OrderItems::Render(void)
{
  CUIDs L = orders . keys ( )                  ;
  int   c = 0                                  ;
  int   order                                  ;
  foreach ( order , L )                        {
    c++                                        ;
    toItem ( order )                           ;
    if ( 0 == ( c % 10 ) )                     {
      qApp -> processEvents ( )                ;
    }                                          ;
  }                                            ;
  functions . setEnabled ( "Updated" , false ) ;
  functions . setEnabled ( "Render"  , false ) ;
}

void N::Finance::OrderItems::runEngine(QScriptValue & api,QString symbol,QDateTime CTM,double bid,double ask)
{
  if ( IsNull ( engine )  ) return                    ;
  CUIDs L = orders . keys ( )                         ;
  if ( L . count ( ) <= 0 ) return                    ;
  int order                                           ;
  foreach ( order , L ) if ( 1 == enabled [ order ] ) {
    CallScript(api,order,symbol,CTM,bid,ask)          ;
  }                                                   ;
  /////////////////////////////////////////////////////
  engine -> collectGarbage (            )             ;
}

bool N::Finance::OrderItems::CallScript (
       QScriptValue & api               ,
       int            order             ,
       QString        symbol            ,
       QDateTime      CTM               ,
       double         bid               ,
       double         ask               )
{
  Order * o = orders [ order ]                          ;
  if ( NULL   == o                       ) return false ;
  if ( symbol != o -> symbol             ) return false ;
  if ( login  != o -> login              ) return false ;
  if ( ! isJava . contains     ( order ) ) return false ;
  if ( ! isJava                [ order ] ) return false ;
  if ( ! jsFunction . contains ( order ) ) return false ;
  ///////////////////////////////////////////////////////
  QString          js = jsFunction [ order ]            ;
  if ( js . length ( ) <= 0              ) return false ;
  QScriptValue global = engine -> globalObject (      ) ;
  QScriptValue eval   = global  . property     ( js   ) ;
  if ( ! eval . isFunction ( )           ) return false ;
  ///////////////////////////////////////////////////////
  QScriptValue     r                                    ;
  QScriptValueList args                                 ;
  bool             result = false                       ;
  OrderTick      * ot     = new OrderTick ( NULL , *o ) ;
  ot -> bid = bid                                       ;
  ot -> ask = ask                                       ;
  args  << api                                          ;
  args  << engine -> newQObject ( ot )                  ;
  ///////////////////////////////////////////////////////
  r      = eval . call   ( QScriptValue ( ) , args )    ;
  result = r    . toBool (                         )    ;
  ///////////////////////////////////////////////////////
  delete ot                                             ;
  return result                                         ;
}
