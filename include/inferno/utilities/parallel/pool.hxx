
/*********************************************************
 *
 *  Copyright (C) 2014 by Vitaliy Vitsentiy
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *********************************************************/


#ifndef __ctpl_stl_thread_pool_H__
#define __ctpl_stl_thread_pool_H__

#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <exception>
#include <future>
#include <mutex>
#include <queue>



// thread pool to run user's functors with signature
//      ret func(int id, other_params)
// where id is the index of the thread that runs the functor
// ret is some return type

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>


namespace inferno{
namespace utilities{


class ThreadPool {
public:
    ThreadPool(size_t);
    //template<class F, class... Args>
    //auto enqueue(F&& f, Args&&... args) 
    //    -> std::future<typename std::result_of<F(Args...)>::type>;

    template<class F>
    void enqueue(F&& f) ;
    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void(int)> > tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;

    bool stop;
};
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [i,this]
            {
                for(;;)
                {
                    std::function<void(int)> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task(i);
                }
            }
        );
}

// add new work item to the pool
template<class F>
void ThreadPool::enqueue(F&& f) 
{
    //using return_type = typename std::result_of<F()>::type;
    
    //std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace(f);
    }
    condition.notify_one();
    //return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}


template<class ITER, class F>
void parallel_foreach_impl(
    ITER iter, 
    ITER end, 
    F && f,
    const uint64_t nItems,
    const uint64_t nThreads,
    std::random_access_iterator_tag
){
    typedef typename std::iterator_traits<ITER>::reference ReferenceType;
    utilities::ThreadPool pool(nThreads);
    uint64_t workload = std::distance(iter, end);
    const float workPerThread = float(workload)/nThreads;
    const uint64_t chunkedWorkPerThread = std::max(uint64_t(std::round(workPerThread/3.0f)),1ul);

    for( ;iter<end; iter+=chunkedWorkPerThread){

        ITER localEnd = iter+chunkedWorkPerThread < end ? iter + chunkedWorkPerThread : end;
        const size_t lc = std::min(workload, chunkedWorkPerThread);
        workload-=lc;
        pool.enqueue(
            [&f, iter, lc]
            (int id)
            {
                for(size_t i=0; i<lc; ++i)
                    f(id, iter[i]);
            }
        );
    }
}



template<class ITER, class F>
void parallel_foreach_impl(
    ITER iter, 
    ITER end, 
    F && f,
    const uint64_t nItems,
    const uint64_t nThreads,
    std::forward_iterator_tag
){

    typedef typename std::iterator_traits<ITER>::reference ReferenceType;
    utilities::ThreadPool pool(nThreads);
    uint64_t workload = nItems;
    const float workPerThread = float(workload)/nThreads;
    const uint64_t chunkedWorkPerThread = std::max(uint64_t(std::round(workPerThread/3.0f)),1ul);

    for(;;){

        const size_t lc = std::min(chunkedWorkPerThread, workload);
        workload -= lc;
        pool.enqueue(
            [&f, iter, lc]
            (int id)
            {
                auto iterCopy = iter;
                for(size_t i=0; i<lc; ++i){
                    f(id, *iterCopy);
                    ++iterCopy;
                }
            }
        );
        if(workload==0)
            break;
        std::advance(iter, lc);
    }

}

template<class ITER, class F>
void parallel_foreach_single_thread(
    ITER begin, 
    ITER end, 
    F && f,
    const int64_t nItems
){
    for(size_t i=0; i<nItems; ++i){
        f(0, *begin);
        ++begin;
    }
}


template<class ITER, class F>
void parallel_foreach(
    ITER begin, 
    ITER end, 
    F && f,
    const int64_t nItems,
    const uint64_t nThreads = 0
){
    const size_t nT = nThreads == 0 ? std::thread::hardware_concurrency() : nThreads;
    if(nT!=1){
        parallel_foreach_impl(begin, end, f, nItems,
            nThreads == 0 ? std::thread::hardware_concurrency() : nThreads,
            typename std::iterator_traits<ITER>::iterator_category());
    }
    else{
        parallel_foreach_single_thread(begin, end, f, nItems);
    }
}

}
}

#endif // __ctpl_stl_thread_pool_H__

