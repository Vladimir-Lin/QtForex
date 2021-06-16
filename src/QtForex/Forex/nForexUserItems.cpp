#include <qtforex.h>

N::Finance::UserItems:: UserItems ( QObject * parent )
                      : QObject   (           parent )
                      , Users     (                  )
                      , engine    ( NULL             )
{
  functions . setEnabled ( "All"         , true  ) ;
  functions . setEnabled ( "Online"      , false ) ;
  functions . setEnabled ( "Group"       , false ) ;
  functions . setEnabled ( "Logins"      , false ) ;
  functions . setEnabled ( "Names"       , false ) ;
  functions . setEnabled ( "Script"      , false ) ;
  functions . setEnabled ( "DeleteItems" , false ) ;
  functions . setEnabled ( "Render"      , false ) ;
  functions . setEnabled ( "First"       , true  ) ;
  nConnect ( this , SIGNAL ( EmitChanged ( ) )     ,
             this , SLOT   ( SendChanged ( ) )   ) ;
}

N::Finance::UserItems::~UserItems(void)
{
  if ( functions . isEnabled ( "DeleteItems" ) ) {
    UUIDs L = items . keys ( )                   ;
    SUID  login                                  ;
    foreach ( login , L )                        {
      delete items [ login ]                     ;
    }                                            ;
    items    . clear ( )                         ;
    logins   . clear ( )                         ;
    visible  . clear ( )                         ;
    ByLogins . clear ( )                         ;
    ByNames  . clear ( )                         ;
  }                                              ;
  ////////////////////////////////////////////////
  if ( NotNull(engine) )                         {
    delete engine                                ;
    engine = NULL                                ;
  }                                              ;
}

bool N::Finance::UserItems::hasEngine(void)
{
  return NotNull ( engine ) ;
}

int N::Finance::UserItems::take(int login)
{
  if ( ! users . contains ( login ) ) {
    return users . count ( )          ;
  }                                   ;
  User            * u                 ;
  QTreeWidgetItem * it                ;
  u  = users [ login ]                ;
  it = items [ login ]                ;
  users   . remove ( login )          ;
  enabled . remove ( login )          ;
  if ( NULL != u  ) delete u          ;
  if ( NULL != it ) delete it         ;
  return users . count ( )            ;
}

void N::Finance::UserItems::assure(int login)
{
  if ( users . contains ( login ) ) return             ;
  if ( items . contains ( login ) ) return             ;
  users   [ login ]  = new User            ( )         ;
  items   [ login ]  = new QTreeWidgetItem ( )         ;
  enabled [ login ]  = 0                               ;
  //////////////////////////////////////////////////////
  int align = Qt::AlignRight | Qt::AlignVCenter        ;
  items   [ login ] -> setTextAlignment (  2 , align ) ;
  items   [ login ] -> setTextAlignment (  3 , align ) ;
  items   [ login ] -> setTextAlignment (  5 , align ) ;
  items   [ login ] -> setTextAlignment (  6 , align ) ;
  items   [ login ] -> setTextAlignment (  7 , align ) ;
  items   [ login ] -> setTextAlignment (  8 , align ) ;
  items   [ login ] -> setTextAlignment ( 10 , align ) ;
}

QString N::Finance::UserItems::toString(double value,int digits)
{
  int     multiply = 1                          ;
  qint64  v                                     ;
  QString s                                     ;
  int     L                                     ;
  for (int i=0;i<digits;i++) multiply *= 10     ;
  v  = (qint64) ( ( value * multiply ) + 0.45 ) ;
  s  = QString::number(v)                       ;
  L  = s . length ( )                           ;
  if ( L <= digits )                            {
    for (int i=L;i<digits;i++)                  {
      s  . prepend ( "0" )                      ;
    }                                           ;
    s  . prepend ( "0."    )                    ;
  } else                                        {
    L -= digits                                 ;
    s  . insert  ( L , '.' )                    ;
  }                                             ;
  return s                                      ;
}

