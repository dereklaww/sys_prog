#include "pe_market.h"

Market *inst_market() {

    Market *market;
    market = malloc(sizeof(Market));

    if (market == NULL) {
        perror("market - malloc failed");
        return NULL;
    }

    market->orderbook = malloc(sizeof(Order*) * INITIAL_CAPACITY);
    
    if (market->orderbook == NULL) {
        perror("market orderbook - malloc failed");
        return NULL;
    }
    market->order_count = 0;
    market->transaction_fee = 0;

    market->completed_trades = malloc(sizeof(Trade*) * INITIAL_CAPACITY);
    market->trade_count = 0;

    return market;
    
}

void delMarket (Market *market) {

    if (market != NULL) {
    
        for (int i = 0; i < market->order_count; i++) {
            free(market->orderbook[i]);
        }
        free(market->orderbook);

        for (int i = 0; i < market->trade_count; i++) {
            free(market->completed_trades[i]);
        }
        free(market->completed_trades);
    }
}

int marketAddOrder(Market *market, Order *order) {

    order->order_index = market->order_count;

    if (market->order_count >= INITIAL_CAPACITY) {
        
        void *temp = realloc(market->orderbook, ((market->order_count + 1) * sizeof(Order*)));

        if (temp == NULL) {
            perror("realloc failed");
            return 1;
        } else {
            market->orderbook = (Order **)temp;
        }
    
        market->orderbook[market->order_count] = order;
    } else {
        market->orderbook[market->order_count] = order;
    }

    market->order_count++;
    return 0;
}

int addTrade(Market *market, Trade *trade) {


    if (market->trade_count >= INITIAL_CAPACITY) {
        int index = market->trade_count;

        void *temp = realloc(market->completed_trades, (market->trade_count + 2) * (sizeof(Trade*)));

        if (temp == NULL) {
            perror("realloc failed");
            return 1;
        } else {
            market->completed_trades = (Trade **)temp;
        }
    
        market->completed_trades[index] = trade;
        market->trade_count++; 
    } else {
        market->completed_trades[market->trade_count] = trade;
        market->trade_count++;
    }
    return 0;
}


void cancelOrder(Market *market, Order *order) {

    if (order != NULL) {
        for (int i = 0; i < market->order_count; i++) {
            if ((market->orderbook[i]->trader->trader_id == order->trader->trader_id) && 
                (market->orderbook[i]->order_id == order->order_id)) {
                    closeOrder(order);
                }
        }
    } 
}

