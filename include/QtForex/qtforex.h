/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001 ~ 2016 Neutrino International Inc.                    *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_FOREX_H
#define QT_FOREX_H

#include <Business>
#include <QtAV>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_QTFOREX_LIB)
#      define Q_FOREX_EXPORT Q_DECL_EXPORT
#    else
#      define Q_FOREX_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_FOREX_EXPORT
#endif

namespace Ui                     {
  class TraderCapital            ;
  class nForexBridgeConfigurator ;
  class SelectChart              ;
}

namespace N
{

class Q_FOREX_EXPORT CurrencyRelations       ;
class Q_FOREX_EXPORT TickTreeItems           ;
class Q_FOREX_EXPORT TraderCapital           ;
class Q_FOREX_EXPORT ForexBridgeConfigurator ;
class Q_FOREX_EXPORT SymbolsQuotes           ;
class Q_FOREX_EXPORT ForexTraders            ;
class Q_FOREX_EXPORT ForexOrders             ;
class Q_FOREX_EXPORT ForexBalance            ;
class Q_FOREX_EXPORT QuotesImporter          ;
class Q_FOREX_EXPORT ForexCommodity          ;
class Q_FOREX_EXPORT SelectChart             ;
class Q_FOREX_EXPORT PriceEditor             ;

namespace Finance
{

class Q_FOREX_EXPORT UserItems    ;
class Q_FOREX_EXPORT OrderItems   ;
class Q_FOREX_EXPORT BalanceItems ;

}

namespace Forex
{

class Q_FOREX_EXPORT AnalyizerPlugin ;
class Q_FOREX_EXPORT ChartPlugin     ;
class Q_FOREX_EXPORT ConnectorGui    ;
class Q_FOREX_EXPORT BridgesManager  ;

}

namespace Finance
{

class Q_FOREX_EXPORT UserItems : public QObject
                               , public Users
{
  Q_OBJECT
  public:

    WMAPs       Variables ;
    BMAPs       visible   ;
    CUIDs       ByLogins  ;
    QStringList ByNames   ;

    explicit     UserItems   (QObject * parent = NULL) ;
    virtual     ~UserItems   (void) ;

    virtual int  take        (int login) ;
    virtual void operate     (void) ;
    virtual bool hasEngine   (void) ;

  protected:

    TreeWidgetMaps              items   ;
    QMap<QTreeWidgetItem *,int> logins  ;
    QScriptEngine             * engine  ;

    virtual void assure      (int login) ;
    virtual void toItem      (int login) ;

    virtual void add         (QTreeWidget * tree,TreeWidgetItems items) ;
    virtual void remove      (QTreeWidget * tree,TreeWidgetItems items) ;

  private:

    QString      toString    (double value,int digits) ;
    virtual bool CallScript  (QScriptValue & eval,User & user) ;

  public slots:

    virtual void Sync        (QTreeWidget * tree) ;
    virtual void SyncAll     (QTreeWidget * tree) ;
    virtual void SyncOnlines (QTreeWidget * tree) ;
    virtual void SyncGroup   (QTreeWidget * tree) ;
    virtual void SyncLogins  (QTreeWidget * tree) ;
    virtual void SyncNames   (QTreeWidget * tree) ;
    virtual void SyncScript  (QTreeWidget * tree) ;
    virtual void Render      (void) ;
    virtual void setEngine   (QScriptEngine * engine) ;

  protected slots:

  private slots:

    void SendChanged         (void) ;

  signals:

    void EmitChanged         (void) ;
    void Changed             (void) ;

} ;

class Q_FOREX_EXPORT OrderItems : public QObject
                                , public Orders
{
  Q_OBJECT
  public:

    BMAPs visible    ;
    BMAPs isJava     ;
    EMAPs jsFunction ;
    IMAPs PIPs       ;

    explicit OrderItems      (QObject * parent = NULL) ;
    virtual ~OrderItems      (void) ;

    #ifdef QT_QTMTAPI_LIB
    virtual Orders & assign  (int total,TradeRecord * trade) ;
    #endif

    virtual int  take        (int order) ;
    virtual void operate     (void) ;
    virtual bool hasEngine   (void) ;
    virtual QScriptEngine * Engine (void) ;
    virtual void runEngine   (QScriptValue & api,
                              QString symbol,
                              QDateTime CTM,
                              double bid,
                              double ask) ;

  protected:

    TreeWidgetMaps              items      ;
    QMap<QTreeWidgetItem *,int> orderItems ;
    QScriptEngine             * engine     ;

    virtual void assure      (int order) ;
    virtual void toItem      (int order) ;

    virtual void add         (QTreeWidget * tree,TreeWidgetItems items) ;
    virtual void remove      (QTreeWidget * tree,TreeWidgetItems items) ;

    virtual QString TradeString (int cmd) ;
    virtual QString Lots        (int volume) ;

  private:

    #ifdef QT_QTMTAPI_LIB
    virtual bool isCorrect   (TradeRecord & trade) ;
    #endif

    virtual bool CallScript  (QScriptValue & api    ,
                              int            order  ,
                              QString        symbol ,
                              QDateTime      CTM    ,
                              double         bid    ,
                              double         ask  ) ;
    virtual int  StopsLevel  (QString symbol) ;
    QString MoneyString      (double v) ;

  public slots:

    virtual void Sync        (QTreeWidget * tree) ;
    virtual void Cleanup     (void) ;
    virtual void Render      (void) ;
    virtual void setEngine   (QScriptEngine * e) ;

  protected slots:

  private slots:

    void SendChanged         (void) ;

  signals:

    void EmitChanged         (void) ;
    void Changed             (void) ;

} ;

class Q_FOREX_EXPORT BalanceItems : public QObject
                                  , public Orders
{
  Q_OBJECT
  public:

    BMAPs visible ;

    explicit BalanceItems    (QObject * parent = NULL) ;
    virtual ~BalanceItems    (void) ;

    #ifdef QT_QTMTAPI_LIB
    virtual Orders & assign  (int total,TradeRecord * trade) ;
    #endif

    virtual int  take        (int order) ;
    virtual void operate     (void) ;
    virtual bool hasEngine   (void) ;
    virtual QScriptEngine * Engine (void) ;
    virtual void runEngine   (QScriptValue & api,
                              QString symbol,
                              QDateTime CTM,
                              double bid,
                              double ask) ;

  protected:

    QMap<int,TableWidgetItems *> items        ;
    QMap<TableWidgetItems *,int> balanceItems ;
    TableWidgetItems          *  sum          ;
    QScriptEngine             *  engine       ;

    virtual void assure      (int order) ;
    virtual void toItem      (int order) ;

    virtual void add         (QTableWidget * table,QList<TableWidgetItems *> items) ;
    virtual void remove      (QTableWidget * table,QList<TableWidgetItems *> items) ;

  private:

    #ifdef QT_QTMTAPI_LIB
    virtual bool isCorrect   (TradeRecord & trade) ;
    #endif

    QString TradeString      (int cmd,double profit) ;

  public slots:

    virtual void Sync        (QTableWidget * table) ;
    virtual void Cleanup     (void) ;
    virtual void Render      (void) ;
    virtual void setEngine   (QScriptEngine * e) ;

  protected slots:

  private slots:

    void SendChanged         (void) ;

  signals:

    void EmitChanged         (void) ;
    void Changed             (void) ;

} ;

}

namespace Forex
{

class Q_FOREX_EXPORT AnalyizerPlugin
{
  public:

