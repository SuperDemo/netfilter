


//危险事件存储数据结构，模拟set
typedef struct EventNode {
    char *data; // data of event;
    struct EventNode *next; //pointer to the next event;
} eventNode;


//定义威胁事件的集合--链表
eventNode *events = NULL;

