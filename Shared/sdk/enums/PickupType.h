/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/PickupType.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum class PickupType
{
    PICKUP_NONE,
    PICKUP_IN_SHOP,
    PICKUP_ON_STREET,
    PICKUP_ONCE,
    PICKUP_ONCE_TIMEOUT,
    PICKUP_ONCE_TIMEOUT_SLOW,
    PICKUP_COLLECTABLE1,
    PICKUP_IN_SHOP_OUT_OF_STOCK,
    PICKUP_MONEY,
    PICKUP_MINE_INACTIVE,
    PICKUP_MINE_ARMED,
    PICKUP_NAUTICAL_MINE_INACTIVE,
    PICKUP_NAUTICAL_MINE_ARMED,
    PICKUP_FLOATINGPACKAGE,
    PICKUP_FLOATINGPACKAGE_FLOATING,
    PICKUP_ON_STREET_SLOW,
    PICKUP_ASSET_REVENUE,
    PICKUP_PROPERTY_LOCKED,
    PICKUP_PROPERTY_FORSALE,
    PICKUP_MONEY_DOESNTDISAPPEAR,
    PICKUP_SNAPSHOT,
    PICKUP_2P,
    PICKUP_ONCE_FOR_MISSION
};
