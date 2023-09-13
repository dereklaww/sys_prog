#ifndef PE_MARKET
#define PE_MARKET

#include "pe_common.h"
#include "pe_mytrader.h"
#include "math.h"

typedef struct {

    Order *incoming_order;
    Order *order_in_book;
    int transaction_quantity;
    long int transaction_price;
    long long int tax;

} Trade;

typedef struct {

    Order **orderbook;
    Trade **completed_trades;
    int trade_count;
    int order_count;
    long long int transaction_fee;

} Market;


Market *inst_market();
void delMarket (Market *market);
void cancelOrder(Market *market, Order *order);
int marketAddOrder(Market *market, Order *order);
int addTrade(Market *market, Trade *trade);
Order *getBestBuyOrder(Order **order, char* product, size_t order_count, long int buyPrice);
Order *getBestSellOrder(Order **order, char* product, size_t order_count, long int buyPrice);
Trade *placeBuyOrder(Market *market, Order *buy_order);
Trade *placeSellOrder(Market *market, Order *sell_order);
void printOrderByProduct(Order **order_book, char* product, size_t order_count);

#endif