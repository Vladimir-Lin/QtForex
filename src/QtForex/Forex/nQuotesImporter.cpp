#include <qtforex.h>

N::QuotesImporter:: QuotesImporter ( QWidget * parent , Plan * p )
                  : TreeDock       (           parent ,        p )
                  , blocks         ( 100000                      )
{
  Configure ( ) ;
}

N::QuotesImporter::~QuotesImporter (void)
{
}

SUID N::QuotesImporter::MemberUuid(void)
{
  SUID u = 0                                               ;
  plan -> settings . beginGroup ( member )                 ;
  if ( plan -> settings . contains ( key ) )               {
    u = plan -> settings . value ( key ) . toULongLong ( ) ;
  }                                                        ;
  plan -> settings . endGroup   (         )                ;
  return u                                                 ;
}

void N::QuotesImporter::Configure(void)
{
  dockingOrientation = Qt::Horizontal                                  ;
  dockingPlace       = Qt::BottomDockWidgetArea                        ;
  dockingPlaces      = Qt::AllDockWidgetAreas                          ;
  //////////////////////////////////////////////////////////////////////
  setWindowIcon                ( QIcon(":/icons/join.png") )           ;
  setWindowTitle               ( tr("Import FOREX quotes") )           ;
  setSuggestion                ( QSize ( 800 , 320 )       )           ;
  setDragDropMode              ( NoDragDrop                )           ;
  setRootIsDecorated           ( false                     )           ;
  setAlternatingRowColors      ( true                      )           ;
  setSelectionMode             ( ExtendedSelection         )           ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded     )           ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded     )           ;
  setColumnCount               ( 7                         )           ;
  //////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                      )           ;
  head -> setText              ( 0 , tr("Supplier")        )           ;
  head -> setText              ( 1 , tr("Symbol"  )        )           ;
  head -> setText              ( 2 , tr("Period"  )        )           ;
  head -> setText              ( 3 , tr("Level"   )        )           ;
  head -> setText              ( 4 , tr("File"    )        )           ;
  head -> setText              ( 5 , tr("Status"  )        )           ;
  head -> setText              ( 6 , ""                    )           ;
  assignHeaderItems            ( head                      )           ;
  //////////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                         )           ;
  MountClicked                 ( 2                         )           ;
  plan->setFont                ( this                      )           ;
  //////////////////////////////////////////////////////////////////////
  setLimitValue                ( 3824001 , 0               )           ;
  member = "Forex"                                                     ;
  key    = "Providers"                                                 ;
  //////////////////////////////////////////////////////////////////////
  periods [ Forex::Ticks ] = "T00"                                     ;
  periods [ Forex::M1    ] = "M01"                                     ;
  periods [ Forex::M5    ] = "M05"                                     ;
  periods [ Forex::M15   ] = "M15"                                     ;
  periods [ Forex::M30   ] = "M30"                                     ;
  periods [ Forex::H1    ] = "H01"                                     ;
  periods [ Forex::H4    ] = "H04"                                     ;
  periods [ Forex::D1    ] = "D01"                                     ;
  periods [ Forex::W1    ] = "W01"                                     ;
  periods [ Forex::MN1   ] = "MN1"                                     ;
  //////////////////////////////////////////////////////////////////////
  pnames  [ Forex::Ticks ] = tr("Ticks")                               ;
  pnames  [ Forex::M1    ] = tr("M1"   )                               ;
  pnames  [ Forex::M5    ] = tr("M5"   )                               ;
  pnames  [ Forex::M15   ] = tr("M15"  )                               ;
  pnames  [ Forex::M30   ] = tr("M30"  )                               ;
  pnames  [ Forex::H1    ] = tr("H1"   )                               ;
  pnames  [ Forex::H4    ] = tr("H4"   )                               ;
  pnames  [ Forex::D1    ] = tr("D1"   )                               ;
  pnames  [ Forex::W1    ] = tr("W1"   )                               ;
  pnames  [ Forex::MN1   ] = tr("MN1"  )                               ;
}

void N::QuotesImporter::closeEvent(QCloseEvent * e)
{
  if ( LimitValues . contains ( 3824001 ) ) {
    if ( LimitValues [ 3824001 ] > 0 )      {
      e -> ignore ( )                       ;
      return                                ;
    }                                       ;
  }                                         ;
  QTreeWidget :: closeEvent ( e )           ;
}

