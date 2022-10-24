#include <iostream>
#include <unistd.h>
#include "Utility/ThreadPoll.h"

using namespace std;
using namespace brook::utility;


// 任务函数
void taskFunc(void *arg)
{
    int num = *(int *) arg;
    printf("thread %ld is working num = %d ...\n", pthread_self(), num);
    sleep(1);
    free(arg);
}

int main()
{
    ThreadPoll *poll = created_thread_poll(10);
    if (poll == NULL) {
        return -1;
    }
    printf("ThreadPoll created\n");

    int i;
    for (i = 0; i < 10; i++) {
        int *n = (int *) malloc(sizeof(int));
        *n = i;
        // 将任务添加到任务队列
        thread_poll_add(poll, taskFunc, n);
    }

    sleep(6);
    thread_poll_destroy(poll);
}