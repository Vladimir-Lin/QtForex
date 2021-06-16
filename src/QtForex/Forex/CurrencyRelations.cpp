#include <qtforex.h>

N::CurrencyRelations:: CurrencyRelations ( QWidget * parent , Plan * p )
                     : TreeDock          (           parent ,        p )
{
  Configure ( ) ;
}

N::CurrencyRelations::~CurrencyRelations(void)
{
}

void N::CurrencyRelations::Configure(void)
{
  NewTreeWidgetItem            ( head                            ) ;
  head -> setText              ( 0 , tr ( "Symbol"             ) ) ;
  head -> setText              ( 1 , tr ( "Base currency"      ) ) ;
  head -> setText              ( 2 , tr ( "Counter currency"   ) ) ;
  head -> setText              ( 3 , tr ( "Name"               ) ) ;
  head -> setText              ( 4 ,      ""                     ) ;
  setWindowTitle               (     tr ( "Currency pairs"     ) ) ;
  setWindowIcon                ( QIcon  ( ":/images/forex.png" ) ) ;
  setDragDropMode              ( DropOnly                        ) ;
  setRootIsDecorated           ( false                           ) ;
  setAlternatingRowColors      ( true                            ) ;
  setSelectionMode             ( ExtendedSelection               ) ;
  setColumnCount               ( 5                               ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded           ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded           ) ;
  assignHeaderItems            ( head                            ) ;
  plan -> setFont              ( this                            ) ;
  MountClicked                 ( 2                               ) ;
  //////////////////////////////////////////////////////////////////
  LimitValues [ 21 ] = 1                                           ;
  //////////////////////////////////////////////////////////////////
  nConnect ( this , SIGNAL ( loadFinished() )                      ,
             this , SLOT   ( listItems   () )                    ) ;
}

bool N::CurrencyRelations::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  return true                                              ;
}

void N::CurrencyRelations::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( Type ) {
    case 10001      :
      List (      ) ;
    break           ;
  }                 ;
}

bool N::CurrencyRelations::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::CurrencyRelations::listItems(void)
{
  int id                                                      ;
  clear               (                                     ) ;
  foreach             ( id , currencies   . Pairs           ) {
    int f =           ( id / 1000                           ) ;
    int t =           ( id % 1000                           ) ;
    NewTreeWidgetItem ( it                                  ) ;
    it  -> setData    ( 0  , Qt::UserRole , id              ) ;
    it  -> setData    ( 1  , Qt::UserRole , f               ) ;
    it  -> setData    ( 2  , Qt::UserRole , t               ) ;
    it  -> setText    ( 0  , currencies   . Pairname ( id ) ) ;
    it  -> setText    ( 1  , currencies   . IdMap    [ f  ] ) ;
    it  -> setText    ( 2  , currencies   . IdMap    [ t  ] ) ;
    it  -> setText    ( 3  , currencies   . Names    [ id ] ) ;
    addTopLevelItem   ( it                                  ) ;
  }                                                           ;
  emit AutoFit        (                                     ) ;
  reportItems         (                                     ) ;
}

void N::CurrencyRelations::List(void)
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
      SUID    u = currencies . PairUuid [ id ]                       ;
      n = SC . getName     ( PlanTable(Names),"uuid",vLanguageId,u ) ;
      currencies . Names [ id ] = n                                  ;
    }                                                                ;
    SC . close             (                                       ) ;
  }                                                                  ;
  SC   . remove            (                                       ) ;
  emit loadFinished        (                                       ) ;
}

void N::CurrencyRelations::Insert(void)
{
  if (LimitValues[21]<=0) return             ;
  NewTreeWidgetItem ( item                 ) ;
  item -> setData   ( 0 , Qt::UserRole , 0 ) ;
  item -> setData   ( 1 , Qt::UserRole , 0 ) ;
  item -> setData   ( 2 , Qt::UserRole , 0 ) ;
  setAlignments     ( item                 ) ;
  addTopLevelItem   ( item                 ) ;
  scrollToItem      ( item                 ) ;
  doubleClicked     ( item , 1             ) ;
}

