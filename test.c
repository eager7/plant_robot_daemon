#include "utils.h"
#include "mthread.h"

tsThread tProducer;
tsThread tConsumer;
tsQueue  tGoods;


static void *ProducerWork(void *ptr)
{
    tsThread *privThread = (tsThread*)ptr;
    int i = 0;
    for(i = 0; i < 5; i++)
    {
        char *producer = (char*)malloc(100);
        memset(producer, 0, 100);
        sprintf(producer, "%d-%d", i, i);
        mQueueEnqueue(&tGoods, producer);
        DBG_vPrintf(T_TRUE, "Producer Goods:%d\n", i);
        sleep(1);
    }
    mThreadFinish(privThread);
    return NULL;
}

static void *ConsumerWork(void *ptr)
{
    tsThread *privThread = (tsThread*)ptr;
    sleep(6);
    int i = 10;
    char *p = NULL;
    for(i = 10; i > 5; i--)
    {
        mQueueDequeue(&tGoods, (void**)&p);
        NOT_vPrintf(T_TRUE, "Consumer Goods:%s\n", p);
        free(p);
        sleep(1);
    }
    mThreadFinish(privThread);
    return NULL;
}


int main(int argc, char *argv[])
{
    DBG_vPrintf(T_TRUE, "my thread lib test\n");

    DBG_vPrintf(T_TRUE, "create queue\n");
    mQueueCreate(&tGoods, 10);//the length of queue is 10

    DBG_vPrintf(T_TRUE, "create two thread\n");
    mThreadStart(ProducerWork, &tProducer, E_THREAD_JOINABLE);
    mThreadStart(ConsumerWork, &tConsumer, E_THREAD_JOINABLE);
    sleep(20);

    mThreadStop(&tProducer);
    mThreadStop(&tConsumer);
    mQueueDestroy(&tGoods);
    
    return 0;
}
