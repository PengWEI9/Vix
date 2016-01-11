/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief TPurseTransaction class interface.
 */

#ifndef DC_TXNLIST_TPURSETRANSACTION_H
#define DC_TXNLIST_TPURSETRANSACTION_H

#include <txnlist/transaction.h>

namespace TxnList {

    // forward declarations
    class TxnManager;

    /**
     * @brief Represents a transaction where tpurse is the payment method.
     */
    class TPurseTransaction : public Transaction
    {
        private:
            /*! The number of the card used for payment. */
            std::string m_cardNumber;
            /*! The sequence number for this transaction, as stored on the card 
                used for payment. */
            TxnSequenceNumber m_cardTxnSequenceNumber;

        public:
            TPurseTransaction(TxnManager &txnManager, 
                    time_t timestamp,
                    const std::string &cardNumber,
                    TxnSequenceNumber cardTxnSequenceNumber) :
                Transaction(txnManager, timestamp, "tpurse"),
                m_cardNumber(cardNumber),
                m_cardTxnSequenceNumber(cardTxnSequenceNumber) {}

            /**
             * @return The number of the card used for payment.
             */
            const std::string &getCardNumber() const 
            {
                return m_cardNumber; 
            }

            /**
             * @return The card transaction sequence number.
             */
            TxnSequenceNumber getCardTxnSequenceNumber() const
            {
                return m_cardTxnSequenceNumber;
            }

            Json::Value toJson() const;
    };

}

#endif
