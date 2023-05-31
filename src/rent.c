#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_BUF 100

typedef struct order {
    char *order_id;
    int start;
    int duration;
    int price;
} ORDER;

typedef struct node {
    ORDER *order;
    struct node *next;
    int revenue;
} NODE;

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

int revenue(NODE *list) {
    if(list == NULL)
        return 0;
    ORDER *order = list->order;
    NODE *comp = selection(list->next, order);
    int rc = revenue(comp);
    int rn = revenue(list->next);
    list->revenue = order->price + rc > rn ? order->price + rc : rn;
    NODE *current = comp;
    while(current != NULL) {
        NODE *to_free = current;
        current = current->next;
        free(to_free);
    }
    return list->revenue;
}

int main(int argc, char *argv[]) {
    static char buf[MAX_BUF];
    NODE *nodes = NULL;
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
            if(nodes == NULL) {
                ORDER *order = (ORDER *)malloc(sizeof(ORDER));
                order->order_id = strdup(order_id);
                order->start = start;
                order->duration = durn;
                order->price = prc;
                NODE *node = (NODE *)malloc(sizeof(NODE));
                node->order = order;
                node->next = NULL;
                nodes = node;
            } else {
                NODE *current = nodes;
                NODE *pred = NULL;
                while(current && current->order->start < start) {
                    pred = current;
                    current = current->next;
                }
                while(current && current->order->start == start & current->order->duration < durn) {
                    pred = current;
                    current = current->next;
                }
                ORDER *order = (ORDER *)malloc(sizeof(ORDER));
                order->order_id = strdup(order_id);
                order->start = start;
                order->duration = durn;
                order->price = prc;
                NODE *node = (NODE *)malloc(sizeof(NODE));
                node->order = order;
                node->next = current;
                if(pred == NULL)
                    nodes = node;
                else {
                    pred->next = node;
                }
            }
        }
        count++;
    }
    fclose(file);
    NODE *node = nodes;
    int total = 0;
    while(node && node->next) {
            assert(node->order->start <= node->next->order->start);
        node = node->next;
        total++;
    }
    printf("%d %d\n", total+1, revenue(nodes));
    if(nodes == NULL)
        return 0;
    node = nodes;
    while(node) {
        free(node->order->order_id);
        free(node->order);
        NODE *to_free = node;
        node = node->next;
        free(to_free);
    }
}

