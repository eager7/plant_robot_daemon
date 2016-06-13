/****************************************************************************
 *
 * MODULE:             thread lib interface
 *
 * COMPONENT:          mthreads.h
 *
 * REVISION:           $Revision: 52723 $
 *
 * DATED:              $Date: 2016-01-04 17:04:13 $
 *
 * AUTHOR:             panchangtao
 *
 ****************************************************************************
 *
 * Copyright panchangtao@gmail.com 2016. All rights reserved
 *
 ***************************************************************************/

#ifndef __MTHREADS_H__
#define __MTHREADS_H__

#include "utils.h"

typedef enum
{
    E_THREAD_OK,
    E_THREAD_ERROR_FAILED,
    E_THREAD_ERROR_TIMEOUT,
    E_THREAD_ERROR_NO_MEM,
} teThreadStatus;

typedef enum
{
    E_THREAD_JOINABLE,  /**< Thread is created so that it can be waited on and joined */
    E_THREAD_DETACHED,  /**< Thread is created detached so all resources are automatically free'd at exit. */
} teThreadDetachState;

typedef enum
{
    /*This type of mutex does not detect deadlock. 
      A thread attempting to relock this mutex without first unlocking it will deadlock. 
      Attempting to unlock a mutex locked by a different thread results in undefined behaviour. 
      Attempting to unlock an unlocked mutex results in undefined behaviour. */
    E_NUM_PTHREAD_MUTEX_NORMAL,    
    /*This type of mutex provides error checking. 
      A thread attempting to relock this mutex without first unlocking it will return with an error. 
      A thread attempting to unlock a mutex which another thread has locked will return with an error. 
      A thread attempting to unlock an unlocked mutex will return with an error. */
    E_NUM_PTHREAD_MUTEX_ERRORCHECK,
    /*A thread attempting to relock this mutex without first unlocking it will succeed in locking the mutex. 
      The relocking deadlock which can occur with mutexes of type PTHREAD_MUTEX_NORMAL cannot occur with this type of mutex. 
      Multiple locks of this mutex require the same number of unlocks to release the mutex before another thread can acquire the mutex. 
      A thread attempting to unlock a mutex which another thread has locked will return with an error. 
      A thread attempting to unlock an unlocked mutex will return with an error.*/
    E_NUM_PTHREAD_MUTEX_RECURSIVE,
    /*Attempting to recursively lock a mutex of this type results in undefined behaviour. 
      Attempting to unlock a mutex of this type which was not locked by the calling thread results in undefined behaviour. 
      Attempting to unlock a mutex of this type which is not locked results in undefined behaviour. 
      An implementation is allowed to map this mutex to one of the other mutex types. */
    E_NUM_PTHREAD_MUTEX_DEFAULT,
}teMutexType;

typedef enum
{
    /*The process-shared attribute is set to PTHREAD_PROCESS_SHARED to permit a read-write lock to be operated upon 
      by any thread that has access to the memory where the read-write lock is allocated, 
      even if the read-write lock is allocated in memory that is shared by multiple processes.*/
    E_NUM_PTHREAD_PROCESS_SHARED,
    /*If the process-shared attribute is PTHREAD_PROCESS_PRIVATE, 
      the read-write lock will only be operated upon by threads created within the same process as the thread that initialised the read-write lock; 
      if threads of differing processes attempt to operate on such a read-write lock, the behaviour is undefined. 
      The default value of the process-shared attribute is PTHREAD_PROCESS_PRIVATE. */
    E_NUM_PTHREAD_PROCESS_PRIVATE,
}teRwLockType;

typedef struct
{
    volatile enum
    {
        E_THREAD_STOPPED,
        E_THREAD_RUNNING,
        E_THREAD_STOPPING,
    } eState;
    teThreadDetachState eThreadDetachState;
    pthread_t pThread_Id;
    void *pvThreadData;
} tsThread;

typedef void *(*tprThreadFunction)(void *psThreadInfoVoid);

teThreadStatus mThreadStart(tprThreadFunction prThreadFunction, tsThread *psThreadInfo, teThreadDetachState eDetachState);
teThreadStatus mThreadStop(tsThread *psThreadInfo);
teThreadStatus mThreadFinish(tsThread *psThreadInfo);
teThreadStatus mThreadYield(void);

typedef enum
{
    E_LOCK_OK,
    E_LOCK_ERROR_FAILED,
    E_LOCK_ERROR_TIMEOUT,
    E_LOCK_ERROR_NO_MEM,
} teLockStatus;

teLockStatus mLockCreate(pthread_mutex_t *psLock, teMutexType eMutexType);
teLockStatus mLockDestroy(pthread_mutex_t *psLock);
teLockStatus mLockLock(pthread_mutex_t *psLock);
teLockStatus mMLockLockTimed(pthread_mutex_t *psLock, uint32 u32WaitTimeout);
teLockStatus mLockTryLock(pthread_mutex_t *psLock);
teLockStatus mLockUnlock(pthread_mutex_t *psLock);

teLockStatus mLockCreateRW(pthread_rwlock_t *rwlock, teRwLockType eRwLockType);
teLockStatus mLockDestroyRW(pthread_rwlock_t *rwlock);
teLockStatus mLockLockRead(pthread_rwlock_t *rwlock);
teLockStatus mLockLockWrite(pthread_rwlock_t *rwlock);
teLockStatus mLockUnlockRW(pthread_rwlock_t *rwlock);

typedef enum
{
    E_QUEUE_OK,
    E_QUEUE_ERROR_FAILED,
    E_QUEUE_ERROR_TIMEOUT,
    E_QUEUE_ERROR_NO_MEM,
} teQueueStatus;

typedef struct
{
    void **apvBuffer;
    uint32 u32Length;
    uint32 u32Front;
    uint32 u32Rear;

    pthread_mutex_t mutex;    
    pthread_cond_t cond_space_available;
    pthread_cond_t cond_data_available;
} tsQueue;

teQueueStatus mQueueCreate(tsQueue *psQueue, uint32 u32Length); 
teQueueStatus mQueueDestroy(tsQueue *psQueue);
teQueueStatus mQueueEnqueue(tsQueue *psQueue, void *pvData);
teQueueStatus mQueueDequeue(tsQueue *psQueue, void **ppvData);
teQueueStatus mQueueDequeueTimed(tsQueue *psQueue, uint32 u32WaitTimeMil, void **ppvData);


/** Atomically add a 32 bit value to another.
 *  \param pu32Value        Pointer to value to update
 *  \param u32Operand       Value to add
 *  \return New value
 */
uint32 u32AtomicAdd(volatile uint32 *pu32Value, uint32 u32Operand);

/** Atomically get the value of a 32 bit value */
#define u32AtomicGet(pu32Value) u32AtomicAdd(pu32Value, 0)


#endif /* __MTHREADS_H__ */


