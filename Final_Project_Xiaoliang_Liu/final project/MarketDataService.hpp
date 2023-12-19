
#ifndef MARKETDATA_SERVICE_HPP
#define MARKETDATA_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "SOA.hpp"

using namespace std;
enum PricingSide { BID, OFFER };


class Order
{
public:
    // ctor for an order
    Order() = default;
    Order(double _price, long _quantity, PricingSide _side);

    // Get the price on the order
    double GetPrice() const;

    // Get the quantity on the order
    long GetQuantity() const;

    // Get the side on the order
    PricingSide GetSide() const;

private:
    double price;
    long quantity;
    PricingSide side;

};


/**
 * Class representing a bid and offer order
 */
class BidOffer
{
public:
    // ctor for bid/offer
    BidOffer() = default;
    BidOffer(const Order& _bidOrder, const Order& _offerOrder);

    // Get the bid order
    const Order& GetBidOrder() const;

    // Get the offer order
    const Order& GetOfferOrder() const;

private:
    Order bidOrder;
    Order offerOrder;
};


/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template<typename T>
class OrderBook
{
public:
    // ctor for the order book
    OrderBook() = default;
    OrderBook(const T& _product, const vector<Order>& _bidStack, const vector<Order>& _offerStack);

    // Get the product
    const T& GetProduct() const;

    // Get the bid stack
    const vector<Order>& GetBidStack() const;

    // Get the offer stack
    const vector<Order>& GetOfferStack() const;

private:
    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;
};


/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class MarketDataService : public Service<string, OrderBook <T> >
{
private:
    map<string, OrderBook<T>> orderbooks;

public:
    // ctor
    MarketDataService() = default;

    // Get data on our service given a key
    OrderBook<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(OrderBook<T>& data);

    // Get the best bid/offer order
    const BidOffer& GetBestBidOffer(string productId);

    // Aggregate the order book
    const OrderBook<T>& AggregateDepth(string productId);
   
};

Order::Order(double _price, long _quantity, PricingSide _side)
{
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const
{
    return price;
}

long Order::GetQuantity() const
{
    return quantity;
}

PricingSide Order::GetSide() const
{
    return side;
}


BidOffer::BidOffer(const Order& _bidOrder, const Order& _offerOrder) :
    bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
    return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
    return offerOrder;
}


template<typename T>
OrderBook<T>::OrderBook(const T& _product, const vector<Order>& _bidStack, const vector<Order>& _offerStack) :
    product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
    return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
    return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
    return offerStack;
}


template <typename T>
OrderBook<T>& MarketDataService<T>::GetData(string key)
{
    return orderbooks[key];
}

template <typename T>
void MarketDataService<T>::OnMessage(OrderBook<T>& data)
{
    orderbooks[data.GetProduct().GetProductId()] = data;
    Service<string, OrderBook<T> >::Notify(data);
}

template <typename T>
const BidOffer& MarketDataService<T>::GetBestBidOffer(string productId)
{
    OrderBook<T> orderbook = orderbooks[productId];
    vector<Order> bid_stack = orderbook.GetBidStack();
    vector<Order> offer_stack = orderbook.GetOfferStack();
    Order best_bid = bid_stack[0];
    Order best_offer = offer_stack[0];

    for (auto& e : bid_stack)
    {
        if (e.GetPrice() > best_bid.GetPrice())
            best_bid = e;
    }
    for (auto& e : offer_stack)
    {
        if (e.GetPrice() < best_offer.GetPrice())
            best_offer = e;
    }
    return BidOffer(best_bid, best_offer);
}

template <typename T>
const OrderBook<T>& MarketDataService<T>::AggregateDepth(string productId)
{
    T product = orderbooks[productId].GetProduct();
    vector<Order> bid_stack = orderbooks[productId].GetBidStack();
    vector<Order> offer_stack = orderbooks[productId].GetOfferStack();

    unordered_map<double, long> bid_map, offer_map;
    for (auto& e : bid_stack)
        bid_map[e.GetPrice()] += e.GetQuantity();
    for (auto& e : offer_stack)
        offer_map[e.GetPrice()] += e.GetQuantity();

    vector<Order> agg_bid_stack, agg_offer_stack;
    for (auto& e : bid_map)
    {
        Order bid(e.first, e.second, BID);
        agg_bid_stack.push_back(bid);
    }
    for (auto& e : offer_map)
    {
        Order offer(e.first, e.second, OFFER);
        agg_offer_stack.push_back(offer);
    }
    return OrderBook<T>(product, agg_bid_stack, agg_offer_stack);
}

#endif