bool N::QuotesImporter::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Start        , Import      ( ) ) ;
  return true                                          ;
}

bool N::QuotesImporter::FocusOut(void)
{
  return true ;
}

void N::QuotesImporter::run(int type,ThreadData * data)
{
  switch ( type )                           {
    case 501                                :
      pushCanStop (         )               ;
      while ( LimitValues [ 3824001 ] > 8 ) {
        Time::msleep(1000)                  ;
      }                                     ;
      LimitValues [ 3824001 ] ++            ;
      Importion ( data -> Arguments )       ;
      LimitValues [ 3824001 ] --            ;
      popCanStop  (         )               ;
    break                                   ;
  }                                         ;
}

void N::QuotesImporter::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Click ) ;
}

void N::QuotesImporter::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                      ;
  ComboBox  * cb = NULL                      ;
  SUID        uu = 0                         ;
  int         st = nTreeInt ( item , 5 )     ;
  ////////////////////////////////////////////
  if ( st > 0 )                              {
    Alert ( Error )                          ;
    return                                   ;
  }                                          ;
  ////////////////////////////////////////////
  switch ( column )                          {
    case 0                                   :
      uu = nTreeUuid ( item , 0 )            ;
      cb = (ComboBox *)setComboBox           (
             item                            ,
             column                          ,
             SIGNAL(activated    (int))      ,
             SLOT  (traderChanged(int))    ) ;
      cb -> setMaxVisibleItems ( 25 )        ;
      addProviders       ( cb   )            ;
      *cb <= uu                              ;
      cb  -> setEditable ( true )            ;
      cb  -> showPopup   (      )            ;
    break                                    ;
    case 1                                   :
      uu = nTreeUuid ( item , 1 )            ;
      cb = (ComboBox *)setComboBox           (
             item                            ,
             column                          ,
             SIGNAL(activated      (int))    ,
             SLOT  (currencyChanged(int))  ) ;
      cb -> setMaxVisibleItems ( 25 )        ;
      addCurrencies ( cb                     ,
                      nTreeUuid ( item,0 )   ,
                      nTreeInt  ( item,3 ) ) ;
      *cb <= uu                              ;
      cb  -> setEditable ( true )            ;
      cb  -> showPopup   (      )            ;
    break                                    ;
    case 2                                   :
      uu = nTreeUuid ( item , 2 )            ;
      cb = (ComboBox *)setComboBox           (
             item                            ,
             column                          ,
             SIGNAL(activated      (int))    ,
             SLOT  (periodChanged  (int))  ) ;
      cb -> setMaxVisibleItems ( 25 )        ;
      addPeriods         ( cb   )            ;
      *cb <= uu                              ;
      cb  -> setEditable ( true )            ;
      cb  -> showPopup   (      )            ;
    break                                    ;
    case 3                                   :
      uu = nTreeUuid ( item , 3 )            ;
      cb = (ComboBox *)setComboBox           (
             item                            ,
             column                          ,
             SIGNAL(activated      (int))    ,
             SLOT  (levelChanged   (int))  ) ;
      cb -> setMaxVisibleItems ( 25 )        ;
      addLevels          ( cb   )            ;
      *cb <= uu                              ;
      cb  -> setEditable ( true )            ;
      cb  -> showPopup   (      )            ;
    break                                    ;
    case 4                                   :
      addFile ( item , column )              ;
    break                                    ;
    case 5                                   :
    break                                    ;
    default                                  :
      le = setLineEdit                       (
             item                            ,
             0                               ,
             SIGNAL(editingFinished())       ,
             SLOT  (editingFinished())     ) ;
      le -> setFocus(Qt::TabFocusReason)     ;
    break                                    ;
  }                                          ;
}

