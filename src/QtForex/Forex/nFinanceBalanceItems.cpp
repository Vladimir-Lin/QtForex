#include <qtforex.h>

N::Finance::BalanceItems:: BalanceItems ( QObject * parent )
                         : QObject      (           parent )
                         , Orders       (                  )
                         , sum          ( NULL             )
                         , engine       ( NULL             )
{
  functions . setEnabled ( "Trading"     , false ) ;
  functions . setEnabled ( "Buy"         , false ) ;
  functions . setEnabled ( "Sell"        , false ) ;
  functions . setEnabled ( "Buy Limit"   , false ) ;
  functions . setEnabled ( "Sell Limit"  , false ) ;
  functions . setEnabled ( "Buy Stop"    , false ) ;
  functions . setEnabled ( "Sell Stop"   , false ) ;
  functions . setEnabled ( "Balance"     , true  ) ;
  functions . setEnabled ( "Credit"      , false ) ;
  functions . setEnabled ( "DeleteItems" , false ) ;
  functions . setEnabled ( "Render"      , false ) ;
  functions . setEnabled ( "First"       , true  ) ;
  //////////////////////////////////////////////////
  nConnect ( this , SIGNAL ( EmitChanged ( ) )     ,
             this , SLOT   ( SendChanged ( ) )   ) ;
  //////////////////////////////////////////////////
  sum = new TableWidgetItems ( )                   ;
  for (int i=0;i<8;i++)                            {
    (*sum) << new QTableWidgetItem ( )             ;
  }                                                ;
  //////////////////////////////////////////////////
  int align = Qt::AlignVCenter | Qt::AlignRight    ;
  for (int i=3;i<=6;i++)                           {
    (*sum) [ i ] -> setTextAlignment ( align )     ;
  }                                                ;
}

N::Finance::BalanceItems::~BalanceItems (void)
{
  for (int i=0;i<8;i++) delete (*sum)[i] ;
  delete sum                             ;
  sum = NULL                             ;
}

#ifdef QT_QTMTAPI_LIB

bool N::Finance::BalanceItems::isCorrect(TradeRecord & trade)
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

N::Finance::Orders & N::Finance::BalanceItems::assign(int total,TradeRecord * trade)
{
  CUIDs   keys = orders . keys ( )      ;
  CUIDs   mews                          ;
  int     id                            ;
  ///////////////////////////////////////
  if ( total <= 0 )                     {
    foreach ( id , keys )               {
      enabled [ id ] = 0                ;
    }                                   ;
    return (*this)                      ;
  }                                     ;
  ///////////////////////////////////////
  for (int i=0;i<total;i++)             {
    if ( isCorrect ( trade [ i ] ) )    {
      id = trade [ i ] . order          ;
      assure ( id )                     ;
      (*orders[id]) = trade [ i ]       ;
      mews << id                        ;
      enabled [ id ] = 1                ;
    }                                   ;
  }                                     ;
  ///////////////////////////////////////
  foreach ( id , keys )                 {
    if ( ! mews . contains ( id ) )     {
      enabled [ id ] = 0                ;
    }                                   ;
  }                                     ;
  ///////////////////////////////////////
  return (*this)                        ;
}

#endif

void N::Finance::BalanceItems::assure(int order)
{
  if ( orders . contains ( order ) ) return            ;
  if ( items  . contains ( order ) ) return            ;
  //////////////////////////////////////////////////////
  TableWidgetItems * twi = new TableWidgetItems ( )    ;
  for (int i=0;i<8;i++)                                {
    (*twi) << new QTableWidgetItem ( )                 ;
  }                                                    ;
  //////////////////////////////////////////////////////
  orders  [ order ]  = new Order            ( )        ;
  items   [ order ]  = twi                             ;
  enabled [ order ]  = 0                               ;
  //////////////////////////////////////////////////////
  int align = Qt::AlignVCenter | Qt::AlignRight        ;
  for (int i=3;i<=6;i++)                               {
    (*twi) [ i ] -> setTextAlignment ( align )         ;
  }                                                    ;
}

int N::Finance::BalanceItems::take(int order)
{
  if ( items . contains ( order ) )         {
    TableWidgetItems * it = items [ order ] ;
    items . remove ( order )                ;
    if ( balanceItems . contains ( it ) )   {
      balanceItems . remove ( it )          ;
    }                                       ;
    for (int i=0;i<8;i++) delete (*it) [i]  ;
    delete it                               ;
  }                                         ;
  if ( visible.contains(order))             {
    visible . remove ( order )              ;
  }                                         ;
  if ( enabled . contains ( order ) )       {
    enabled . remove ( order )              ;
  }                                         ;
  return Orders::take ( order )             ;
}

void N::Finance::BalanceItems::operate(void)
{
  functions . setEnabled ( "Updated" , true ) ;
  emit EmitChanged ( )                        ;
}

QString N::Finance::BalanceItems::TradeString(int cmd,double profit)
{
  if ( OP_BALANCE != cmd ) return tr("Record"  ) ;
  if ( profit     <  0   ) return tr("Withdraw") ;
  if ( profit     >  0   ) return tr("Deposit" ) ;
  return                          tr("Nothing" ) ;
}