    explicit AnalyizerPlugin(void) ;
    virtual ~AnalyizerPlugin(void) ;

    virtual int        type         (void                ) const = 0 ;
    virtual QString    name         (void                ) const = 0 ;
    virtual bool       initialize   (Plan     * plan     ,
                                     Sql      & sql      ) = 0 ;
    virtual bool       multiple     (void                ) = 0 ;
    virtual int        requires     (void                ) = 0 ;
    virtual QWidget *  configure    (QWidget  * parent   ,
                                     QString    action   ,
                                     SUID       supplier ,
                                     QString    symbol   ,
                                     QString    level    ,
                                     QString    period   ,
                                     int        layout   ,
                                     QString  & error    ) = 0 ;
    virtual QByteArray data         (QWidget  * widget   ) = 0 ;
    virtual bool       acceptable   (QString    action   ,
                                     SUID       supplier ,
                                     QString    symbol   ,
                                     QString    level    ,
                                     QString    period   ,
                                     QByteArray config   ) = 0 ;
    virtual bool       go           (bool     & running  ,
                                     QWidget  * Ui       ,
                                     QByteArray config   ) = 0 ;

} ;

class Q_FOREX_EXPORT ChartPlugin
{
  public:

    explicit ChartPlugin(void) ;
    virtual ~ChartPlugin(void) ;

