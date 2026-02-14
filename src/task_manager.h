#pragma once 
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>

#include "singleton.h"
#include "logger.h"

namespace kubvc::utility {
    class TaskManager : public Singleton<TaskManager> {
        public:
            TaskManager();
            ~TaskManager();
            // add new task 
            void add(std::function<void()>&& func);
            // get size of tasks queue        
            inline std::size_t size() const { return m_size; }
        private:
            void worker();
            
            std::vector<std::thread> m_threads;
            std::queue<std::function<void()>> m_tasks;
            std::mutex m_mutex;
            std::atomic_bool m_stopThreads;
            std::atomic<std::size_t> m_size;
            std::condition_variable m_conditionVariable;
    };

    inline TaskManager::TaskManager() {        
        const auto threadCount = std::thread::hardware_concurrency();
        for (std::int32_t i = 0; i < threadCount; ++i) {
            KUB_DEBUG("Add new thread {}", i);
            m_threads.push_back(std::thread(&TaskManager::worker, this));
        }
    }
    
    inline TaskManager::~TaskManager() {
        KUB_DEBUG("destroy task manager...");

        // stop all threads before we are clear threads list 
        m_size = 0;
        m_stopThreads = true;
        m_conditionVariable.notify_all();

        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }        
        
        m_threads.clear();
        m_threads.shrink_to_fit();
    }
    
    inline void TaskManager::add(std::function<void()>&& func) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(func));
        m_conditionVariable.notify_one();
        ++m_size;
    }

    inline void TaskManager::worker() {         
        while (true) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_conditionVariable.wait(lock, [this]() { 
                return !m_tasks.empty() || m_stopThreads;
            });

            // first we are checking need we stop thread               
            if (m_stopThreads) {
                KUB_DEBUG("thread is stopped");
                return;
            }            
            else if (m_tasks.empty()) { // else we are waiting for new task 
                continue;
            } 

            auto task = std::move(m_tasks.front());
            m_tasks.pop();                                            
            if (task != nullptr) {
                task();
                --m_size;
            }
        }
    }
}