#include <qtforex.h>

N::TickTreeItems:: TickTreeItems   ( QObject * parent )
                 : QObject         (           parent )
                 , TickTransmitter (                  )
{
  setEnabled ( "Trend" , true ) ;
}

N::TickTreeItems::~TickTreeItems (void)
{
//  QString s                  ;
//  foreach ( s , Symbols )    {
//    delete SymbolItems [ s ] ;
//  }                          ;
  SymbolItems . clear ( )    ;
  Items       . clear ( )    ;
  Symbols     . clear ( )    ;
  Digits      . clear ( )    ;
  Multiply    . clear ( )    ;
  CTMs        . clear ( )    ;
  LOCALs      . clear ( )    ;
  BIDs        . clear ( )    ;
  ASKs        . clear ( )    ;
}

void N::TickTreeItems::Append(QString symbol)
{
  mutexz . lock   ( )                                         ;
  if ( ! Symbols . contains ( symbol ) )                      {
    NewTreeWidgetItem ( it )                                  ;
    Finance::Ticks ticks                                      ;
    Symbols    << symbol                                      ;
    Digits      [ symbol ] = 5                                ;
    Multiply    [ symbol ] = 100000                           ;
    CTMs        [ symbol ] = nTimeNow                         ;
    LOCALs      [ symbol ] = nTimeNow                         ;
    BIDs        [ symbol ] = 0                                ;
    ASKs        [ symbol ] = 0                                ;
    TICKs       [ symbol ] = ticks                            ;
    SymbolItems [ symbol ] = it                               ;
    Visiblity   [ symbol ] = true                             ;
    setEnabled  ( symbol , false )                            ;
    TICKs       [ symbol ] . setValue ( "Range" , 60 * 1000 ) ;
    TICKs       [ symbol ] . setValue ( "Highest" , true    ) ;
    TICKs       [ symbol ] . setValue ( "Lowest"  , true    ) ;
    Items << it                                               ;
    it -> setText ( 0 , symbol )                              ;
    for (int i = 1 ; i <= 8 ; i++ )                           {
      it -> setTextAlignment                                  (
        i                                                     ,
        Qt::AlignVCenter                                      |
        Qt::AlignRight                                      ) ;
    }                                                         ;
  }                                                           ;
  mutexz . unlock ( )                                         ;
}

void N::TickTreeItems::setDigits(QString symbol,int digits)
{
  int m = 1                  ;
  mutexz . lock   ( )        ;
  Digits [ symbol ] = digits ;
  for (int i=0;i<digits;i++) {
    m *= 10                  ;
  }                          ;
  Multiply [ symbol ] = m    ;
  mutexz . unlock ( )        ;
}

QString N::TickTreeItems::toDigits(QString symbol,double value)
{
  int     d = Digits   [ symbol ] ;
  int     m = Multiply [ symbol ] ;
  int     v = (int)( value * m )  ;
  QString s = QString::number(v)  ;
  d = s . length ( ) - d          ;
  if ( d > 0 )                    {
    s . insert  ( d , "." )       ;
  } else                          {
    s . prepend ( "0."    )       ;
  }                               ;
  return  s                       ;
}

void N::TickTreeItems::Hide(QString symbol)
{
  mutexz . lock   ( )          ;
  Visiblity [ symbol ] = false ;
  mutexz . unlock ( )          ;
}

void N::TickTreeItems::Quote(QString symbol,QDateTime CTM,double bid,double ask)
{
  Finance::Tick tick                                     ;
  mutexz . lock   ( )                                    ;
  setEnabled ( "Transmit" , true )                       ;
  ////////////////////////////////////////////////////////
  int m = Multiply [ symbol ]                            ;
  CTMs   [ symbol ] = CTM                                ;
  LOCALs [ symbol ] = nTimeNow                           ;
  BIDs   [ symbol ] = bid                                ;
  ASKs   [ symbol ] = ask                                ;
  ////////////////////////////////////////////////////////
  tick . Type       = Finance::Tick::MetaTrader          ;
  tick . symbol     = symbol                             ;
  tick . Time       = CTMs   [ symbol ] . toTime_t ( )   ;
  tick . Local      = LOCALs [ symbol ] . toTime_t ( )   ;
  tick . Bid        = bid                                ;
  tick . Ask        = ask                                ;
  tick . Mid        = ( tick . Bid + tick . Ask ) / 2    ;
  tick . BidVolume  = 0                                  ;
  tick . AskVolume  = 0                                  ;
  tick . Time      *= 1000                               ;
  tick . Local     *= 1000                               ;
  tick . iBid       = (int) ( ( tick . Bid * m ) + 0.5 ) ;
  tick . iAsk       = (int) ( ( tick . Ask * m ) + 0.5 ) ;
  tick . iMid       = (int) ( ( tick . Mid * m ) + 0.5 ) ;
  TICKs  [ symbol ] . add ( tick )                       ;
  ////////////////////////////////////////////////////////
  mutexz . unlock ( )                                    ;
}

