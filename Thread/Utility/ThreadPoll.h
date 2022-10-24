//
// Created by 12858 on 2022/10/24.
//

#ifndef THREAD_THREADPOLL_H
#define THREAD_THREADPOLL_H

#include <cstdio>
#include <cstdlib>
#include <pthread.h>

namespace brook::utility
{
    // 任务
    typedef struct Task
    {
        void (*function)(void *arg);

        void *arg;
        struct Task *next;
    } Task;

    // 线程池
    typedef struct ThreadPoll
    {
        Task *queueFront;
        Task *queueRear;

        // 线程数量
        int num;

        // 线程号
        pthread_t *threadId;

        // 互斥锁
        pthread_mutex_t mutex;

        // 条件变量
        pthread_cond_t cond;

        // 关闭线程池标志位 0 表示不关闭 1 表示关闭
        int showdown;
    } ThreadPoll;

    void *worker(void *arg)
    {
        ThreadPoll *poll = (ThreadPoll *) arg;
        sleep(1);
        while (1) {
            pthread_mutex_lock(&poll->mutex);
            // 如果任务队列为空且线程池没有被关闭
            while (poll->queueFront == poll->queueRear && poll->showdown == 0) {
                pthread_cond_wait(&poll->cond, &poll->mutex);
            }

            // 如果线程池关闭
            if (poll->showdown == 1) {
                pthread_mutex_unlock(&poll->mutex);
                printf("ThreadPoll Closed Thread %llu exit... \n", pthread_self());
                pthread_exit((void *) 0);
            }

            // 从任务队列里面取出一个任务，并且执行
            Task task;
            Task *t = poll->queueFront->next;
            task.function = t->function;
            task.arg = t->arg;
            poll->queueFront->next = t->next;
            free(t);
            if (poll->queueFront->next == NULL) {
                poll->queueRear = poll->queueFront;
            }

            // 释放互斥锁
            pthread_mutex_unlock(&poll->mutex);
            // 执行任务
            // printf("thread %ld start working...\n", pthread_self());
            task.function(task.arg);
            // printf("thread %ld end working...\n", pthread_self());
        }

    }

    ThreadPoll *created_thread_poll(int num)
    {

        // 申请线程池结构体
        ThreadPoll *poll = (ThreadPoll *) malloc(sizeof(ThreadPoll));
        if (poll == NULL) {
            fprintf(stderr, "malloc ThreadPoll failed \n");
            return NULL;
        }

        // 初始化线程数量
        poll->num = num;

        // 初始化线程队列
        poll->queueFront = (Task *) malloc(sizeof(Task));
        if (poll->queueFront == NULL) {
            fprintf(stderr, "malloc Task failed \n");
            free(poll);
            return NULL;
        }
        poll->queueRear = poll->queueFront;
        poll->queueFront->next = NULL;


        // 初始化线程 id
        poll->threadId = (pthread_t *) malloc(sizeof(pthread_t) * num);
        if (poll->threadId == NULL) {
            fprintf(stderr, "malloc threadId failed \n");
            free(poll->queueFront);
            free(poll);
            return NULL;
        }

        // 初始化线程
        int i;
        for (i = 0; i < num; i++) {
            if (pthread_create(&poll->threadId[i], NULL, worker, poll) != 0) {
                fprintf(stderr, "pthread_create failed \n");
                free(poll->queueFront);
                free(poll->threadId);
                free(poll);
                return NULL;
            }
            // pthread_join(poll->threadId[i], NULL); // 不能使用 join 因为是阻塞的 主线程阻塞
            pthread_detach(poll->threadId[i]); //线程结束后自动释放资源
        }

        // 初始化互斥锁和条件变量
        pthread_mutex_init(&poll->mutex, NULL);
        pthread_cond_init(&poll->cond, NULL);

        // 初始化关闭线程池的标志位
        poll->showdown = 0;
    }

    void thread_poll_add(ThreadPoll *poll, void (*function)(void *), void *arg)
    {
        pthread_mutex_lock(&poll->mutex);
        // 将任务入队
        Task *t = (Task *) malloc(sizeof(Task));
        if (t == NULL) {
            fprintf(stderr, "malloc Task failed \n");
            return;
        }
        t->function = function;
        t->arg = arg;
        t->next = NULL;
        poll->queueRear->next = t;
        poll->queueRear = t;

        pthread_mutex_unlock(&poll->mutex);
        pthread_cond_signal(&poll->cond);
    }

    void thread_poll_destroy(ThreadPoll *poll)
    {
        poll->showdown = 1;

        // 唤醒所有线程
        int i;
        for (i = 0; i < poll->num; i++) {
            pthread_cond_signal(&poll->cond);
        }

        // 释放线程号
        if (poll->threadId) {
            free(poll->threadId);
        }

        // 释放任务队列
        while (poll->queueFront->next) {
            Task *t = poll->queueFront->next;
            poll->queueFront->next = t->next;
            free(t);
        }
        free(poll->queueFront);

        // 销毁互斥量和条件变量
        pthread_mutex_destroy(&poll->mutex);
        pthread_cond_destroy(&poll->cond);

        // 释放线程池结构体
        free(poll);
    }
}
#endif // THREAD_THREADPOLL_H

