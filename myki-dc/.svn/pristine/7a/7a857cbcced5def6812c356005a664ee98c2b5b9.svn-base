#include "transaction.h"
#include "txnmanager.h"
#include "cartitem.h"
#include "helpers.h"
#include <csf.h>
#include <boost/lexical_cast.hpp>

using namespace TxnList;
using namespace std;
using boost::lexical_cast;

void Transaction::addCartItem(CartItemNumber itemNumber, 
        const CartItemPtr &cartItemPtr)
{
    CsDbg(4, "Transaction::addCartItem: %d", itemNumber);

    if (!m_cartItems.insert(make_pair(itemNumber, cartItemPtr)).second)
    {
        throw runtime_error("Cart item " + lexical_cast<string>(itemNumber) 
                + " exists");
    }
}

Json::Value Transaction::toJson() const
{
    Json::Value json;

    json["saletimestamp"] = static_cast<int>(getTimestamp());
    json["paymenttype"] = getPaymentType();

    for (CartItemMap::const_iterator iter = m_cartItems.begin(); 
            iter != m_cartItems.end(); ++iter)
    {
        Json::Value tmp = iter->second->toJson();
        tmp["itemnumber"] = iter->first;
        json["cartitems"].append(tmp);
    }

    return json;
}

bool Transaction::areAllItemsReversed() const
{
    for (CartItemMap::const_iterator iter = m_cartItems.begin(); 
            iter != m_cartItems.end(); ++iter)
    {
        if (!iter->second->isReversed()) return false;
    }
    
    return true;
}

void Transaction::reverseCartItem(CartItemNumber itemNumber)
{
    CsDbg(4, "Transaction::reverseCartItem: %d", itemNumber);
    CartItemPtr cartItemPtr = getCartItem(itemNumber);

    if (cartItemPtr == NULL)
    {
        throw runtime_error("Cart item " + lexical_cast<string>(itemNumber)
                + " does not exist");
    }

    cartItemPtr->setReversed(true);

    if (areAllItemsReversed())
    {
        // if all items are reversed, remove the transaction.
        m_txnManager.removeTransaction(*this);
    }
}