void N::QuotesImporter::addProviders(ComboBox * cbo)
{
  GroupItems GI ( plan )                        ;
  SUID       ug = MemberUuid ( )                ;
  SUID       u                                  ;
  UUIDs      uuids                              ;
  NAMEs      names                              ;
  ///////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                   ;
    uuids = GI . Subordination                  (
              SC                                ,
              ug                                ,
              Types  :: Division                ,
              Types  :: Organization            ,
              Groups :: Subordination           ,
              SC . OrderByAsc ( "position" ) )  ;
    foreach ( u , uuids )                       {
      QStringList Names = GI . Names ( SC , u ) ;
      if (Names.count()>0)                      {
        names [ u ] = Names . first ( )         ;
      }                                         ;
    }                                           ;
  ErrorSQL ( SC , plan->sql )                   ;
  LeaveSQL ( SC , plan->sql )                   ;
  ///////////////////////////////////////////////
  cbo -> appendNames ( uuids , names )          ;
}

void N::QuotesImporter::addCurrencies(ComboBox * cbo,SUID provider,int level)
{
  SqlConnection   SC ( plan -> sql                  ) ;
  Forex::Importer FI                                  ;
  if ( SC . open   ( "CurrencyRelations" , "List" ) ) {
    QStringList CL                                    ;
    FI . GetSymbols                                   (
      SC                                              ,
      PlanTable(ForexCommodities)                     ,
      provider                                        ,
      level                                           ,
      CL                                            ) ;
    if ( CL.count() > 0 )                             {
      ( (QComboBox *) cbo ) -> addItems ( CL )        ;
    } else                                            {
      cbo->addItem ( tr("No currency available")    ) ;
    }                                                 ;
    SC . close     (                                ) ;
  }                                                   ;
  SC   . remove    (                                ) ;
}

void N::QuotesImporter::addPeriods(ComboBox * combo)
{
  combo -> addItem ( tr("Ticks" ) , Forex::Ticks ) ;
  combo -> addItem ( tr("M1"    ) , Forex::M1    ) ;
  combo -> addItem ( tr("M5"    ) , Forex::M5    ) ;
  combo -> addItem ( tr("M15"   ) , Forex::M15   ) ;
  combo -> addItem ( tr("M30"   ) , Forex::M30   ) ;
  combo -> addItem ( tr("H1"    ) , Forex::H1    ) ;
  combo -> addItem ( tr("H4"    ) , Forex::H4    ) ;
  combo -> addItem ( tr("D1"    ) , Forex::D1    ) ;
  combo -> addItem ( tr("W1"    ) , Forex::W1    ) ;
  combo -> addItem ( tr("MN1"   ) , Forex::MN1   ) ;
}

void N::QuotesImporter::addLevels(ComboBox * combo)
{
  for (int i='A' ; i <= 'Z' ; i++ )                        {
    combo -> addItem ( QString(QChar((char)i)) , i - 'A' ) ;
  }                                                        ;
}

void N::QuotesImporter::Insert(void)
{
  NewTreeWidgetItem ( IT                   )                   ;
  removeOldItem     ( true , ItemColumn    )                   ;
  for (int i=0;i<7;i++) IT -> setData ( i , Qt::UserRole , 0 ) ;
  IT -> setData ( 2 , Qt::UserRole , -1 )                      ;
  if ( topLevelItemCount ( ) > 0 )                             {
    QTreeWidgetItem * it                                       ;
    int               last                                     ;
    last = topLevelItemCount (          ) - 1                  ;
    it   = topLevelItem      ( last     )                      ;
    IT  -> setText ( 0 , it -> text ( 0 )                    ) ;
    IT  -> setText ( 1 , it -> text ( 1 )                    ) ;
    IT  -> setText ( 2 , it -> text ( 2 )                    ) ;
    IT  -> setText ( 3 , it -> text ( 3 )                    ) ;
    IT  -> setData ( 0 , Qt::UserRole , nTreeUuid ( it , 0 ) ) ;
    IT  -> setData ( 2 , Qt::UserRole , nTreeInt  ( it , 2 ) ) ;
    IT  -> setData ( 3 , Qt::UserRole , nTreeInt  ( it , 3 ) ) ;
  }                                                            ;
  addTopLevelItem   ( IT                   )                   ;
  DoProcessEvents                                              ;
  scrollToItem      ( IT                   )                   ;
  doubleClicked     ( IT , 0               )                   ;
}