void N::Finance::UserItems::toItem(int login)
{
  if ( ! users . contains ( login ) ) return                   ;
  if ( ! items . contains ( login ) ) return                   ;
  //////////////////////////////////////////////////////////////
  User            * u  = users [ login ]                       ;
  QTreeWidgetItem * it = items [ login ]                       ;
  QString           v                                          ;
  //////////////////////////////////////////////////////////////
  if ( 0 == u -> enable )                                      {
//    v = QChar ( 0x2717 )                                       ;
    v = QChar ( 0x2718 )                                       ;
    it -> setText   (  0 , v                                 ) ;
  } else                                                       {
    v = QChar ( 0x2705 )                                       ;
//    v = QChar ( 0x2713 )                                       ;
//    v = QChar ( 0x2714 )                                       ;
    it -> setText   (  0 , v                                 ) ;
  }                                                            ;
  if ( u -> online )                                           {
    v = QChar ( 0x21c4 )                                       ;
    it -> setText   (  1 , v                                 ) ;
  }                                                            ;
  it   -> setText   (  2 , QString::number ( u -> login    ) ) ;
  it   -> setText   (  3 , QString::number ( u -> leverage ) ) ;
  it   -> setText   (  4 , u -> name                         ) ;
  it   -> setText   (  5 , toString ( u -> balance , 2     ) ) ;
  it   -> setText   (  6 , toString ( u -> equity  , 2     ) ) ;
  it   -> setText   (  7 , toString ( u -> margin  , 2     ) ) ;
  it   -> setText   (  8 , toString ( u -> lots()  , 2     ) ) ;
  it   -> setText   (  9 , u -> IP                           ) ;
  it   -> setText   ( 10 , QString::number ( u -> counter  ) ) ;
  it   -> setText   ( 11 , u -> group                        ) ;
  it   -> setText   ( 12 , u -> city                         ) ;
  it   -> setText   ( 13 , u -> phone                        ) ;
  it   -> setText   ( 14 , u -> email                        ) ;
  it   -> setText   ( 15 , u -> status                       ) ;
  //////////////////////////////////////////////////////////////
  QBrush brush                                                 ;
  //////////////////////////////////////////////////////////////
  if ( u -> balance < 0 )                                      {
    brush = QBrush ( QColor ( 255 ,   0 ,   0 ) )              ;
    for (int i=2;i<16;i++) it -> setForeground ( i , brush )   ;
    return                                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  if ( u -> volume  > 0 )                                      {
    brush = QBrush ( QColor (   0 ,   0 , 255 ) )              ;
    for (int i=2;i<16;i++) it -> setForeground ( i , brush )   ;
    return                                                     ;
  }                                                            ;
}

void N::Finance::UserItems::operate(void)
{
  functions . setEnabled ( "Updated" , true ) ;
  emit EmitChanged ( )                        ;
}

void N::Finance::UserItems::SendChanged (void)
{
  emit Changed ( ) ;
}

void N::Finance::UserItems::Sync(QTreeWidget * tree)
{
  if ( functions . isEnabled ( "All"    ) ) {
    SyncAll     ( tree )                    ;
    return                                  ;
  }                                         ;
  if ( functions . isEnabled ( "Online" ) ) {
    SyncOnlines ( tree )                    ;
    return                                  ;
  }                                         ;
  if ( functions . isEnabled ( "Group"  ) ) {
    SyncGroup   ( tree )                    ;
    return                                  ;
  }                                         ;
  if ( functions . isEnabled ( "Logins" ) ) {
    SyncLogins  ( tree )                    ;
    return                                  ;
  }                                         ;
  if ( functions . isEnabled ( "Names"  ) ) {
    SyncNames   ( tree )                    ;
    return                                  ;
  }                                         ;
  if ( functions . isEnabled ( "Script" ) ) {
    SyncScript  ( tree )                    ;
    return                                  ;
  }                                         ;
}

void N::Finance::UserItems::add(QTreeWidget * tree,TreeWidgetItems it)
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
  QString s     = tr("%1 accounts").arg(total)  ;
  tree -> setToolTip ( s )                      ;
}

void N::Finance::UserItems::remove(QTreeWidget * tree,TreeWidgetItems it)
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

