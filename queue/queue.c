#include "queue.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

/**
	*  说明：初始化队列
	*	参数：
	*		queue_len：queue包含的节点数量大小
	*	返回值：成功则返回队列的指针，失败则返回NULL
	*/
Queue* Queue_Init(unsigned int queue_len)
{
	Queue* queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	if(queue == NULL)
	{
		return NULL;
	}
	queue->length = 0;
	queue->head = NULL;
	queue->tail = NULL;

    // init queue node
    QueueNode *queue_matrix = (QueueNode *)malloc(queue_len * sizeof(QueueNode));
    if (queue_matrix == NULL)
    {
        fprintf(stderr, "Error: malloc memory for Queue failed\n");
        free(queue);
        return NULL;
    }

    for(int i = 0; i < queue_len; i++)
    {
        queue_matrix[i].sequence = i;  //just for debug
        queue_matrix[i].data.time_ms = 0;
        queue_matrix[i].data.status = 0;
        queue_matrix[i].next = &queue_matrix[(i+1) % queue_len];
        queue_matrix[i].prev = &queue_matrix[(i-1+queue_len) % queue_len];
        if (i == 0)
        {
            queue->head = &queue_matrix[i];
        }
        else if ((i + 1) % queue_len == 0)
        {
            queue->tail = &queue_matrix[i];
        }
          
    }
    queue->curr = queue->head;
    printf("Queue init success\n");
	return queue;
}
 
/**
	*  说明：从队列的当前添加
	*	参数：
	*		queue：队列指针
	*		data：添加的数据域
	*	返回值：添加成功返回0，失败返回-1
	*/
int Queue_push(Queue *queue, Data *data)
{
    if (queue == NULL || data == NULL)
    {
        return -1;
    }

    queue->curr->data.time_ms = data->time_ms;
    queue->curr->data.status = data->status;

    queue->curr = queue->curr->next;
	return 0;
}
 
/**
	*  说明：从队列的当前后退删减
	*	参数：
	*		queue：队列指针
	*		data：返回删减的数据域
	*	返回值：添加成功返回0，失败返回-1
	*/
int Queue_pop(Queue *queue, Data *data) 
{
    if (queue == NULL || data == NULL)
    {
        return -1;
    }

    data->time_ms = queue->curr->data.time_ms;
    data->status = queue->curr->data.status;

    queue->curr = queue->curr->prev;
	return 0;
}

/**
	*  说明：释放queue空间
	*	参数：
	*		queue：队列指针
	*/
void Queue_Free(Queue* queue)
{
    free(queue->head);
	free(queue);
}
 

/**
	*  说明：打印Queue
	*	参数：
	*		queue：队列指针
	*/
void Queue_print(Queue* queue)
{
    if (!queue)
    {
        return;
    }

    QueueNode *stop = NULL;
    for(QueueNode *cur = queue->curr; cur != stop; cur = cur->next)
    {
        if (cur->data.time_ms == 0)
            continue;
        printf("Sequence %lld:  time %lldus status %d\n", cur->sequence, cur->data.time_ms, cur->data.status);
        if (stop == NULL)
        {
            stop = cur;
        }
    }
}

//测试
void Queue_Test()
{
    Queue *queue = NULL;
    Data data;
    struct timeval tv;

    queue = Queue_Init(40);
    for (int i = 0; i < 70; i++)
    {
        gettimeofday(&tv,NULL);
        data.time_ms = tv.tv_sec*1000000 + tv.tv_usec;  //微秒
        data.status = i % 255;

        Queue_push(queue, &data);
    }
    Queue_print(queue);
    Queue_Free(queue);
}

int main()
{
    Queue_Test();
    return 0;
}

