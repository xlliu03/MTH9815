

#ifndef CONNECTORS_HPP
#define CONNECTORS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/date_time.hpp>

#include "SOA.hpp"
#include "DataGenerator.hpp"
#include "HistoricalDataService.hpp"
#include "StreamingService.hpp"
#include "PricingService.hpp"
#include "PositionService.hpp"
#include "RiskService.hpp"
#include "MarketDataService.hpp"
#include "ExecutionService.hpp"
#include "InquiryService.hpp"
#include "TradeBookingservice.hpp"

using namespace std;
using namespace boost::posix_time;

// Convert the fractional bond price to a numerical price
double FractionalToPrice(string frac_price)
{
    double price;
    string s1 = "";
    string s2 = "";
    string s3 = "";

    int cnt = 0;
    for (int i = 0; i < frac_price.size(); i++)
    {
        if (frac_price[i] == '-')
        {
            ++cnt;
            continue;
        }

        if (cnt == 0)
            s1.push_back(frac_price[i]);
        else if (cnt == 1)
        {
            s2.push_back(frac_price[i]);
            cnt++;
        }
        else if (cnt == 2)
        {
            s2.push_back(frac_price[i]);
            cnt++;
        }
        else
        {
            if (frac_price[i] == '+')
                frac_price[i] = '4';
            s3.push_back(frac_price[i]);
        }
    }

    price = stod(s1) + stod(s2) * 1.0 / 32.0 + stod(s3) * 1.0 / 256.0;
    return price;
}