void N::QuotesImporter::Delete(void)
{
  TreeWidgetItems items = selectedItems ( ) ;
  nDropOut ( items . count ( ) <= 0 )       ;
  TreeWidgetItems deletion                  ;
  ///////////////////////////////////////////
  for (int i=0;i<items.count();i++)         {
    QTreeWidgetItem * it                    ;
    int               s                     ;
    it = items [ i ]                        ;
    s  = nTreeInt ( it , 5 )                ;
    switch ( s )                            {
      case  3                               :
      break                                 ;
      case  4                               :
      break                                 ;
      case  0                               :
      case  1                               :
      case  2                               :
      default                               :
        deletion << it                      ;
      break                                 ;
    }                                       ;
  }                                         ;
  ///////////////////////////////////////////
  nDropOut ( deletion . count ( ) <= 0 )    ;
  ///////////////////////////////////////////
  for (int i=0;i<deletion.count();i++)      {
    takeItem ( deletion [ i ] )             ;
  }                                         ;
  Alert ( Done )                            ;
}

void N::QuotesImporter::traderChanged (int provider)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( provider                ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 0    , n                ) ;
  ItemEditing -> setData     ( 0    , Qt::UserRole , u ) ;
  CheckIt                    ( ItemEditing             ) ;
  removeOldItem              ( true , 0                ) ;
}

void N::QuotesImporter::currencyChanged (int currency)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( currency                ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 1    , n                ) ;
  CheckIt                    ( ItemEditing             ) ;
  removeOldItem              ( true , 1                ) ;
}

void N::QuotesImporter::periodChanged(int period)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( period                  ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 2    , n                ) ;
  ItemEditing -> setData     ( 2    , Qt::UserRole , u ) ;
  CheckIt                    ( ItemEditing             ) ;
  removeOldItem              ( true , 2                ) ;
}

void N::QuotesImporter::levelChanged(int level)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( level                   ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 3    , n                ) ;
  ItemEditing -> setData     ( 3    , Qt::UserRole , u ) ;
  CheckIt                    ( ItemEditing             ) ;
  removeOldItem              ( true , 3                ) ;
}

void N::QuotesImporter::addFile(QTreeWidgetItem * item,int column)
{
  QString filename = item -> text ( column )                           ;
  QString suffixes = ""                                                ;
  //////////////////////////////////////////////////////////////////////
  suffixes += tr("Comma-Separated Values File (*.csv)")                ;
  suffixes += ";;"                                                     ;
  suffixes += tr("Any files (*.*)"                    )                ;
  //////////////////////////////////////////////////////////////////////
  if ( filename . length ( ) <= 0 )                                    {
    if ( plan -> Variables . contains ( "QuotesImporter" ) )           {
      filename = plan -> Variables [ "QuotesImporter" ] . toString ( ) ;
    }                                                                  ;
  }                                                                    ;
  filename  = QFileDialog::getOpenFileName                             (
                this                                                   ,
                tr("Forex quotes")                                     ,
                filename                                               ,
                suffixes                                             ) ;
  if ( filename . length ( ) <= 0 ) return                             ;
  //////////////////////////////////////////////////////////////////////
  QFileInfo FX ( filename )                                            ;
  QString   FDX = FX . absolutePath ( )                                ;
  plan -> Variables [ "QuotesImporter" ] = FDX                         ;
  //////////////////////////////////////////////////////////////////////
  item -> setText ( column , filename )                                ;
  CheckIt         ( item              )                                ;
}

bool N::QuotesImporter::isReady(QTreeWidgetItem * item)
{
  QString filename = item -> text ( 4 )          ;
  if ( filename . length ( ) <= 0 ) return false ;
  ////////////////////////////////////////////////
  SUID u = nTreeUuid ( item , 0 )                ;
  if ( u <= 0 ) return false                     ;
  QString symbol = item -> text ( 1 )            ;
  if ( symbol . length ( ) <= 0 ) return false   ;
  ////////////////////////////////////////////////
  return true                                    ;
}

