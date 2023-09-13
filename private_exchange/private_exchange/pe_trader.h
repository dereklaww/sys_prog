#ifndef PE_TRADER_H
#define PE_TRADER_H

#include "pe_common.h"

#endif

#define BUFFER_READ     40
#define BUFFER_WRITE    128
#define COMMAND_ARGC    6
#define MAX_TRADERS     999999
#define MAX_ORDER_ID    999999
#define MAX_QUANTITY    999999
#define MAX_UNIT_PRICE  999999

enum message_type {
    ORDER_RECEIVED = 0,
    INVALID_MESSAGE,
    VALID_MESSAGE,
    MARKET_OPEN
};

enum order_type {
    BUY = 0,
    SELL
};


struct Order {

    int order_type; // BUY - 0, SELL - 1
    char item[BUFFER_READ];
    int quantity;
    long int unit_price;
    int order_id;
};