// Connector the the historical position service
template <typename V>
class HistoricalPositionConnector :public Connector<Position<V>>
{
public:
    void Publish(Position<V> data)      // print the position into the file
    {
        ofstream out("output/positions.txt", ios::app);
        V product = data.GetProduct();
        out << product.GetProductId() << ", ";
        map<string, double> positions = data.GetAllPositions();
        for (auto& ele : positions)
            out << ele.first << ":" << ele.second << " ";
        out << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the historical risk service
template <typename V>
class HistoricalRiskConnector :public Connector<PV01<V>>
{
public:
    void Publish(PV01<V> data)       // print the risk into the file
    {
        ofstream out("output/risk.txt", ios::app);
        V product = data.GetProduct();
        out << product.GetProductId() << ", " << data.GetPV01() << ", "
            << data.GetQuantity() << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the streaming service
template<typename V>
class HistoricalStreamingConnector : public Connector<PriceStream<V> >
{
public:
    void Publish(PriceStream<V> data)      // print the price streams into the file
    {
        ofstream out("output/streaming.txt", ios::app);
        V product = data.GetProduct();
        PriceStreamOrder bid_order = data.GetBidOrder();
        PriceStreamOrder offer_order = data.GetOfferOrder();
        out << product.GetProductId() << ", " << bid_order.GetPrice() << ", " << 
            offer_order.GetPrice() << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the historical execution service
template<typename V>
class HistoricalExecutionConnector : public Connector<ExecutionOrder<V> >
{
public:
    void Publish(ExecutionOrder<V> data)        // print the execution records into the file
    {
        ofstream out("output/executions.txt", ios::app);
        V product = data.GetProduct();
        string side;
        if (data.GetPricingSide() == BID)
            side = "BUY";
        else
            side = "SELL";

        out << product.GetProductId() << "," << data.GetOrderId() << ","
            << side << "," << data.GetPrice() << "," << 
            data.GetVisibleQuantity() << "," << data.GetHiddenQuantity() << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the historical inquiries service
template<typename V>
class HistoricalInquiryConnector : public Connector<Inquiry<V> >
{
public:
    void Publish(Inquiry<V> data)       // print the inquiry data into the file
    {
        ofstream out("output/all_inquiries.txt", ios::app);
        string state;
        InquiryState state_enum = data.GetState();
        if (state_enum == RECEIVED)
            state = "RECEIVED";
        else if (state_enum == QUOTED)
            state = "QUOTED";
        else if (state_enum == DONE)
            state = "DONE";
        string side;
        auto side_enum = data.GetSide();
        if (side_enum == BUY)
            side = "BUY";
        else
            side = "SELL";
        
        out << data.GetProduct().GetProductId() << ", " << data.GetInquiryId()
            << ", " << side << ", " << data.GetPrice() << ", " << state << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the GUI service
template<typename V>
class GUIConnector : public Connector<Price<V> >
{
public:
    GUIConnector() = default;

    void Publish(Price<V>& data)
    {
        ofstream out("output/gui.txt", ios::app);
        ptime cur_time = microsec_clock::local_time();
        V product = data.GetProduct();
        out << cur_time << "  " << product.GetProductId() << ", " << data.GetMid() << ", "
            << data.GetBidOfferSpread() << endl;
        out.close();
    }

    void Subscribe(string file_name) {}     // publish only
};


// Connector to the trade booking service
template<typename V>
class TradeBookingConnector : public Connector<Trade<V>>
{
private:
    TradeBookingService<V>* service;

public:
    TradeBookingConnector(TradeBookingService<V>* _service) : service(_service) {}

    void Publish(Trade<V>& data) {}   // subsribe-only

    void Subscribe(string file_name)        // Read trading records from the given file
    {
        ifstream in(file_name);
        ptime cur_time;
        if (in.is_open()) 
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Processing trade data from " << file_name << "..." << endl;
            string line;
            vector<string> line_seg;
            while (getline(in, line))
            {
                string seg;
                line_seg.clear();
                stringstream line_stream(line);
                while (getline(line_stream, seg, ','))      // parse the comma-separated string
                {
                    line_seg.push_back(seg);
                }

                string productID = line_seg[0];
                V product = Bond(productID, CUSIP, g_tickers[productID], g_coupons[productID], g_dates[productID]);
                string tradeID = line_seg[1];
                string book = line_seg[2];
                double price = FractionalToPrice(line_seg[3]);      // get the decimal price
                
                long quantity = stol(line_seg[4]);
                Side side;
                if (line_seg[5] == "BUY")
                    side = BUY;
                else
                    side = SELL;
                
                Trade<V> trade(product, tradeID, price, book, quantity, side);
                service->OnMessage(trade);
            }
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Trade data processed.\n\n";
        }
        else
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  ERROR: File " << file_name << " can not be opened.\n\n";
        }
    }
};


// Connector to the pricing service
template<typename V>
class PricingConnector : public Connector<Price <V> >
{
private:
    PricingService<V>* service;

public:
    PricingConnector(PricingService<V>* _service) : service(_service) {}

    void Publish(Price <V>& data) {}        // subscribe only

    void Subscribe(string file_name)        // read price data from the given file
    {
        ifstream in(file_name);
        int counter = 0;
        ptime cur_time;
        if (in.is_open())
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Processing price data from " << file_name << "..." << endl;
            string line;
            while (getline(in, line))
            {
                ++counter;
                if (counter > 1000000)
                    counter = 1;
                string seg;
                vector<string> line_seg;
                stringstream line_stream(line);
                while (getline(line_stream, seg, ','))      // parse the comma-separated string
                {
                    line_seg.push_back(seg);
                }

                string productID = line_seg[0];
                if (counter % 100000 == 0)
                {
                    cur_time = microsec_clock::local_time();
                    cout << cur_time << "  " << counter << " prices processed for " << productID << ".\n";
                }
                V product = Bond(productID, CUSIP, g_tickers[productID], g_coupons[productID], g_dates[productID]);
                double bid = FractionalToPrice(line_seg[1]);
                double ask = FractionalToPrice(line_seg[2]);
                double mid = (bid + ask) / 2;
                double spread = ask - bid;

                Price<V> price(product, mid, spread);
                service->OnMessage(price);
            }
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Price data processed.\n\n";
        }
        else
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  ERROR: File " << file_name << " can not be opened.\n\n";
        }
    }
};


// Connector to the market data service
template<typename V>
class MarketDataConnector : public Connector<OrderBook <V> >
{
private:
    MarketDataService<V>* service;

public:
    MarketDataConnector(MarketDataService<V>* _service) : service(_service) {}
    
    void Publish(OrderBook <V>& data) {}      // subscribe only

    void Subscribe(string file_name)        // read market data from the given file
    {
        ifstream in(file_name);
        int counter = 0;
        ptime cur_time;
        if (in.is_open())
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Processing order book data from " << file_name << "..." << endl;
            string line;
            
            double bid_price, offer_price;
            while (getline(in, line))
            {
                ++counter;
                string seg;
                vector<string> line_seg;
                stringstream line_stream(line);
                while (getline(line_stream, seg, ','))      // parse the comma-separated string
                {
                    line_seg.push_back(seg);
                }
                
                string productID = line_seg[0];
                if (counter % 1000000 == 0)
                {
                    cur_time = microsec_clock::local_time();
                    cout << cur_time << "  " << "All order book data processed for " << productID << ".\n";
                }
                V product = Bond(productID, CUSIP, g_tickers[productID], g_coupons[productID], g_dates[productID]);
                vector<Order> bid_stack, offer_stack;
                for (int i = 0; i < 5; i++)
                {
                    bid_price = FractionalToPrice(line_seg[2 * i + 1]);
                    offer_price = FractionalToPrice(line_seg[2 * i + 2]);
                    bid_stack.push_back(Order(bid_price, 1000000 * (i + 1), BID));
                    offer_stack.push_back(Order(offer_price, 1000000 * (i + 1), OFFER));
                }
                
                OrderBook<V> order_book(product, bid_stack, offer_stack);
                service->OnMessage(order_book);
            }
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Order book data processed.\n\n";
        }
        else
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  ERROR: File " << file_name << " can not be opened.\n\n";
        }
    }
};


// Connector to the inquiry service
template<typename V>
class InquiryConnector : public Connector<Inquiry<V> >
{
private:
    InquiryService<V>* service;

public:
    InquiryConnector(InquiryService<V>* _service) : service(_service) {}

    void Publish(Inquiry<V>& data)
    {
        if (data.GetState() == RECEIVED)
        {
            Inquiry<V> new_inquiry = data;
            new_inquiry.SetState(QUOTED);
            service->OnMessage(new_inquiry);
        }
    }

    void Subscribe(string file_name)
    {
        ifstream in(file_name);
        ptime cur_time;
        if (in.is_open())
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Processing inquiry data from " << file_name << "..." << endl;
            string line;
            vector<string> line_seg;

            string inquiryID, productID;
            Side side;
            while (getline(in, line))
            {
                string seg;
                line_seg.clear();
                stringstream line_stream(line);
                while (getline(line_stream, seg, ','))      // parse the comma-separated string
                {
                    line_seg.push_back(seg);
                }

                productID = line_seg[0];
                inquiryID = line_seg[1];
                V product = Bond(productID, CUSIP, g_tickers[productID], g_coupons[productID], g_dates[productID]);
                double price = FractionalToPrice(line_seg[2]);
                long quantity = stol(line_seg[3]);
                if (line_seg[4] == "BUY")
                    side = BUY;
                else
                    side = SELL;

                Inquiry<V> inquiry(inquiryID, product, side, quantity, price, RECEIVED);
                service->OnMessage(inquiry);
            }
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  Inquiry data processed.\n\n";
        }
        else
        {
            cur_time = microsec_clock::local_time();
            cout << cur_time << "  ERROR: File " << file_name << " can not be opened.\n\n";
        }
    }
};


#endif 
