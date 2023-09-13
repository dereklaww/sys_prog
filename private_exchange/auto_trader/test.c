#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "pe_exchange.h"


int main() {

    FILE *file = fopen("products.txt", "r");
    char buffer[BUFFER_READ];

    if (file == NULL) {

        perror("Error opening file\n");
        return -1;
    }

    strcpy(buffer, "");

    fgets(buffer, BUFFER_READ, file);   
    buffer[strcspn(buffer, "\n")] = 0;

    int numLines = atoi(buffer);

    char data[numLines][BUFFER_READ];

    for (int i = 0; i < numLines; i++) {
        fgets(buffer, BUFFER_READ, file);
        buffer[strcspn(buffer, "\n")] = 0;

        strcpy(data[i], buffer);
    }

    fclose(file);

    printf("%d\n", numLines);

    for (int i = 0; i < numLines; i++)
        printf("%s\n", data[i]);

}