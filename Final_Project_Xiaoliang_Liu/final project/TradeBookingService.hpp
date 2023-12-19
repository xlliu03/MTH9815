

#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <map>
#include "SOA.hpp"

using namespace std;

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{
public:
    // ctor for a trade
    Trade() = default;
    Trade(const T& _product, string _tradeId, double _price, string _book, double _quantity, Side _side);

    // Get the product
    const T& GetProduct() const;

    // Get the trade ID
    const string& GetTradeId() const;

    // Get the mid price
    double GetPrice() const;

    // Get the book
    const string& GetBook() const;

    // Get the quantity
    double GetQuantity() const;

    // Get the side
    Side GetSide() const;

private:
    T product;
    string tradeId;
    double price;
    string book;
    double quantity;
    Side side;
};


/**
 * Trade Booking Service to book Trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string, Trade <T> >
{
private:
    map<string, Trade<T>> trades;

public:
    // default constructor
    TradeBookingService() = default;

    // Book the trade
    void BookTrade(Trade<T>& trade);

    // Get data on our service given a key
    Trade<T>& GetData(string key);
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Trade <T>& data);
};

template<typename T>
Trade<T>::Trade(const T& _product, string _tradeId, double _price, string _book, double _quantity, Side _side) :
    product(_product)
{
    tradeId = _tradeId;
    price = _price;
    book = _book;
    quantity = _quantity;
    side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
    return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
    return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
    return book;
}

template<typename T>
double Trade<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
    return side;
}

template<typename T>
void TradeBookingService<T>::BookTrade(Trade<T>& trade)
{
    Service<string, Trade<T> >::Notify(trade);
}

template <typename T>
Trade<T>& TradeBookingService<T>::GetData(string key)
{
    return trades[key];
}

template <typename T>
void TradeBookingService<T>::OnMessage(Trade <T>& data)
{
    trades[data.GetTradeId()] = data;
    Service<string, Trade<T> >::Notify(data);
}

#endif
