

#include "DataGenerator.hpp"
#include "AlgoExecutionService.hpp"
#include "AlgoStreamingService.hpp"
#include "Connectors.hpp"
#include "ExecutionService.hpp"
#include "GUIService.hpp"
#include "HistoricalDataService.hpp"
#include "InquiryService.hpp"
#include "Listeners.hpp"
#include "MarketDataService.hpp"
#include "PositionService.hpp"
#include "PricingService.hpp"
#include "Products.hpp"
#include "RiskService.hpp"
#include "SOA.hpp"
#include "StreamingService.hpp"
#include "TradeBookingService.hpp"

using namespace std;

void Generate_Data()
{
    Generate_Trades("input/trades.txt");

    Generate_Prices("input/prices.txt");

    Generate_Mktdata("input/marketdata.txt");

    Generate_Inquiry("input/inquiries.txt");
}


int main()
{
    Generate_Data();       

    TradeBookingService<Bond> trade_booking_service;
    PositionService<Bond> position_service;
    PositionServiceListener<Bond> position_listener(&position_service);
    // The trade booking service should be linked to a position service via listener
    trade_booking_service.AddListener(&position_listener);

    RiskService<Bond> risk_service;
    RiskServiceListener<Bond> risk_service_listener(&risk_service);
    // The position service should be linked to a risk service via listener
    position_service.AddListener(&risk_service_listener);

    HistoricalPositionService<Bond> historical_position_service;
    HistoricalPositionListener<Bond> historical_position_listener(&historical_position_service);
    // Link the position service to the historical position listener
    position_service.AddListener(&historical_position_listener);

    HistoricalRiskService<Bond> historical_risk_service;
    HistoricalRiskListener<Bond> historical_risk_listener(&historical_risk_service);
    // Link the risk service to the historical risk listener
    risk_service.AddListener(&historical_risk_listener);

    /*
    * Part (b). Process price data from input/prices.txt
    * Generate two output files: output/gui.txt and output/streaming.txt
    * 
    * prices.txt -> pricing service -> gui service -> gui.txt
    * prices.txt -> pricing service -> also streaming service -> streaming service -> historical streaming service 
       -> streaming.txt
    */

    GUIService<Bond> gui_service;
    GUIServiceListener<Bond> gui_listener(&gui_service);
    PricingService<Bond> pricing_service;
    // Link the pricing service to the gui listener
    pricing_service.AddListener(&gui_listener);

    AlgoStreamingService<Bond> algo_streaming_service;
    AlgoStreamingServiceListener<Bond> algo_streaming_listener(&algo_streaming_service);
    // Link the pricing service to the algo streaming listener
    pricing_service.AddListener(&algo_streaming_listener);

    StreamingService<Bond> streaming_service;
    StreamingServiceListener<Bond> streaming_listener(&streaming_service);
    // Link the algo streaming service to the streaming listener
    algo_streaming_service.AddListener(&streaming_listener);

    HistoricalStreamingService<Bond> historical_streaming_service;
    HistoricalStreamingListener<Bond> historical_streaming_listener(&historical_streaming_service);
    // Link the streaming service to the historcial streaming listener
    streaming_service.AddListener(&historical_streaming_listener);

    /*
    * Part (c). Process order book data from input/marketorder.txt
    * Generate one file: output/execution.txt
    * Update two files: output/positions.txt and outpout/risk.txt
    * 
    * marketdata.txt -> market data service -> algo execution service -> execution service -> historical execution
        service -> executions.txt
    * marketdata.txt -> market data service -> algo execution service -> execution service -> trade booking service
        -> same as part (a)
    */

    MarketDataService<Bond> market_data_service;
    AlgoExecutionService<Bond> algo_execution_service;
    AlgoExecutionServiceListener<Bond> algo_execution_listener(&algo_execution_service);
    // Link the market data service to the algo execution listener
    market_data_service.AddListener(&algo_execution_listener);

    ExecutionService<Bond> execution_service;
    ExecutionServiceListener<Bond> execution_listener(&execution_service);
    // Link the algo execution service to the execution listener
    algo_execution_service.AddListener(&execution_listener);

    TradeBookingServiceListener<Bond> trade_booking_listener(&trade_booking_service);
    // Link the execution service to the trade booking listener
    execution_service.AddListener(&trade_booking_listener);

    HistoricalExecutionService<Bond> historical_execution_service;
    HistoricalExecutionListener<Bond> historical_execution_listener(&historical_execution_service);
    // Link the execution service to the historical execution listener
    execution_service.AddListener(&historical_execution_listener);

    /*
    * Part (d). Process inquiry data from input/inquires.txt
    * Generate one file: output/all_inquiries.txt
    * 
    * inquiries.txt -> inquiry service -> historical inquiry service -> all_inquiries.txt
    */

    InquiryService<Bond> inquiry_service;
    HistoricalInquiryService<Bond> historical_inquiry_service;
    HistoricalInquiryListener<Bond> historical_inquiry_listner(&historical_inquiry_service);
    inquiry_service.AddListener(&historical_inquiry_listner);


    // Fire up the system
    TradeBookingConnector<Bond> trade_connector(&trade_booking_service);
    PricingConnector<Bond> pricing_connector(&pricing_service);
    MarketDataConnector<Bond> market_data_connector(&market_data_service);
    InquiryConnector<Bond> inquiry_connector(&inquiry_service);

    trade_connector.Subscribe("input/trades.txt");
    pricing_connector.Subscribe("input/prices.txt");
    market_data_connector.Subscribe("input/marketdata.txt");
    inquiry_connector.Subscribe("input/inquiries.txt");

    return 0;
}
