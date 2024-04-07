#pragma once
#include <event.h>
#include <vector>
#include "log.h"
#include "workthread.h"
#include "chatdb.h"

class ThreadPool
{
    SINGLETON(ThreadPool);
public:
    void run(int num,ChatDB* db,int filePort);
    void put(int cfd);                          //放到子线程处理
    WorkerThread* get();
private:
    int m_index=0;
    int m_num=0;
    std::vector<WorkerThread*> m_pool;
};