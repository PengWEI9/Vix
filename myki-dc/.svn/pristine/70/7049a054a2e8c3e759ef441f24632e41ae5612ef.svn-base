/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Transaction class implementation.
 */

#ifndef DC_TXNLIST_TRANSACTION_H
#define DC_TXNLIST_TRANSACTION_H

#include <txnlist/types.h>
#include <json/json.h>

namespace TxnList {

    // forward declarations
    class TxnManager;

    /**
     * @brief Represents a customer sales transaction. 
     * 
     * A transaction contains a list of cart items that were purchased.
     */
    class Transaction
    {
        public:
            Transaction(TxnManager &txnManager, time_t timestamp,
                    const std::string &paymentType) :
                m_txnManager(txnManager),
                m_timestamp(timestamp),
                m_paymentType(paymentType) {}

            /**
             * @return transaction timestamp.
             */
            time_t getTimestamp() const 
            {
                return m_timestamp; 
            }

            /**
             * @return Payment type, eg 'cash' or 'tpurse'.
             */
            const std::string &getPaymentType() const 
            { 
                return m_paymentType; 
            }

            /**
             * @return The number of cart items.
             */
            size_t size() const { return m_cartItems.size(); }

            /**
             * @brief Retrieves the cart item with the given cart item number.
             * @param itemNumber The cart item number to search for.
             * @return The cart item, or NULL if none found.
             */
            virtual const CartItemPtr getCartItem(CartItemNumber itemNumber) const
            {
                CartItemMap::const_iterator iter = m_cartItems.find(itemNumber);

                if (iter == m_cartItems.end()) return CartItemPtr();

                return iter->second;
            }

            /**
             * @brief Reverses the cart item associated with the given item 
             * number.
             *
             * If all items have been reversed for this transaction, it is then
             * removed from the map in the TxnManager.
             *
             * @param itemNumber The cart item number to search for.
             * @throw runtime_error If no cart item could be found.
             */
            virtual void reverseCartItem(CartItemNumber itemNumber);

            /**
             * @brief Adds a cart item.
             * @param itemNumber The item number of the given cart item.
             * @param cartItemPtr The cart item to add.
             * @throw runtime_errof If cart item already exists.
             */
            virtual void addCartItem(CartItemNumber itemNumber, 
                    const CartItemPtr &cartItemPtr);

            /**
             * @return True if all cart items are reversed, otherwise false.
             */
            virtual bool areAllItemsReversed() const;

            /**
             * @return The transaction, converted to a JSON object.
             */
            virtual Json::Value toJson() const;

            virtual ~Transaction() {}

        private:
            TxnManager &m_txnManager;
            /*! When the transaction occurred */
            time_t m_timestamp;
            /*! Payment type (eg 'cash') */
            std::string m_paymentType;
            CartItemMap m_cartItems;
    };
}

#endif
