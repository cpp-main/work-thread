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
#ifndef HEVAKE_WORK_THREAD_H
#define HEVAKE_WORK_THREAD_H

#include <functional>

namespace hevake {

/**
 * 工作线程
 *
 * 是 cpp-tbox 中 WorkThread 的精减版
 */
class WorkThread {
  public:
    using TaskId = uint32_t;
    using NonReturnFunc = std::function<void ()>;

  public:
    explicit WorkThread();
    virtual ~WorkThread();

  public:
    /// 使用worker线程执行某个函数
    /**
     * \param backend_task    让worker线程执行的函数对象
     * \return TaskId         任务Token
     */
    TaskId execute(const NonReturnFunc &backend_task);

    enum class TaskStatus {
        kWaiting,   //! 等待中
        kExecuting, //! 执行中
        kNotFound   //! 未找到（可能已完成）
    };

    /// 获取任务的状态
    TaskStatus getTaskStatus(TaskId task_id) const;

    /// 取消指定的任务
    /**
     * \param task_id   任务Token
     * \return  int     0: 成功
     *                  1: 没有找到该任务（已执行）
     *                  2: 该任务正在执行
     */
    int cancel(TaskId task_id);

    /// 取消所有未完成的任务，清理资源，并等待worker线程结束
    void cleanup();

  protected:
    void threadProc();

    bool shouldThreadExitWaiting() const;   //! 判定子线程是否需要退出条件变量的wait()函数

    struct Task;
    Task* popOneTask(); //! 取出一个优先级最高的任务

  private:
    struct Data;
    Data *d_ = nullptr;
};

}

#endif //HEVAKE_WORK_THREAD_H