void N::CurrencyRelations::doubleClicked(QTreeWidgetItem * item,int column)
{
  if (column == 0)                        {
    int s = nTreeInt ( item , 0 )         ;
    if (s<=0)                             {
      Alert ( Error )                     ;
      return                              ;
    }                                     ;
    SUID u = currencies . PairUuid [ s ]  ;
    emit Edit ( item->text(3) , u )       ;
    return                                ;
  }                                       ;
  /////////////////////////////////////////
  if (column == 3)                        {
    ///////////////////////////////////////
    int s = nTreeInt ( item , 0 )         ;
    if (s<=0)                             {
      Alert ( Error )                     ;
      return                              ;
    }                                     ;
    ///////////////////////////////////////
    QLineEdit * line                      ;
    line = setLineEdit                    (
             item,column                  ,
             SIGNAL(returnPressed  ())    ,
             SLOT  (nameChanged    ()) )  ;
    line->setFocus( Qt::TabFocusReason )  ;
    ///////////////////////////////////////
    return                                ;
  }                                       ;
  /////////////////////////////////////////
  if (column != 1 && column != 2 ) return ;
  /////////////////////////////////////////
  int f = nTreeInt ( item , 1 )           ;
  int x = nTreeInt ( item , 2 )           ;
  if ( f     != 0   && x    != 0 ) return ;
  /////////////////////////////////////////
  ComboBox  * cc                          ;
  int t = nTreeInt ( item , column )      ;
  cc   = (ComboBox *) setComboBox         (
           item,column                    ,
           SIGNAL(activated      (int))   ,
           SLOT  (currencyChanged(int)) ) ;
  /////////////////////////////////////////
  currencies . append    ( cc , 2 )       ;
  currencies . indexOf   ( cc , t )       ;
  cc->setMaxVisibleItems ( 30     )       ;
  cc->setEditable        ( true   )       ;
  cc->showPopup          (        )       ;
}

void N::CurrencyRelations::currencyChanged(int)
{
  if (IsNull(ItemEditing)) return                           ;
  QComboBox * cc = Casting(QComboBox,ItemWidget)            ;
  if (IsNull(cc)) return                                    ;
  SUID au = N::GetUuid(cc)                                  ;
  ItemEditing -> setText ( ItemColumn , cc->currentText() ) ;
  ItemEditing -> setData ( ItemColumn , Qt::UserRole , au ) ;
  Finalize               ( ItemEditing                    ) ;
}

void N::CurrencyRelations::nameChanged(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  //////////////////////////////////////////////////
  QString name = line->text()                      ;
  int     sid  = nTreeInt ( ItemEditing , 0 )      ;
  if (sid<=0)                                      {
    removeOldItem ( false , 3 )                    ;
    return                                         ;
  }                                                ;
  //////////////////////////////////////////////////
  SUID u = currencies . PairUuid [ sid ]           ;
  EnterSQL      ( SC , plan->sql )                 ;
    if (u>0)                                       {
      SC . assureName                              (
        PlanTable(Names)                           ,
        u                                          ,
        vLanguageId                                ,
        name                                    )  ;
      ItemEditing -> setText ( 3 , line->text() )  ;
    }                                              ;
  LeaveSQL      ( SC , plan->sql )                 ;
  Alert         ( Done           )                 ;
  removeOldItem ( false , 3      )                 ;
}

