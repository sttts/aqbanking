/* This is a generated file. Please do not edit. */
#ifndef TRANSACTION_P_H
#define TRANSACTION_P_H

#include "transaction_l.h"

struct AB_TRANSACTION {
  GWEN_INHERIT_ELEMENT(AB_TRANSACTION)
  GWEN_LIST_ELEMENT(AB_TRANSACTION)
  int _usage;
  int _modified;

  int localCountryCode;
  char *localBankCode;
  char *localAccountNumber;
  char *localSuffix;
  char *localName;
  int remoteCountryCode;
  char *remoteBankCode;
  char *remoteAccountNumber;
  char *remoteSuffix;
  GWEN_STRINGLIST *remoteName;
  char *uniqueId;
  GWEN_TIME *valutaDate;
  GWEN_TIME *date;
  AB_VALUE *value;
  int textKey;
  char *transactionKey;
  char *customerReference;
  char *bankReference;
  int transactionCode;
  char *transactionText;
  char *primanota;
  GWEN_STRINGLIST *purpose;
};


/** @name Local Account Info
 *
This group contains information about the local account.
*/
/*@{*/






/*@}*/

/** @name Remote Account Info
 *
This group contains information about the remote account.
*/
/*@{*/






/*@}*/


/** @name Dates
*/
/*@{*/


/*@}*/

/** @name Value
*/
/*@{*/

/*@}*/

/** @name Info Which Is Not Supported by All Backends
 *
<p>
This group contains information which differ between backends.
</p>
<p>
Some of this information might not even be
<b>
supported
</b>
by every backends.
</p>
*/
/*@{*/









/*@}*/


#endif /* TRANSACTION_P_H */
