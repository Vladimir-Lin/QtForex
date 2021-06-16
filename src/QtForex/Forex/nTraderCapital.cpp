#include <qtforex.h>
#include "ui_TraderCapital.h"

N::TraderCapital:: TraderCapital ( QWidget * parent , Plan * p )
                 : Widget        (           parent ,        p )
                 , ui            ( new Ui::TraderCapital       )
                 , data          ( NULL                        )
                 , mode          ( NOTHING                     )
{
  ui -> setupUi ( this ) ;
}

N::TraderCapital::~TraderCapital(void)
{
  delete ui ;
}

void N::TraderCapital::setMode(int m)
{
  bool d = ( 0 != ( m & DEPOSIT  ) )  ;
  bool w = ( 0 != ( m & WITHDRAW ) )  ;
  bool f = ( 0 != ( m & DONE     ) )  ;
  mode = m                            ;
  ui  -> Deposit  -> setVisible ( d ) ;
  ui  -> Withdraw -> setVisible ( w ) ;
  ui  -> Done     -> setVisible ( f ) ;
}

void N::TraderCapital::setLogin(int login,QString name)
{
  ui -> Account -> setText ( QString::number ( login ) ) ;
  ui -> Name    -> setText ( name                      ) ;
}

void N::TraderCapital::deposit(void)
{
  int     login   = ui -> Account -> text ( ) . toInt    ( ) ;
  double  amount  = ui -> Amount  -> text ( ) . toDouble ( ) ;
  QString comment = ui -> Comment -> text ( )                ;
  if ( amount <= 0 ) return                                  ;
  emit Deposit  ( this , login , amount , comment , data )   ;
}

void N::TraderCapital::withdraw(void)
{
  int     login   = ui -> Account -> text ( ) . toInt    ( ) ;
  double  amount  = ui -> Amount  -> text ( ) . toDouble ( ) ;
  QString comment = ui -> Comment -> text ( )                ;
  if ( amount <= 0 ) return                                  ;
  emit Withdraw ( this , login , amount , comment , data )   ;
}

void N::TraderCapital::done(void)
{
  emit Done ( this ) ;
}
