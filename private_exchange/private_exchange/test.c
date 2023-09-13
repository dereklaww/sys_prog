#include "pe_common.h"

enum {
    ORDER_RECEIVED = 0,
    INVALID_MESSAGE,
    AMEND_REQUEST,
    CANCEL_REQUEST
};

int read_message(char* message, char** market_argv) {

    message[strcspn(message, ";")] = 0;

    // check NULL input
    if (!strcmp(message, "\0")) {
        printf("Null input\n");
        return INVALID_MESSAGE;  
    }

    // check leading and ending whitespace 
    if (message[0] == ' ' || message[strlen(message) - 1] == ' ') {
        printf("Invalid - leading/end\n");
        return INVALID_MESSAGE;
    }

    // check extra whitespace
    int i = 0;
    while (i < strlen(message) - 1) {

        if (message[i] == ' ' && message[i+1] == ' ') {
            printf("Invalid - consecutive whitespaces\n");
            return INVALID_MESSAGE;
        }
        i++;
    }

    // splitting message into arguments
    char *ptr = strtok(message, " ");

    int arg_count = 0;

    while (ptr != NULL) {
        strcpy(market_argv[arg_count++], ptr);
        ptr = strtok(NULL, " ");

        if (arg_count > 5) {
            return INVALID_MESSAGE;
        }
    }

    printf("in function - %s %s %s %s %s\n", market_argv[0], market_argv[1],
    market_argv[2], market_argv[3], market_argv[4]);

    if ((!strcmp(market_argv[0], "SELL")) || (!strcmp(market_argv[0], "BUY"))) {

        if (arg_count != 5) {
            printf("Order - wrong number of arguments: %d\n", arg_count);
            return INVALID_MESSAGE;
        }

        return ORDER_RECEIVED;
    
    } else if (!strcmp(market_argv[0], "AMEND")) {

        if (arg_count != 4) {
            printf("Amend - wrong number of arguments: %d\n", arg_count);
            return INVALID_MESSAGE;
        }

        return AMEND_REQUEST;
    } else if (!strcmp(market_argv[0], "CANCEL")) {

        if (arg_count != 2) {
            printf("Cancel - wrong number of arguments: %d\n", arg_count);
            return INVALID_MESSAGE;
        }

        return CANCEL_REQUEST;
    
    } else {
        return INVALID_MESSAGE; 
    }

}

typedef struct {

    int element_id;

} test_element;

typedef struct {
    
    test_element** elements;
    int element_count;

} test_array;

test_array* create_array() {

    test_array *array;
    array = malloc(sizeof(test_array));
    

    array->elements = malloc(sizeof(test_element*) * 10);
    array->element_count = 0;

    return array;
}

test_element* create_element(int id) {

    test_element* element;
    element = malloc(sizeof(test_element));

    element->element_id = id;

    return element;
}

void add_element(test_array* array, test_element* element) {

    array->elements[array->element_count] = element;

    printf("%d\n", array->elements[array->element_count]->element_id);

    array->element_count++;
}

int reverse(const void *a, const void *b) {

    const test_element *order_a = ( const test_element * ) a;
    const test_element *order_b = ( const test_element * ) b;

    if ((order_a -> element_id) < (order_b -> element_id)) {
        return 1;
    } else if ((order_a -> element_id) > (order_b -> element_id)) {
        return -1;
    } else {
        return 0;
    }
}

int main() {

    // char message[40];
    // strcpy(message, "BUY 0 GPU 8 800;");

    // char** market_argv = malloc(5 * sizeof(char*));

    // for (int i = 0; i < 5; i++) {
    //     market_argv[i] = malloc(10 * sizeof(char));
    // }

    // read_message(message, market_argv);

    // printf("out of function - %s %s %s %s %s\n", market_argv[0], market_argv[1],
    // market_argv[2], market_argv[3], market_argv[4]);

    // for (int i = 0; i < 5; i++) {
    //     free(market_argv[i]);
    // }
    // free(market_argv);

    // test_element* element0 = create_element(0);
    // test_element* element1 = create_element(1);
    // test_element* element2 = create_element(2);
    // test_element* element3 = create_element(3);

    // test_array* array = create_array();

    // add_element(array, element0);
    // add_element(array, element2);
    // add_element(array, element1);
    // add_element(array, element3);

    // test_element** array_to_sort;
    // array_to_sort = malloc(20 * sizeof(test_element*));

    // for (int i = 0; i < array->element_count; i++) {
    //     array_to_sort[i] = array->elements[i];
    // }

    // qsort(array_to_sort, 3, sizeof(test_element*), &reverse);

    // for (int i = 0; i < array->element_count; i++) {
    //     printf("%d\n", array_to_sort[i]->element_id);
    // }

    // char message[] ="BUY 0 Avocado 15 730 5";
    // int arg_count = 0;
    // char *p = strtok (message, " ");
    // char *array[5];

    // while (p != NULL)
    // {
    //     array[arg_count++] = p;
    //     p = strtok (NULL, " ");

    //     if (arg_count == 5) {
    //         break;
    //     }
    // }

    // for (int i = 0; i < 5; ++i) 
    //     printf("%s\n", array[i]);

    int product_count = 3;

    long *bank = malloc(product_count * sizeof(long));

    for (int i = 0; i < product_count; i++) {
        bank[i] = 0;
    }

    int amount = 40000;
    int quantity = 10000;


    bank[0] = 1;
    bank[1] = 2;
    bank[2] = (amount * quantity);

    printf("%ld\n", bank[2]);

}