/**************************************************************************
*   ID :  BR_LLSC_6_15
*
*    Asymmetric Pricing - Partial coverage
*    We have partial coverage as full coverage is
*    taken care above
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  Dynamic.CurrentTrip.IsThinZone
*    8.  TAppControl.Directory.Status
*    9.  TAppTProduct.PurchaseValue
*    10.  Dynamic.CombinedProductValue
*    11.  Dynamic.ProposedMergezone.Low
*    12.  Dynamic.ProposedMergezone.High
*    13.  Dynamic.ProposedMergeFare
*    14.  Dynamic.ProposedMergeDirection
*    15.  Dynamic.CombinedProductValue
*    16.  Dynamic.CappingContibution
*    17.  Tariff.ApplicableFare
*    18.  Dynamic.CappingEpassZoneLow
*    19.  Dynamic.CappingEpassZoneHigh
*    20.  Dynamic.CurrentTripDirection
*    21.  Dynamic.IsForcedScanOff
*    22.  Dynamic.ForcedScanOffDate
*    23.  Dynamic.FareRouteID
*    24.  Dynamic.CurrentDateTime
*    25.  TAppControl.PassengerCode
*    26.  Dynamic.AdditionalHoursThisTrip
*    27.  Tariff. NHourMaximumDuration
*    28.  Tariff.NHourExtendThreshold
*    29.  Tariff.NhourExtendPeriod
*    30.  Tariff.MaximiumProductExpiryTime
*    31.  TApp.TProduct.StartDateTime
*    32.  TApp.TProduct.EndDateTime
*    33.  Dynamic.CombinedZonerangeOffPeakFare
*    34.  Dynamic.OffPeakDiscountRate
*
*
*    A.  Combined off peak product value. (Its sister variable is Dynamic.CombinedProductValue)10
*    B.  Current trip peak fare
*    C.  Current trip off peak fare
*    D.  Capping Fare
*    E.  Merge Product Purchase Value
*    F.  E-Pass contribution
*
*    Given this rules complexity, additional
*    denotations for local variable usage have been
*    included for readability purposes.
*
*    Description
*    1.  If  IsForcedScanOff(21) is true then set OffPeakDiscountRate(34) to 0; ie No Off peak discount rates will
*        apply at force scan off.
*    2.  Produce a preferential zonal map (nb: A zonal map is structure to determine which products cover
*        which zones. For each zone only 1 products of type: e-Pass, Weekly, Daily, single trip, or Nhour -
*        where the border status is not set, can exist; For Nhour products with different border status more than
*        1 can cover the same zone ), For every product that is e-Pass is and if the e-Pass is activated
*           a.  Determine the zones that the products cover - recording these on a zonal map
*    3.  For every product that is type Weekly
*           a.  Determine the zones that the products cover -update the product coverage on the zonal map
*    4.  For every product that is type Daily
*           a.  Determine the zones that the products cover -update the product coverage on the zonal map
*    5.  For every product that is type n-Hour or Single trip
*           a.  Determine the zones that the products cover -update the product coverage on the zonal map
*    6.  For each contiguous block of zones that is of type e-Pass, Weekly or Daily, determine the fare and
*        accumulate this value into as the Combined product value(10):
*        a.  The contiguous block of zones
*           i.   Passenger type(25)
*           ii.  The current date time(24)
*           iii. Determined fare route(23)
*    7.  Set the Combined off peak product value(A) to the value of the combined product value(10)
*    8.  For each contiguous block of products that contains 1 or more N-hour or Single Trip products;
*        accumulate the product's purchase value into the CombinedProductvalue(10)
*    9.   If the OffPeakDiscountRate(34) is not zero then
*         a.  For each contiguous bock of product that contains 1 or more n-hour or Single Trip products
*           i.  If the product control bitmap indicates on the product indicates the current trip is an off-peak;
*               Accumulate the combined off peak product purchase value(A) as product value(9) multiplied as 1
*               less OffPeakDiscountRate(34)
*           ii.   Else increment the combined off peak product(A) value by the product value(9).
*    10.  Else set the Combined off peak product value(A) to the value of the combined product value(10)
*    11.  If the current trip is being processed as a force scan off ie IsForcedScanOff(21)  = false then set the
*         proposed merge zone direction(14) to disabled
*    12.  Determine the current trip peak fare(B):
*          i.   The zone range of the current trip(5),6
*          ii.  Passenger type(25)
*          iii. Determined fare route(23)
*          iv.  The current date time(24)
*          v.   The scan on time
*          vi.  The scan off time
*          vii. The current trip direction
*       b.  Determine the current trip off peak fare(C) by multiplying the current trip fare by 1 minus the Minus the
*           off peak discount rate(34)
*    13.  If the current trip is an off peak trip (ie the off peak discount rate(34) is not 0) and the value of proposed off
*         peak merge fare(33) less the combined off peak product value(A) is less than or equal to the current trip off
*         peak fare(C)
*       OR,
*         If the current trip is not an off peak trip (ie the off peak discount(34) rate is 0) and the value of proposed
*         peak merge fare(13) less the combined peak product value(10) is less than or equal to the current trip peak
*         fare(B) then we are able to use the proposed merge.
*       a.  Calculate the capping contribution(F) to be added as a result of any active e-Pass:
*          (1)  From the zonal map; for each contiguous block of zones that is of type e-Pass, determine
*               their fare as:
*              (a)  The Low zone and high zone of contiguous block of  type e-Pass
*              (b)  Passenger type(25)
*              (c)  Determined fare route(23)
*              (d)  The scan on time
*              (e)  The scan off time
*
*       b.  Set the capping contribution low zone(18) to the low zone for the e-Pass product from the zonal map
*       c.  Set the capping contribution high zone(19) to the high zone for the e-Pass product from the zonal map
*       d.  If the current trip is an off peak trip (ie Offpeak discount rate(34) is not equal to zero) and the combined
*           off peak  Product value(A) is not equal to the combined peak product value(10) (i.e both the current trip
*           and one or more n-hour were marked as off peak products) then
*               i.   Set the Capping Fare(D) to the Proposed Merge Peak Fare(13).
*               ii.  Set the Merge Product Purchase Value(E) to the proposed peak merge fare(13).
*               iii. If the value of the proposed merge off peak fare(33) less the combined off peak product value(A) is
*                    greater than zero (0) then
*          (1)  Increase the applicable fare(17) by the proposed merge off peak fare(33) less the combined
*          product off peak value(A)
*
*       e.  Else If the current trip is an off peak (ie Offpeak discount rate(34) is not equal to zero) and the
*           combined off peak product value(A) is equal to the combined peak product value(10) (ie there is no
*           existing n-hour product marked off peak)
*             i.   Set the Capping Fare(D) to the Proposed Merge Peak Fare(13)
*             ii.  Set the Merge Product Purchase Value(E) to the proposed peak merge fare(13).
*             iii. If the value of the proposed merge off peak fare(33) less the combined peak product  value(10) is
*                  greater than zero then
*           (1)  Increase the applicable fare(17) by the proposed merge off peak fare(33)  less the combined
*                peak product  value(10)
*    f.  Else it is a normal peak trip (merging with another normal trip or its normal with off peak assessed at
*        peak)
*       i.  If this is a forced Scan Off(21) AND if Proposed Merge Peak Fare(13) less Combined Peak Product
*           Value(10) is equal to the current trip peak fare(B) AND Combined Peak Product Value(10) is 0
*          (1)  If the capping option is set to 1 then:
*               (a) Set the Merge Product Purchase Value(E) to the product in use product purchase value(9)
*                   (ie the provisional fare)
*               (b) Set the capping fare(D) to the product in use product purchase value(9) (ie the provisional
*                   fare)
*          (2)  If the provisional capping option is set to option 2 then:
*               (a)  Set the Merge Product Purchase Value(E) to the product in use product purchase value(9)
*               (ie the provisional fare)
*               (b)  Set the capping fare(D) to Trip Peak FareB
*       ii.  Else
*            (1)  Set Merge Product Purchase Value(E) to Proposed Merge Peak fare(13)
*            (2)  Set Capping Fare(D) to the  Proposed Merge Peak Fare(13)
*       iii.  Increase the applicable fare(17) by Merge Product Purchase Value(E)  less the combined peak
*             product  value(10)
*       iv.  Set the Merge Product Purchase Value(E) to the ProposedMergePeakFare(13).
*    g.  Increase the capping Contribution(16) by the Capping Fare(D) less the combined peak product value(10).
*    h.  Increase the Capping Contribution(16) by the E-Pass contributionF.
*       i.  Determine the trip extension for the combined merged zone range(11),12 as
*           i.      Count the number of travelled zones from combined merge zone low(11) to the combined merge
*                   zone high(12) (not counting any zones marked as city saver)
*           ii.     Determine the number of extensions to give: for every whole multiple of zones(28)  that can be
*                   divided into the zone count
*           iii.    Determine the additional number of hours(26): multiple the number of extension by the number of
*                   hours entitled too(29)
*           iv.     If the number of additional hours(26) plus the n-hour period is greater than the Maximum allowed
*                   n-hour(27) period then set the number of additional hours(26) to the Maximum
*    j.  Determine which is the oldest n-hour product to update based on product expiry time
*    k.  If there are no products of type n-Hour then:
*       i.  If  the low zone of the current trip is equal to the high zone of the current trip and if that zone is
*           marked as a city Saver Zone and if the fare of a Single Trip is not equal that to a  n-Hour fare
*           AND Proposed Merge Peak fare(13) less the Combined peak product value(10) is equal to Peak
*           Trip Fare(B) then:
*           (1)  Perform a ProductSale/Upgrade transaction using:
*               (a)  Product ID of Single Trip,
*               (b)  Clear the provisional bit on the product control bitmap(2)
*               (c)  Set the zone range to trip zone range(5),6
*               (d)  Set the Purchase value(9) to the Merge Product Purchase Value(E) .
*               (e)  If the number of additional hours(26) is not zero then
*               (f)  Set the expiry time(31) of the product to the greater of the current expiry(31) or
*                    the start time(30) plus the default n-hour period plus number of additional
*                    hours(26).
*           (2)  Perform a ProductUpdate/none transaction to
*               (a)  If the off-peak discount rate(34) is not zero set the off peak bit on the product control
*                    bitmap to true
*       ii.  Else
*           (1)  Then update the product in use(1) (ie the provisional product). Perform a
*                ProductSale/upgrade transaction using
*                   (a)  Set the product purchase value(9) to proposed peak merge fare(13)
*                   (b)  Set the zone range to combined zone range(11),(12)
*                   (c)  Adjust for city saver zone
*                       1.  If the Low zone of the proposed merge zone range(11) minus 1 is a city saver
*                           decrement the low zone(5) by one;
*                       2.  If the High zone of the trip zone range(12)  plus 1 is a city saver increment the
*                           high zone(6) by one
*                   (d)  Clear the provisional bit on the product control bitmap(2)
*                   (e)  If the number of additional hours(26) is not zero then
*                           1.  Set the expiry time(31) of the product to the greater of the current expiry(31) or
*                               the start time(30) plus the default n-hour period plus number of additional
*                               hours(26).
*           (2)  Perform a ProductUpdate/none transaction to
*                   (a)  Set the trip direction to the Proposed merge zone direction(14)
*                   (b)  If the off-peak discount rate(34) is not zero set the off peak bit on the product control
*                        bitmap to true
*    l.  Else
*       i.  Upgrade the oldest product n-hour or city saver product
*           (1)  If the oldest product is a City Saver product then perform a ProductSale/Upgrade
*               (a)  alter the product type to n-Hour
*           (2)  Else upgrade the oldest n-hour perform a ProductUpdate/Extend
*       ii.  Update the zone range to include the combined product zone range
*           (a)  If the low zone of n-Hour product(3) is greater than or equal to the low zone of the
*                combined zone range(11), set the low zone of the n-hour to the low zone of the
*                combined zone range(11).
*           (b)  If the high zone of the n-Hour product(4) is less than or equal to the high zone of the
*                combined zone range(12), set the high zone of the n-Hour to the high zone of the
*                combined zone range(11)
*           (c)  Adjust for city saver zone
*                   1.  If the Low zone of the combined zone range(11) minus 1 is a city saver
*                       decrement the low zone(5) by one;
*                   2.  If the High zone of the combined zone range(12) plus 1 is a city saver
*                       increment the high zone(6) by one
*       iii.  Set the product purchase value(9) to the proposed merge fare(13)
*       iv.  If there are additional hours(26) to include in on the product (ie . AdditionalHoursThisTrip(26) is
*            greater than zero)
*             (a)  Set the expiry time(31) of the product to the greater of the oldest expiry time current(31)
*                  expiry or the oldest product start time(30) plus the default n-hour period plus number of
*                  additional hours(26).
*
*       v.  Perform a ProductUpdate/none to
*             (1)  Set the trip direction to the proposed merge trip direction(14)
*             (2)  Clear the border flag on Product Bitmap.
*             (3)  If the off-peak discount rate(34) is not zero set the off peak bit on the product control bitmap
*                  to true
*       vi.  Set this product as the product in use(1) using a TAppUpdate/SetProductInUse transaction.
*       vii.  Perform a ProductUpdate/Invalidate on the provisional product.
*    m.  For each product of type n-Hour that was merged (except for the product in use(1))
*           i.  Checking  to see if a product control bit map is marked as off peak
*           ii.  Perform ProductUpdate/Invalidate transaction
*    n.  If Current Trip is not an Offpeak trip
*           i.  If any product was marked off peak and if the off-peak discount rate(34) is not zero then perform
*               a product update/none on the product in use and set the off peak bit on the product control bit
*               map to true
*    14.  Else we are not able to merge and will upgrade the provisional product to the n-Hour product
*    15.  Re-determine the trip direction for the current trip zone range
*       a.  If this is a force scan of sequence (ie IsForceScanOff(21) = True) then set the trip direction to disabled
*       b.  else
*          (1)  Reset the proposed merge trip direction(14) to unknown
*          (2)  For each n-hour product on the zonal map between the current trip low zone(5) and current
*               trip high zone(6)
*                (a)  If the proposed merge trip direction(14) is Unknown, set the proposed merge trip
*                     direction(14) to the product trip direction
*                (b)  Else if the proposed merge trip direction(14) is not equal to the product control bitmap
*                     trip direction(2), and the product trip direction(2) is not equal to Unknown, set the proposed
*                     merge trip direction(14) to Disabled
*           (3)  If the proposed merge trip direction(14) is Unknown, and the Current Trip direction(20) is not
*                unknown; set the proposed merge trip direction(14) to the product trip direction(20).
*          (4)  Else if the proposed merge trip direction(14) is not equal to the current trip direction(20) and the
*               current trip direction(20) is not equal to Unknown, set the proposed merge trip direction(14) to
*               Disabled
*    16.  Calculate the value of the limited zonal map as follows
*           a.  Reset the combined product value(10)
*           b.  Reset the combined off peak product valueA
*           c.  For each contiguous block of zones from the zonal map limited to the current trip zone low(5) to zone
*               high(6)
*             i.  Determine the n-Hour fare for:
*                (1)  The low zone and high zone of the contiguous block
*                (2)  The passenger type(25)
*                (3)  The current date time(24)
*             ii.  If the all of the products within the contiguous block contain only products where the product
*                  control map off peak bit is true then
*                    (1)  Accumulate the combined off peak product value(A)  adding as 1 minus the off peak discount
*                         rate(34) multiplied by the determined fare
*             iii.  Else accumulate the determined fare and add it to the combined off peak product valueA
*             iv.  Accumulate the determined fare  add the value to the combined product value(10)
*    17.  Determine the fare for the non-merge
*       a.  If the current trip is off peak (ie. The off peak(34) discount rate is not zero) and if the combined peak
*           product value(10) is not equal to the combined off peak product value(A) then
*              i.   Set the MergeProductPurchaseValue(E) to the peak trip fareB.
*              ii.  Set the Capping Fare(D) to the peak trip fareB.
*              iii. If the current trip off peak fare(C) less the combined off peak product value(A) is greater than zero
*                   then
*                  (1)  Increase the applicable fare(17) by the current trip off peak  fare(C) minus the combined off
*                       peak product valueA
*       b.  Else if the current trip is off peak (ie. The offpeak discount rate(34) is not zero) and the combined peak
*           product value(10) is equal to the combined off peak product value(A) (ie there is no existing off peak trip)
*             i.   Set the MergeProductPurchaseValue(E)  to the peak trip fareB.
*             ii.  Set the Capping Fare(D) to the peak trip fareB.
*             iii. If the current trip off peak fare(C) less the combined peak product value(10) is greater than zero
*                  then
*                (1)  Increase the applicable fare(17) by the current trip off peak fare(C) minus the peak combined
*                     product value(10)
*    c.  Else its a normal peak trip (merging with another normal trip or its normal with off peak assessed at
*        peak)
*             i.  If this a Forced Scan Off and the combined product purchase value(10) is equal to 0 then
*                   (1)  If the Capping Mode is Option 1
*                      (a)  Set Merge Product Purchase value(E) to the product in use Purchase value(9).
*                      (b)  Set the Capping Fare(D) to the product in use Purchase value(9).
*                   (2)  Else if the Capping Mode is Option 2
*                      (a)  Set MergeProductPurchaseValue(E) to the product in use Purchase Value(9).
*                      (b)  Set the Capping Fare(D) to the peak Trip FareB.
*             ii.  Else
*                   (1)  Set the MergeProductPurchaseValue(E) to the peak Trip FareB.
*                   (2)  Set the Capping Fare(D) to the peak trip fareB.
*             iii.   Increase the applicable fare(17) by the MergeProductPurchaseValue(E) minus the  combined peak
*                    product value(10)
*    18.  Perform a ProductSale/Upgrade on the product in use:
*          a.  If the low(5) and high(6) zones of the current trip are equal and if the single trip fare of the current trip is
*              not equal to the n-Hour cap fare set the product ID to Single Trip.
*          b.  Clear the provisional bit of the product control bitmap
*          c.  Update the zone range to include the current trip zone range
*               i.  If the low zone(3) of n-hour is not equal to the low zone of the current trip(5), set the low zone of
*                   the n-hour to the low zone of the current trip(5).
*               ii. If the high zone(4) of the n-hour is not equal to the high zone of the current trip(6), set the high zone
*                   of the n-hour to the high zone of the current trip(6)
*          d.  Adjust for city saver zone
*               i.  If the low zone of the current product(3) minus 1 is a city saver decrement the product low zone(3) by one;
*               ii.  If the High zone of the current product(4) plus 1 is a city saver increment the product high zone(4) by one
*          e.  Product purchase value, If the between current trip zone low(5) and current trip zone high(6);  one or
*              more products of type daily,weekly or activated e-Pass exist  then
*               i.  set the product purchase value(9) to the current trip peak fareB
*               ii. else set the product purchase value(9) to the greater of either the current trip peak fare(B) minus
*                   the combined product value(10) or zero
*    19.  If there is a over lapping (n-hour or single trip) product  such that 1 or more zones are in common with
*         the current trip zone range; then update the product performing a ProductUpdate/Extend transaction
*          (1)  Set the end time(32) of the product in use to the end date time(32) of the oldest n-hour/single
*               trip product
*    20.  Using zone map with adjacent zonal theory; Determine the adjacent or overlapping product zone range
*         (zone low and zone high) based upon the current trip zone range
*          a.  Referring to the zonal map and the current trip zone low(5), determine where the end low zone of  the
*              contiguous and/or adjacent zonal coverage has starts;  (such that each product zone low less 1 is
*              greater than or equal to product n HighZone) (regardless of the product type)
*          b.  Referring to the zonal map and the current trip zone high(6) , determine where the end high zone of
*              the contiguous and or adjacent zonal coverage end; (such that each product zone high plus 1 is less
*          c.  Count the number of travelled zones from adjacent zone low to the adjacent product zone high (not
*              counting any zones marked as city saver)
*          d.  Determine the number of extensions to give: for every whole multiple of zones(28) that can be divided
*              into the zone count
*          e.  Determine the additional number of hours(26): multiple the number of extension by the number of
*              hours entitled too(29)
*          f.  If the number of additional hours(26) plus the default n-hour period is greater than the Maximum(27)
*              allowed n-hour period then set the number of additional hours to the Maximum(27).
*          g.  If there are additional hours(26) to be included (ie . AdditionalHoursThisTrip(26) is greater than zero)
*          i.  For each n-hour or single trip product (partially or fully) within the adjacent trip zone range
*             (1)  Perform a ProductUpdate/Extend transaction
*             (a)  Set the expiry time(31) of the product to the greater of:
*                1.  the current expiry time
*                2.  the start time  plus the default n-hour period plus number of additional
*                    hours(26).
*    21.  For each n-hour product (partially or fully) within the trip zone range excluding the product in use
*           a.  Perform a ProductUpdate/none
*               i.    Set the trip direction bit of the product control bitmap to the proposed trip direction(14).
*    22.   Perform a ProductUpdate/none on the product in use(1)
*          a.  Set the trip direction bit of the product control bitmap to the proposed trip direction(14).
*          b.  If the current trip is an off peak trip (ie the offpeak discount rate(34) is not zero) or if the combined
*              offpeak product value(A) is not equal to the combined product value(10) then set the off peak bit of the
*              product control bit map to true.
*    23.  Increase the capping contribution(16) by the greater of Capping Fare(D) minus the combined peak product
*         value(10) or zero (0)
*    24.  Calumulate the e-Pass capping contribution(F) as:
*          a.  For each contiguous block of zones from the zonal map, limited by the low zone(5) of current trip to
*              the high(6) zone of the current trip where the product of type e-Pass
*             i.   The low zone of the contiguous block
*             ii.  The High zone of the contiguous block
*             iii. The passenger type(25)
*             iv.  The current date time(24)
*             v.   The determined fare route(23)
*          b.  Increase the capping contribution(16) by the e-Pass contribution amount(F).
*
*    Notes
*    The value to consider is the different for
*    capping contribution
*
*    This is how much capping contribution the
*    EPass is worth and its only counted if the
*    Epass is not covered by other products. (This
*    is handled elsewhere)
*
*    Could be made consistent with iii like "if current
*    trip is offpeak and proposed merge offpeak
*    fare less the combined offpeak product value
*    is equal to the current trip offpeak fare and
*    combined offpeak product value is zero
*
*
*    So it's a merge
*
*    Case (a) It's a simple merge
*
*    The capping options are from product config
*    and are:
*    '1': The provisional fare is charged, and the
*    provisional fare is added to capping fare totals.
*
*    '2': The provisional fare is charged, and the
*    standard (actual) fare is added to the capping
*    fare totals.
*
*    Why there is a difference between ii.2 and iii.2
*    ?
*
*    Why Starttime and not endtime?
*
*    We check here to see if there was any off peak
*    products that were 'consumed' in the merge
*
*    If any product was consumed here we update
*    the product in use and set it as an off peak trip
*    Did not exactly understand this?
*
*    The expiry time of the new product is changed
*    to match that of the earliest existing n-hour
*    product with which the  new product has a
*    zone in common.
*
*    Trip extension is based on the whole zone
*    range, and any extension IS added to the
*    expiry time of ALL products.
*
*    Inactive e-passes are not considered as they
*    are not on the zonal mnap
*
*    Else the original trip extension done on the
*    provisional exists thus the trip extension
*    already applies.
*
*    This is done to ensure the customer gets the
*    full value of the n-hour on the next trip
*
*    Else default case
*
*    Post Condition
*    1.  The trip will now be fully paid for at scan off
*    2.  An existing product extended to include the current trip
*    3.  The applicable fare is now set pending deduction
*
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cd.h>

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Purpose :
//  ---------
//  ???
//
//  This rule is executed as part of a scan-off or a forced scan-off, always after
//  rule 6_14 has been bypassed.
//
//  Pre-requisite rules :
//  ---------------------
//  -   6_14 (bypassed).
//
//  Assumptions :
//  -------------
//  -   DynamicData.isForcedScanOff has been set to TRUE or FALSE.
//
//  Pseudo-code :
//  -------------
//  If forced scan-off,
//      then off-peak discount rate = 0,
//      else it's the application defined off-peak discount rate (ie DynamicData.offPeakDiscountRate)
//  Create a preferential zone map - this is a map of which product applies to which zone.
//  Calculate combined product value - this is the combined peak and off-peak values of the products on the card.
//
//=============================================================================

RuleResult_e BR_LLSC_6_15( MYKI_BR_ContextData_t *pData )
{
    Currency_t  combinedOffPeakProductValue = 0;    // A
    Currency_t  currentTripPeakFare         = 0;    // B
    Currency_t  currentTripOffPeakFare      = 0;    // C
    Currency_t  cappingFare                 = 0;    // D
    Currency_t  mergedProductPurchaseValue  = 0;    // E
    Currency_t  ePassContribution           = 0;    // F
    int         dir =-1;
    Time_t      extendedDateTime;

    ProductIterator     iNhour;
    MYKI_TAControl_t    *pMYKI_TAControl = NULL;
    MYKI_TAProduct_t    *pProductInUse = NULL;
    MYKI_Directory_t    *pDirectoryInUse = NULL;
    MYKI_TAProduct_t    UpdatedProduct ={0};

    memset(&UpdatedProduct,0,sizeof(UpdatedProduct));
    memset(&iNhour,0,sizeof(iNhour));

    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Start (Asymmetric Pricing - Partial coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_15 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Pre-conditions : The BR document says that all steps are "pre-conditions" but in fact it appears that none are
    //

    //
    //  Processing
    //

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_15 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Step 1 :  If this is a forced scan-off, set the off-peak discount rate to 0 (ie, no off-peak discount will apply)

    if ( pData->DynamicData.isForcedScanOff )
    {
        pData->DynamicData.offPeakDiscountRate = 0;
    }

    //  Steps 2 - 5 :   Create preferential zone map.

    if ( myki_br_CreateZoneMap( pData, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh ) < 0 )
    {
        CsErrx( "BR_LLSC_6_15 : myki_br_CreateZoneMap() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Steps 6 - 10 :  Calculate combined product value.

    if ( myki_br_CalculateCombinedProductValue( pData, pMYKI_TAControl->PassengerCode, &pData->DynamicData.combinedProductValue, &combinedOffPeakProductValue ) < 0 )
    {
        CsErrx( "BR_LLSC_6_15 : myki_br_CalculateCombinedProductValue() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Step 11 : If this is a forced scan-off, set the trip direction to disabled

    if ( pData->DynamicData.isForcedScanOff )
    {
        pData->InternalData.ProposedMerge.TripDirection = TRIP_DIRECTION_DISABLED;
    }

    //  Find the product in use (used by other steps below)

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsErrx( "BR_LLSC_6_15 : No product in use" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_15 : Cannot get product in use (product %d)", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  Step 12 : Determine the current trip peak fare

    if
    (
        myki_br_getFareStoredValueEx
        (
            &currentTripPeakFare,
            pData,
            pData->DynamicData.currentTripZoneLow,
            pData->DynamicData.currentTripZoneHigh,
            pMYKI_TAControl->PassengerCode,
            pData->DynamicData.currentTripDirection,
            pData->DynamicData.fareRouteIdIsValid,
            pData->DynamicData.fareRouteId,
            pProductInUse->StartDateTime,
            pData->DynamicData.currentDateTime
        ) < 0
    )
    {
        CsErrx( "BR_LLSC_6_15 : myki_br_getFareStoredValueEx() failed" );
        return RULE_RESULT_ERROR;
    }

    currentTripOffPeakFare  = CalculateDiscountedFare( currentTripPeakFare, pData->DynamicData.offPeakDiscountRate );

    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Product In Use    = %d", pMYKI_TAControl->ProductInUse );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Forced scan-off   = %d", pData->DynamicData.isForcedScanOff );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Merge Direction   = %d", pData->InternalData.ProposedMerge.TripDirection );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Discount Rate     = %d", pData->DynamicData.offPeakDiscountRate );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Peak Fare         = %d", currentTripPeakFare );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Off-Peak Fare     = %d", currentTripOffPeakFare );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Merge Peak        = %d", pData->InternalData.ProposedMerge.Fare );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Merge Off-Peak    = %d", pData->InternalData.ProposedMerge.OffPeakFare );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Combined Peak     = %d", pData->DynamicData.combinedProductValue );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Combined Off-Peak = %d", combinedOffPeakProductValue );

    //  Step 13
    //       If the current trip is an off peak trip (ie the off peak discount rate(34) is not 0) and the value of proposed off
    //       peak merge fare(33) less the combined off peak product value(A) is less than or equal to the current trip off
    //       peak fare(C)
    //     OR,
    //       If the current trip is not an off peak trip (ie the off peak discount(34) rate is 0) and the value of proposed
    //       peak merge fare(13) less the combined peak product value(10) is less than or equal to the current trip peak
    //       fare(B) then we are able to use the proposed merge.


    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : OffPeak : Proposed Merge Fare (%d) - Combined Product Value (%d) = %d, Current Trip Fare = %d", pData->InternalData.ProposedMerge.OffPeakFare, combinedOffPeakProductValue, pData->InternalData.ProposedMerge.OffPeakFare - combinedOffPeakProductValue, currentTripOffPeakFare );
    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Peak    : Proposed Merge Fare (%d) - Combined Product Value (%d) = %d, Current Trip Fare = %d", pData->InternalData.ProposedMerge.Fare, pData->DynamicData.combinedProductValue, pData->InternalData.ProposedMerge.Fare - pData->DynamicData.combinedProductValue, currentTripPeakFare );

    if
    (
        (
            (pData->DynamicData.offPeakDiscountRate != 0) &&
            ( (pData->InternalData.ProposedMerge.OffPeakFare - combinedOffPeakProductValue) <= currentTripOffPeakFare)
        ) ||
        (
            (pData->DynamicData.offPeakDiscountRate == 0) &&
            ( (pData->InternalData.ProposedMerge.Fare - pData->DynamicData.combinedProductValue) <= currentTripPeakFare )
        )
    )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : %s Proposed Merge Fare - Combined Product Value <= Current Trip Fare : Perform merge", pData->DynamicData.offPeakDiscountRate ? "OffPeak" : "Peak" );

        //  Step 13.a
        //  Step 13.b
        //  Step 13.c

        if ( myki_br_CalculateEPassCappingContribution( pData, pMYKI_TAControl->PassengerCode ) < 0 )
        {
            CsErrx( "BR_LLSC_6_15 : Unable to calculate ePass capping contribution" );
            return RULE_RESULT_ERROR;
        }

        //  Step 13.d

        if ( pData->DynamicData.offPeakDiscountRate != 0 && combinedOffPeakProductValue != pData->DynamicData.combinedProductValue )
        {
            cappingFare = pData->InternalData.ProposedMerge.Fare;
            mergedProductPurchaseValue = pData->InternalData.ProposedMerge.Fare;
            if ( pData->InternalData.ProposedMerge.OffPeakFare > combinedOffPeakProductValue )
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 13.d : Increasing applicable fare by %d from %d to %d", (pData->InternalData.ProposedMerge.OffPeakFare - combinedOffPeakProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (pData->InternalData.ProposedMerge.OffPeakFare - combinedOffPeakProductValue) );
                pData->ReturnedData.applicableFare += pData->InternalData.ProposedMerge.OffPeakFare - combinedOffPeakProductValue;
            }
        }

        //  Step 13.e

        else if ( pData->DynamicData.offPeakDiscountRate != 0 && combinedOffPeakProductValue == pData->DynamicData.combinedProductValue )
        {
            cappingFare = pData->InternalData.ProposedMerge.Fare;
            mergedProductPurchaseValue = pData->InternalData.ProposedMerge.Fare;
            if ( pData->InternalData.ProposedMerge.OffPeakFare > pData->DynamicData.combinedProductValue )
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 13.e : Increasing applicable fare by %d from %d to %d", (pData->InternalData.ProposedMerge.OffPeakFare - pData->DynamicData.combinedProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (pData->InternalData.ProposedMerge.OffPeakFare - pData->DynamicData.combinedProductValue) );
                pData->ReturnedData.applicableFare += pData->InternalData.ProposedMerge.OffPeakFare - pData->DynamicData.combinedProductValue;
            }
        }

        //  Step 13.f

        else    /* pData->DynamicData.offPeakDiscountRate == 0 */
        {
            //  Step 13.f.i
            if
            (
                pData->DynamicData.isForcedScanOff &&
                pData->DynamicData.combinedProductValue == 0 &&
                pData->InternalData.ProposedMerge.Fare == currentTripPeakFare
            )
            {
                //  Step 13.f.i.1
                mergedProductPurchaseValue = pProductInUse->PurchaseValue;

                switch ( pData->Tariff.provisionalCappingOption )
                {
                    case PROVISIONAL_CAPPING_OPTION_USE_PROVISIONAL_FARE :
                        cappingFare = pProductInUse->PurchaseValue;
                        break;
                    case PROVISIONAL_CAPPING_OPTION_USE_STANDARD_FARE :
                        cappingFare = currentTripPeakFare;
                        break;
                    default :
                        CsErrx( "BR_LLSC_6_15 : Provisional capping option (%d) not supported by this rule", pData->Tariff.provisionalCappingOption );
                        return RULE_RESULT_ERROR;
                }
            }
            //  Step 13.f.ii
            else
            {
                mergedProductPurchaseValue = pData->InternalData.ProposedMerge.Fare;
                cappingFare = pData->InternalData.ProposedMerge.Fare;
            }

            //  Step 13.f.iii
            CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 13.f : Increasing applicable fare by %d from %d to %d", (mergedProductPurchaseValue - pData->DynamicData.combinedProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (mergedProductPurchaseValue - pData->DynamicData.combinedProductValue) );
            pData->ReturnedData.applicableFare += mergedProductPurchaseValue - pData->DynamicData.combinedProductValue;

            //  Step 13.f.iv
            mergedProductPurchaseValue = pData->InternalData.ProposedMerge.Fare;
        }

        //  Step 13.g

        pData->DynamicData.cappingContribution += cappingFare - pData->DynamicData.combinedProductValue;

        //  Step 13.h

        pData->DynamicData.cappingContribution += ePassContribution;

        //  Step 13.i : Determine the trip extension for the combine merged zone range

        pData->DynamicData.additionalMinutesThisTrip = myki_br_GetAdditionalMinutes( pData, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh );

        //  Step 13.j : Determine the oldest n-hour product to update based on product expiry time (may be the provisional product, in which case this will be the only nHour).
        //              Note : We don't care about the zone range in this case.

        dir = myki_br_GetOldestNHourDirContained( pMYKI_TAControl, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh );

        if ( dir <= 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_15 : No nHour product in zone range %d - %d, using provisional", pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh );
            dir = pMYKI_TAControl->ProductInUse;
        }

        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Oldest n-hour dir = %d", dir );

        //  Step 13.k : If there are no nHour products (other than the provisional, which is the product in use)

        if ( dir == pMYKI_TAControl->ProductInUse )     // Oldest (and only) nHour is the product in use
        {
            //  Step 13.k.i

            MYKI_CD_FaresStoredValue_t  currentTripFare;

            //  Assumes low and high zones are in range - this should be checked!
            if ( ! MYKI_CD_getFaresStoredValueStructure( (U8_t)pData->DynamicData.currentTripZoneLow, (U8_t)pData->DynamicData.currentTripZoneHigh, &currentTripFare ) )
            {
                CsErrx( "BR_LLSC_6_15 : MYKI_CD_getFaresStoredValueStructure() failed" );
                return RULE_RESULT_ERROR;
            }

            //  Brendan from KamCo has confirmed that CitySaver tickets are now out-of-scope, and that therefore
            //  this test will always fail because the low zone and high zone of a trip will never both be a
            //  CitySaver zone. As a result, the "if" path will now simply return an error (just in case).
            //
            //  Step 13.k.i :   If  the low zone of the current trip is equal to the high zone of the current trip, and
            //                  if that zone is marked as a city Saver Zone, and
            //                  if the fare of a Single Trip is not equal that to a  n-Hour fare, and
            //                  if Proposed Merge Peak fare13 less the Combined peak product value10 is equal to Peak Trip Fare(B) then :

            if
            (
                pData->DynamicData.currentTripZoneLow == pData->DynamicData.currentTripZoneHigh    &&
                MYKI_CD_isCitySaverZone(pData->DynamicData.currentTripZoneLow)                     &&
                currentTripFare.single_trip != currentTripFare.nhour                               &&
                pData->InternalData.ProposedMerge.Fare - pData->DynamicData.combinedProductValue == currentTripPeakFare
            )
            {
                CsErrx( "BR_LLSC_6_15: This path should never execute because CitySaver tickets are no longer supported" );
                return RULE_RESULT_ERROR;
            }

            //  Step 13.k.ii : The above condition was false

            else
            {
                //  Step 13.k.ii.1 : Update the product in use from a provisional product to a real product
                //                   This means a product sale/upgrade rather than a product update/extend because
                //                   we are clearing the provisional bit and therefore "selling" a real product to the
                //                   patron using the provisional product and other information.

                Currency_t      purchaseValue;
                int             zoneLow;
                int             zoneHigh;
                Time_t          endDateTime;

                //  Step 13.k.ii.1.a   : Set the product purchase value to the proposed merge peak fare

                purchaseValue = pData->InternalData.ProposedMerge.Fare;

                //  Step 13.k.ii.1.b-e : Extend the product zone range to include the proposed merge zone range

                zoneLow       = pData->InternalData.ProposedMerge.ZoneLow;
                zoneHigh      = pData->InternalData.ProposedMerge.ZoneHigh;

                ADJUST_FOR_CITYSAVER( zoneLow, zoneHigh );

                //  Step 13.k.ii.1.f : Clear the provisional bit (this is done by the sale/upgrade call)

                //  Step 13.k.ii.1.g-h : Extend the product expiry date/time if there are additional hours

                endDateTime = pProductInUse->EndDateTime;
                if ( pData->DynamicData.additionalMinutesThisTrip != 0 )
                {
                    extendedDateTime = myki_br_NHourEndDateTimeEx
                            (
                                pData,
                                pProductInUse->StartDateTime,
                                myki_br_GetProductDuration( pData, PRODUCT_TYPE_NHOUR ),
                                MINUTES_TO_SECONDS( pData->DynamicData.additionalMinutesThisTrip )
                            );

                    //  Set the new end date/time to the greater of the extended end date/time and the current end date/time

                    if ( extendedDateTime > endDateTime )
                        endDateTime = extendedDateTime;
                }

                CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 13.k.ii.1" );
                if ( myki_br_ldt_ProductSale_Upgrade( pData, pDirectoryInUse, pDirectoryInUse->ProductId, zoneLow, zoneHigh, purchaseValue, endDateTime ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_15 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
                    return RULE_RESULT_ERROR;
                }

                //  Step 13.k.ii.2 :    Perform a ProductUpdate/none transaction to
                //                      (a)  Set the trip direction to the Proposed merge zone direction(14)
                //                      (b)  If the off-peak discount rate(34) is not zero set the off peak bit on the product control
                //                           bitmap to true
                //
                //  Note :  Ashish has confirmed that we should only do a ProductUpdate/None if :
                //          (a) The proposed trip direction is NOT unknown (ie, it's different to the provisional product direction
                //              which is guaranteed to be unknown (0) because it's provisional), or
                //          (b) the off-peak bit should be set (ie, off-peak discount rate is not zero).

                UpdatedProduct = *pProductInUse;

                if ( pData->DynamicData.offPeakDiscountRate != 0 || pData->InternalData.ProposedMerge.TripDirection != TRIP_DIRECTION_UNKNOWN )
                {
                    myki_br_setProductTripDirection( &UpdatedProduct, pData->InternalData.ProposedMerge.TripDirection );

                    if ( pData->DynamicData.offPeakDiscountRate != 0 )
                        myki_br_setProductOffPeakStatus( &UpdatedProduct, TRUE  );      // Set product to off-peak if off-peak discount is not zero
                    else
                        myki_br_setProductOffPeakStatus( &UpdatedProduct, FALSE );      // Otherwise set it to peak


                    if ( myki_br_ldt_ProductUpdate_IfChanged( pData, pDirectoryInUse, pProductInUse, &UpdatedProduct ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_15 : myki_br_ldt_ProductUpdate() failed" );
                        return RULE_RESULT_ERROR;
                    }
                }
            }
        }

        //  Step 13.l : Else (there is at least one nHour product other than the product in use, and 'dir' is the index of the oldest one)

        else
        {
            //  Step 13.l.i

            MYKI_TAProduct_t    *pProductOldestnHour = NULL;
            MYKI_Directory_t    *pDirectoryOldestnHour = NULL;
            U8_t                ProductBitmap;
            int                 offPeak;
            int                 i;

            if ( myki_br_GetCardProduct( dir, &pDirectoryOldestnHour, &pProductOldestnHour ) < 0 )
            {
                CsErrx( "BR_LLSC_6_15 : Failed to get product %d details", dir );
                return RULE_RESULT_ERROR;
            }

            if ( myki_br_cd_GetProductType( pDirectoryOldestnHour->ProductId ) == PRODUCT_TYPE_SINGLE )
            {
                CsErrx( "BR_LLSC_6_15: This path should never execute because CitySaver tickets are no longer supported" );
                return RULE_RESULT_ERROR;
            }
            else
            {
                int             bAlwaysCreateProductUpdateNone  = FALSE;
                TripDirection_t ProductTripDirection            = myki_br_getProductTripDirection( pProductOldestnHour );

                //  Step 13.l.ii   : Update the oldest nHour product
                //  Step 13.l.ii   : - Perform a product update/extend
                //  Step 13.l.ii   :   - Extend the product zone range to include the proposed merge zone range
                //  Step 13.l.iii  :   - Set the product purchase value to the proposed merge peak fare
                //  Step 13.l.iv   :   - Extend the product expiry time if there any additional hours
                //  Step 13.l.v    : - Perform a product update/none
                //  Step 13.l.v.1  :   - Set the product trip direction to the proposed merge trip direction
                //  Step 13.l.v.2  :   - Clear the product border status
                //  Step 13.l.v.3  :   - If the off-peak discount status is not 0, set the product off-peak bit

                ProductBitmap = pProductOldestnHour->ControlBitmap;

                if
                (
                    pData->StaticData.AcsCompatibilityMode != FALSE &&
                    pData->InternalData.ProposedMerge.TripDirection == TRIP_DIRECTION_DISABLED &&
                    ProductTripDirection != TRIP_DIRECTION_UNKNOWN &&
                    ProductTripDirection != TRIP_DIRECTION_DISABLED
                )
                {
                    //  NOTE:   ACS sets TripDir to "Outbound" instead of the correct "Disabled"
                    //
                    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Applied ACS Compatibility Mode - Set TripDir to OUTBOUND" );
                    myki_br_SetBitmapDirection( &ProductBitmap, TRIP_DIRECTION_OUTBOUND );
                    bAlwaysCreateProductUpdateNone  = TRUE;
                }
                else
                {
                    myki_br_SetBitmapDirection( &ProductBitmap, pData->InternalData.ProposedMerge.TripDirection );
                }   /*  end-of-if */
                myki_br_SetBitmapBorderStatus( &ProductBitmap, BORDER_STATUS_NONE );
                if ( pData->DynamicData.offPeakDiscountRate > 0 )
                    ProductBitmap |= TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;

                if ( pData->StaticData.AcsCompatibilityMode != FALSE )
                {
                    if
                    (
                        ( pData->DynamicData.offPeakDiscountRate > 0 ) ||
                        ( ( ProductBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) != 0 )
                    )
                    {
                        bAlwaysCreateProductUpdateNone  = TRUE;
                    }   /* end-of-if */
                }   /* end-of-if */

                myki_br_ExtendProduct
                (
                    pData,                                              // Context
                    pDirectoryOldestnHour,                              // Directory entry
                    pProductOldestnHour,                                // Product
                    pData->InternalData.ProposedMerge.ZoneLow,          // Low Zone
                    pData->InternalData.ProposedMerge.ZoneHigh,         // High Zone
                    pData->InternalData.ProposedMerge.Fare,             // Purchase Value
                    pData->DynamicData.additionalMinutesThisTrip,       // Additional Minutes
                    ProductBitmap,                                      // Direction, border status, provisional, off-peak, premium, autoload
                    bAlwaysCreateProductUpdateNone
                );

                //  Step 13.l.vi : Set this product as the product in use(1) using a TAppUpdate/SetProductInUse transaction.
                myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectoryOldestnHour );

                //  Step 13.l.vii : Invalidate the provisional product (that was previously in use)
                CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 13.l.vii" );
                myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectoryInUse );

                //  Step 13.l.vi (continue)
                pDirectoryInUse     = pDirectoryOldestnHour;
                pProductInUse       = pProductOldestnHour;

                //  Step 13.m : For each nHour product that was merged (other than the new product in use)
                //              Invalidate the product
                //              "Each product that was merged" means every nhour product we accumulated in step 8.

                offPeak = FALSE;
                for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
                {
                    if
                    (
                        ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_NHOUR ) &&
                        ( pData->InternalData.ZoneMap.productList[ i ].dirIndex != pMYKI_TAControl->ProductInUse )
                    )
                    {
                        if ( pData->InternalData.ZoneMap.productList[ i ].isOffPeak )
                            offPeak = TRUE;
                        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 13.m" );
                        myki_br_ldt_ProductUpdate_Invalidate( pData, &pMYKI_TAControl->Directory[ pData->InternalData.ZoneMap.productList[ i ].dirIndex ] );
                    }
                }

                //  NOTE:   Step 13.n seems to be redundant since the product-in-use
                //          is the oldest n-Hour product (step 13.l.vi) and the offpeak bit
                //          is already set in step 13.l.v.3.

                //  13.n.   If Current Trip is not an Offpeak trip
                if ( pData->DynamicData.isOffPeak == FALSE )
                {
                    //  i.  If any product was marked off peak and if the off-peak discount rate(34)
                    //      is not zero then
                    if
                    (
                        offPeak != FALSE &&
                        pData->DynamicData.offPeakDiscountRate != 0 &&
                        ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP ) == 0
                    )
                    {
                        //  perform a product update/none on the product in use and set the
                        //  off peak bit on the product control bit map to true
                        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 13.n.i *** SHOULD NEVER EXECUTE!" );
                        myki_br_ldt_ProductUpdate_SetOffPeak( pData, pDirectoryInUse );
                    }   /* end-of-if */
                }   /* end-of-if */
            }
        }
    }

    //  Step 14 : We are not able to merge and will upgrade the provisional product to the n-Hour product

    else
    {
        int                 zoneCovered;
        int                 i;
        int                 zoneLow;
        int                 zoneHigh;
        Currency_t          fare;
        int                 allOffPeak;
        int                 provisionalOffPeak;
        MYKI_TAProduct_t    *pProductOldestnHour    = NULL;
        MYKI_Directory_t    *pDirectoryOldestnHour  = NULL;

        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : %s Proposed Merge Fare - Combined Product Value > Current Trip Fare : No merge, upgrade provisional product (index %d)", pData->DynamicData.offPeakDiscountRate ? "OffPeak" : "Peak", pMYKI_TAControl->ProductInUse );

        //  Step 15
        if (pData->DynamicData.isForcedScanOff)
        {
            //  Step 15.a
            pData->DynamicData.currentTripDirection = TRIP_DIRECTION_DISABLED;
        }
        else
        {
            //  Step 15.b
            //  Step 15.b.i
            myki_br_ClearProposedMergeTripDirection( pData );

            //  Step 15.b.ii
            for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
            {
                MYKI_Directory_t    *pDirectory = NULL;
                MYKI_TAProduct_t    *pProduct   = NULL;

                if
                (
                    ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_NHOUR ) &&
                    myki_br_ZonesOverlapOrAdjacent
                    (
                        pData->InternalData.ZoneMap.productList[ i ].zoneLow, pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                        pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh
                    )
                )
                {
                    if ( myki_br_GetCardProduct( pData->InternalData.ZoneMap.productList[ i ].dirIndex, &pDirectory, &pProduct ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_15 : Failed to get product at directory index %d", pData->InternalData.ZoneMap.productList[ i ].dirIndex );
                        return RULE_RESULT_ERROR;
                    }

                    myki_br_UpdateProposedMergeTripDirection( pData, myki_br_getProductTripDirection( pProduct ) );
                }
            }

            //  Step 15.b.iii
            //  Step 15.b.iv
            myki_br_UpdateProposedMergeTripDirection( pData, pData->DynamicData.currentTripDirection );
        }

        //  Step 16

        //  Step 16.a
        //  Step 16.b
        pData->DynamicData.combinedProductValue = 0;
        combinedOffPeakProductValue             = 0;

        //  Step 16.c : For each contiguous block of zones between the current trip low and high zones,
        //              where all zones in the block contain any active product :
        //                Determine the nHour fare using :
        //                - Low zone       = low zone of the block
        //                - High zone      = high zone of the block
        //                - Passenger Type = card passenger type
        //                - Date/Time      = current data/time
        //                Add the full fare to the combined peak product value
        //                If all products in the block are set to off-peak
        //                  Add the discounted fare to the combined off-peak product value
        //                else
        //                  Add the full fare to the combined off-peak product value

        provisionalOffPeak = pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;

        zoneCovered = FALSE;
        for ( i = pData->DynamicData.currentTripZoneLow; i <= pData->DynamicData.currentTripZoneHigh + 1; i++ )
        {
            if ( ( i == pData->DynamicData.currentTripZoneHigh + 1 ) || ( pData->InternalData.ZoneMap.zoneList[ i ].priority < 0 ) )
            {
                if ( zoneCovered )
                {
                    zoneHigh = i - 1;

                    //  Fare is the nHour fare for this zone range for the current passenger type and date/time.
                    //  ASssuming nHour fare is the same as Single Peak fare.
                    if
                    (
                        myki_br_getFareStoredValueEx
                        (
                            &fare,
                            pData,
                            zoneLow,
                            zoneHigh,
                            pMYKI_TAControl->PassengerCode,
                            pData->DynamicData.currentTripDirection,
                            pData->DynamicData.fareRouteIdIsValid,
                            pData->DynamicData.fareRouteId,
                            pProductInUse->StartDateTime,
                            pData->DynamicData.currentDateTime
                        ) < 0
                    )
                    {
                        CsErrx( "myki_br_CalculateCombinedProductValue : myki_br_getFareStoredValueEx() failed" );
                        return -1;
                    }

                    pData->DynamicData.combinedProductValue += fare;

                    if ( allOffPeak && provisionalOffPeak )
                        combinedOffPeakProductValue         += CalculateDiscountedFare( fare, pData->DynamicData.offPeakDiscountRate );
                    else
                        combinedOffPeakProductValue         += fare;

                    zoneCovered = FALSE;
                }
            }
            else
            {
                if ( ! zoneCovered )    // If this is the first zone in a new block (ie the previous zone was not covered)
                {
                    zoneLow = i;        // Record it as the block's low zone
                    allOffPeak = TRUE;  // And assume all zones are off-peak until we find one that isn't
                    zoneCovered = TRUE;
                }

                //  Check every zone in a block for a peak zone
                if ( ! pData->InternalData.ZoneMap.zoneList[ i ].isOffPeak )
                {
                    allOffPeak = FALSE;
                }
            }
        }

        //  Step 17 : Determine the fare for the non-merge

        //  Step 17.a
        if (pData->DynamicData.offPeakDiscountRate != 0 && pData->DynamicData.combinedProductValue != combinedOffPeakProductValue)
        {
            //  Step 17.a.i
            //  Step 17.a.ii
            //  Step 17.a.iii
            mergedProductPurchaseValue = currentTripPeakFare;
            cappingFare = currentTripPeakFare;
            CsDbg( BRLL_RULE, "currentTripOffPeakFare = %d, combinedOffPeakProductValue = %d", currentTripOffPeakFare, combinedOffPeakProductValue );
            if (currentTripOffPeakFare - combinedOffPeakProductValue > 0 )
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 17.a : Increasing applicable fare by %d from %d to %d", (currentTripOffPeakFare - combinedOffPeakProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (currentTripOffPeakFare - combinedOffPeakProductValue) );
                pData->ReturnedData.applicableFare += currentTripOffPeakFare - combinedOffPeakProductValue;
            }
        }
        //  Step 17.b
        else if (pData->DynamicData.offPeakDiscountRate != 0 && pData->DynamicData.combinedProductValue == combinedOffPeakProductValue )
        {
            //  Step 17.b.i
            //  Step 17.b.ii
            //  Step 17.b.iii
            mergedProductPurchaseValue = currentTripPeakFare;
            cappingFare = currentTripPeakFare;
            if (currentTripOffPeakFare - pData->DynamicData.combinedProductValue > 0)
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 17.b : Increasing applicable fare by %d from %d to %d", (currentTripOffPeakFare - pData->DynamicData.combinedProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (currentTripOffPeakFare - pData->DynamicData.combinedProductValue) );
                pData->ReturnedData.applicableFare += currentTripOffPeakFare - pData->DynamicData.combinedProductValue;
            }
        }
        //  Step 17.c
        else
        {
            //  Step 17.c.i
            if (pData->DynamicData.isForcedScanOff && pData->DynamicData.combinedProductValue == 0 )
            {
                if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_USE_PROVISIONAL_FARE )
                {
                    mergedProductPurchaseValue = pProductInUse->PurchaseValue;
                    cappingFare                = pProductInUse->PurchaseValue;
                }
                else if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_USE_STANDARD_FARE )
                {
                    mergedProductPurchaseValue = pProductInUse->PurchaseValue;
                    cappingFare                = currentTripPeakFare;
                }
            }
            //  Step 17.c.ii
            else
            {
                mergedProductPurchaseValue = currentTripPeakFare;
                cappingFare                = currentTripPeakFare;
            }

            //  Step 17.c.iii
            CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 17.c : Increasing applicable fare by %d from %d to %d", (mergedProductPurchaseValue - pData->DynamicData.combinedProductValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (mergedProductPurchaseValue - pData->DynamicData.combinedProductValue) );
            pData->ReturnedData.applicableFare += mergedProductPurchaseValue - pData->DynamicData.combinedProductValue;
        }

        //  Step 18 : Perform a product sale upgrade for the product in use
        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 18 - Perform a product sale / upgrade of the product in use (index %d)", pMYKI_TAControl->ProductInUse );

        {
            Currency_t                  purchaseValue;
            int                         found;

            //  Step 18.a (code removed because single trip (CitySaver) tickets are no longer supported.

            //  Step 18.b

            pProductInUse->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

            //  Step 18.c

            if ( pProductInUse->ZoneLow != pData->DynamicData.currentTripZoneLow )
            {
                pProductInUse->ZoneLow = pData->DynamicData.currentTripZoneLow;
            }

            if ( pProductInUse->ZoneHigh != pData->DynamicData.currentTripZoneHigh )
            {
                pProductInUse->ZoneHigh = pData->DynamicData.currentTripZoneHigh;
            }

            //  Step 18.d

            ADJUST_FOR_CITYSAVER( pProductInUse->ZoneLow, pProductInUse->ZoneHigh );

            //  Step 18.e : Product purchase value, If the between current trip zone low(5)
            //              and current trip zone high(6); one or more products of type daily,
            //              weekly or activated e-Pass exist then

            found = FALSE;
            for ( i = pData->DynamicData.currentTripZoneLow; i <= pData->DynamicData.currentTripZoneHigh; i++ )
            {
                MYKI_Directory_t   *pDirectory  = NULL;
                ProductType_e       ProductType = PRODUCT_TYPE_UNKNOWN;

                if ( pData->InternalData.ZoneMap.zoneList[ i ].dirIndex <= 0 )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Zone(%d) not covered", i );
                    continue;
                }   /* end-of-if */

                if ( myki_br_GetCardProduct( pData->InternalData.ZoneMap.zoneList[ i ].dirIndex, &pDirectory, NULL ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_15 : myki_br_GetCardProduct(%d) failed", pData->InternalData.ZoneMap.zoneList[ i ].dirIndex );
                    return RULE_RESULT_ERROR;
                }   /* end-of-if */

                if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : TAppProduct[%d].Status not activated",
                            pData->InternalData.ZoneMap.zoneList[ i ].dirIndex );
                    continue;
                }   /* end-of-if */

                ProductType = myki_br_cd_GetProductType( pDirectory->ProductId );
                if
                (
                    ProductType != PRODUCT_TYPE_DAILY  &&
                    ProductType != PRODUCT_TYPE_WEEKLY &&
                    ProductType != PRODUCT_TYPE_EPASS
                )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : TAppProduct[%d].ProductType(%d) not Daily/Weekly/ePass",
                            pData->InternalData.ZoneMap.zoneList[ i ].dirIndex, ProductType );
                    continue;
                }   /* end-of-if */

                found   = TRUE;
                break;
            }   /* end-of-for */

            if ( found )
            {
                //  Step 18.e.i : set the product purchase value(9) to the current trip peak fare(B)
                purchaseValue = currentTripPeakFare;
            }
            else
            {
                //  Step 18.2.ii :  else set the product purchase value(9) to the greater of either the
                //                  current trip peak fare(B) minus the combined product value(10) or zero
                if ( pData->DynamicData.combinedProductValue < currentTripPeakFare )
                    purchaseValue = currentTripPeakFare - pData->DynamicData.combinedProductValue;
                else
                    purchaseValue = 0;
            }

            //  Step 18.f
            CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 18.f - Call myki_br_ldt_ProductSale_Upgrade_SameProduct(), index = %d", pMYKI_TAControl->ProductInUse );

            if ( myki_br_ldt_ProductSale_Upgrade_SameProduct( pData, pDirectoryInUse, pProductInUse->ZoneLow, pProductInUse->ZoneHigh, purchaseValue, pProductInUse->EndDateTime ) < 0 )
            {
                CsErrx( "BR_LLSC_6_15 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
                return RULE_RESULT_ERROR;
            }
        }

        //  19. If there is a over lapping (n-hour or single trip) product such that 1 or more zones are in common with the
        //      current trip zone range;
        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 19" );
        dir = myki_br_GetOldestNHourDirOverlap( pMYKI_TAControl, pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );
        if ( dir >= 0 && dir != pMYKI_TAControl->ProductInUse )
        {
            //  then update the product [in use] performing a ProductUpdate/Extend transaction
            //  a.  Set the end time(32) of the product in use to the end date time(32) of the oldest n-hour/single trip product
            if ( myki_br_GetCardProduct( dir, &pDirectoryOldestnHour, &pProductOldestnHour ) < 0 )
            {
                CsErrx( "BR_LLSC_6_15 : myki_br_GetCardProduct(%d) failed", dir );
                return RULE_RESULT_ERROR;
            }

            //  NOTE:   ACS performs ProductUpdate/Extend regardless of whether
            //          TAppProduct.EndDateTime to be changed or not.
            if
            (
                ( pData->StaticData.AcsCompatibilityMode != FALSE ) ||
                ( pProductInUse->EndDateTime != pProductOldestnHour->EndDateTime )
            )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Extending product index %d expiry from %d to %d", pMYKI_TAControl->ProductInUse, pProductInUse->EndDateTime, pProductOldestnHour->EndDateTime );
                if ( myki_br_ldt_ProductUpdate_ExtendExpiryDateTime( pData, pDirectoryInUse, pProductOldestnHour->EndDateTime, FALSE ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_15 : myki_br_ldt_ProductUpdate_ExtendExpiryDateTime() failed" );
                    return RULE_RESULT_ERROR;
                }
            }
        }

        //  Step 20.a : Find the lowest continually covered zone from the current trip low zone downwards

        for ( zoneLow = pData->DynamicData.currentTripZoneLow - 1; zoneLow >= pData->InternalData.ZoneMap.zoneLow; zoneLow-- )
        {
            if ( pData->InternalData.ZoneMap.zoneList[ zoneLow ].priority < 0 )
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 20.a : Zone %d not covered", zoneLow );
                break;
            }

            CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 20.a : Zone %d covered", zoneLow );
        }
        zoneLow++;

        //  Step 20.b : Find the highest continually covered zone from the current trip high zone upwards

        for ( zoneHigh = pData->DynamicData.currentTripZoneHigh + 1; zoneHigh <= pData->InternalData.ZoneMap.zoneHigh; zoneHigh++ )
        {
            if ( pData->InternalData.ZoneMap.zoneList[ zoneHigh ].priority < 0 )
            {
                CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 20.b : Zone %d not covered", zoneHigh );
                break;
            }

            CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 20.b : Zone %d covered", zoneHigh );
        }
        zoneHigh--;

        //  Step 20.c
        //  Step 20.d
        //  Step 20.e
        //  Step 20.f

        pData->DynamicData.additionalMinutesThisTrip = myki_br_GetAdditionalMinutes( pData, zoneLow, zoneHigh );

        //  Step 20.g : If there are additional hours(26) to be included (ie. AdditionalHoursThisTrip(26)
        //              is greater than zero)

        if ( pData->DynamicData.additionalMinutesThisTrip > 0 )
        {
            //  Step 20.g.i :   For each n-hour or single trip product (partially or fully) within the adjacent trip zone range
            //                  (1)  Perform a ProductUpdate/Extend transaction
            //                      (a)  Set the expiry time(31) of the product to the greater of:
            //                      (b)  the current expiry time
            //                      (c)  the start time plus the default n-hour period plus number of additional hours(26)
            //  Note : This includes the provisional product.

            myki_br_InitProductIterator( pMYKI_TAControl, &iNhour, PRODUCT_TYPE_NHOUR );

            while ( myki_br_ProductIterate( &iNhour ) > 0 )
            {
                MYKI_Directory_t    *pDirectory = iNhour.pDirectory;
                MYKI_TAProduct_t    *pProduct   = iNhour.pProduct;

                if ( myki_br_ZonesOverlapOrAdjacent( pProduct->ZoneLow, pProduct->ZoneHigh, zoneLow, zoneHigh ) )
                {
                    extendedDateTime = myki_br_NHourEndDateTimeEx
                            (
                                pData,
                                pProduct->StartDateTime,
                                myki_br_GetProductDuration( pData, PRODUCT_TYPE_NHOUR ),
                                MINUTES_TO_SECONDS( pData->DynamicData.additionalMinutesThisTrip )
                            );

                    CsDbg( BRLL_FIELD, "BR_LLSC_6_15 : Step 20.g : Checking product at index %d : Expiry = %d, Extended = %d", iNhour.index, pProduct->EndDateTime, extendedDateTime );

                    if ( pProduct->EndDateTime < extendedDateTime )
                    {
                        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 20.g : Extend product at index %d expiry from %d to %d", iNhour.index, pProduct->EndDateTime, extendedDateTime );
                        myki_br_ldt_ProductUpdate_ExtendExpiryDateTime( pData, pDirectory, extendedDateTime, TRUE );
                    }
                }
            }

            if ( iNhour.error )
            {
                CsErrx( "BR_LLSC_6_15 : Iteration Failure getting nHour products" );
                return RULE_RESULT_ERROR;
            }
        }

        //  21. For each n-hour product (partially or fully) within the trip zone range excluding the product in use 
        //      a.  Perform a ProductUpdate/none 
        //          i.  Set the trip direction bit of the product control bitmap to the proposed trip direction(14)
        //
        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 21" );
        myki_br_InitProductIterator( pMYKI_TAControl, &iNhour, PRODUCT_TYPE_NHOUR );

        while ( myki_br_ProductIterate( &iNhour ) > 0 )
        {
            MYKI_Directory_t    *pDirectory = iNhour.pDirectory;
            MYKI_TAProduct_t    *pProduct   = iNhour.pProduct;

            if ( iNhour.index != pMYKI_TAControl->ProductInUse )
            {
                //  NOTE:   Known ACS defect performing ProductUpdate/None for product not
                //          within the current trip zone range (refer to MBURFI-38)
                //
                if ( myki_br_ZonesOverlap(
                            pProduct->ZoneLow, pProduct->ZoneHigh,
                            pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh ) )
                {
                    if ( myki_br_getProductTripDirection( pProduct ) != pData->InternalData.ProposedMerge.TripDirection )
                        myki_br_ldt_ProductUpdate_SetTripDirection( pData, pDirectory, pData->InternalData.ProposedMerge.TripDirection );
                }
            }
        }

        if ( iNhour.error )
        {
            CsErrx( "BR_LLSC_6_15 : Iteration Failure getting nHour products" );
            return RULE_RESULT_ERROR;
        }

        //  Step 22

        UpdatedProduct = *pProductInUse;        //  First, create a copy of the product so we can modify it

        //  Step 22.a

        myki_br_setProductTripDirection( &UpdatedProduct, pData->InternalData.ProposedMerge.TripDirection );

        //  Step 22.b

        if
        (
            pData->DynamicData.offPeakDiscountRate != 0 ||
            combinedOffPeakProductValue != pData->DynamicData.combinedProductValue
        )
        {
            UpdatedProduct.ControlBitmap |= TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;
        }

        //  Step 22
        CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Step 22" );

        //  NOTE:   ACS performed ProductUpdate/None regardless of product changes or not
        //          if there is one or more products in the combined zone map
        if ( pData->StaticData.AcsCompatibilityMode != FALSE && pData->InternalData.ZoneMap.productCount > 0 )
            myki_br_ldt_ProductUpdate( pData, pDirectoryInUse, pProductInUse, &UpdatedProduct );
        else
            myki_br_ldt_ProductUpdate_IfChanged( pData, pDirectoryInUse, pProductInUse, &UpdatedProduct );

        //  Step 23

        if ( cappingFare - pData->DynamicData.combinedProductValue > 0 )
        {
            pData->DynamicData.cappingContribution += cappingFare - pData->DynamicData.combinedProductValue;
        }

        //  Step 24 :   For each contiguous block of zones between the current trip low and high zones :
        //                  If the product is an ePass
        //                      Calculate the ePass fare using :
        //                          Low zone = low zone of the block
        //                          High zone = high zone of the block
        //                          Passenger type from the card
        //                          Current date/time
        //                          Determined fare route
        //                      Accumulate this value into the capping contribution (no need for separate internal ePass contribution variable)

        for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
        {
            //  a.  For each contiguous block of zones from the zonal map, limited by the low zone(5)
            //      of current trip to the high(6) zone of the current trip where the product of type e-Pass 
            //      i.  The low zone of the contiguous block
            //      ii. The High zone of the contiguous block 
            //      iii.The passenger type(25)
            //      iv. The current date time(24)
            //      v.  The determined fare route(2)
            //
            //  NOTE:   Brendan told us to include adjacent zone but that would contradict the
            //          "limited by the low zone of current trip to the high zone of the current trip"
            //          requirement. ACS does not include adjacent zone.
            //
            if
            (
                ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_EPASS ) &&
                myki_br_ZonesOverlap    //  myki_br_ZonesOverlapOrAdjacent
                (
                    pData->InternalData.ZoneMap.productList[ i ].zoneLow, pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                    pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh
                )
            )
            {
                //  Fare is the peak fare for this zone range for the current passenger type and date/time.
                if
                (
                    myki_br_getFareStoredValueEx
                    (
                        &fare,
                        pData,
                        pData->DynamicData.currentTripZoneLow > pData->InternalData.ZoneMap.productList[ i ].zoneLow ?
                            pData->DynamicData.currentTripZoneLow :
                            pData->InternalData.ZoneMap.productList[ i ].zoneLow,   //  zoneLow,
                        pData->DynamicData.currentTripZoneHigh < pData->InternalData.ZoneMap.productList[ i ].zoneHigh ?
                            pData->DynamicData.currentTripZoneHigh :
                            pData->InternalData.ZoneMap.productList[ i ].zoneHigh,  //  zoneHigh,
                        pMYKI_TAControl->PassengerCode,
                        pData->DynamicData.currentTripDirection,                        
                        pData->DynamicData.fareRouteIdIsValid,
                        pData->DynamicData.fareRouteId,
                        pProductInUse->StartDateTime,
                        pData->DynamicData.currentDateTime
                    ) < 0
                )
                {
                    CsErrx( "myki_br_CalculateCombinedProductValue : myki_br_getFareStoredValueEx() failed" );
                    return RULE_RESULT_ERROR;
                }

                //  b.  Increase the capping contribution(16) by the e-Pass contribution amount(F).
                //
                pData->DynamicData.cappingContribution += fare;
            }
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Dynamic.CappingContribution = %d", pData->DynamicData.cappingContribution );

    CsDbg( BRLL_RULE, "BR_LLSC_6_15 : Executed" );
    return RULE_RESULT_EXECUTED;
}

