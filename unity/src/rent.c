#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 100

typedef struct order {
    char *order_id;
    int start;
    int duration;
    int price;
    int revenue;
} ORDER;

typedef struct node {
    ORDER *order;
    struct node *next;
} NODE;

NODE *insert_into_orders(NODE *nodes, char *id, int start, int durn, int pr) {
    if(nodes == NULL) {
        ORDER *order = (ORDER *)malloc(sizeof(ORDER));
        order->order_id = strdup(id);
        order->start = start;
        order->duration = durn;
        order->price = pr;
        order->revenue = -1;
        NODE *node = (NODE *)malloc(sizeof(NODE));
        node->order = order;
        node->next = NULL;
        return node;
    }
    NODE *initial = nodes;
    NODE *pred = NULL;
    while(nodes && nodes->order->start < start) {
        pred = nodes;
        nodes = nodes->next;
    }
    while(nodes && nodes->order->start == start & nodes->order->duration < durn) {
        pred = nodes;
        nodes = nodes->next;
    }
    ORDER *order = (ORDER *)malloc(sizeof(ORDER));
    order->order_id = strdup(id);
    order->start = start;
    order->duration = durn;
    order->price = pr;
    order->revenue = -1;
    NODE *node = (NODE *)malloc(sizeof(NODE));
    node->order = order;
    node->next = nodes;
    if(pred == NULL)
        initial = node;
    else {
        pred->next = node;
    }
    return initial;
}

NODE *add_order_with_args(NODE *nodes, char *argv[], int first_arg) {
    int i = first_arg;
    char *id = argv[i++];
    int start = atoi(argv[i++]);
    int duration = atoi(argv[i++]);
    int price = atoi(argv[i]);
    return insert_into_orders(nodes, id, start, duration, price);
}

NODE *delete_order_with_id(NODE *nodes, char *argv[], int first_arg) {
    char *id = argv[first_arg]; 
    NODE *node = nodes;
    NODE *initial = nodes;
    NODE *prev = NULL;
    if(node == NULL) {
        fprintf(stderr, "file is empty\n");
        return NULL;
    }

    while(node && strcmp(node->order->order_id, id)) { 
        prev = node;
        node = node->next;
    }
    if(node != NULL) {
        if(prev == NULL) {
            initial = node->next;
        }
        else {
            prev->next = node->next;
        }
        free(node->order->order_id);
        free(node->order);
    } else {
        fprintf(stderr, "%s id not found\n", id);
    }
    return initial;

}
void print_nodes(NODE *node) {
    int count = 0;
    while(node) {
        count++;
        ORDER *o = node->order;
        printf("%6d %s %7d %5d %10d\n", count, o->order_id, o->start, o->duration, o->price);
        node = node->next;
    }
}
void destroy_nodes(NODE *node) {
    if(node == NULL)
        return;
    while(node) {
        free(node->order->order_id);
        free(node->order);
        NODE *to_free = node;
        node = node->next;
        free(to_free);
    }
}

NODE *selection(NODE *nodes, ORDER *order) {
    int start = order->start;
    int end   = start + order->duration;
    int year  = start / 1000;
    int year_end = year * 1000 + 365;
    if(end > year_end) {
        int days = end - year_end;
        end = (year + 1) * 1000 + days;
    }
    NODE *list = NULL;
    NODE *initial = NULL;
    NODE *prev = NULL;
    while(nodes != NULL) {
        if(nodes->order->start >= end) {
            list=(NODE *)malloc(sizeof(NODE));
            if(prev)
                prev->next = list;
            list->order = nodes->order;
            list->next = NULL;
            if(!initial)
                initial = list;
            prev = list;
        }
        nodes = nodes->next;
    }
    return initial;
}

void delete_list(NODE *nodes) {
    NODE *current = nodes;
    while(current != NULL) {
        NODE *to_free = current;
        current = current->next;
        free(to_free);
    }
}

int revenue(NODE *list) {
    if(list == NULL)
        return 0;
    ORDER *order = list->order;
    if(order->revenue != -1)
        return order->revenue;
    NODE *comp = selection(list->next, order);
    int rc = revenue(comp);
    int rn = revenue(list->next);
    order->revenue = order->price + rc > rn ? order->price + rc : rn;
    delete_list(comp);
    return order->revenue;
}

int main(int argc, char *argv[]) {
    static char buf[MAX_BUF];
    NODE *nodes = NULL;
    int command = 1;
    int first_arg;
    for(int i=1; i<argc; i++) {
        if(!strcmp(argv[i], "-l"))
            command = 0;
        else if(!strcmp(argv[i], "-r"))
            command = 1;
        else if(!strcmp(argv[i], "-a")) {
            if(argc < 5) {
                fprintf(stderr, "usage: rent -a ID START DURTN PRICE\n");
                exit(1);
            }
            command = 2;
            first_arg = i+1;
        }
        else if(!strcmp(argv[i], "-d")) {
            if(argc < 3) {
                fprintf(stderr, "usage: rent -d ID\n");
                exit(1);
            }
            command = 3;
            first_arg = i+1;
        }
    }
    char *filename = getenv("LAGS_ORDER_FILE");
    if (filename == NULL) {
        fprintf(stderr, "which file? set LAGS_ORDER_FILE var\n");
        exit(1);
    }
    FILE *file = fopen(filename, "r");
    int count = 0;
    while(fgets(buf, MAX_BUF, file)) {
        if(count > 0) {
            char order_id[50];
            int start;
            int durn;
            int prc;
            char *token = strtok(buf, ",");
            strcpy(order_id, token);
            token = strtok(NULL, ",");
            start = atoi(token);
            token = strtok(NULL, ",");
            durn = atoi(token);
            token = strtok(NULL, ",");
            prc = atoi(token);
            nodes = insert_into_orders(nodes, order_id, start, durn, prc);
        }
        count++;
    }
    fclose(file);
    if(command == 1)
        printf("%d\n", revenue(nodes));
    else if (command == 0) {
        print_nodes(nodes);
    } else if (command == 2) {
        nodes = add_order_with_args(nodes, argv, first_arg);
        file = fopen(filename, "w");
        fprintf(file, "Id,Start,Duration,Price\n");
        NODE *node = nodes;
        while(node) {
            ORDER *o = node->order;
            fprintf(file, "%s,%d,%d,%d\n", o->order_id, o->start, o->duration, o->price);
            node = node->next;
        }
        fclose(file);
    } else if (command == 3) {
        nodes = delete_order_with_id(nodes, argv, first_arg);
        file = fopen(filename, "w");
        fprintf(file, "Id,Start,Duration,Price\n");
        NODE *node = nodes;
        while(node) {
            ORDER *o = node->order;
            fprintf(file, "%s,%d,%d,%d\n", o->order_id, o->start, o->duration, o->price);
            node = node->next;
        }
        fclose(file);
    }
    destroy_nodes(nodes);
}