void N::CurrencyRelations::Finalize(QTreeWidgetItem * item)
{
  int f = nTreeInt ( item , 1 )             ;
  int x = nTreeInt ( item , 2 )             ;
  ///////////////////////////////////////////
  if ( f == 0 || x == 0 )                   {
    removeOldItem ( false , ItemColumn )    ;
    return                                  ;
  }                                         ;
  ///////////////////////////////////////////
  int id = ( f * 1000 ) + x                 ;
  if (currencies.PairUuid.contains(id))     {
    removeOldItem ( true , ItemColumn  )    ;
    Alert         ( Error              )    ;
    return                                  ;
  }                                         ;
  ///////////////////////////////////////////
  QString pair                              ;
  QString name                              ;
  QString F                                 ;
  QString X                                 ;
  ///////////////////////////////////////////
  F     = currencies . IdMap    [ f  ]      ;
  X     = currencies . IdMap    [ x  ]      ;
  pair  = currencies . Pairname ( id )      ;
  name  = currencies . Names    [ f  ]      ;
  name += tr("/")                           ;
  name += currencies . Names    [ x  ]      ;
  item -> setData ( 0 , Qt::UserRole , id ) ;
  item -> setData ( 1 , Qt::UserRole , f  ) ;
  item -> setData ( 2 , Qt::UserRole , x  ) ;
  item -> setText ( 0 , pair              ) ;
  item -> setText ( 1 , F                 ) ;
  item -> setText ( 2 , X                 ) ;
  item -> setText ( 3 , name              ) ;
  ///////////////////////////////////////////
  EnterSQL ( SC , plan->sql )               ;
    /////////////////////////////////////////
    QString Q                               ;
    SUID    u                               ;
    /////////////////////////////////////////
    u = SC . Unique                         (
          PlanTable(MajorUuid)              ,
          "uuid"                            ,
          9008                            ) ;
    SC . assureUuid                         (
      PlanTable(MajorUuid)                  ,
      u                                     ,
      Types::CurrencyPair                 ) ;
    /////////////////////////////////////////
    Q = SC . sql . InsertInto               (
          PlanTable(CurrencyPairs)          ,
          3                                 ,
          "uuid"                            ,
          "fromcurrency"                    ,
          "tocurrency"                    ) ;
    SC . Prepare ( Q                      ) ;
    SC . Bind    ( "uuid"         , u     ) ;
    SC . Bind    ( "fromcurrency" , f     ) ;
    SC . Bind    ( "tocurrency"   , x     ) ;
    SC . Exec    (                        ) ;
    /////////////////////////////////////////
    SC . assureName                         (
      PlanTable(Names)                      ,
      u                                     ,
      vLanguageId                           ,
      name                                ) ;
    /////////////////////////////////////////
    currencies . Pairs          << id       ;
    currencies . PairUuid [ id ] = u        ;
    currencies . UuidPair [ u  ] = id       ;
    currencies . Names    [ id ] = name     ;
    /////////////////////////////////////////
  LeaveSQL      ( SC    , plan->sql  )      ;
  removeOldItem ( false , ItemColumn )      ;
  Alert         ( Done               )      ;
}

bool N::CurrencyRelations::Menu(QPoint pos)
{
  QTreeWidgetItem * item = itemAt ( pos )             ;
  MenuManager       mm ( this )                       ;
  QAction         * aa                                ;
  UUIDs             Uuids                             ;
  if (NotNull(item))                                  {
    mm.add(101,tr("Edit"))                            ;
  }                                                   ;
  mm . add ( 201 , tr("Insert"      )               ) ;
  mm . addSeparator()                                 ;
  mm . add ( 501 , tr("Refresh"     )               ) ;
  mm . addSeparator()                                 ;
  mm . add ( 601 , tr("Translations")               ) ;
  DockingMenu  ( mm   )                               ;
  mm . setFont ( plan )                               ;
  aa = mm.exec()                                      ;
  if (IsNull(aa)) return true                         ;
  if (RunDocking(mm,aa)) return true                  ;
  switch (mm[aa])                                     {
    case 101                                          :
      if (NotNull(item))                              {
        int  s = nTreeInt ( item , 0 )                ;
        SUID u = currencies . PairUuid [ s ]          ;
        emit Edit ( item->text(3) , u )               ;
      }                                               ;
    break                                             ;
    case 201                                          :
      Insert  ( )                                     ;
    break                                             ;
    case 501                                          :
      startup ( )                                     ;
    break                                             ;
    case 601                                          :
      Uuids = currencies . UuidPair . keys ( )        ;
      emit Translations ( windowTitle() , Uuids )     ;
    break                                             ;
    default                                           :
    break                                             ;
  }                                                   ;
  return true                                         ;
}
