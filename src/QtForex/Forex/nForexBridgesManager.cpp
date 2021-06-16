#include <qtforex.h>

N::Forex::BridgesManager:: BridgesManager ( QObject * parent )
                         : QObject        (           parent )
{
}

N::Forex::BridgesManager::~BridgesManager (void)
{
  Unload ( ) ;
}

N::Forex::ConnectorGui * N::Forex::BridgesManager::Connector(int type)
{
  for (int i=0;i<ForexConnectors.count();i++)        {
    if ( type == ForexConnectors [ i ] -> type ( ) ) {
      return ForexConnectors [ i ]                   ;
    }                                                ;
  }                                                  ;
  return NULL                                        ;
}

N::Forex::Dealer * N::Forex::BridgesManager::Dealer(QString identifier)
{
  for (int i=0;i<ForexDealers.count();i++)                   {
    Forex::Dealer * dealer = ForexDealers [ i ]              ;
    QString         id                                       ;
    id = dealer -> Variables [ "Identifier" ] . toString ( ) ;
    if ( identifier == id ) return dealer                    ;
  }                                                          ;
  return NULL                                                ;
}

int N::Forex::BridgesManager::add(Forex::Dealer * dealer)
{
  mutex [ 0 ] . lock   ( )                    ;
  if ( ! ForexDealers . contains ( dealer ) ) {
    ForexDealers << dealer                    ;
  }                                           ;
  mutex [ 0 ] . unlock ( )                    ;
  return ForexDealers . count ( )             ;
}

int N::Forex::BridgesManager::remove(Forex::Dealer * dealer)
{
  mutex [ 0 ] . lock   ( )                    ;
  if ( ForexDealers . contains ( dealer ) )   {
    int index                                 ;
    index = ForexDealers . indexOf ( dealer ) ;
    if ( index >= 0 )                         {
      ForexDealers . takeAt ( index )         ;
    }                                         ;
  }                                           ;
  mutex [ 0 ] . unlock ( )                    ;
  return ForexDealers . count ( )             ;
}

bool N::Forex::BridgesManager::Load(QDir & dir)
{
  QStringList   DLLs                                           ;
  QFileInfoList fil                                            ;
  #ifdef Q_OS_WIN
  DLLs << "*.dll"                                              ;
  #else
  DLLs << "*.so"                                               ;
  #endif
  fil = dir . entryInfoList ( DLLs )                           ;
  if ( fil . count ( ) <= 0 ) return false                     ;
  //////////////////////////////////////////////////////////////
  for (int ii = 0 ; ii < fil . count ( ) ; ii++ )              {
    QString         pname  = fil [ ii ] . absoluteFilePath ( ) ;
    QPluginLoader * loader = new QPluginLoader ( pname )       ;
    nSafeExec ( loader )                                       {
      QObject * plugin = loader -> instance ( )                ;
      nSafeExec ( plugin )                                     {
        N::Forex::ConnectorGui * cg                            ;
        cg = Casting ( N::Forex::ConnectorGui , plugin )       ;
        nSafeExec ( cg )                                       {
          ForexConnectors << cg                                ;
          ConnectorLoaders [ cg ] = loader                     ;
        } else                                                 {
          loader -> unload ( )                                 ;
          delete loader                                        ;
        }                                                      ;
      } else                                                   {
        delete loader                                          ;
      }                                                        ;
    }                                                          ;
  }                                                            ;
  return ( ForexConnectors . count ( ) > 0 )                   ;
}

bool N::Forex::BridgesManager::UpdateSettings(QDir dir)
{
  QString  ss = dir . absoluteFilePath ( "Settings.ini"               ) ;
  Settings sf     ( ss , QSettings::IniFormat                         ) ;
  sf . beginGroup ( "Settings"                                        ) ;
  sf . setValue   ( "Size" , ForexDealers . count ( )                 ) ;
  for (int i = 0 ; i < ForexDealers . count ( ) ; i++ )                 {
    QString k = QString::number(i+1)                                    ;
    QString s                                                           ;
    s = ForexDealers [ i ] -> Variables [ "Identifier" ] . toString ( ) ;
    sf . setValue ( k , s                                             ) ;
  }                                                                     ;
  sf . endGroup   (                                                   ) ;
  for (int i = 0 ; i < ForexDealers . count ( ) ; i++ )                 {
    QString k                                                           ;
    QString n                                                           ;
    QString s                                                           ;
    int     t                                                           ;
    k = ForexDealers [ i ] -> Variables [ "Identifier" ] . toString ( ) ;
    s = ForexDealers [ i ] -> Variables [ "Settings"   ] . toString ( ) ;
    n = ForexDealers [ i ] -> Name                                      ;
    t = ForexDealers [ i ] -> DealerType ( )                            ;
    sf . beginGroup ( k                                               ) ;
    sf . setValue   ( "Name"     , n                                  ) ;
    sf . setValue   ( "Type"     , t                                  ) ;
    sf . setValue   ( "Settings" , s                                  ) ;
    sf . setValue   ( "Trade"    , false                              ) ;
    sf . endGroup   (                                                 ) ;
  }                                                                     ;
  return true                                                           ;
}

