#ifndef _QUEUE_H
#define _QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif
 
//定义bool类型
#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
	
 
    typedef struct _Data
    {
        unsigned long long time_ms;
        char status;
    }Data;
 
	//队列节点
	typedef struct _QueueNode
    {
		Data data; //数据域
		struct _QueueNode* next;//指向下一个节点
		struct _QueueNode* prev;//指向上一个节点
        unsigned long long sequence; //sequence number, just for debug
	}QueueNode;
 
	//队列的结构体
	typedef struct _Queue
    {
		QueueNode *head;//队列的头部，固定值
		QueueNode *tail;//队列的尾部，固定值
		QueueNode *curr;//队列当前节点
		unsigned long length;//当前队列的长度
	}Queue;
 
	
 
    /**
     *  说明：初始化队列
     *	参数：
     *		queue_len：queue包含的节点数量大小
     *	返回值：成功则返回队列的指针，失败则返回NULL
     */
    Queue* Queue_Init(unsigned int queue_len);
 
    /**
     *  说明：从队列的当前添加
     *	参数：
     *		queue：队列指针
     *		data：添加的数据域
     *	返回值：添加成功返回0，失败返回-1
     */
    int Queue_push(Queue *queue, Data *data);
 
    /**
     *  说明：从队列的当前后退删减
     *	参数：
     *		queue：队列指针
     *		data：返回删减的数据域，调用者负责申请内存
     *	返回值：添加成功返回0，失败返回-1
     */
    int Queue_pop(Queue *queue, Data *data); 
 
	/**
	 *  说明：队列释放
	 *	参数：
	 *		queue：队列指针
	 */
	void Queue_Free(Queue* queue);
 
	//测试
	void Queue_Test();
 
#ifdef __cplusplus
}
#endif
#endif
