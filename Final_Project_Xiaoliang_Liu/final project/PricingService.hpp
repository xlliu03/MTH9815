
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include <map>
#include "SOA.hpp"

using namespace std;

/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{
public:
    // ctor for a price
    Price() = default;
    Price(const T& _product, double _mid, double _bidOfferSpread);

    // Get the product
    const T& GetProduct() const;

    // Get the mid price
    double GetMid() const;

    // Get the bid/offer spread around the mid
    double GetBidOfferSpread() const;

private:
    T product;
    double mid;
    double bidOfferSpread;
};


/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string, Price <T> >
{
private:
    map<string, Price<T>> prices;
    
public:
    // ctor
    PricingService() = default;

    // Get data on our service given a key
    Price<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<T>& data);
};


template<typename T>
Price<T>::Price(const T& _product, double _mid, double _bidOfferSpread) :
    product(_product)
{
    mid = _mid;
    bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
    return product;
}

template<typename T>
double Price<T>::GetMid() const
{
    return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
    return bidOfferSpread;
}


template <typename T>
Price<T>& PricingService<T>::GetData(string key)
{
    return prices[key];
}

template <typename T>
void PricingService<T>::OnMessage(Price<T>& data)
{
    prices[data.GetProduct().GetProductId()] = data;
    Service<string, Price<T> >::Notify(data);
}

#endif
