/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief CashTransaction class interface.
 */

#ifndef DC_TXNLIST_CASHTRANSACTION_H
#define DC_TXNLIST_CASHTRANSACTION_H

#include <txnlist/transaction.h>

namespace TxnList {

    // forward declarations
    class TxnManager;

    /**
     * @brief Represents a transaction where cash is the payment method.
     */
    class CashTransaction : public Transaction
    {
        public:
            CashTransaction(TxnManager &txnManager, 
                    time_t timestamp) :
                Transaction(txnManager, timestamp, "cash") {}
    };
}

#endif
