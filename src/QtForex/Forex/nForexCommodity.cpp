#include <qtforex.h>

N::ForexCommodity:: ForexCommodity ( QWidget * parent , Plan * p            )
                  : TreeDock       (           parent ,        p            )
                  , Ownership      ( 0                , Types::Organization )
                  , level          ( ""                                     )
{
  Configure ( ) ;
}

N::ForexCommodity::~ForexCommodity (void)
{
}

void N::ForexCommodity::Configure(void)
{
  setWindowIcon                ( QIcon(":/icons/model.png")      ) ;
  setWindowTitle               ( tr("FOREX Trading commodities") ) ;
  setSuggestion                ( QSize ( 1020 , 320 )            ) ;
  setDragDropMode              ( NoDragDrop                      ) ;
  setRootIsDecorated           ( false                           ) ;
  setAlternatingRowColors      ( true                            ) ;
  setSelectionMode             ( ExtendedSelection               ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded           ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded           ) ;
  setColumnCount               ( 10                              ) ;
  //////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                            ) ;
  head -> setText              ( 0 , tr("Name"       )           ) ;
  head -> setText              ( 1 , tr("Symbol"     )           ) ;
  head -> setText              ( 2 , tr("Database"   )           ) ;
  head -> setText              ( 3 , tr("Digits"     )           ) ;
  head -> setText              ( 4 , tr("Full digits")           ) ;
  head -> setText              ( 5 , tr("Time zone"  )           ) ;
  head -> setText              ( 6 , tr("Level"      )           ) ;
  head -> setText              ( 7 , tr("From year"  )           ) ;
  head -> setText              ( 8 , tr("To year"    )           ) ;
  head -> setText              ( 9 , ""                          ) ;
  assignHeaderItems            ( head                            ) ;
  setColumnWidth               ( 9 , 3                           ) ;
  //////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                               ) ;
  MountClicked                 ( 2                               ) ;
  plan->setFont                ( this                            ) ;
}

bool N::ForexCommodity::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Paste        , Paste       ( ) ) ;
  return true                                          ;
}

bool N::ForexCommodity::FocusOut(void)
{
  return true ;
}

void N::ForexCommodity::run(int type,ThreadData * data)
{
  switch ( type ) {
    case 10001    :
      List ( )    ;
    break         ;
  }               ;
}

