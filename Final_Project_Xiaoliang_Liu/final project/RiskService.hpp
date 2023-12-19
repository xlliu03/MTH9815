

#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include <string>
#include <vector>
#include "SOA.hpp"
#include "PositionService.hpp"
#include "DataGenerator.hpp"

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{
public:
    // ctor
    PV01() = default;
    PV01(const T& _product, double _pv01, double _quantity);

    // Get the product on this PV01 value
    const T& GetProduct() const;
    
    // Get the PV01 value
    double GetPV01() const;
    
    // Get the quantity that this risk value is associated with
    double GetQuantity() const;

    // Change the quantity
    void UpdateQuantity(double _quantity);

private:
    T product;
    double pv01;
    double quantity;
};


/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{
public:
    // ctor
    BucketedSector() = default;
    BucketedSector(const vector<T>& _products, string _name);

    // Get the products associated with this bucket
    const vector<T>& GetProducts() const;

    // Get the name of the bucket
    const string& GetName() const;

private:
    vector<T> products;
    string name;
};


/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string, PV01 <T> >
{
private:
    map<string, PV01 <T> > pv01s;

public:
    // ctor
    RiskService() = default;
    
    // Get data on our service given a key
    PV01 <T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PV01<T>& data);

    // Add a position that the service will risk
    void AddPosition(Position<T>& position);

    // Get the bucketed risk for the bucket sector
    const PV01<BucketedSector<T>>& GetBucketedRisk(const BucketedSector<T>& _sector) const;

};


template<typename T>
PV01<T>::PV01(const T& _product, double _pv01, double _quantity) :
    product(_product)
{
    pv01 = _pv01;
    quantity = _quantity;
}

template <typename T>
const T& PV01<T>::GetProduct() const
{
    return product;
}

template <typename T>
double PV01<T>::GetPV01() const
{
    return pv01;
}

template <typename T>
double PV01<T>::GetQuantity() const
{
    return quantity;
}

template <typename T>
void PV01<T>::UpdateQuantity(double _quantity)
{
    quantity += _quantity;
}


template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
    products(_products)
{
    name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
    return name;
}


template <typename T>
PV01<T>& RiskService<T>::GetData(string key)
{
    return pv01s.find(key)->second;
}

template <typename T>
void RiskService<T>::OnMessage(PV01<T>& data)
{
    pv01s[data.GetProduct().GetProductId()] = data;
}

template <typename T>
void RiskService<T>::AddPosition(Position<T>& position)
{
    T product = position.GetProduct();
    string product_id = product.GetProductId();
    double quantity = position.GetAggregatePosition();
    double pv = g_PV01s[product_id];

    PV01<T> new_pv01(product, pv, quantity);
    pv01s[product_id] = new_pv01;
    Service<string, PV01 <T> >::Notify(new_pv01);
}

template<typename T>
const PV01<BucketedSector<T>>& RiskService<T>::GetBucketedRisk(const BucketedSector<T>& sector) const
{
    BucketedSector<T> product = sector;
    double pv01 = 0;
    for (auto& p : sector.GetProducts())
    {
        string id = p.GetProductId();
        pv01 += (pv01s[id].GetPV01() * pv01s[id].GetQuantity());
    }
    return PV01<BucketedSector<T>>(product, pv01, 1);
}

#endif
