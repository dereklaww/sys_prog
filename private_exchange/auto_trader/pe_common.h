#ifndef PE_COMMON_H
#define PE_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_EXCHANGE "/tmp/pe_exchange_%d"
#define FIFO_TRADER "/tmp/pe_trader_%d"
#define FEE_PERCENTAGE 1

#define MAX_TRADERS     999999
#define MAX_ORDER_ID    999999
#define MAX_QUANTITY    999999
#define MAX_UNIT_PRICE  999999

#endif