void N::TickTreeItems::Update(void)
{
  QString s                                                  ;
  mutexz . lock   ( )                                        ;
  foreach ( s , Symbols )                                    {
    QTreeWidgetItem * it  = SymbolItems [ s                ] ;
    QString           BID = toDigits    ( s , BIDs [ s ]   ) ;
    QString           ASK = toDigits    ( s , ASKs [ s ]   ) ;
    double            pip = ASKs [ s ] - BIDs [ s ]          ;
    double            High                                   ;
    double            Low                                    ;
    double            Mid                                    ;
    double            mid                                    ;
    QString           Spread                                 ;
    QString           HIGH                                   ;
    QString           LOW                                    ;
    bool              up                                     ;
    QBrush            brush                                  ;
    int               ticos                                  ;
    QString           trend                                  ;
    TICKs [ s ] . operate ( )                                ;
    pip   *= Multiply [ s ]                                  ;
    pip   += 0.1                                             ;
    High   = TICKs [ s ] . value ( "High" ) . toDouble (   ) ;
    Low    = TICKs [ s ] . value ( "Low"  ) . toDouble (   ) ;
    Mid    = ( High       + Low        ) / 2                 ;
    mid    = ( ASKs [ s ] + BIDs [ s ] ) / 2                 ;
    up     = ( mid > Mid )                                   ;
    if ( High <      0.0 ) High = 0                          ;
    if ( Low  > 100000.0 ) Low  = 0                          ;
    HIGH   = toDigits ( s , High )                           ;
    LOW    = toDigits ( s , Low  )                           ;
    Spread = QString::number((int)pip)                       ;
    ticos  = TICKs [ s ] . count ( )                         ;
    trend  = up ? QChar ( (ushort) 0x21D1 )                  :
                  QChar ( (ushort) 0x21D3 )                  ;
    if ( isEnabled ( "Trend" ) )                             {
      brush  = up ? QBrush ( QColor (   0 ,   0 , 255 )    ) :
                    QBrush ( QColor ( 255 ,   0 ,   0 )    ) ;
    } else                                                   {
      brush  =      QBrush ( QColor (   0 ,   0 ,   0 )    ) ;
    }                                                        ;
    it -> setText ( 0 , s                                  ) ;
    it -> setText ( 1 , BID                                ) ;
    it -> setText ( 2 , ASK                                ) ;
    it -> setText ( 3 , HIGH                               ) ;
    it -> setText ( 4 , LOW                                ) ;
    it -> setText ( 5 , Spread                             ) ;
    it -> setText ( 6 , CTMs   [ s ] .toString("hh:mm:ss") ) ;
    it -> setText ( 7 , LOCALs [ s ] .toString("hh:mm:ss") ) ;
    it -> setText ( 8 , QString::number(ticos)             ) ;
    it -> setText ( 9 , trend                              ) ;
    for (int i=0;i<=9;i++)                                   {
      it -> setForeground ( i , brush      )                 ;
    }                                                        ;
  }                                                          ;
  setEnabled ( "Transmit" , false )                          ;
  mutexz . unlock ( )                                        ;
}

void N::TickTreeItems::Ready(void)
{
  mutexz . lock   (                ) ;
  setEnabled      ( "Ready" , true ) ;
  mutexz . unlock (                ) ;
}

void N::TickTreeItems::Sync(QTreeWidget * tree)
{
  QString s                                             ;
  bool    updated = false                               ;
  mutexz . lock   ( )                                   ;
  foreach ( s , Symbols )                               {
    if ( ! isEnabled ( s ) )                            {
      if ( Visiblity [ s ] )                            {
        QTreeWidgetItem * it                            ;
        it = SymbolItems [ s ]                          ;
        setEnabled ( s , true )                         ;
        tree -> addTopLevelItem ( it )                  ;
        updated = true                                  ;
      }                                                 ;
    }                                                   ;
  }                                                     ;
  if ( updated )                                        {
    s = tr("%1 symbols").arg(tree->topLevelItemCount()) ;
    tree -> setToolTip ( s )                            ;
  }                                                     ;
  mutexz . unlock ( )                                   ;
}

void N::TickTreeItems::Full(QTreeWidget * tree)
{
  QString s                          ;
  mutexz . lock   ( )                ;
  foreach ( s , Symbols )            {
    if ( ! Visiblity [ s ] )         {
      QTreeWidgetItem * it           ;
      it = SymbolItems [ s ]         ;
      setEnabled ( s , true )        ;
      Visiblity [ s ] = true         ;
      tree -> addTopLevelItem ( it ) ;
    }                                ;
  }                                  ;
  mutexz . unlock ( )                ;
}

void N::TickTreeItems::BySymbols(QTreeWidget * tree,QStringList list)
{
  QString s                                  ;
  mutexz . lock   ( )                        ;
  while ( tree -> topLevelItemCount ( ) >0 ) {
    tree -> takeTopLevelItem ( 0 )           ;
  }                                          ;
  foreach ( s , Symbols )                    {
    Visiblity [ s ] = false                  ;
    setEnabled ( s , false )                 ;
  }                                          ;
  foreach ( s , list )                       {
    if ( Symbols . contains ( s ) )          {
      QTreeWidgetItem * it                   ;
      it = SymbolItems [ s ]                 ;
      setEnabled ( s , true )                ;
      Visiblity [ s ] = true                 ;
      tree -> addTopLevelItem ( it )         ;
    }                                        ;
  }                                          ;
  mutexz . unlock ( )                        ;
}
