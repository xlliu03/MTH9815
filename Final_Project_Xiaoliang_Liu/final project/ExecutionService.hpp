

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include <map>
#include "SOA.hpp"
#include "MarketDataService.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{
public:
    // ctor for an order
    ExecutionOrder() = default;
    ExecutionOrder(const T& _product, PricingSide _side, string _orderId, OrderType _orderType,
        double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId,
        bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the order ID
    const string& GetOrderId() const;

    // Get the order type on this order
    OrderType GetOrderType() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    double GetVisibleQuantity() const;

    // Get the hidden quantity
    double GetHiddenQuantity() const;

    // Get the parent order ID
    const string& GetParentOrderId() const;

    // Is child order?
    bool IsChildOrder() const;

    // get pricing side
    PricingSide GetPricingSide() const;
    
private:
    T product;
    PricingSide side;
    string orderId;
    OrderType orderType;
    double price;
    double visibleQuantity;
    double hiddenQuantity;
    string parentOrderId;
    bool isChildOrder;
};


/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class ExecutionService : public Service<string, ExecutionOrder <T> >
{
private:
    map<string, ExecutionOrder<T>> execution_orders;

public:
    // Get data on our service given a key
    ExecutionOrder<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(ExecutionOrder<T>& data);

    // Execute an order on a market
    void ExecuteOrder(ExecutionOrder<T>& order, Market market);
    
};


template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
    product(_product)
{
    side = _side;
    orderId = _orderId;
    orderType = _orderType;
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    parentOrderId = _parentOrderId;
    isChildOrder = _isChildOrder;
}


template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
    return product;
}
template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
    return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
    return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
    return price;
}

template<typename T>
double ExecutionOrder<T>::GetVisibleQuantity() const
{
    return visibleQuantity;
}

template<typename T>
double ExecutionOrder<T>::GetHiddenQuantity() const
{
    return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
    return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
    return isChildOrder;
}

template <typename T>
PricingSide ExecutionOrder<T>::GetPricingSide() const
{
    return side;
}


template <typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string key)
{
    return execution_orders[key];
}

template <typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T>& data)
{
    execution_orders[data.GetProduct().GetProductId()] = data;
}

template <typename T>
void ExecutionService<T>::ExecuteOrder(ExecutionOrder<T>& order, Market market)
{
    execution_orders[order.GetProduct().GetProductId()] = order;
    Service<string, ExecutionOrder <T> >::Notify(order);
}

#endif
