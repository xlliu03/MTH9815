
#ifndef ALGO_EXECUTION_SERVICE_HPP
#define ALGO_EXECUTION_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include "SOA.hpp"
#include "MarketDataService.hpp"
#include "ExecutionService.hpp"

using namespace std;

template <class T>
class AlgoExecutionService : public Service<string, ExecutionOrder<T>>
{
private:
    map<string, ExecutionOrder<T>> execution_orders;
    int counter;
    double spread_tol;

public:

    AlgoExecutionService();

    ExecutionOrder<T>& GetData(string key);

    void OnMessage(ExecutionOrder<T>& data);

    void ExecuteOrder(OrderBook<T>& data);
};

template <typename T>
AlgoExecutionService<T>::AlgoExecutionService() : counter(0), spread_tol(1.0 / 128)
{
    execution_orders = map<string, ExecutionOrder<T>>();
}

template <typename T>
ExecutionOrder<T>& AlgoExecutionService<T>::GetData(string key)
{
    return execution_orders[key];
}

template <typename T>
void AlgoExecutionService<T>::OnMessage(ExecutionOrder<T>& data)
{
    execution_orders[data.GetOrderId()] = data;
}

template <typename T>
void AlgoExecutionService<T>::ExecuteOrder(OrderBook<T>& data)
{
    T product = data.GetProduct();
    vector<Order> bid_stack = data.GetBidStack();
    vector<Order> offer_stack = data.GetOfferStack();
    Order best_bid = bid_stack.empty() ? Order() : bid_stack[0];
    Order best_offer = offer_stack.empty() ? Order() : offer_stack[0];

    for (const auto& e : bid_stack)
    {
        if (e.GetPrice() > best_bid.GetPrice())
            best_bid = e;
    }
    for (const auto& e : offer_stack)
    {
        if (e.GetPrice() < best_offer.GetPrice())
            best_offer = e;
    }

    double price, quantity;
    PricingSide side;
    if (!bid_stack.empty() && !offer_stack.empty() && (best_offer.GetPrice() - best_bid.GetPrice() > spread_tol))
    {
        if (counter % 2 == 0) // bid order
        {
            price = best_bid.GetPrice();
            quantity = best_bid.GetQuantity();
            side = BID;
        }
        else // offer order
        {
            price = best_offer.GetPrice();
            quantity = best_offer.GetQuantity();
            side = OFFER;
        }

        string tradeID = "TRADEID_" + to_string(counter);
        ExecutionOrder<T> execu_order(product, side, tradeID, MARKET, price, quantity, 2 * quantity, "", false);
        execution_orders[execu_order.GetOrderId()] = execu_order;
        ++counter;
        Service<string, ExecutionOrder<T>>::Notify(execu_order);
    }
}

#endif