void N::QuotesImporter::CheckIt(QTreeWidgetItem * item)
{
  int p = nTreeInt ( item , 2 )                       ;
  if ( p > 0 ) return                                 ;
  /////////////////////////////////////////////////////
  QString filename = item -> text ( 4 )               ;
  if ( filename . length ( ) <= 0 ) return            ;
  /////////////////////////////////////////////////////
  SUID u = nTreeUuid ( item , 0 )                     ;
  if ( u <= 0 ) return                                ;
  QString symbol = item -> text ( 1 )                 ;
  if ( symbol . length ( ) <= 0 ) return              ;
  /////////////////////////////////////////////////////
  QFile      F ( filename )                           ;
  if ( ! F . exists ( ) ) return                      ;
  if ( ! F . open ( QIODevice::ReadOnly ) ) return    ;
  QStringList L                                       ;
  QString     R                                       ;
  QByteArray  body                                    ;
  int         lines = 0                               ;
  bool        keep  = true                            ;
  while ( keep )                                      {
    body = F . readLine ( )                           ;
    if ( body . size ( ) > 0 )                        {
      R = QString::fromUtf8 ( body )                  ;
      R = R . replace ( "\r" , "" )                   ;
      R = R . replace ( "\n" , "" )                   ;
      if ( R . length ( ) > 0 )                       {
        L << R                                        ;
        lines ++                                      ;
        if ( lines > 100 ) keep = false               ;
      } else keep = false                             ;
    } else keep = false                               ;
  }                                                   ;
  F . close ( )                                       ;
  /////////////////////////////////////////////////////
  if ( L . count ( ) <= 1 ) return                    ;
  /////////////////////////////////////////////////////
  CUIDs xids                                          ;
  IMAPs imap                                          ;
  /////////////////////////////////////////////////////
  xids << Forex::M1                                   ;
  xids << Forex::M5                                   ;
  xids << Forex::M15                                  ;
  xids << Forex::M30                                  ;
  xids << Forex::H1                                   ;
  xids << Forex::H4                                   ;
  xids << Forex::D1                                   ;
  xids << Forex::W1                                   ;
  xids << Forex::MN1                                  ;
  /////////////////////////////////////////////////////
  for (int i=0;i<xids.count();i++)                    {
    imap [ xids [ i ] ] = 0                           ;
  }                                                   ;
  /////////////////////////////////////////////////////
  QDateTime   Last                                    ;
  QDateTime   Now                                     ;
  QDate       D                                       ;
  QTime       T                                       ;
  QStringList S                                       ;
  for (int i=0;i<L.count();i++)                       {
    R = L [ i ]                                       ;
    S = R . split ( "," )                             ;
    if ( S.count() > 2 )                              {
      T   . setHMS            ( 0 , 0 , 0 , 0       ) ;
      D   = QDate::fromString ( S[0] , "yyyy.MM.dd" ) ;
      T   = QTime::fromString ( S[1] , "hh:mm"      ) ;
      Now . setDate           ( D                   ) ;
      Now . setTime           ( T                   ) ;
    }                                                 ;
    if ( i > 0 )                                      {
      int dt = Last . secsTo ( Now )                  ;
      imap [ dt ] ++                                  ;
    }                                                 ;
    Last = Now                                        ;
  }                                                   ;
  /////////////////////////////////////////////////////
  int period = -1                                     ;
  int maxv   = -1                                     ;
  int cvt    = 0                                      ;
  int mvt    = 0                                      ;
  for (int i=0;i<xids.count();i++)                    {
    period = xids [ i ]                               ;
    if ( imap . contains ( period * 60 ) )            {
      mvt = imap [ period * 60 ]                      ;
      if ( mvt > cvt )                                {
        cvt  = mvt                                    ;
        maxv = period                                 ;
      }                                               ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  if ( maxv < 0 )                                     {
    switch ( maxv )                                   {
      case Forex::Ticks                               :
        R = tr("Ticks")                               ;
      break                                           ;
      case Forex::M1                                  :
        R = tr("M1"   )                               ;
      break                                           ;
      case Forex::M5                                  :
        R = tr("M5"   )                               ;
      break                                           ;
      case Forex::M15                                 :
        R = tr("M15"  )                               ;
      break                                           ;
      case Forex::M30                                 :
        R = tr("M30"  )                               ;
      break                                           ;
      case Forex::H1                                  :
        R = tr("H1"   )                               ;
      break                                           ;
      case Forex::H4                                  :
        R = tr("H4"   )                               ;
      break                                           ;
      case Forex::D1                                  :
        R = tr("D1"   )                               ;
      break                                           ;
      case Forex::W1                                  :
        R = tr("W1"   )                               ;
      break                                           ;
      case Forex::MN1                                 :
        R = tr("MN1"  )                               ;
      break                                           ;
      default                                         :
      return                                          ;
    }                                                 ;
  } else maxv = 0                                     ;
  /////////////////////////////////////////////////////
  item -> setData ( 2 , Qt::UserRole , maxv )         ;
  item -> setText ( 2 , R                   )         ;
}

void N::QuotesImporter::Import(void)
{
  QTreeWidgetItem * it = currentItem ( )       ;
  if ( IsNull(it) ) return                     ;
  if ( ! isReady     ( it     ) ) return       ;
  if ( 0 != nTreeInt ( it , 5 ) ) return       ;
  //////////////////////////////////////////////
  VarArgs args                                 ;
  it   -> setData     ( 5 , Qt::UserRole , 1 ) ;
  it   -> setText     ( 5 , tr("Waiting")    ) ;
  args << nTreeUuid   ( it , 0               ) ;
  args << it -> text  (      1               ) ;
  args << nTreeInt    ( it , 2               ) ;
  args << nTreeInt    ( it , 3               ) ;
  args << it -> text  (      4               ) ;
  args << VoidVariant ( it                   ) ;
  start               ( 501 , args           ) ;
}

void N::QuotesImporter::Importion(VarArgs & args)
{
  Forex::Importer   fi                                                       ;
  SUID              provider  = args [ 0 ] . toULongLong ( )                 ;
  QString           symbol    = args [ 1 ] . toString    ( )                 ;
  int               period    = args [ 2 ] . toInt       ( )                 ;
  int               level     = args [ 3 ] . toInt       ( )                 ;
  QString           filename  = args [ 4 ] . toString    ( )                 ;
  QTreeWidgetItem * item      = (QTreeWidgetItem *)VariantToVoid ( args[5] ) ;
  QString           supplier  = item -> text ( 0 )                           ;
  bool              correct   = true                                         ;
  Currencies        currencies                                               ;
  ////////////////////////////////////////////////////////////////////////////
  if ( provider              <= 0 ) correct = false                          ;
  if ( symbol   . length ( ) <= 0 ) correct = false                          ;
  if ( period                <  0 ) correct = false                          ;
  if ( filename . length ( ) <= 0 ) correct = false                          ;
  if ( ! correct )                                                           {
    Alert ( Error )                                                          ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SqlConnection XC ( plan -> sql )                                           ;
  if ( XC . open ( __FUNCTION__ , QtUUID::createUuidString() ) )               {
    currencies . load      ( XC , PlanTable(Currency     )     )             ;
    currencies . loadPairs ( XC , PlanTable(CurrencyPairs)     )             ;
    currencies . loadName  ( XC , PlanTable(Names),vLanguageId )             ;
    //////////////////////////////////////////////////////////////////////////
    fi . Get ( XC                                                            ,
               PlanTable(ForexCommodities)                                   ,
               provider                                                      ,
               level                                                         ,
               symbol                                                      ) ;
    fi.identifier = fi . FetchIdentifier ( XC                                ,
                                           PlanTable(Identifiers)            ,
                                           provider                        ) ;
    fi.Connexion  = XC . GetConnection   ( PlanTable(SqlConnections)         ,
                                           fi . SQL                        ) ;
    //////////////////////////////////////////////////////////////////////////
    XC . close ( )                                                           ;
  } else                                                                     {
    XC . close ( )                                                           ;
  }                                                                          ;
  XC . remove  ( )                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! fi . Connexion . isValid ( ) )                                      {
    Alert ( Error )                                                          ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  int sid     = currencies . UuidPair [ fi.Pair ]                            ;
  fi . symbol = currencies . Pairname ( sid     )                            ;
  fi . setPeriod ( period )                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  int pid                                                                    ;
  int total = 0                                                              ;
  emit OnBusy  ( )                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  QFile F ( filename )                                                       ;
  if ( F . open ( QIODevice::ReadOnly ) )                                    {
    QByteArray B                                                             ;
    do                                                                       {
      B = F . readLine ( )                                                   ;
      if ( B . size ( ) > 0 ) total ++                                       ;
    } while ( B . size ( ) > 0 )                                             ;
    F . close ( )                                                            ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( total > 0 )                                                           {
    qint64    value = 0                                                      ;
    bool      go    = true                                                   ;
    pid = plan->Progress                                                     (
            tr("Import %1 into %2").arg(filename).arg(supplier)              ,
            tr("%v/%m")                                                    ) ;
    plan -> Start    ( pid , &value , &go   )                                ;
    plan -> setRange ( pid , 0      , total )                                ;
    Time :: skip     ( 300                  )                                ;
    item -> setData  ( 5 , Qt::UserRole , 3 )                                ;
    item -> setText  ( 5 , tr("Started")    )                                ;
    SqlConnection SC ( fi . Connexion       )                                ;
    if ( SC . open ( __FUNCTION__ , QtUUID::createUuidString() ) )             {
      if ( ! fi . PeriodExists ( SC ) ) fi . CreatePeriods ( SC )            ;
      ////////////////////////////////////////////////////////////////////////
      value = 0                                                              ;
      plan -> setRange     ( pid , 0 , total )                               ;
      plan -> ProgressText ( pid , tr("Importing FOREX quotes"))             ;
      ////////////////////////////////////////////////////////////////////////
      QFile X ( filename )                                                   ;
      if ( X . open ( QIODevice::ReadOnly ) )                                {
        QByteArray  B                                                        ;
        QStringList LL                                                       ;
        QString     R                                                        ;
        bool        correct                                                  ;
        int         totalx                                                   ;
        do                                                                   {
          LL . clear ( )                                                     ;
          correct = true                                                     ;
          for (int i=0;correct && go && i<blocks;i++)                        {
            B = X . readLine ( )                                             ;
            R = QString::fromUtf8 ( B )                                      ;
            R = R . replace ( "\r" , "" )                                    ;
            R = R . replace ( "\n" , "" )                                    ;
            if ( R . length ( ) <= 0 ) correct = false ; else LL << R        ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
          totalx = LL . count ( )                                            ;
          if ( totalx > 0 )                                                  {
            for (int i=0;go && i<totalx;i++)                                 {
              R = LL [ i ]                                                   ;
              value ++                                                       ;
              if ( R . length ( ) > 0 ) fi . Insert ( SC , R )               ;
            }                                                                ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
        } while ( go && B . size ( ) > 0 )                                   ;
        X . close ( )                                                        ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      Alert          ( Done                 )                                ;
      SC . close     (                      )                                ;
    } else                                                                   {
      Alert          ( Error                )                                ;
      SC . close     (                      )                                ;
    }                                                                        ;
    SC . remove      (                      )                                ;
    item -> setData  ( 5 , Qt::UserRole , 2 )                                ;
    item -> setText  ( 5 , tr("Completed")  )                                ;
    plan -> Finish   ( pid                  )                                ;
  }                                                                          ;
  emit GoRelax ( )                                                           ;
}

void N::QuotesImporter::StartAll(void)
{
  for (int i=0;i<topLevelItemCount();i++) {
    QTreeWidgetItem * it                  ;
    it = topLevelItem ( i     )           ;
    setCurrentItem    ( it    )           ;
    Import            (       )           ;
    Time::skip        ( 10000 )           ;
  }                                       ;
}

void N::QuotesImporter::Fullsets(void)
{
  if ( topLevelItemCount ( ) <= 0 ) return                       ;
  removeOldItem ( true , ItemColumn )                            ;
  ////////////////////////////////////////////////////////////////
  IMAPs PIX                                                      ;
  PIX [ 0 ] = Forex::M5                                          ;
  PIX [ 1 ] = Forex::M15                                         ;
  PIX [ 2 ] = Forex::M30                                         ;
  PIX [ 3 ] = Forex::H1                                          ;
  PIX [ 4 ] = Forex::H4                                          ;
  PIX [ 5 ] = Forex::D1                                          ;
  PIX [ 6 ] = Forex::W1                                          ;
  PIX [ 7 ] = Forex::MN1                                         ;
  ////////////////////////////////////////////////////////////////
  QTreeWidgetItem * it                                           ;
  int               last                                         ;
  last = topLevelItemCount (          ) - 1                      ;
  it   = topLevelItem      ( last     )                          ;
  ////////////////////////////////////////////////////////////////
  for (int p=0;p<8;p++)                                          {
    NewTreeWidgetItem ( IT                   )                   ;
    for (int i=0;i<7;i++) IT -> setData ( i , Qt::UserRole , 0 ) ;
    IT -> setData ( 2 , Qt::UserRole , PIX [ p ]               ) ;
    IT -> setText ( 0 , it -> text ( 0 )                       ) ;
    IT -> setText ( 1 , it -> text ( 1 )                       ) ;
    IT -> setText ( 2 , pnames [ PIX [ p ] ]                   ) ;
    IT -> setText ( 3 , it -> text ( 3 )                       ) ;
    IT -> setData ( 0 , Qt::UserRole , nTreeUuid ( it , 0 )    ) ;
    IT -> setData ( 3 , Qt::UserRole , nTreeInt  ( it , 3 )    ) ;
    addTopLevelItem   ( IT                   )                   ;
    DoProcessEvents                                              ;
    scrollToItem      ( IT                   )                   ;
  }                                                              ;
}

bool N::QuotesImporter::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                        ;
  QAction         * aa                             ;
  QTreeWidgetItem * it  = itemAt ( pos )           ;
  SpinBox         * spb = NULL                     ;
  int               cc  = currentColumn ( )        ;
  //////////////////////////////////////////////////
  spb  = new SpinBox  ( NULL , plan              ) ;
  spb -> setRange     ( 0    , 1000000           ) ;
  spb -> setValue     ( blocks                   ) ;
  spb -> setAlignment ( Qt::AlignRight             |
                        Qt::AlignVCenter         ) ;
  spb -> setPrefix    ( tr("Block : ")           ) ;
  spb -> External = &blocks                        ;
  //////////////////////////////////////////////////
  mm   . add ( 101 , tr("Add"   ) )                ;
  if ( NotNull(it) )                               {
    int sc = nTreeInt ( it , 5 )                   ;
    if ( sc < 3 )                                  {
      mm . add ( 102 , tr("Delete") )              ;
    }                                              ;
  }                                                ;
  if ( NotNull(it) && isReady(it) && ( 5 == cc ) ) {
    if ( 0 == nTreeInt(it,5) )                     {
      mm . addSeparator ( )                        ;
      mm . add ( 501 , tr("Start" ) )              ;
    }                                              ;
  }                                                ;
  mm   . addSeparator (                          ) ;
  mm   . add          ( 301 , tr("Start all")    ) ;
  if ( NotNull(it) )                               {
    mm . add          ( 302 , tr("Fullsets" )    ) ;
  }                                                ;
  mm   . addSeparator (                          ) ;
  mm   . add          ( 90573 , spb              ) ;
  DockingMenu ( mm        )                        ;
  aa = mm . exec ( )                               ;
  //////////////////////////////////////////////////
  if ( IsNull     ( aa      ) ) return true        ;
  if ( RunDocking ( mm , aa ) ) return true        ;
  //////////////////////////////////////////////////
  VarArgs args                                     ;
  switch ( mm [ aa ] )                             {
    case 101                                       :
      Insert   ( )                                 ;
    break                                          ;
    case 102                                       :
      Delete   ( )                                 ;
    break                                          ;
    case 301                                       :
      StartAll ( )                                 ;
    break                                          ;
    case 302                                       :
      Fullsets ( )                                 ;
    break                                          ;
    case 501                                       :
      it   -> setData     ( 5 , Qt::UserRole , 1 ) ;
      it   -> setText     ( 5 , tr("Waiting")    ) ;
      args << nTreeUuid   ( it , 0               ) ;
      args << it -> text  (      1               ) ;
      args << nTreeInt    ( it , 2               ) ;
      args << nTreeInt    ( it , 3               ) ;
      args << it -> text  (      4               ) ;
      args << VoidVariant ( it                   ) ;
      start               ( 501 , args           ) ;
    break                                          ;
    default                                        :
    break                                          ;
  }                                                ;
  return true                                      ;
}
