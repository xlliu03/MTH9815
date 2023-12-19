

#ifndef LISTENERS_HPP
#define LISTENERS_HPP

#include "PositionService.hpp"
#include "RiskService.hpp"
#include "HistoricalDataService.hpp"
#include "GUIService.hpp"
#include "ExecutionService.hpp"
#include "AlgoExecutionService.hpp"
#include "AlgoStreamingService.hpp"
#include "InquiryService.hpp"

using namespace std;

// Listener to the position service
template<typename T>
class PositionServiceListener : public ServiceListener<Trade<T> >
{
private:
    PositionService<T>* service;
public:
    PositionServiceListener(PositionService<T>* _service) : service(_service) {}
    void ProcessAdd(Trade<T>& data)
    {
        service->AddTrade(data);
    }
    void ProcessRemove(Trade<T>& data) {}
    void ProcessUpdate(Trade<T>& data) {}
};


// Listener to the risk service
template<typename T>
class RiskServiceListener : public ServiceListener<Position<T> >
{
private:
    RiskService<T>* service;
public:
    RiskServiceListener(RiskService<T>* _service) : service(_service) {}
    void ProcessAdd(Position<T>& data)
    {
        service->AddPosition(data);
    }
    void ProcessRemove(Position<T>& data) {}
    void ProcessUpdate(Position<T>& data) {}
};


template <typename T>
class HistoricalPositionListener :public ServiceListener<Position<T>>
{
private:
    HistoricalPositionService<T>* service;

public:
    HistoricalPositionListener(HistoricalPositionService<T>* _service) :service(_service) {}
    void ProcessAdd(Position<T>& data)
    {
        string id = data.GetProduct().GetProductId();
        service->PersistData(id, data);
    }
    void ProcessRemove(Position<T>& data) {}
    void ProcessUpdate(Position<T>& data) {}
};


template <typename T>
class HistoricalRiskListener :public ServiceListener<PV01<T>>
{
private:
    HistoricalRiskService<T>* service;

public:
    HistoricalRiskListener(HistoricalRiskService<T>* _service) : service(_service) {}
    void ProcessAdd(PV01<T>& data)
    {
        string id = data.GetProduct().GetProductId();
        service->PersistData(id, data);
    }
    void ProcessRemove(PV01<T>& data) {}
    void ProcessUpdate(PV01<T>& data) {}
};


template<typename T>
class HistoricalStreamingListener :public ServiceListener<PriceStream<T> >
{
private:
    HistoricalStreamingService<T>* service;
public:
    HistoricalStreamingListener(HistoricalStreamingService<T>* _service) : service(_service) {}
    void ProcessAdd(PriceStream<T>& data)
    {
        string id = data.GetProduct().GetProductId();
        service->PersistData(id, data);
    }
    void ProcessRemove(PriceStream<T>& data) {}
    void ProcessUpdate(PriceStream<T>& data) {}
};


// Listener to the historical execution service
template<typename T>
class HistoricalExecutionListener :public ServiceListener<ExecutionOrder<T> >
{
private:
    HistoricalExecutionService<T>* service;
public:
    HistoricalExecutionListener(HistoricalExecutionService<T>* _service) : service(_service) {}
    void ProcessAdd(ExecutionOrder<T>& data)
    {
        string id = data.GetProduct().GetProductId();
        service->PersistData(id, data);
    }
    virtual void ProcessRemove(ExecutionOrder<T>& data) {}
    virtual void ProcessUpdate(ExecutionOrder<T>& data) {}
};


// Listener to the historical inquiry service
template<typename T>
class HistoricalInquiryListener :public ServiceListener<Inquiry<T> >
{
private:
    HistoricalInquiryService<T>* service;
public:
    HistoricalInquiryListener(HistoricalInquiryService<T>* _service) : service(_service) {}
    void ProcessAdd(Inquiry<T>& data)
    {
        string id = data.GetInquiryId();
        service->PersistData(id, data);
    }
    virtual void ProcessRemove(Inquiry<T>& data) {}
    virtual void ProcessUpdate(Inquiry<T>& data) {}
};


// Listener to the GUI service
template<typename T>
class GUIServiceListener : public ServiceListener<Price<T> >
{
private:
    GUIService<T>* service;
public:
    GUIServiceListener(GUIService<T>* _service) : service(_service) {}
    void ProcessAdd(Price<T>& data)
    {
        service->OnMessage(data);
    }
    void ProcessRemove(Price<T>& data) {}
    void ProcessUpdate(Price<T>& data) {}
};


// Listener to the streaming service
template<typename T>
class StreamingServiceListener :public ServiceListener<PriceStream <T> >
{
private:
    StreamingService<T>* service;
public:
    StreamingServiceListener(StreamingService<T>* _service) : service(_service) {}
    void ProcessAdd(PriceStream <T>& data)
    {
        service->PublishPrice(data);
    }
    void ProcessRemove(PriceStream <T>& data) {}
    void ProcessUpdate(PriceStream <T>& data) {}
};


// Listener to the execution service
template<typename T>
class ExecutionServiceListener :public ServiceListener<ExecutionOrder<T> >
{
private:
    ExecutionService<T>* service;
public:
    ExecutionServiceListener(ExecutionService<T>* _service) : service(_service) {}
    void ProcessAdd(ExecutionOrder<T>& data)
    {
        service->ExecuteOrder(data, CME);
    }
    void ProcessRemove(ExecutionOrder<T>& data) {}
    void ProcessUpdate(ExecutionOrder<T>& data) {}
};


// Listener to the algo streaming service
template <typename T>
class AlgoStreamingServiceListener :public ServiceListener<Price<T> >
{
private:
    AlgoStreamingService<T>* service;
public:
    AlgoStreamingServiceListener(AlgoStreamingService<T>* _service) : service(_service) {}
    void ProcessAdd(Price<T>& data)
    {
        service->PublishPrice(data);
    }
    void ProcessRemove(Price<T>& data) {}
    void ProcessUpdate(Price<T>& data) {}
};


// Listener to the algo execution service
template <typename T>
class AlgoExecutionServiceListener :public ServiceListener<OrderBook <T> >
{
private:
    AlgoExecutionService<T>* service;
public:
    AlgoExecutionServiceListener(AlgoExecutionService<T>* _service) : service(_service) {}
    void ProcessAdd(OrderBook<T>& data)
    {
        service->ExecuteOrder(data);
    }
    void ProcessRemove(OrderBook<T>& data) {}
    void ProcessUpdate(OrderBook<T>& data) {}
};


// Listener to the trade booking service
template<typename T>
class TradeBookingServiceListener :public ServiceListener<ExecutionOrder <T> >
{
private:
    TradeBookingService<T>* service;
    int counter;

public:
    TradeBookingServiceListener(TradeBookingService<T>* _service) : service(_service), counter(0) {}
    void ProcessAdd(ExecutionOrder <T>& data)
    {
        T product = data.GetProduct();
        string tradeid = data.GetOrderId();
        double price = data.GetPrice();
        string book;
        if (counter % 3 == 0)
            book = "TRSY1";
        else if (counter % 3 == 1)
            book = "TRSY2";
        else
            book = "TRSY3";

        double quantity = data.GetVisibleQuantity();
        PricingSide side = data.GetPricingSide();
        Side order_side;
        if (side == BID)
            order_side = BUY;
        else
            order_side = SELL;

        Trade<T> trade(product, tradeid, price, book, quantity, order_side);
        service->OnMessage(trade);
    }

    void ProcessRemove(ExecutionOrder <T>& data) {}
    void ProcessUpdate(ExecutionOrder <T>& data) {}
};


#endif 
