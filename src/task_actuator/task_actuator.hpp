/********************************************************
 * Description : task actuator (thread pool)
 * Reference   : github.com/progschj/WebrtcThreadPool.git
 * Modifier    : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2022 - 2023
 ********************************************************/

#ifndef TASK_ACTUATOR_HPP
#define TASK_ACTUATOR_HPP


#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class TaskActuator
{
public:
    TaskActuator(size_t thread_count)
        : m_running(true)
    {
        for (size_t index = 0; index < thread_count; ++index)
        {
            m_threads.emplace_back
            (
                [this]
                {
                    while (m_running)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> locker(m_task_mutex);

                            while (m_running && m_task_queue.empty())
                            {
                                m_task_condition.wait(locker);
                            }

                            if (!m_running)
                            {
                                break;
                            }

                            if (m_task_queue.empty())
                            {
                                continue;
                            }

                            task = std::move(m_task_queue.front());
                            m_task_queue.pop();
                        }

                        task();
                    }
                }
            );
        }
    }

    ~TaskActuator()
    {
        m_running = false;

        m_task_condition.notify_all();

        for (std::vector<std::thread>::iterator iter = m_threads.begin(); m_threads.end() != iter; ++iter)
        {
            std::thread & thread = *iter;
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

public:
    template<class Func, class ... Args>
    auto async_exec(Func && func, Args &&... args) -> std::future<typename std::result_of<Func(Args...)>::type>
    {
        using return_type = typename std::result_of<Func(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

        std::future<return_type> result = task->get_future();

        {
            std::lock_guard<std::mutex> locker(m_task_mutex);

            if (m_running)
            {
                m_task_queue.emplace([task]() { (*task)(); });
            }
        }

        m_task_condition.notify_one();

        return (result);
    }

private:
    volatile bool                       m_running;
    std::vector<std::thread>            m_threads;
    std::queue<std::function<void()>>   m_task_queue;
    std::mutex                          m_task_mutex;
    std::condition_variable             m_task_condition;
};


#endif // TASK_ACTUATOR_HPP
