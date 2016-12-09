#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>

#include <linux/string.h>
#include <linux/slab.h>

#include "alarmDetection.h"


//存储节点代表的事件名称
char* buffer[NODES_NUM];

//存储每条轨迹的节点数目
int traceNumbers[TR_NUM+1] = {0, TR1_NODES, TR2_NODES, TR3_NODES};

//存储轨迹的邻接表表
struct form trace[TR_NUM+1];

//初始化轨迹结点信息
void init_buffer(){
	
	//轨迹TR1------DCS COLLECTION
	buffer[0]="PollingData";
	buffer[1]="MDataUpdate";
	buffer[2]="DataUpdate";
	
	//轨迹TR2------DCS ALARM
	buffer[3]="AlarmUpdate";
	buffer[4]="ProblemUpdate";
	buffer[5]="ProblemAck";

	//轨迹TR3------DCS CONTROL
	buffer[6]="MdControl";
	buffer[7]="MdReady";
	buffer[8]="MdAction";
}

//初始化轨迹邻接表 
void init_traces(){
	int i=0;
	int temp = 0;
	int nodeSum = 0;
	node* newNode = NULL, *nextNode = NULL;
	for(i=0; i<= TR_NUM; ++i){
		trace[i].traceId = i;
		
		if(i){
			temp = traceNumbers[i];
			while(temp--){
				newNode = (node* )kzalloc(sizeof(node), GFP_KERNEL);
				newNode->traceId = i;
				newNode->flag = 0;	//暂时没有用到				
				newNode->data = buffer[nodeSum++];	

				if(!nextNode){
					trace[i].next = newNode;
				}else{
					nextNode->next = newNode;
				}
				
				nextNode = newNode;
				nextNode->next = NULL;
			}

			nextNode = NULL;

		}else{
			trace[i].next = NULL;		
		}
	}
	
}


//导出本模块函数
EXPORT_SYMBOL(trace);
EXPORT_SYMBOL(buffer);

//内核模块加载时调用
static int __init init(void){
    //初始化操作：events事件数组以及轨迹的邻接表

    init_buffer();
    init_traces();

    printk(KERN_EMERG "成功加载alarmDetection模块！\n");

    return 0;
}

//内核模块卸载时调用
static void __exit fini(void){

    printk("成功移除alarmDetection模块！\n");
}

module_init(init);
module_exit(fini);

