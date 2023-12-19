#ifndef ALGO_STREAMING_SERVICE_HPP
#define ALGO_STREAMING_SERVICE_HPP

#include <map>
#include <random>
#include <string>
#include "SOA.hpp"
#include "Products.hpp"
#include "PricingService.hpp"
#include "StreamingService.hpp"

template <class V>
class AlgoStreamingService : public Service<string, PriceStream<V>>
{
private:
    map<string, PriceStream<V>> pricestreams;
    random_device rd; // Random device for generating random numbers
    default_random_engine generator{ rd() }; // Random number generator

public:
    // Get data on our service given a key
    PriceStream<V>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<V>& data);

    // Publish two-way prices
    void PublishPrice(Price<V>& data);
};

template <typename V>
PriceStream<V>& AlgoStreamingService<V>::GetData(string key)
{
    return pricestreams[key];
}

template <typename V>
void AlgoStreamingService<V>::OnMessage(PriceStream<V>& data)
{
    pricestreams[data.GetProduct().GetProductId()] = data;
}

template <typename V>
void AlgoStreamingService<V>::PublishPrice(Price<V>& data)
{
    V product = data.GetProduct();
    double bid_price = data.GetMid() - data.GetBidOfferSpread() / 2;
    double ask_price = data.GetMid() + data.GetBidOfferSpread() / 2;
    uniform_int_distribution<long> distribution(1000000, 1999999);
    long visible_size = distribution(generator); // Generating random visible size
    PriceStreamOrder bid_order(bid_price, visible_size, 2 * visible_size, BID);
    PriceStreamOrder ask_order(ask_price, visible_size, 2 * visible_size, OFFER);
    PriceStream<V> price_stream(data.GetProduct(), bid_order, ask_order);

    pricestreams[price_stream.GetProduct().GetProductId()] = price_stream;
    Service<string, PriceStream<V>>::Notify(price_stream);
}

#endif

