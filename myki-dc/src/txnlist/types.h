/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains declarations of custom types within the TxnList sub-module.
 */

#ifndef DC_TXNTYPES_H
#define DC_TXNTYPES_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <iostream>

namespace TxnList {

    // forward declarations
    class Transaction;
    class CartItem;

    // custom data types
    typedef int TxnSequenceNumber;
    typedef int CartItemNumber;
    typedef int Cents;
    typedef int ProductId;
    typedef int Seconds;

    // custom convenience types
    typedef boost::shared_ptr<CartItem> CartItemPtr;
    typedef std::map<CartItemNumber, CartItemPtr> CartItemMap;
    typedef boost::shared_ptr<Transaction> TransactionPtr;
    typedef std::map<TxnSequenceNumber, TransactionPtr> TxnMap;
}

#endif