bool N::Forex::BridgesManager::StartAll(Plan * plan)
{
  QDir     dir = plan -> Path ( Directory::Users     ).absoluteFilePath("Forex") ;
  QDir     tmp = plan -> Path ( Directory::Temporary ).absoluteFilePath("Forex") ;
  QString  fi  = dir   . absoluteFilePath ( "Settings.ini" )                 ;
  Settings sf ( fi , QSettings::IniFormat )                                  ;
  QStrings ll                                                                ;
  int      ss                                                                ;
  ////////////////////////////////////////////////////////////////////////////
  tmp . mkdir ( tmp . absolutePath ( ) )                                     ;
  ////////////////////////////////////////////////////////////////////////////
  sf . beginGroup ( "Settings" )                                             ;
  ss = sf . value ( "Size"     ) . toInt ( )                                 ;
  for (int i=0;i<ss;i++)                                                     {
    QString k = QString::number  ( i + 1 )                                   ;
    QString n = sf . valueString ( k     )                                   ;
    if ( n . length ( ) > 0 ) ll << n                                        ;
  }                                                                          ;
  sf . endGroup   (            )                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ll . count ( ) > 0 )                                                  {
    ForexManipulator    * fm                                                 ;
    Forex::ConnectorGui * cg                                                 ;
    QString               k                                                  ;
    QString               n                                                  ;
    QString               s                                                  ;
    int                   t                                                  ;
    bool                  b                                                  ;
    int                 * fa                                                 ;
    //////////////////////////////////////////////////////////////////////////
    fm  = (ForexManipulator *) plan -> Manipulators [ "FOREX" ]              ;
    fm -> setTradings ( ll . count ( ) )                                     ;
    fa  = fm -> TradingsArray ( )                                            ;
    for (int i=0;i<1024;i++) fa [ i ] = 0                                    ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( k , ll )                                                       {
      sf . beginGroup       ( k          )                                   ;
      n  = sf . valueString ( "Name"     )                                   ;
      t  = sf . value       ( "Type"     ) . toInt  ( )                      ;
      s  = sf . valueString ( "Settings" )                                   ;
      b  = sf . value       ( "Trade"    ) . toBool ( )                      ;
      sf . endGroup         (            )                                   ;
      cg = Connector        ( t          )                                   ;
      if                    ( NULL != cg )                                   {
        Forex::Dealer * dealer = cg -> dealer ( NULL )                       ;
        if ( NULL != dealer )                                                {
          MtChannelSaver * mc  = new MtChannelSaver ( )                      ;
          QString          mcs = QString("%1.mcs").arg(k)                    ;
          mcs     = tmp . absoluteFilePath ( mcs )                           ;
          dealer -> Name                         = n                         ;
          dealer -> Variables [ "Identifier"   ] = k                         ;
          dealer -> Variables [ "Settings"     ] = s                         ;
          dealer -> Variables [ "Quotes"       ] = false                     ;
          dealer -> Variables [ "Traders"      ] = false                     ;
          dealer -> Variables [ "Orders"       ] = false                     ;
          dealer -> Variables [ "Groups"       ] = false                     ;
          dealer -> Variables [ "Transmission" ] = false                     ;
          dealer -> Variables [ "Manager"      ] = false                     ;
          add ( dealer )                                                     ;
          cg -> Configurator ( NULL , plan , dealer )                        ;
          mc -> Variables . remove ( "Duration" )                            ;
          mc -> Variables [ "File" ] = mcs                                   ;
          Channels [ dealer ] = mc                                           ;
          dealer -> Link                ( "Bridge" , this                  ) ;
          dealer -> AddChannels         ( mc                               ) ;
          dealer -> connectManipulator  ( plan -> Manipulators [ "FOREX" ] ) ;
          if ( b ) dealer -> startTrade (                                  ) ;
          mc -> start                   ( 10001                            ) ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  return true                                                                ;
}

bool N::Forex::BridgesManager::CloseAll(void)
{
  for (int i = 0 ; i < ForexDealers . count ( ) ; i++) {
    Forex::Dealer * dealer = ForexDealers [ i ]        ;
    switch ( dealer -> isTrading ( ) )                 {
      case 0                                           :
      break                                            ;
      case 1                                           :
        dealer -> stopTrade ( )                        ;
      break                                            ;
      case 2                                           :
      break                                            ;
    }                                                  ;
  }                                                    ;
  return true                                          ;
}

bool N::Forex::BridgesManager::Unload(void)
{
  #pragma message("FOREX Bridges Manager require a way to clean up without problems")
  for (int i=0;i<ForexDealers.count();i++)             {
    ForexDealers [ i ] -> Variables [ "Wait" ] = true  ;
    ForexDealers [ i ] -> stopTrade ( )                ;
    delete ForexDealers [ i ]                          ;
  }                                                    ;
  for (int i=0;i<ForexConnectors.count();i++)          {
    Forex::ConnectorGui * cg = ForexConnectors  [ i  ] ;
    QPluginLoader       * pg = ConnectorLoaders [ cg ] ;
    pg -> unload ( )                                   ;
    delete cg                                          ;
    delete pg                                          ;
  }                                                    ;
  ForexDealers     . clear ( )                         ;
  ForexConnectors  . clear ( )                         ;
  ConnectorLoaders . clear ( )                         ;
  return true                                          ;
}

void N::Forex::BridgesManager::Stop(void)
{
  for (int i = 0 ; i < ForexDealers . count ( ) ; i++) {
    Forex::Dealer * dealer = ForexDealers [ i      ]   ;
    if ( NULL != dealer )                              {
      MtChannels  * mc     = Channels     [ dealer ]   ;
      if ( NULL != mc )                                {
        mc -> Variables [ "Running" ] = false          ;
      }                                                ;
    }                                                  ;
  }                                                    ;
}