    virtual int           type       (void) const = 0 ;
    virtual QString       name       (void) const = 0 ;
    virtual QIcon         icon       (void) const = 0 ;
    virtual int           style      (void) const = 0 ;
    virtual bool          initialize (Plan         & plan    ,
                                      QByteArray   & config  ) = 0 ;
    virtual QWidget     * configure  (QWidget      * parent  ,
                                      QByteArray   & config  ,
                                      int            layout  ,
                                      QString      & error   ) = 0 ;
    virtual QByteArray    data       (QWidget      * widget  ) = 0 ;
    virtual SceneObject * display    (QWidget      * parent  ,
                                      QByteArray   & config  ) = 0 ;
    virtual bool          acceptable (SceneObject  * scene   ,
                                      QByteArray   & config  ) = 0 ;
    virtual bool          startup    (SceneObject  * scene   ,
                                      QByteArray   & config  ) = 0 ;
    virtual bool          prepare    (QWidget      * widget  ,
                                      Plan         * plan    ,
                                      SceneObject  * scene   ,
                                      QByteArray   & config  ) = 0 ;
    virtual bool          go         (bool         & running ,
                                      Plan         * plan    ,
                                      SceneObject  * scene   ,
                                      QByteArray   & config  ) = 0 ;

} ;

class Q_FOREX_EXPORT ConnectorGui : public Connector
{
  public:

    explicit          ConnectorGui (void) ;
    virtual          ~ConnectorGui (void) ;

    virtual QWidget * Configurator (QObject * parent,Plan * plan,Dealer * dealer) = 0 ;

  protected:

  private:

} ;

class Q_FOREX_EXPORT BridgesManager : public QObject
{
  Q_OBJECT
  public:

    QList < Forex::Dealer                         *> ForexDealers     ;
    QList < Forex::ConnectorGui                   *> ForexConnectors  ;
    QMap  < Forex::ConnectorGui * , QPluginLoader *> ConnectorLoaders ;
    QMap  < Forex::Dealer       * , MtChannels    *> Channels         ;
    QMap  < Forex::Dealer       * , SymbolsQuotes *> Quotes           ;
    QMap  < SymbolsQuotes       * , Forex::Dealer *> QuotesDealer     ;
    QMap  < Forex::Dealer       * , ForexTraders  *> Traders          ;
    QMap  < ForexTraders        * , Forex::Dealer *> TradersDealer    ;
    Mutexz                                           mutex            ;

    explicit                      BridgesManager (QObject * parent = NULL) ;
    virtual                      ~BridgesManager (void) ;

    virtual Forex::ConnectorGui * Connector      (int type) ;
    virtual Forex::Dealer       * Dealer         (QString identifier) ;
    virtual int                   add            (Forex::Dealer * dealer) ;
    virtual int                   remove         (Forex::Dealer * dealer) ;

    virtual bool                  Load           (QDir & dir) ;
    virtual bool                  UpdateSettings (QDir   dir) ;

    virtual bool                  StartAll       (Plan * plan) ;
    virtual bool                  CloseAll       (void) ;
    virtual bool                  Unload         (void) ;

  protected:

  private:

  public slots:

    virtual void                  Stop           (void) ;

  protected slots:

  private slots:

  signals:

} ;

}

class Q_FOREX_EXPORT CurrencyRelations : public TreeDock
{
  Q_OBJECT
  public:

    explicit CurrencyRelations   (StandardConstructor) ;
    virtual ~CurrencyRelations   (void) ;

  protected:

    Currencies currencies ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