void N::Finance::UserItems::SyncAll(QTreeWidget * tree)
{
  CUIDs             L = users . keys ( )         ;
  int               login                        ;
  TreeWidgetItems   twi                          ;
  TreeWidgetItems   rwi                          ;
  QTreeWidgetItem * it                           ;
  foreach ( login , L )                          {
    it = items [ login ]                         ;
    if ( 0 == enabled [ login ] )                {
      if (   logins . contains ( it ) )          {
        rwi << it                                ;
        logins . remove ( it )                   ;
        visible [ login ] = false                ;
      }                                          ;
    } else                                       {
      if ( ! logins . contains ( it ) )          {
        logins [ it ] = login                    ;
        visible [ login ] = true                 ;
        twi << it                                ;
      } else                                     {
        if ( ! visible [ login ] )               {
          twi << it                              ;
        }                                        ;
        visible [ login ] = true                 ;
      }                                          ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  remove ( tree , rwi )                          ;
  add    ( tree , twi )                          ;
}

void N::Finance::UserItems::SyncOnlines(QTreeWidget * tree)
{
  CUIDs             L = users . keys ( )           ;
  int               login                          ;
  TreeWidgetItems   twi                            ;
  TreeWidgetItems   rwi                            ;
  QTreeWidgetItem * it                             ;
  foreach ( login , L )                            {
    it = items [ login ]                           ;
    if ( 0 == enabled [ login ] )                  {
      if (   logins . contains ( it ) )            {
        rwi << it                                  ;
        logins . remove ( it )                     ;
        visible [ login ] = false                  ;
      }                                            ;
    } else                                         {
      if ( ! logins . contains ( it ) )            {
        logins [ it ] = login                      ;
        if ( users [ login ] -> online )           {
          twi << it                                ;
          visible [ login ] = true                 ;
        } else                                     {
          visible [ login ] = false                ;
        }                                          ;
      } else                                       {
        if ( ! users [ login ] -> online )         {
          rwi << it                                ;
          visible [ login ] = false                ;
        } else                                     {
          if ( ! visible [ login ] )               {
            twi << it                              ;
          }                                        ;
          visible [ login ] = true                 ;
        }                                          ;
      }                                            ;
    }                                              ;
  }                                                ;
  //////////////////////////////////////////////////
  remove ( tree , rwi )                            ;
  add    ( tree , twi )                            ;
}

void N::Finance::UserItems::SyncGroup(QTreeWidget * tree)
{
  CUIDs             L = users . keys ( )         ;
  int               login                        ;
  TreeWidgetItems   twi                          ;
  TreeWidgetItems   rwi                          ;
  QTreeWidgetItem * it                           ;
  QString           group                        ;
  group = Variables [ "Group" ] . toString ( )   ;
  foreach ( login , L )                          {
    it = items [ login ]                         ;
    if ( 0 == enabled [ login ] )                {
      if (   logins . contains ( it ) )          {
        rwi << it                                ;
        logins . remove ( it )                   ;
        visible [ login ] = false                ;
      }                                          ;
    } else                                       {
      if ( ! logins . contains ( it ) )          {
        logins [ it ] = login                    ;
        if ( group == users [ login ] -> group ) {
          twi << it                              ;
          visible [ login ] = true               ;
        } else                                   {
          visible [ login ] = false              ;
        }                                        ;
      } else                                     {
        if ( group != users [ login ] -> group ) {
          rwi << it                              ;
          visible [ login ] = false              ;
        } else                                   {
          if ( ! visible [ login ] )             {
            twi << it                            ;
          }                                      ;
          visible [ login ] = true               ;
        }                                        ;
      }                                          ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  remove ( tree , rwi )                          ;
  add    ( tree , twi )                          ;
}

void N::Finance::UserItems::SyncLogins(QTreeWidget * tree)
{
  CUIDs             L = users . keys ( )           ;
  int               login                          ;
  TreeWidgetItems   twi                            ;
  TreeWidgetItems   rwi                            ;
  QTreeWidgetItem * it                             ;
  foreach ( login , L )                            {
    it = items [ login ]                           ;
    if ( 0 == enabled [ login ] )                  {
      if (   logins . contains ( it ) )            {
        rwi << it                                  ;
        logins . remove ( it )                     ;
        visible [ login ] = false                  ;
      }                                            ;
    } else                                         {
      if ( ! logins . contains ( it ) )            {
        logins [ it ] = login                      ;
        if (   ByLogins . contains ( login ) )     {
          twi << it                                ;
          visible [ login ] = true                 ;
        } else                                     {
          visible [ login ] = false                ;
        }                                          ;
      } else                                       {
        if ( ! ByLogins . contains ( login ) )     {
          rwi << it                                ;
          visible [ login ] = false                ;
        } else                                     {
          if ( ! visible [ login ] )               {
            twi << it                              ;
          }                                        ;
          visible [ login ] = true                 ;
        }                                          ;
      }                                            ;
    }                                              ;
  }                                                ;
  //////////////////////////////////////////////////
  remove ( tree , rwi )                            ;
  add    ( tree , twi )                            ;
}

void N::Finance::UserItems::SyncNames(QTreeWidget * tree)
{
  CUIDs             L = users . keys ( )                        ;
  int               login                                       ;
  TreeWidgetItems   twi                                         ;
  TreeWidgetItems   rwi                                         ;
  QTreeWidgetItem * it                                          ;
  foreach ( login , L )                                         {
    it = items [ login ]                                        ;
    if ( 0 == enabled [ login ] )                               {
      if (   logins . contains ( it ) )                         {
        rwi << it                                               ;
        logins . remove ( it )                                  ;
        visible [ login ] = false                               ;
      }                                                         ;
    } else                                                      {
      if ( ! logins . contains ( it ) )                         {
        logins [ it ] = login                                   ;
        if (   ByNames . contains ( users [ login ] -> name ) ) {
          twi << it                                             ;
          visible [ login ] = true                              ;
        } else                                                  {
          visible [ login ] = false                             ;
        }                                                       ;
      } else                                                    {
        if ( ! ByNames . contains ( users [ login ] -> name ) ) {
          rwi << it                                             ;
          visible [ login ] = false                             ;
        } else                                                  {
          if ( ! visible [ login ] )                            {
            twi << it                                           ;
          }                                                     ;
          visible [ login ] = true                              ;
        }                                                       ;
      }                                                         ;
    }                                                           ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  remove ( tree , rwi )                                         ;
  add    ( tree , twi )                                         ;
}

bool N::Finance::UserItems::CallScript(QScriptValue & eval,User & u)
{
  QScriptValue     r                                 ;
  QScriptValueList args                              ;
  bool             result = false                    ;
  args  << u . enable                                ;
  args  << u . online                                ;
  args  << u . login                                 ;
  args  << u . leverage                              ;
  args  << u . name                                  ;
  args  << u . balance                               ;
  args  << u . equity                                ;
  args  << u . margin                                ;
  args  << u . lots ( )                              ;
  args  << u . IP                                    ;
  args  << u . counter                               ;
  args  << u . group                                 ;
  args  << u . country                               ;
  args  << u . phone                                 ;
  args  << u . email                                 ;
  args  << u . status                                ;
  r      = eval . call   ( QScriptValue ( ) , args ) ;
  result = r    . toBool (                         ) ;
  return result                                      ;
}

void N::Finance::UserItems::SyncScript(QTreeWidget * tree)
{
  if ( IsNull ( engine )      ) return                           ;
  QString           java   = Variables [ "Java" ] . toString ( ) ;
  if ( java . length ( ) <= 0 ) return                           ;
  CUIDs             L = users . keys ( )                         ;
  int               login                                        ;
  TreeWidgetItems   twi                                          ;
  TreeWidgetItems   rwi                                          ;
  QTreeWidgetItem * it                                           ;
  QScriptValue      global = engine -> globalObject (      )     ;
  QScriptValue      eval   = global  . property     ( java )     ;
  if ( ! eval . isFunction ( ) ) return                          ;
  foreach ( login , L )                                          {
    it = items [ login ]                                         ;
    if ( 0 == enabled [ login ] )                                {
      if (   logins . contains ( it ) )                          {
        rwi << it                                                ;
        logins . remove ( it )                                   ;
        visible [ login ] = false                                ;
      }                                                          ;
    } else                                                       {
      if ( ! logins . contains ( it ) )                          {
        logins [ it ] = login                                    ;
        if (   CallScript ( eval , * ( users [ login ] ) ) )     {
          twi << it                                              ;
          visible [ login ] = true                               ;
        } else                                                   {
          visible [ login ] = false                              ;
        }                                                        ;
      } else                                                     {
        if ( ! CallScript ( eval , * ( users [ login ] ) ) )     {
          rwi << it                                              ;
          visible [ login ] = false                              ;
        } else                                                   {
          if ( ! visible [ login ] )                             {
            twi << it                                            ;
          }                                                      ;
          visible [ login ] = true                               ;
        }                                                        ;
      }                                                          ;
    }                                                            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  engine -> collectGarbage (            )                        ;
  remove                   ( tree , rwi )                        ;
  add                      ( tree , twi )                        ;
}

void N::Finance::UserItems::setEngine(QScriptEngine * e)
{
  if ( NULL != engine ) delete engine ;
  engine = e                          ;
}

void N::Finance::UserItems::Render(void)
{
  CUIDs L = users . keys ( )                   ;
  int   c = 0                                  ;
  int   login                                  ;
  foreach ( login , L )                        {
    c++                                        ;
    toItem ( login )                           ;
    if ( 0 == ( c % 10 ) )                     {
      qApp -> processEvents ( )                ;
    }                                          ;
  }                                            ;
  functions . setEnabled ( "Updated" , false ) ;
  functions . setEnabled ( "Render"  , false ) ;
}