void N::Finance::BalanceItems::toItem(int order)
{
  if ( ! orders . contains ( order ) ) return                             ;
  if ( ! items  . contains ( order ) ) return                             ;
  /////////////////////////////////////////////////////////////////////////
  Order            * u        = orders [ order ]                          ;
  TableWidgetItems * it       = items  [ order ]                          ;
  /////////////////////////////////////////////////////////////////////////
  (*it)[0] -> setText ( QString::number ( order                       ) ) ;
  (*it)[1] -> setText ( u -> OpenTime . toString("yyyy.MM.dd hh:mm:ss") ) ;
  (*it)[2] -> setText ( TradeString     ( u -> cmd , u -> profit      ) ) ;
  (*it)[3] -> setText ( QString::number ( u -> commission             ) ) ;
  (*it)[4] -> setText ( QString::number ( u -> taxes                  ) ) ;
  (*it)[5] -> setText ( QString::number ( u -> storage                ) ) ;
  (*it)[6] -> setText ( QString::number ( u -> profit                 ) ) ;
  (*it)[7] -> setText ( u -> comment                                    ) ;
}

void N::Finance::BalanceItems::add(QTableWidget * table,QList<TableWidgetItems *> it)
{
  if ( NULL == table      ) return            ;
  if ( it . count ( ) > 0 )                   {
    int row   = table -> rowCount ( )         ;
    int total = it     . count    ( )         ;
    table -> setRowCount ( row + total )      ;
    for (int i=0;i<total;i++,row++)           {
      TableWidgetItems * ix = it [ i ]        ;
      for (int j=0;j<8;j++)                   {
        table -> setItem ( i , j , (*ix)[j] ) ;
      }                                       ;
    }                                         ;
  }                                           ;
  /////////////////////////////////////////////
  int     total = table -> rowCount ( )       ;
  QString s     = tr("%1 entries").arg(total) ;
  table -> setToolTip ( s )                   ;
}

void N::Finance::BalanceItems::remove(QTableWidget * table,QList<TableWidgetItems *> it)
{
  if ( NULL == table ) return                 ;
  if ( it . count ( ) > 0 )                   {
    for (int i=0;i<it.count();i++)            {
      TableWidgetItems * ix = it [ i ]        ;
      int                rr = (*ix)[0]->row() ;
      if ( rr >= 0 ) table -> removeRow (rr)  ;
    }                                         ;
  }                                           ;
  /////////////////////////////////////////////
  int     total = table -> rowCount ( )       ;
  QString s     = tr("%1 entries").arg(total) ;
  table -> setToolTip ( s )                   ;
}

bool N::Finance::BalanceItems::hasEngine(void)
{
  return NotNull ( engine ) ;
}

QScriptEngine * N::Finance::BalanceItems::Engine (void)
{
  return engine ;
}

void N::Finance::BalanceItems::setEngine(QScriptEngine * e)
{
  if ( NULL != engine ) delete engine ;
  engine = e                          ;
}

void N::Finance::BalanceItems::runEngine(QScriptValue & api,QString symbol,QDateTime CTM,double bid,double ask)
{
}

void N::Finance::BalanceItems::SendChanged (void)
{
  emit Changed ( ) ;
}

void N::Finance::BalanceItems::Sync(QTableWidget * table)
{
  double                    total = 0                 ;
  int                       rc    = 0                 ;
  CUIDs                     L     = orders . keys ( ) ;
  int                       order                     ;
  QList<TableWidgetItems *> twi                       ;
  QList<TableWidgetItems *> rwi                       ;
  TableWidgetItems       *  it                        ;
  foreach ( order , L )                               {
    it = items [ order ]                              ;
    if ( 0 == enabled [ order ] )                     {
      if (   balanceItems . contains ( it ) )         {
        rwi << it                                     ;
        balanceItems . remove ( it )                  ;
        visible [ order ] = false                     ;
      }                                               ;
    } else                                            {
      total += orders [ order ] -> profit             ;
      rc    ++                                        ;
      if ( ! balanceItems . contains ( it ) )         {
        balanceItems [ it    ] = order                ;
        visible    [ order ] = true                   ;
        twi << it                                     ;
      } else                                          {
        if ( ! visible [ order ] )                    {
          twi << it                                   ;
        }                                             ;
        visible [ order ] = true                      ;
      }                                               ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  remove ( table , rwi )                              ;
  add    ( table , twi )                              ;
  /////////////////////////////////////////////////////
  QString mx = 0                                      ;
  if ( total > 0 ) mx = tr("Deposit")                 ;
              else mx = tr("Profit" )                 ;
  if ( total < 0 ) total = -total                     ;
  (*sum)[2] -> setText ( mx                     )     ;
  (*sum)[6] -> setText ( QString::number(total) )     ;
  /////////////////////////////////////////////////////
  int c = rwi . count ( ) + twi . count ( )           ;
  if ( c > 0 )                                        {
    table -> setRowCount ( rc + 1 )                   ;
    for (int j=0;j<8;j++)                             {
      table -> setItem ( rc , j , (*sum)[j] )         ;
    }                                                 ;
  }                                                   ;
}

void N::Finance::BalanceItems::Cleanup(void)
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

void N::Finance::BalanceItems::Render(void)
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