void orderSort(Order **order, size_t order_count) {

    for (int i = 0; i < order_count; i++) {
        for (int j = i; j < order_count; j++) {
            if (order[i]->unit_price > order[j]->unit_price) {
                Order* temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }
}

void orderSortReverse(Order **order, size_t order_count) {

    for (int i = 0; i < order_count; i++) {
        for (int j = i; j < order_count; j++) {
            if (order[i]->unit_price < order[j]->unit_price) {
                Order* temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }
}

void printOrderByProduct(Order **order_book, char* product, size_t order_count) {
    Order **matched_buy_orders;
    Order **matched_sell_orders;
    matched_buy_orders = calloc(order_count, sizeof(Order*));
    matched_sell_orders = calloc(order_count, sizeof(Order*));

    int matched_buy_count = 0;
    int matched_sell_count = 0;
    int buy_level = 0;
    int sell_level = 0;

    for (int i = 0; i < order_count; i++) {

        int new_price = 1;

        if (!strcmp(order_book[i]->item, product) && (order_book[i]->order_type == SELL)
        && !(order_book[i]->completed)) {
            matched_sell_orders[matched_sell_count] = order_book[i];

            for (int j = 0; j < matched_sell_count; j++) {
                if (matched_sell_orders[j]->unit_price == order_book[i]->unit_price){
                    new_price = 0;
                }
            }

            if (new_price) {
                sell_level++;
            }
            matched_sell_count++;
        }
    }

    for (int i = 0; i < order_count; i++) {

        int new_price = 1;

        if (!strcmp(order_book[i]->item, product) && (order_book[i]->order_type == BUY)
        && !(order_book[i]->completed)) {
            matched_buy_orders[matched_buy_count] = order_book[i];

            for (int j = 0; j < matched_buy_count; j++) {
                if (matched_buy_orders[j]->unit_price == order_book[i]->unit_price){
                    new_price = 0;
                }
            }

            if (new_price) {
                buy_level++;
            }
            matched_buy_count++;
        }
    }
    
    // [PEX]    Product: GPU; Buy levels: 3; Sell levels: 1

    printf("%s\tProduct: %s; Buy levels: %d; Sell levels: %d\n", LOG_PREFIX, product,
    buy_level, sell_level);


    if (matched_sell_count > 0) {
        orderSortReverse(matched_sell_orders, matched_sell_count);

        int matching_orders_count = 0;
        int matching_orders_total_quantity = 0;

        for (int i = 0; i < matched_sell_count - 1; i++) {
            if (matched_sell_orders[i] -> unit_price == matched_sell_orders[i + 1] -> unit_price) {
                matching_orders_count++;
                matching_orders_total_quantity = matching_orders_total_quantity + matched_sell_orders[i]-> quantity;
            } else {
                // [PEX]        BUY 21 @ $500 (1 order)
                
                if (matching_orders_count == 0 && matching_orders_total_quantity == 0) {
                    printf("%s\t\tSELL %d @ $%ld (1 order)\n", LOG_PREFIX, matched_sell_orders[i]->quantity,
                    matched_sell_orders[i] -> unit_price);
                } else {
                // [PEX]        BUY 60 @ $501 (2 orders)
                    matching_orders_total_quantity = matching_orders_total_quantity + matched_sell_orders[i]-> quantity;
                    matching_orders_count++;
                    printf("%s\t\tSELL %d @ $%ld (%d orders)\n", LOG_PREFIX, matching_orders_total_quantity,
                    matched_sell_orders[i] -> unit_price, matching_orders_count);
                    matching_orders_count = 0;
                    matching_orders_total_quantity = 0;
                }
            }
        }

        if (matching_orders_count == 0 && matching_orders_total_quantity == 0) {
            printf("%s\t\tSELL %d @ $%ld (1 order)\n", LOG_PREFIX, 
            matched_sell_orders[matched_sell_count - 1]->quantity,
            matched_sell_orders[matched_sell_count - 1] -> unit_price);
        } else {
        // [PEX]        BUY 60 @ $501 (2 orders)
            matching_orders_total_quantity = matching_orders_total_quantity + matched_sell_orders[matched_sell_count - 1]-> quantity;
            matching_orders_count++;
            printf("%s\t\tSELL %d @ $%ld (%d orders)\n", LOG_PREFIX, matching_orders_total_quantity,
            matched_sell_orders[matched_sell_count - 1] -> unit_price, matching_orders_count);
        }
    }

    if (matched_buy_count > 0) {
        orderSortReverse(matched_buy_orders, matched_buy_count);

        int matching_orders_count = 0;
        int matching_orders_total_quantity = 0;

        for (int i = 0; i < matched_buy_count - 1; i++) {
            if (matched_buy_orders[i] -> unit_price == matched_buy_orders[i + 1] -> unit_price) {
                matching_orders_count++;
                matching_orders_total_quantity = matching_orders_total_quantity + matched_buy_orders[i]-> quantity;
            } else {
                // [PEX]        BUY 21 @ $500 (1 order)
                
                if (matching_orders_count == 0 && matching_orders_total_quantity == 0) {
                    printf("%s\t\tBUY %d @ $%ld (1 order)\n", LOG_PREFIX, matched_buy_orders[i]->quantity,
                    matched_buy_orders[i] -> unit_price);
                } else {

                // [PEX]        BUY 60 @ $501 (2 orders)
                    matching_orders_total_quantity = matching_orders_total_quantity + matched_buy_orders[i]-> quantity;
                    matching_orders_count++;
                    printf("%s\t\tBUY %d @ $%ld (%d orders)\n", LOG_PREFIX, matching_orders_total_quantity,
                    matched_buy_orders[i] -> unit_price, matching_orders_count);
                    matching_orders_count = 0;
                    matching_orders_total_quantity = 0;
                }
            }
        }

        if (matching_orders_count == 0 && matching_orders_total_quantity == 0) {
            printf("%s\t\tBUY %d @ $%ld (1 order)\n", LOG_PREFIX, 
            matched_buy_orders[matched_buy_count - 1]->quantity,
            matched_buy_orders[matched_buy_count - 1] -> unit_price);
        } else {

        // [PEX]        BUY 60 @ $501 (2 orders)
            matching_orders_total_quantity = matching_orders_total_quantity + matched_buy_orders[matched_buy_count - 1]-> quantity;
            matching_orders_count++;

            printf("%s\t\tBUY %d @ $%ld (%d orders)\n", LOG_PREFIX, matching_orders_total_quantity,
            matched_buy_orders[matched_buy_count - 1] -> unit_price, 
            matching_orders_count);
        }
    }

    free(matched_buy_orders);
    free(matched_sell_orders);
}

Order *getBestBuyOrder(Order **order_book, char* product, size_t order_count, long int sellPrice) {

    Order **matched_orders;
    matched_orders = calloc(order_count, sizeof(Order*));

    int matched_count = 0;
    for (int i = 0; i < order_count; i++) {

        if (!strcmp(order_book[i]->item, product) && (order_book[i]->order_type == BUY)
        && !(order_book[i]->completed)) {
            matched_orders[matched_count] = order_book[i];
            matched_count++;
        }
    }

    if (matched_count > 0) {
        orderSortReverse(matched_orders, matched_count);
        int match_index = 0;

        while (match_index < matched_count) {
            if (matched_orders[match_index]->unit_price >= sellPrice) {

                for (int order_index = 0; order_index < order_count; order_index++) {
                    if ((order_book[order_index]->trader->trader_id == matched_orders[match_index]->trader->trader_id) &&
                        (order_book[order_index]->order_id == matched_orders[match_index]->order_id) && 
                        (order_book[order_index]->order_type == BUY) &&
                        !(order_book[order_index]->completed)) {
                            free(matched_orders);
                            return order_book[order_index];
                        }
                }
            }
            match_index++;
        }
    }

    free(matched_orders);
    return NULL;
}

Order *getBestSellOrder(Order **order_book, char* product, size_t order_count, long int buyPrice) {

    Order **matched_orders;
    matched_orders = calloc(order_count, sizeof(Order*));

    int matched_count = 0;
    for (int i = 0; i < order_count; i++) {
        if (!strcmp(order_book[i]->item, product) && (order_book[i]->order_type == SELL) 
        && (!order_book[i]->completed)) {
            matched_orders[matched_count] = order_book[i];
            matched_count++;
        }
    }

    if (matched_count > 0) {
        orderSort(matched_orders, matched_count);

        int match_index = 0;

        while (match_index < matched_count) {
            if (matched_orders[match_index]->unit_price <= buyPrice) {

                for (int order_index = 0; order_index < order_count; order_index++) {
                    if ((order_book[order_index]->trader->trader_id == matched_orders[match_index]->trader->trader_id) &&
                        (order_book[order_index]->order_id == matched_orders[match_index]->order_id) && 
                        (order_book[order_index]->order_type == SELL) &&
                        !(order_book[order_index]->completed)) {
                            free(matched_orders);
                            return order_book[order_index];
                        }
                }
            }
            match_index++;
        }
    }

    free(matched_orders);
    return NULL;
}


Trade *placeBuyOrder(Market *market, Order *buy_order) {

    if ((buy_order == NULL) || (buy_order->order_type == SELL) || (buy_order->completed)){
        return NULL;
    }

    Order* best_sell_order;
    Trade* trade;


    best_sell_order = getBestSellOrder(market->orderbook, buy_order->item, 
        market->order_count, buy_order->unit_price);

    if (best_sell_order == NULL) {
        return NULL;
    }

    if (buy_order->quantity <= 0 || best_sell_order->quantity <= 0) {
        return NULL;
    }

    int sellAmount = 0;

    if (buy_order->quantity > best_sell_order->quantity) {
        sellAmount = best_sell_order->quantity;
    } else {
        sellAmount = buy_order->quantity;
    }

    long int sellPrice = best_sell_order->unit_price;

    // process completed trade

    trade = malloc(sizeof(Trade));
    trade->transaction_quantity = sellAmount;
    trade->transaction_price = sellPrice;
    trade->incoming_order = buy_order;
    trade->order_in_book = best_sell_order;

    addTrade(market, trade);

    adjustAmount(best_sell_order, sellAmount);
    adjustAmount(buy_order, sellAmount);

    if (buy(buy_order->trader, buy_order->item, sellAmount, sellPrice)) {
        perror("buy failed");
        return NULL;
    }

    if (sell(best_sell_order->trader, best_sell_order->item, sellAmount, sellPrice)) {
        perror("sell failed");
        return NULL;
    }

    long long int transaction_tax = (long long int) roundl(sellAmount * sellPrice * 0.01);
    trade->tax = transaction_tax;

    if (buy_order->order_index > best_sell_order->order_index) {
        tax(buy_order->trader, transaction_tax, buy_order->item);
    } else if (buy_order->order_index < best_sell_order->order_index) {
        tax(best_sell_order->trader, transaction_tax, best_sell_order->item);
    }

    market->transaction_fee = market->transaction_fee + transaction_tax;

    if (best_sell_order->quantity <= 0) {
        closeOrder(best_sell_order);
    }

    if (buy_order->quantity <= 0) {
        closeOrder(buy_order);
    }


    return trade;
}

Trade *placeSellOrder(Market *market, Order *sell_order) {

    if ((sell_order == NULL) || (sell_order->order_type == BUY) || (sell_order->completed)){
        return NULL;
    }

    Order* best_buy_order;
    Trade* trade;

    best_buy_order = getBestBuyOrder(market->orderbook, sell_order->item, 
        market->order_count, sell_order->unit_price);

    if (best_buy_order == NULL) {
        return NULL;
    }

    if (sell_order->quantity <= 0 || best_buy_order->quantity <= 0) {
        return NULL;
    }

    int buyAmount = 0;

    if (sell_order->quantity > best_buy_order->quantity) {
        buyAmount = best_buy_order->quantity;
    } else {
        buyAmount = sell_order->quantity;
    }

    long int buyPrice = best_buy_order->unit_price;

    // process completed trade

    trade = malloc(sizeof(Trade));
    trade->transaction_quantity = buyAmount;
    trade->transaction_price = buyPrice;
    trade->incoming_order = sell_order;
    trade->order_in_book = best_buy_order;

    addTrade(market, trade);

    adjustAmount(best_buy_order, buyAmount);
    adjustAmount(sell_order, buyAmount);

    if (sell(sell_order->trader, sell_order->item, buyAmount, buyPrice)) {
        perror("buy failed");
        return NULL;
    }

    if (buy(best_buy_order->trader, best_buy_order->item, buyAmount, buyPrice)) {
        perror("sell failed");
        return NULL;
    }

    long long int transaction_tax = (long long int) roundl((long long int)buyAmount * (long long int)buyPrice * 0.01);

    if (sell_order->order_index > best_buy_order->order_index) {
        tax(sell_order->trader, transaction_tax, sell_order->item);
    } else if (sell_order->order_index < best_buy_order->order_index) {
        tax(best_buy_order->trader, transaction_tax, best_buy_order->item);
    }

    market->transaction_fee = market->transaction_fee + transaction_tax;
    trade->tax = transaction_tax;

    if (best_buy_order->quantity <= 0) {
        closeOrder(best_buy_order);
    }

    if (sell_order->quantity <= 0) {
        closeOrder(sell_order);
    }


    return trade;
}