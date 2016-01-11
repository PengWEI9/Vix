/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains the interface for the CartItem class.
 */

#ifndef DC_TXNLIST_ITEM_H
#define DC_TXNLIST_ITEM_H

#include <txnlist/types.h>
#include <json/json.h>
#include <string>

namespace TxnList {

    /**
     * @brief Represents a cart item within a transaction.
     */
    class CartItem {

        private:
            /*! Transaction type, eg 'tpurseload' */
            std::string m_txnType;
            Cents m_price;
            /*! Also known as 'ticket no' for surcharge products */
            ProductId m_productId;
            /*! eg "GTS Timetable" */
            std::string m_shortDescription;
            /*! Whether or not GST has been applied to the price. */
            bool m_gstApplicable;

            bool m_reversed;

        public:
            CartItem(const std::string &txnType, 
                    Cents price,
                    ProductId productId,
                    const std::string &shortDescription,
                    bool gstApplicable,
                    bool reversed = false) :
                m_txnType(txnType),
                m_price(price),
                m_productId(productId),
                m_shortDescription(shortDescription),
                m_gstApplicable(gstApplicable),
                m_reversed(reversed) {}

            /**
             * @return Transaction type (eg 'tpurseload').
             */
            const std::string &getType() const { return m_txnType; }

            /**
             * @return Cart item price in cents.
             */
            const Cents &getPrice() const { return m_price; }

            /**
             * @return Product ID.
             */
            ProductId getProductId() const
            {
                return m_productId;
            }

            /**
             * @return Product short description.
             */
            const std::string &getShortDescription() const
            {
                return m_shortDescription;
            }

            /**
             * @return True if GST is applicable, otherwise false.
             */
            bool isGstApplicable() const
            {
                return m_gstApplicable;
            }

            /**
             * @return True if the cart item has been reversed, otherwise false.
             */
            bool isReversed() const { return m_reversed; }

            /**
             * @brief Sets the cart item to reverse (or not).
             * @param True to set to reversed, otherwise false.
             */
            void setReversed(bool reversed) { m_reversed = reversed; }

            /**
             * @return The transaction as a JSON object.
             */
            virtual Json::Value toJson() const;

            virtual ~CartItem() {};
    };
}

#endif
