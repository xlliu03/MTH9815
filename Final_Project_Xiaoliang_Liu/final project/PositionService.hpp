

#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "SOA.hpp"
#include "TradeBookingService.hpp"

using namespace std;

template<typename T>
class Position
{
public:
    // ctor
    Position() = default;
    Position(const T& _product);

    // Get the product
    const T& GetProduct() const;

    // Get the position quantity
    double GetPosition(string& book);

    // Return all positions
    map<string, double> GetAllPositions();

    // Get the aggregate position
    double GetAggregatePosition();

    // Update the position
    void UpdatePosition(string& book, double quantity, Side side);

private:
    T product;
    map<string, double> positions;
};



/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string, Position <T> >
{
private:
    map<string, Position<T> > positions;

public:
    // default constructor
    PositionService() = default;
    
    // Add a trading record to the service
    void AddTrade(const Trade<T>& trade);

    // Get data on our service given a key
    Position <T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Position<T>& data);
};


template <typename T>
Position<T>::Position(const T& _product): product(_product) {}

template<typename T>
const T& Position<T>::GetProduct() const
{
    return product;
}

template<typename T>
double Position<T>::GetPosition(string& book)
{
    return positions[book];
}

template <typename T>
map<string, double> Position<T>::GetAllPositions()
{
    return positions;
}

template <typename T>
double Position<T>::GetAggregatePosition()
{
    double aggregate_pos = 0;
    for (auto& p : positions)
    {
        aggregate_pos += p.second;
    }
    return aggregate_pos;
}

template <typename T>
void Position<T>::UpdatePosition(string& book, double quantity, Side side)
{
    if (side == BUY)
        positions[book] += quantity;
    else
        positions[book] -= quantity;
}


template <typename T>
Position<T>& PositionService<T>::GetData(string key)
{
    return positions[key];
}

template<typename T>
void PositionService<T>::OnMessage(Position<T>& data)
{
    positions[data.GetProduct().GetProductId()] = data;
}

template <typename T>
void PositionService<T>::AddTrade(const Trade<T>& trade)
{
    T product = trade.GetProduct();
    string product_id = product.GetProductId();
    string book = trade.GetBook();
    double quantity = trade.GetQuantity();
    Side side = trade.GetSide();

    if (positions.find(product_id) != positions.end())
    {
        positions[product_id].UpdatePosition(book, quantity, side);
    }
    else
    {
        Position<T> pos(product);
        pos.UpdatePosition(book, quantity, side);
        positions.insert(pair<string, Position<T>>(product_id, pos));
    }

    Service<string, Position <T> >::Notify(positions[product_id]);
}

#endif
