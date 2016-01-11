/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains interface of TopupCartItem class.
 */

#ifndef DC_TXNLIST_TOPUPCARTITEM_H
#define DC_TXNLIST_TOPUPCARTITEM_H

#include "cartitem.h"

namespace TxnList {

    /**
     * @brief A Topup transaction.
     */
    class TopupCartItem : public CartItem {

        private:
            std::string m_txnType;
            /*! Card transaction sequence number */
            TxnSequenceNumber m_cardSequenceNumber;
            /*! Card serial number */
            std::string m_cardSerialNumber;

        public:
            TopupCartItem(const std::string &txnType,
                    Cents price,
                    ProductId productId,
                    const std::string &shortDescription,
                    bool gstApplicable,
                    TxnSequenceNumber cardSequenceNumber,
                    const std::string &cardSerialNumber,
                    bool reversed = false) :
                CartItem(txnType, price, productId, shortDescription, 
                        gstApplicable, reversed),
                m_cardSequenceNumber(cardSequenceNumber),
                m_cardSerialNumber(cardSerialNumber) {}

            /**
             * @return The transaction number written to the card for this
             * transaction.
             */
            TxnSequenceNumber getCardTxnSequenceNumber() const
            {
                return m_cardSequenceNumber;
            }

            /**
             * @return Transaction card serial number.
             */
            const std::string &getCardSerialNumber() const 
            { 
                return m_cardSerialNumber;
            }

            /**
             * @see TxnList::Txn::toJson()
             */
            Json::Value toJson() const;
    };
}


#endif
