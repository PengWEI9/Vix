//=============================================================================
//
//  Define the constant values contained in various card fields.
//
//  These should really be in their own package, as they sit just above
//  Myki Card Services, but just below Myki Business Ruless. They're here
//  in the BR at the moment because BR is the only thing that uses them so
//  far. If these constants become used by any package other than BR, they
//  should be broken out into their own package and prefixed appropriately.
//
//=============================================================================

#ifndef MYKI_BR_CARD_CONTANTS_H_
#define MYKI_BR_CARD_CONTANTS_H_

#define OPERATION_MODE_TEST             0x01    // Bit 0 : Test Mode

    /** TAppUsageLog.PaymentMethod values */
enum
{
    TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED = (0),      /**< Undefined */
    TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE,               /**< T-Purse */
    TAPP_USAGE_LOG_PAYMENT_METHOD_CASH,                 /**< Cash */
    TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_REFUNDABLE,       /**< EFT (Refundable) */
    TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_NONREFUNDABLE,    /**< EFT (Non-refundable) */
    TAPP_USAGE_LOG_PAYMENT_METHOD_RECURRING_AUTOLOAD,   /**< Recurring autoload */
    TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD,       /**< Ad-hoc autoload */
    TAPP_USAGE_LOG_PAYMENT_METHOD_CHEQUE,               /**< Cheque */
    TAPP_USAGE_LOG_PAYMENT_METHOD_VOUCHER,              /**< Voucher */
    TAPP_USAGE_LOG_PAYMENT_METHOD_TRANSFER,             /**< Transfer */

};

    /** TAppUsageLog.ControlBitmap bit values */
enum
{
    TAPP_USAGE_LOG_CONTROL_VALUE        = (0x01),   /* BIT0 */
    TAPP_USAGE_LOG_CONTROL_USAGE        = (0x02),   /* BIT1 */
    TAPP_USAGE_LOG_CONTROL_PRODUCT      = (0x04),   /* BIT2 */
    TAPP_USAGE_LOG_CONTROL_BIT3         = (0x08)    /* BIT3 - RFU */
};

    /** TAppLoadLog.ControlBitmap bit values */
enum
{
    TAPP_LOAD_LOG_CONTROL_VALUE         = (0x01),   /* BIT0 */
    TAPP_LOAD_LOG_CONTROL_PROVISIONAL   = (0x02),   /* BIT1 */
    TAPP_LOAD_LOG_CONTROL_PRODUCT       = (0x04),   /* BIT2 */
    TAPP_LOAD_LOG_CONTROL_BIT3          = (0x08)    /* BIT3 - RFU */
};

#endif  // MYKI_BR_CARD_CONTANTS_H_