bool N::ForexCommodity::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::ForexCommodity::List(void)
{
  emit OnBusy  (                )                                         ;
  EnterSQL     ( SC , plan->sql )                                         ;
    QString Q                                                             ;
    currencies . clear     (                                   )          ;
    currencies . load      ( SC , PlanTable(Currency     )     )          ;
    currencies . loadPairs ( SC , PlanTable(CurrencyPairs)     )          ;
    currencies . loadName  ( SC , PlanTable(Names),vLanguageId )          ;
    if ( ! isStandby() )                                                  {
      CUIDs IDs                                                           ;
      int   id                                                            ;
      int   LX = -1                                                       ;
      if ( level . length ( ) > 0 ) LX = level . at(0) . toLatin1() - 'A' ;
      /////////////////////////////////////////////////////////////////////
      Q = SC . sql . SelectFrom                                           (
            "id"                                                          ,
            PlanTable(ForexCommodities)                                   ,
            SC.WhereUuid(uuid)                                            ,
            SC.OrderByAsc("id")                                         ) ;
      SqlLoopNow ( SC , Q )                                               ;
        IDs << SC . Int ( 0 )                                             ;
      SqlLoopErr ( SC , Q )                                               ;
      SqlLoopEnd ( SC , Q )                                               ;
      /////////////////////////////////////////////////////////////////////
      foreach ( id , IDs )                                                {
        Q = SC . sql . SelectFrom                                         (
              "connexion,pair,digits,full,timezone,level,fromYear,toYear,name" ,
              PlanTable(ForexCommodities)                                 ,
              QString("where id = %1").arg(id)                          ) ;
        if ( SC . Fetch ( Q ) )                                           {
          SUID    connexion = SC . Uuid   ( 0 )                           ;
          SUID    pair      = SC . Uuid   ( 1 )                           ;
          int     digits    = SC . Int    ( 2 )                           ;
          int     full      = SC . Int    ( 3 )                           ;
          QString timezone  = SC . String ( 4 )                           ;
          int     Level     = SC . Int    ( 5 )                           ;
          int     fromYear  = SC . Int    ( 6 )                           ;
          int     toYear    = SC . Int    ( 7 )                           ;
          QString name      = SC . String ( 8 )                           ;
          int     pid       = currencies . UuidPair [ pair ]              ;
          QString p = currencies . Pairname ( pid )                       ;
          QString n                                                       ;
          QString c                                                       ;
          bool    correct = true                                          ;
          /////////////////////////////////////////////////////////////////
          Q = SC . sql . SelectFrom                                       (
                "name"                                                    ,
                PlanTable(SqlConnections)                                 ,
                SC.WhereUuid(connexion)                               )   ;
          if ( SC . Fetch ( Q ) ) c = SC . String ( 0 )                   ;
          n = SC . getName ( PlanTable(Names),"uuid",vLanguageId,pair )   ;
          /////////////////////////////////////////////////////////////////
          if ( ( level . length ( ) > 0 ) && ( Level != LX ) )            {
            correct = false                                               ;
          }                                                               ;
          /////////////////////////////////////////////////////////////////
          if ( correct )                                                  {
            NewTreeWidgetItem ( IT )                                      ;
            IT -> setData ( 0 , Qt::UserRole , 0                       )  ;
            IT -> setData ( 1 , Qt::UserRole , pair                    )  ;
            IT -> setData ( 2 , Qt::UserRole , connexion               )  ;
            IT -> setData ( 3 , Qt::UserRole , digits                  )  ;
            IT -> setData ( 4 , Qt::UserRole , full                    )  ;
            IT -> setData ( 5 , Qt::UserRole , 0                       )  ;
            IT -> setData ( 6 , Qt::UserRole , Level                   )  ;
            IT -> setData ( 7 , Qt::UserRole , fromYear                )  ;
            IT -> setData ( 8 , Qt::UserRole , toYear                  )  ;
            IT -> setData ( 9 , Qt::UserRole , id                      )  ;
            IT -> setText ( 0 , name                                   )  ;
            IT -> setText ( 1 , QString("%1 (%2)").arg(p).arg(n)       )  ;
            IT -> setText ( 2 , c                                      )  ;
            IT -> setText ( 3 , QString::number(digits)                )  ;
            IT -> setText ( 4 , QString::number(full  )                )  ;
            IT -> setText ( 5 , timezone                               )  ;
            IT -> setText ( 6 , QString( QChar( (char) 'A' + Level ) ) )  ;
            IT -> setText ( 7 , QString::number(fromYear)              )  ;
            IT -> setText ( 8 , QString::number(toYear  )              )  ;
            addTopLevelItem   ( IT )                                      ;
          }                                                               ;
        }                                                                 ;
      }                                                                   ;
    }                                                                     ;
  ErrorSQL     ( SC , plan->sql )                                         ;
  LeaveSQL     ( SC , plan->sql )                                         ;
  reportItems  (                )                                         ;
  emit GoRelax (                )                                         ;
  emit AutoFit (                )                                         ;
  Alert        ( Done           )                                         ;
}

void N::ForexCommodity::singleClicked(QTreeWidgetItem *,int)
{
  Alert         ( Click             )          ;
  if ( currentItem ( ) == ItemEditing ) return ;
  removeOldItem ( true , ItemColumn )          ;
}

