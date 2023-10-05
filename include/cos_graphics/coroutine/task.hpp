#pragma once

#include <thread>
#include <functional>

template<typename RetType, typename... Args>
class CGTask
{
    using Func = RetType(Args...);
    std::function<Func> function;
    std::thread task_thread;
    RetType result;

private:
    void ThreadFunc(Args&&... args)
    {
        
        result = function(std::forward(args...));
    }

public:
    CGTask(std::function<Func> function) : function(function) {}

    inline void Run(Args&&... args)
    {
        task_thread = std::thread(ThreadFunc, std::forward(args...));
    }

    inline RetType Resume()
    {
        if (task_thread.joinable())
            task_thread.join();
        
        return result;
    }

    ~CGTask()
    {
        if (task_thread.joinable())
            task_thread.join();
    }

};