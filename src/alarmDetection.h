
//DEFINE TRUE OR FALSE
#define TRUE 1
#define FALSE 0

//NUMBER OF TRACES
#define TR_NUM 3
//NUMBER OF EVENTS
#define NODES_NUM 9

//NUMBER OF NODES IN TRACES OF DATA_COLLECTION_SYSTEM
#define TR1_NODES 3  //DAS_TR1_NODES
#define TR2_NODES 3  //DAS_TR2_NODES
#define TR3_NODES 3  //HMI_TR1_NODES 

//邻接表指针级数据结构
typedef struct node {
    unsigned traceId; //index of node in the trace;
    int flag; // if node is found in the trace ,flag=1; else, flag=0;
    char *data; // data of message;
    struct node *next; //pointer to the next event(vertex);
} node;

//邻接表数据结构
typedef struct form {
    unsigned traceId;//index of node in the trace;
    struct node *next;
} form;

void init_buffer(void);

void init_traces(void);
