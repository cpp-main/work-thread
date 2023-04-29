/**
 * MIT License
 *
 * Copyright (c) 2018 Hevake Lee <hevake@126.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "work_thread.h"

#include <set>
#include <deque>
#include <thread>
#include <mutex>
#include <algorithm>
#include <condition_variable>

namespace hevake {

//! WorkThread 的私有数据
struct WorkThread::Data {
    std::mutex lock;                    //!< 互斥锁
    std::condition_variable cond_var;   //!< 条件变量

    std::thread work_thread;

    TaskId task_id_alloc_ = 0;

    std::deque<Task*> undo_tasks_deque; //!< 排队中的任务队列
    std::set<Task*>   doing_tasks;      //!< 正在处理的任务集合

    bool stop_flag = false; //!< 是否立即停止标记
};

/**
 * 任务项
 */
struct WorkThread::Task {
    TaskId id;
    NonReturnFunc backend_task;   //! 任务在工作线程中执行函数
};

/////////////////////////////////////////////////////////////////////////////////

WorkThread::WorkThread() :
    d_(new Data)
{
    d_->work_thread = std::thread(std::bind(&WorkThread::threadProc, this));
    d_->stop_flag = false;
}

WorkThread::~WorkThread()
{
    cleanup();
}

WorkThread::TaskId WorkThread::execute(const NonReturnFunc &backend_task)
{
    TaskId id = 0;

    Task *item = new Task;
    if (item == nullptr)
        return id;

    id = ++d_->task_id_alloc_;
    item->backend_task = backend_task;
    item->id = id;

    {
        std::lock_guard<std::mutex> lg(d_->lock);
        d_->undo_tasks_deque.push_back(item);
    }

    d_->cond_var.notify_one();

    return id;
}

WorkThread::TaskStatus WorkThread::getTaskStatus(TaskId id) const
{
    std::lock_guard<std::mutex> lg(d_->lock);

    auto undo_iter = std::find_if(d_->undo_tasks_deque.begin(), d_->undo_tasks_deque.end(),
                                  [id](Task *item) { return item->id == id; });
    if (undo_iter != d_->undo_tasks_deque.end())
        return TaskStatus::kWaiting;

    auto doing_iter = std::find_if(d_->doing_tasks.begin(), d_->doing_tasks.end(),
                                   [id](Task *item) { return item->id == id; });
    if (doing_iter != d_->doing_tasks.end())
        return TaskStatus::kExecuting;

    return TaskStatus::kNotFound;
}

/**
 * 返回值如下：
 * 0: 取消成功
 * 1: 没有找到该任务
 * 2: 该任务正在执行
 */
int WorkThread::cancel(TaskId id)
{
    std::lock_guard<std::mutex> lg(d_->lock);

    //! 如果正在执行
    auto undo_iter = std::find_if(d_->doing_tasks.begin(), d_->doing_tasks.end(),
                                  [id](Task *item) { return item->id == id; });
    if (undo_iter != d_->doing_tasks.end())
        return 2;

    auto doing_iter = std::find_if(d_->doing_tasks.begin(), d_->doing_tasks.end(),
                                   [id](Task *item) { return item->id == id; });
    if (doing_iter != d_->doing_tasks.end()) {
        d_->doing_tasks.erase(doing_iter);
        delete *doing_iter;
        return 0;
    }

    return 1;   //! 返回没有找到
}

void WorkThread::cleanup()
{
    if (d_ == nullptr)
        return;

    {
        std::lock_guard<std::mutex> lg(d_->lock);
        //! 清空task中的任务
        while (!d_->undo_tasks_deque.empty()) {
            auto task = d_->undo_tasks_deque.front();
            d_->undo_tasks_deque.pop_front();
            delete task;
        }
    }

    d_->stop_flag = true;
    d_->cond_var.notify_all();

    d_->work_thread.join();

    if (d_ != nullptr) {
      delete d_;
      d_ = nullptr;
    }
}

void WorkThread::threadProc()
{
    while (true) {
        Task* item = nullptr;
        {
            std::unique_lock<std::mutex> lk(d_->lock);

            //! 等待任务
            d_->cond_var.wait(lk, std::bind(&WorkThread::shouldThreadExitWaiting, this));

            /**
             * 有两种情况会从 cond_var.wait() 退出
             * 1. 任务队列中有任务需要执行时
             * 2. 析构时要求所有工作线程退出时
             *
             * 所以，下面检查 stop_flag 看是不是请求退出
             */
            if (d_->stop_flag)
                break;

            item = popOneTask();    //! 从任务队列中取出优先级最高的任务
        }

        //! 后面就是去执行任务，不需要再加锁了
        if (item != nullptr) {
            {
                std::lock_guard<std::mutex> lg(d_->lock);
                d_->doing_tasks.insert(item);
            }

            if (item->backend_task)
                item->backend_task();

            {
                std::lock_guard<std::mutex> lg(d_->lock);
                d_->doing_tasks.erase(item);
            }
            delete item;
        }
    }
}

bool WorkThread::shouldThreadExitWaiting() const
{
    return d_->stop_flag || !d_->undo_tasks_deque.empty();
}

WorkThread::Task* WorkThread::popOneTask()
{
    if (!d_->undo_tasks_deque.empty()) {
        Task* task = d_->undo_tasks_deque.front();
        d_->undo_tasks_deque.pop_front();
        return task;
    }
    return nullptr;
}

}