void N::ForexCommodity::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                                                ;
  ComboBox  * cb = NULL                                                ;
  SUID        uu = 0                                                   ;
  //////////////////////////////////////////////////////////////////////
  switch ( column )                                                    {
    case 0                                                             :
      le = setLineEdit                                                 (
             item                                                      ,
             0                                                         ,
             SIGNAL(editingFinished())                                 ,
             SLOT  (nameFinished   ())                               ) ;
      le -> setFocus(Qt::TabFocusReason)                               ;
    break                                                              ;
    case 1                                                             :
      uu = nTreeUuid ( item , 1 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (currencyChanged(int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addCurrencies      ( cb   )                                      ;
      *cb <= uu                                                        ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    case 2                                                             :
      uu = nTreeUuid ( item , 2 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (connectChanged (int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addConnections    ( cb   )                                       ;
      *cb <= uu                                                        ;
      cb -> setEditable ( true )                                       ;
      cb -> showPopup   (      )                                       ;
    break                                                              ;
    case 3                                                             :
      uu = nTreeUuid ( item , 3 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (digitChanged   (int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addDigits          ( cb   )                                      ;
      *cb <= uu                                                        ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    case 4                                                             :
      uu = nTreeUuid ( item , 4 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (fullChanged    (int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addFull            ( cb   )                                      ;
      *cb <= uu                                                        ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    case 5                                                             :
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (timezoneChanged(int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addTimeZones       ( cb   )                                      ;
      cb  -> setCurrentIndex ( cb -> findText ( item -> text ( 5 ) ) ) ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    case 6                                                             :
      if ( level . length ( ) <= 0 )                                   {
        uu = nTreeUuid ( item , 6 )                                    ;
        cb = (ComboBox *)setComboBox                                   (
               item                                                    ,
               column                                                  ,
               SIGNAL(activated      (int))                            ,
               SLOT  (levelChanged   (int))                          ) ;
        cb -> setMaxVisibleItems ( 20 )                                ;
        addLevels          ( cb   )                                    ;
        *cb <= uu                                                      ;
        cb  -> setEditable ( true )                                    ;
        cb  -> showPopup   (      )                                    ;
      }                                                                ;
    break                                                              ;
    case 7                                                             :
      uu = nTreeUuid ( item , 7 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (fromChanged    (int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addYears           ( cb   )                                      ;
      *cb <= uu                                                        ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    case 8                                                             :
      uu = nTreeUuid ( item , 8 )                                      ;
      cb = (ComboBox *)setComboBox                                     (
             item                                                      ,
             column                                                    ,
             SIGNAL(activated      (int))                              ,
             SLOT  (toChanged      (int))                            ) ;
      cb -> setMaxVisibleItems ( 20 )                                  ;
      addYears           ( cb   )                                      ;
      *cb <= uu                                                        ;
      cb  -> setEditable ( true )                                      ;
      cb  -> showPopup   (      )                                      ;
    break                                                              ;
    default                                                            :
      le = setLineEdit                                                 (
             item                                                      ,
             0                                                         ,
             SIGNAL(editingFinished())                                 ,
             SLOT  (editingFinished())                               ) ;
      le -> setFocus(Qt::TabFocusReason)                               ;
    break                                                              ;
  }                                                                    ;
}

void N::ForexCommodity::addCurrencies(ComboBox * cbo)
{
  SqlConnection SC         ( plan -> sql                           ) ;
  currencies . clear       (                                       ) ;
  if ( SC . open           ( "CurrencyRelations" , "List" )        ) {
    int id                                                           ;
    currencies . load      ( SC , PlanTable(Currency     )         ) ;
    currencies . loadPairs ( SC , PlanTable(CurrencyPairs)         ) ;
    currencies . loadName  ( SC , PlanTable(Names),vLanguageId     ) ;
    foreach                ( id , currencies . Pairs               ) {
      QString n                                                      ;
      QString p = currencies . Pairname ( id )                       ;
      SUID    u = currencies . PairUuid [ id ]                       ;
      n = SC . getName     ( PlanTable(Names),"uuid",vLanguageId,u ) ;
      currencies . Names [ id ] = n                                  ;
      cbo -> addItem       ( QString("%1 (%2)").arg(p).arg(n) , u  ) ;
    }                                                                ;
    SC . close             (                                       ) ;
  }                                                                  ;
  SC   . remove            (                                       ) ;
}

void N::ForexCommodity::addConnections(ComboBox * cbo)
{
  EnterSQL ( SC , plan->sql )       ;
    QString Q                       ;
    Q = SC . sql . SelectFrom       (
        "uuid,name"                 ,
        PlanTable(SqlConnections)   ,
        SC . OrderByAsc ( "id" ) )  ;
    SqlLoopNow ( SC , Q )           ;
      SUID    u = SC . Uuid   ( 0 ) ;
      QString n = SC . String ( 1 ) ;
      cbo -> addItem ( n , u )      ;
    SqlLoopErr ( SC , Q )           ;
    SqlLoopEnd ( SC , Q )           ;
  LeaveSQL ( SC , plan->sql )       ;
}

void N::ForexCommodity::addDigits(ComboBox * combo)
{
  for (int i=0;i<=8;i++)                        {
    combo -> addItem ( QString::number(i) , i ) ;
  }                                             ;
}

void N::ForexCommodity::addFull(ComboBox * combo)
{
  for (int i=0;i<=20;i++)                       {
    combo -> addItem ( QString::number(i) , i ) ;
  }                                             ;
}

void N::ForexCommodity::addTimeZones(ComboBox * combo)
{
  QList<QByteArray> TZs = QTimeZone::availableTimeZoneIds() ;
  QString           T                                       ;
  for (int i=0;i<TZs.count();i++)                           {
    T      = QString::fromUtf8 ( TZs[i] )                   ;
    combo -> addItem ( T )                                  ;
  }                                                         ;
}

void N::ForexCommodity::addLevels(ComboBox * combo)
{
  for (int i='A' ; i <= 'Z' ; i++ )                        {
    combo -> addItem ( QString(QChar((char)i)) , i - 'A' ) ;
  }                                                        ;
}

void N::ForexCommodity::addYears(ComboBox * combo)
{
  for (int i=1900;i<=2100;i++)                  {
    combo -> addItem ( QString::number(i) , i ) ;
  }                                             ;
}

void N::ForexCommodity::Insert(void)
{
  NewTreeWidgetItem        ( IT                   ) ;
  removeOldItem            ( true , ItemColumn    ) ;
  for (int i=0;i<10;i++)                            {
    IT -> setData          ( i , Qt::UserRole , 0 ) ;
  }                                                 ;
  if ( level . length ( ) > 0 )                     {
    int u = level . at ( 0 ) . toLatin1 ( ) - 'A'   ;
    IT          -> setText ( 6 , level            ) ;
    ItemEditing -> setData ( 6 , Qt::UserRole , u ) ;
  }                                                 ;
  addTopLevelItem          ( IT                   ) ;
  DoProcessEvents                                   ;
  scrollToItem             ( IT                   ) ;
  doubleClicked            ( IT , 0               ) ;
}

void N::ForexCommodity::Delete(void)
{
  TreeWidgetItems items = selectedItems ( ) ;
  nDropOut ( items . count ( ) <= 0 )       ;
  TreeWidgetItems deletion                  ;
  ///////////////////////////////////////////
  for (int i=0;i<items.count();i++)         {
    QTreeWidgetItem * it                    ;
    int               s                     ;
    it = items [ i ]                        ;
    deletion << it                      ;
  }                                         ;
  ///////////////////////////////////////////
  nDropOut ( deletion . count ( ) <= 0 )    ;
  ///////////////////////////////////////////
  for (int i=0;i<deletion.count();i++)      {
    takeItem ( deletion [ i ] )             ;
  }                                         ;
  Alert ( Done )                            ;
}

void N::ForexCommodity::Paste(void)
{
  QString T = nClipboardText                   ;
  if ( T . length ( ) <= 0 ) return            ;
  QStringList L = T . split ( "\n" )           ;
  L = File::PurifyLines ( L )                  ;
  foreach ( T , L ) if ( T . length ( ) > 0 )  {
    NewTreeWidgetItem ( IT                   ) ;
    for (int i=0;i<10;i++)                     {
      IT -> setData   ( i , Qt::UserRole , 0 ) ;
    }                                          ;
    IT -> setText     ( 0 , T                ) ;
    addTopLevelItem   ( IT                   ) ;
    DoProcessEvents                            ;
    scrollToItem      ( IT                   ) ;
  }                                            ;
}

void N::ForexCommodity::setLevel(QString Level)
{
  level = Level ;
}

void N::ForexCommodity::UpdateItem(QTreeWidgetItem * item)
{
  Forex::Importer FI                                       ;
  FI . Name      = item->text(0)                           ;
  FI . Supplier  = uuid                                    ;
  FI . Pair      = nTreeUuid(item,1)                       ;
  FI . SQL       = nTreeUuid(item,2)                       ;
  FI . setDigits ( nTreeInt (item,3) )                     ;
  FI . Full      = nTreeInt (item,4)                       ;
  FI . TimeZone  = item -> text ( 5 )                      ;
  FI . setLevel  ( nTreeInt (item,6) )                     ;
  FI . FromYear  = nTreeInt (item,7)                       ;
  FI . ToYear    = nTreeInt (item,8)                       ;
  FI . Id        = nTreeInt (item,9)                       ;
  //////////////////////////////////////////////////////////
  if ( ! FI . isCorrect ( ) )                              {
    Alert ( Click )                                        ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  EnterSQL  ( SC , plan -> sql  )                          ;
    bool correct                                           ;
    correct = FI . Renew                                   (
      SC                                                   ,
      PlanTable ( ForexCommodities )                     ) ;
    if      ( correct           )                          {
      item -> setData ( 6 , Qt::UserRole , FI.Level      ) ;
      item -> setText ( 6 , FI.level                     ) ;
      item -> setData ( 7 , Qt::UserRole , FI.FromYear   ) ;
      item -> setText ( 7 , QString::number(FI.FromYear) ) ;
      item -> setData ( 8 , Qt::UserRole , FI.ToYear     ) ;
      item -> setText ( 8 , QString::number(FI.ToYear)   ) ;
      item -> setData ( 9 , Qt::UserRole , FI.Id         ) ;
      Alert ( Done              )                          ;
    } else                                                 {
      Alert ( Error             )                          ;
    }                                                      ;
  ErrorSQL  ( SC , plan -> sql  )                          ;
    Alert   ( Error             )                          ;
  LeaveSQL  ( SC , plan -> sql  )                          ;
}

void N::ForexCommodity::nameFinished(void)
{
  QLineEdit * name = Casting   ( QLineEdit  , ItemWidget ) ;
  if (IsNull(name))                                        {
    removeOldItem              (                         ) ;
    return                                                 ;
  }                                                        ;
  QString goods = name -> text (                         ) ;
  if                           ( goods . length ( ) <= 0 ) {
    removeOldItem              (                         ) ;
    Alert                      ( Error                   ) ;
    return                                                 ;
  }                                                        ;
  ItemEditing -> setText       ( 0 , goods               ) ;
  UpdateItem                   ( ItemEditing             ) ;
  removeOldItem                (                         ) ;
}

void N::ForexCommodity::currencyChanged (int currency)
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
  ItemEditing -> setData     ( 1    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 1                ) ;
}

void N::ForexCommodity::connectChanged(int C)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( C                       ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 2    , n                ) ;
  ItemEditing -> setData     ( 2    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 2                ) ;
}

void N::ForexCommodity::digitChanged(int digit)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( digit                   ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 3    , n                ) ;
  ItemEditing -> setData     ( 3    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 3                ) ;
}

void N::ForexCommodity::fullChanged(int digit)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( digit                   ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 4    , n                ) ;
  ItemEditing -> setData     ( 4    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 4                ) ;
}

void N::ForexCommodity::timezoneChanged(int)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  QString n                                              ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 5    , n                ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 5                ) ;
}

void N::ForexCommodity::levelChanged(int Level)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( Level                   ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 6    , n                ) ;
  ItemEditing -> setData     ( 6    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 6                ) ;
}

void N::ForexCommodity::fromChanged(int from)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( from                    ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 7    , n                ) ;
  ItemEditing -> setData     ( 7    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 7                ) ;
}

void N::ForexCommodity::toChanged(int to)
{
  nDropOut                   ( IsNull ( ItemEditing )  ) ;
  ComboBox * comb                                        ;
  comb = Casting             ( ComboBox , ItemWidget   ) ;
  nDropOut                   ( IsNull(comb)            ) ;
  SUID    u                                              ;
  QString n                                              ;
  u = comb    -> toUuid      ( to                      ) ;
  n = comb    -> currentText (                         ) ;
  ItemEditing -> setText     ( 8    , n                ) ;
  ItemEditing -> setData     ( 8    , Qt::UserRole , u ) ;
  UpdateItem                 ( ItemEditing             ) ;
  removeOldItem              ( true , 8                ) ;
}

bool N::ForexCommodity::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                 ;
  QAction         * aa                      ;
  QTreeWidgetItem * it = itemAt ( pos )     ;
  int               cc = currentColumn ( )  ;
  ///////////////////////////////////////////
  mm   . add ( 101 , tr("Refresh") )        ;
  mm   . addSeparator ( )                   ;
  mm   . add ( 201 , tr("Add"    ) )        ;
  mm   . add ( 202 , tr("Delete" ) )        ;
  mm   . add ( 203 , tr("Paste"  ) )        ;
  mm   . addSeparator ( )                   ;
  DockingMenu ( mm        )                 ;
  aa = mm . exec ( )                        ;
  ///////////////////////////////////////////
  if ( IsNull     ( aa      ) ) return true ;
  if ( RunDocking ( mm , aa ) ) return true ;
  ///////////////////////////////////////////
  switch ( mm [ aa ] )                      {
    case 101                                :
      startup ( )                           ;
    break                                   ;
    case 201                                :
      Insert  ( )                           ;
    break                                   ;
    case 202                                :
      Delete  ( )                           ;
    break                                   ;
    case 203                                :
      Paste   ( )                           ;
    break                                   ;
    default                                 :
    break                                   ;
  }                                         ;
  return true                               ;
}