    virtual void run             (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void List            (void) ;
    virtual void Insert          (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void currencyChanged (int index) ;
    virtual void nameChanged     (void) ;
    virtual void Finalize        (QTreeWidgetItem * item) ;

  private slots:

    virtual void listItems       (void) ;

  signals:

    void loadFinished            (void) ;
    void Edit                    (QString name,SUID uuid) ;
    void Translations            (QString title,UUIDs & Uuids) ;

} ;

class Q_FOREX_EXPORT TickTreeItems : public QObject
                                   , public TickTransmitter
{
  Q_OBJECT
  public:

    explicit     TickTreeItems (QObject * parent = NULL) ;
    virtual     ~TickTreeItems (void) ;

    virtual void Append        (QString symbol) ;
    virtual void setDigits     (QString symbol,int digits) ;
    virtual void Hide          (QString symbol) ;
    virtual void Quote         (QString symbol,QDateTime CTM,double bid,double ask) ;
    virtual void Update        (void) ;
    virtual void Ready         (void) ;

  protected:

    QStringList                     Symbols     ;
    UMAPs                           Digits      ;
    UMAPs                           Multiply    ;
    QMap<QString,QDateTime        > CTMs        ;
    QMap<QString,QDateTime        > LOCALs      ;
    QMap<QString,double           > BIDs        ;
    QMap<QString,double           > ASKs        ;
    QMap<QString,Finance::Ticks   > TICKs       ;
    TreeWidgetItems                 Items       ;
    QMap<QString,QTreeWidgetItem *> SymbolItems ;
    LMAPs                           Visiblity   ;
    QMutex                          mutexz      ;

  private:

    QString toDigits           (QString symbol,double value) ;

  public slots:

    virtual void Sync          (QTreeWidget * tree) ;
    virtual void Full          (QTreeWidget * tree) ;
    virtual void BySymbols     (QTreeWidget * tree,QStringList list) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_FOREX_EXPORT TraderCapital : public Widget
{
  Q_OBJECT
  public:

    enum           {
      NOTHING  = 0 ,
      DEPOSIT  = 1 ,
      WITHDRAW = 2 ,
      DONE     = 4
    }              ;

    WMAPs  Variables ;
    void * data      ;

    explicit     TraderCapital (StandardConstructor) ;
    virtual     ~TraderCapital (void);

  protected:

    Ui::TraderCapital * ui   ;
    int                 mode ;

  private:

  public slots:

    virtual void setMode       (int mode) ;
    virtual void setLogin      (int login,QString name) ;

  protected slots:

    virtual void deposit       (void) ;
    virtual void withdraw      (void) ;
    virtual void done          (void) ;

  private slots:

  signals:

    void Deposit               (QWidget * widget,int login,double Amount,QString comment,void * data) ;
    void Withdraw              (QWidget * widget,int login,double Amount,QString comment,void * data) ;
    void Done                  (QWidget * widget) ;

} ;

class Q_FOREX_EXPORT ForexBridgeConfigurator : public Widget
{
  Q_OBJECT
  public:

    Forex::BridgesManager * Bridges ;

    explicit     ForexBridgeConfigurator (StandardConstructor) ;
    virtual     ~ForexBridgeConfigurator (void) ;

  protected:

    Ui::nForexBridgeConfigurator * ui        ;
    Forex::ConnectorGui          * connector ;
    Forex::Dealer                * dealer    ;
    QWidget                      * conf      ;
    QDir                           Dir       ;

    virtual void resizeEvent             (QResizeEvent * event) ;
    virtual void Configure               (void) ;

  private:

  public slots:

    virtual void List                    (void) ;
    virtual void setDirectory            (QDir dir) ;

    virtual void Add                     (void) ;
    virtual void Configurator            (void) ;
    virtual void Cancel                  (void) ;

    virtual void Relocation              (void) ;

  protected slots:

    virtual void TextChanged             (QString name) ;
    virtual void ModuleChanged           (int index) ;

  private slots:

  signals:

    void         Update                  (void) ;

} ;

class Q_FOREX_EXPORT SymbolsQuotes : public TreeDock
{
  Q_OBJECT
  public:

    WMAPs Variables ;

    explicit SymbolsQuotes        (StandardConstructor) ;
    virtual ~SymbolsQuotes        (void) ;

    TickTreeItems * TickItems     (void) ;

  protected:

    int             TimeId   ;
    TickTreeItems * TickTree ;

    virtual void timerEvent       (QTimerEvent * event) ;

    virtual bool FocusIn          (void) ;
    virtual bool FocusOut         (void) ;

    virtual void Configure        (void) ;

    virtual void run              (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool Shutdown         (void) ;

    virtual void Insert           (void) ;
    virtual void Delete           (void) ;
    virtual void Refresh          (void) ;

    virtual void GoUp             (void) ;
    virtual void GoDown           (void) ;

    virtual void Insert           (QTreeWidgetItem * item) ;
    virtual void Delete           (QTreeWidgetItem * item) ;
    virtual void ChartWindow      (QTreeWidgetItem * item) ;
    virtual void TickChart        (QTreeWidgetItem * item) ;
    virtual void DepthOfMarket    (QTreeWidgetItem * item) ;

    virtual void AllSymbols       (void) ;
    virtual void LoadSymbols      (void) ;
    virtual void SaveSymbols      (void) ;
    virtual void LoadStyleSheet   (void) ;
    virtual void SaveStyleSheet   (void) ;

  protected slots:

    virtual bool Menu             (QPoint pos) ;

    virtual void singleClicked    (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked    (QTreeWidgetItem * item,int column) ;

    virtual void KeepPath         (QString path) ;
    QString      GetPath          (void) ;

  private slots:

    void         Painting         (void) ;

  signals:

    void NewOrder                 (QString identifier,QTreeWidgetItem * item) ;
    void Remove                   (QString identifier,QTreeWidgetItem * item) ;
    void Chart                    (QString identifier,QTreeWidgetItem * item) ;
    void Tick                     (QString identifier,QTreeWidgetItem * item) ;
    void Market                   (QString identifier,QTreeWidgetItem * item) ;
    void Sell                     (QString identifier,QTreeWidgetItem * item) ;
    void Buy                      (QString identifier,QTreeWidgetItem * item) ;
    void titleChanged             (QWidget * widget) ;

} ;

class Q_FOREX_EXPORT ForexTraders : public TreeDock
{
  Q_OBJECT
  public:

    QString Identifier ;

    explicit ForexTraders      (StandardConstructor) ;
    virtual ~ForexTraders      (void) ;

    Finance::UserItems * GetItems (void) ;

  protected:

    NAMEs                names  ;
    LineEdit           * search ;
    Finance::UserItems * items  ;
    int                  TimeId ;

    virtual void timerEvent      (QTimerEvent * e) ;

    virtual bool FocusIn         (void) ;
    virtual bool FocusOut        (void) ;

    virtual void Configure       (void) ;

    virtual void run             (int type, ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual bool Shutdown        (void) ;

    virtual void Delete          (void) ;
    virtual void Search          (void) ;

    virtual void LoadStyleSheet  (void) ;
    virtual void SaveStyleSheet  (void) ;

    virtual void AssignMode      (QString mode) ;
    virtual bool AssignEngine    (void) ;
    virtual bool AssignEngine    (QString filename) ;

    virtual void setUserItem     (Finance::UserItems * items) ;

    virtual bool AssignLogins    (void) ;
    virtual bool AssignNames     (void) ;

    virtual void SaveLogins      (void) ;
    virtual void SaveNames       (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    virtual void Hide            (QTreeWidgetItem * item) ;
    virtual void Details         (QTreeWidgetItem * item) ;
    virtual void Join            (QTreeWidgetItem * item) ;
    virtual void History         (QTreeWidgetItem * item) ;
    virtual void Balance         (QTreeWidgetItem * item) ;
    virtual void Credit          (QTreeWidgetItem * item) ;
    virtual void Deposit         (QTreeWidgetItem * item) ;
    virtual void Withdraw        (QTreeWidgetItem * item) ;

    virtual void ItemsChanged    (void) ;

    virtual void KeepPath        (QString path) ;
    QString      GetPath         (void) ;

    virtual void keywordPressed  (void) ;

  private slots:

  signals:

    void Associate               (ForexTraders * traders) ;
    void GetOrders               (QString identifier,int login,QString group) ;
    void Join                    (QString identifier,int login,QString name,QString group) ;
    void History                 (QString identifier,int login,QString name,QString group) ;
    void Balance                 (QString identifier,int login,QString name,QString group) ;
    void Credit                  (QString identifier,int login,QString name,QString group) ;
    void Details                 (QString identifier,int login,QString name,QString group) ;
    void Deposit                 (QString identifier,int login,QString name,QString group) ;
    void Withdraw                (QString identifier,int login,QString name,QString group) ;
    void New                     (QString identifier) ;

} ;

class Q_FOREX_EXPORT ForexOrders : public TreeDock
{
  Q_OBJECT
  public:

    QString Identifier ;

    explicit ForexOrders           (StandardConstructor) ;
    virtual ~ForexOrders           (void) ;

    Finance::OrderItems * GetItems (void) ;

  protected:

    NAMEs                 names  ;
    Finance::OrderItems * items  ;
    int                   TimeId ;

    virtual void timerEvent        (QTimerEvent * e) ;

    virtual bool FocusIn           (void) ;
    virtual bool FocusOut          (void) ;

    virtual void Configure         (void) ;

    virtual void run               (int type, ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual bool Shutdown          (void) ;

    virtual void LoadStyleSheet    (void) ;
    virtual void SaveStyleSheet    (void) ;

    virtual bool AssignEngine      (void) ;
    virtual bool AssignEngine      (QString filename) ;

    virtual void setOrderItem      (Finance::OrderItems * items) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    virtual void ItemsChanged      (void) ;
    virtual void profitsChanged    (void) ;
    virtual void commentChanged    (void) ;

    virtual void KeepPath          (QString path) ;
    QString      GetPath           (void) ;

  private slots:

  signals:

    void NewOrder                  (QString identifier,int login) ;
    void CloseAllOrders            (QString identifier,int login) ;
    void CloseOrder                (QString identifier,int login,int order) ;
    void ModifyOrder               (QString identifier,int login,int order) ;
    void DisplayOrder              (QString identifier,int login,int order) ;
    void ModifyProfits             (QString identifier,int login,int order,double sl,double tp) ;
    void ModifyComment             (QString identifier,int login,int order,QString comment) ;

} ;

class Q_FOREX_EXPORT ForexBalance : public TableWidget
{
  Q_OBJECT
  public:

    WMAPs Variables ;

    explicit      ForexBalance       (StandardConstructor) ;
    virtual      ~ForexBalance       (void) ;

    virtual QSize sizeHint           (void) const ;

    Finance::BalanceItems * GetItems (void) ;

  protected:

    Finance::BalanceItems * items  ;
    int                     TimeId ;

    virtual void timerEvent          (QTimerEvent * e) ;

    virtual bool FocusIn             (void) ;
    virtual bool FocusOut            (void) ;

    virtual void Configure           (void) ;

    virtual void run                 (int type, ThreadData * data) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual bool Shutdown            (void) ;

    virtual void Export              (void) ;

    virtual void LoadStyleSheet      (void) ;
    virtual void SaveStyleSheet      (void) ;

    virtual bool AssignEngine        (void) ;
    virtual bool AssignEngine        (QString filename) ;

    virtual void setBalanceItem      (Finance::BalanceItems * items) ;

  protected slots:

    virtual bool Menu                (QPoint pos) ;

    virtual void ItemsChanged        (void) ;

    virtual void KeepPath            (QString path) ;
    QString      GetPath             (void) ;

  private slots:

  signals:

    void Deposit                     (QString identifier,int login,QString name,QString group) ;
    void Withdraw                    (QString identifier,int login,QString name,QString group) ;

} ;

class Q_FOREX_EXPORT QuotesImporter : public TreeDock
{
  Q_OBJECT
  public:

    explicit QuotesImporter      (StandardConstructor) ;
    virtual ~QuotesImporter      (void) ;

  protected:

    QString member  ;
    QString key     ;
    NAMEs   periods ;
    NAMEs   pnames  ;
    int     blocks  ;

    virtual SUID MemberUuid      (void) ;

    virtual void closeEvent      (QCloseEvent * event) ;

    virtual bool FocusIn         (void) ;
    virtual bool FocusOut        (void) ;

    virtual void Configure       (void) ;

    virtual void run             (int type, ThreadData * data) ;

  private:

    void         addProviders    (ComboBox * combo) ;
    void         addCurrencies   (ComboBox * combo,SUID provider,int level) ;
    void         addPeriods      (ComboBox * combo) ;
    void         addLevels       (ComboBox * combo) ;
    void         CheckIt         (QTreeWidgetItem * item) ;
    bool         isReady         (QTreeWidgetItem * item) ;

  public slots:

    virtual void Insert          (void) ;
    virtual void Delete          (void) ;
    virtual void Import          (void) ;
    virtual void Importion       (VarArgs & args) ;
    virtual void StartAll        (void) ;
    virtual void Fullsets        (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void addFile         (QTreeWidgetItem * item,int column) ;

    virtual void traderChanged   (int provider) ;
    virtual void currencyChanged (int currency) ;
    virtual void periodChanged   (int period  ) ;
    virtual void levelChanged    (int level   ) ;

  private slots:

  signals:

} ;

class Q_FOREX_EXPORT ForexCommodity : public TreeDock
                                    , public Ownership
{
  Q_OBJECT
  public:

    explicit ForexCommodity      (StandardConstructor) ;
    virtual ~ForexCommodity      (void) ;

  protected:

    Currencies currencies ;
    QString    level      ;

    virtual bool FocusIn         (void) ;
    virtual bool FocusOut        (void) ;

    virtual void Configure       (void) ;

    virtual void run             (int type, ThreadData * data) ;

    virtual void UpdateItem      (QTreeWidgetItem * item) ;

  private:

    void         addCurrencies   (ComboBox * combo) ;
    void         addConnections  (ComboBox * combo) ;
    void         addDigits       (ComboBox * combo) ;
    void         addFull         (ComboBox * combo) ;
    void         addTimeZones    (ComboBox * combo) ;
    void         addLevels       (ComboBox * combo) ;
    void         addYears        (ComboBox * combo) ;

  public slots:

    virtual bool startup         (void) ;

    virtual void List            (void) ;
    virtual void Insert          (void) ;
    virtual void Delete          (void) ;
    virtual void Paste           (void) ;

    virtual void setLevel        (QString level) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    virtual void nameFinished    (void) ;
    virtual void currencyChanged (int currency) ;
    virtual void connectChanged  (int Connect ) ;
    virtual void digitChanged    (int digit   ) ;
    virtual void fullChanged     (int digit   ) ;
    virtual void timezoneChanged (int tz      ) ;
    virtual void levelChanged    (int level   ) ;
    virtual void fromChanged     (int from    ) ;
    virtual void toChanged       (int to      ) ;

  private slots:

  signals:

} ;

class Q_FOREX_EXPORT SelectChart : public Widget
{
  Q_OBJECT
  public:

    explicit        SelectChart  (StandardConstructor) ;
    virtual        ~SelectChart  (void) ;

    virtual QString ChartName    (void) ;
    virtual QString Supplier     (void) ;
    virtual QString Symbol       (void) ;
    virtual int     Level        (void) ;
    virtual int     Period       (void) ;
    virtual bool    isReversal   (void) ;

  protected:

    Ui::SelectChart * ui          ;
    EMAPs             PeriodNames ;

    virtual void    resizeEvent  (QResizeEvent * event) ;
    virtual void    showEvent    (QShowEvent   * event) ;

    virtual void    Configure    (void) ;

    virtual void    run          (int Type,ThreadData * data) ;
    virtual bool    ItemRunner   (int Type,ThreadData * data) ;
    virtual bool    Load         (ThreadData * data) ;
    virtual bool    LoadSymbols  (ThreadData * data) ;

  private:

  public slots:

    virtual bool    Relocation   (void) ;
    virtual bool    startup      (void) ;

    virtual void    Add          (void) ;
    virtual void    Cancel       (void) ;

  protected slots:

    virtual void    Changed      (int id) ;
    virtual void    StateChanged (int state) ;
    virtual void    NameChanged  (QString name) ;

  private slots:

    void            SymbolCombo  (bool enabled) ;

  signals:

    void            AddChart     (QWidget * object) ;
    void            CancelChart  (QWidget * object) ;
    void            Leave        (QWidget * object) ;
    void            EnableSymbol (bool enabled) ;

} ;

class Q_FOREX_EXPORT PriceEditor : public TableWidget
{
  Q_OBJECT
  public:

    explicit        PriceEditor    (StandardConstructor) ;
    virtual        ~PriceEditor    (void) ;

  protected:

    Monetary         monetary     ;
    UMAPs            PeriodNames  ;
    YMAPs            PeriodLimits ;
    QStringList      TicksLabels  ;
    QStringList      PeriodLabels ;
    QString          supplier     ;
    QString          symbol       ;
    QString          original     ;
    QString          base         ;
    int              level        ;
    int              period       ;
    int              digits       ;
    bool             reversal     ;
    bool             editing      ;
    bool             utc          ;
    qint64           multiply     ;
    Range<QDateTime> range        ;

    virtual bool    FocusIn        (void) ;

    virtual void    Configure      (void) ;
    virtual void    InstallMsgs    (void) ;
    virtual void    InstallIcons   (void) ;

    virtual void    run            (int T,ThreadData * data) ;
    virtual bool    ItemRunner     (int T,ThreadData * data) ;

    virtual bool    UpdateMonetary (void) ;

    virtual void    LoadTicks      (ThreadData * data) ;
    virtual void    LoadPeriod     (ThreadData * data) ;
    virtual void    Load           (ThreadData * data) ;
    virtual void    UpdateTick     (ThreadData * data) ;
    virtual void    UpdatePeriod   (ThreadData * data) ;
    virtual void    Update         (ThreadData * data) ;
    virtual void    GetMonetary    (ThreadData * data) ;

    virtual void    TickValues     (SqlConnection & SC,SqlConnection & XC,int row) ;
    virtual void    PeriodValues   (SqlConnection & SC,SqlConnection & XC,int row) ;

    virtual void    setCellValue   (int row,int column,int  value,bool editable) ;
    virtual void    setCellValue   (int row,int column,TUID value,bool editable) ;
    virtual void    setCellTime    (int row,int column,TUID value,bool editable) ;

    virtual QString fromTick       (int row) ;
    virtual QString fromPeriod     (int row) ;

  private:

    QString         VTS            (int value) ;

  public slots:

    virtual bool    startup        (void) ;
    virtual bool    startup        (VarArgs args) ;

    virtual void    Refresh        (void) ;
    virtual void    PageUp         (void) ;
    virtual void    PageDown       (void) ;

    virtual void    setSupplier    (QString supplier,int level) ;
    virtual void    setPeriod      (QString period) ;
    virtual void    setPeriod      (int period) ;
    virtual void    setReversal    (bool reversal) ;
    virtual void    setRange       (QDateTime from,QDateTime to) ;
    virtual void    setSymbol      (QString symbol) ;

    virtual void    Export         (void) ;
    virtual bool    ExportTicks    (QString filename) ;
    virtual bool    ExportPeriod   (QString filename) ;

  protected slots:

    virtual bool    Menu           (QPoint pos) ;

    virtual void    ShowTicks      (void) ;
    virtual void    ShowPeriod     (void) ;

    virtual void    dataChanged    (int row, int column) ;

  private slots:

  signals:

    void            LabelTicks     (void) ;
    void            LabelPeriod    (void) ;

} ;

}

Q_DECLARE_INTERFACE(N::Forex::AnalyizerPlugin , "com.neutrino.forex.analyizer"      )
Q_DECLARE_INTERFACE(N::Forex::ChartPlugin     , "com.neutrino.forex.chart"          )
Q_DECLARE_INTERFACE(N::Forex::ConnectorGui    , "com.neutrino.forex.connectors.gui" )

QT_END_NAMESPACE

#endif
