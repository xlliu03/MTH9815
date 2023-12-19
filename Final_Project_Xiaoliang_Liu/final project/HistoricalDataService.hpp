

#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include <string>
#include "SOA.hpp"
#include "Products.hpp"
#include "Connectors.hpp"
#include "PositionService.hpp"
#include "RiskService.hpp"
#include "StreamingService.hpp"
#include "InquiryService.hpp"
#include "ExecutionService.hpp"

template<typename T>
class HistoricalDataService : Service<string, T>
{
public:
    HistoricalDataService() = default;

    // Get data on our service given a key
    virtual T& GetData(string key)
    {
        T tmp;
        return tmp;
    }

    // The callback that a Connector should invoke for any new or updated data
    virtual void OnMessage(T& data) {}

    // Persist data to a store
    virtual void PersistData(string persistKey, T& data) {}
};


template <typename V>
class HistoricalPositionConnector;

template<typename V>
class HistoricalPositionService : HistoricalDataService<Position<V> >
{
private:
    HistoricalPositionConnector<V>* connector;
    
public:
    // ctor
    HistoricalPositionService() = default;
    HistoricalPositionService(HistoricalPositionConnector<V>* _connector): connector(_connector) {}

    // Persist data to a store
    void PersistData(string persistKey, Position<V>& data) 
    {
        connector->Publish(data);
    }
};


template <typename V>
class HistoricalRiskConnector;

template<typename V>
class HistoricalRiskService : HistoricalDataService<PV01<V> >
{
private:
    HistoricalRiskConnector<V>* connector;

public:
    // ctor
    HistoricalRiskService() = default;
    HistoricalRiskService(HistoricalRiskConnector<V>* _connector) : connector(_connector) {}

    // Persist data to a store
    void PersistData(string persistKey, PV01<V>& data) override
    {
        connector->Publish(data);
    }
};


template <typename V>
class HistoricalStreamingConnector;

template<typename V>
class HistoricalStreamingService : HistoricalDataService<PriceStream <V> >
{
private:
    HistoricalStreamingConnector<V>* connector;

public:
    // ctor
    HistoricalStreamingService() = default;
    HistoricalStreamingService(HistoricalStreamingConnector<V>* _connector) : connector(_connector) {}

    // Persist data to a store
    void PersistData(string persistKey, PriceStream<V>& data) override
    {
        connector->Publish(data);
    }
};


template <typename V>
class HistoricalExecutionConnector;

template<typename V>
class HistoricalExecutionService : HistoricalDataService<ExecutionOrder<V> >
{
private:
    HistoricalExecutionConnector<V>* connector;

public:
    // ctor
    HistoricalExecutionService() = default;
    HistoricalExecutionService(HistoricalExecutionConnector<V>* _connector) : connector(_connector) {}

    // Persist data to a store
    void PersistData(string persistKey, ExecutionOrder<V>& data) override
    {
        connector->Publish(data);
    }
};


template <typename V>
class HistoricalInquiryConnector;

template<typename V>
class HistoricalInquiryService : HistoricalDataService<Inquiry<V> >
{
private:
    HistoricalInquiryConnector<V>* connector;

public:
    // ctor
    HistoricalInquiryService() = default;
    HistoricalInquiryService(HistoricalInquiryConnector<V>* _connector) : connector(_connector) {}

    // Persist data to a store
    void PersistData(string persistKey, Inquiry<V>& data) override
    {
        connector->Publish(data);
    }
};

#endif
