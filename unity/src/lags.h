
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

NODE *insert_into_orders(NODE *, char *, int, int, int);
int revenue(NODE *);
void destroy_nodes(NODE *); 
