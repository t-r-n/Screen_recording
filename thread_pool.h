#if 0
//用法
int main()
try {
    ilovers::TaskExecutor executor{ 10 };

    std::future<void> ff = executor.commit(f);
    std::future<int> fg = executor.commit(G{});
    std::future<std::string> fh = executor.commit([]()->std::string { std::cout << "hello, h !" << std::endl; return "hello,fh !"; });

    executor.shutdown();

    ff.get();
    std::cout << fg.get() << " " << fh.get() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    executor.restart();    // 重启任务
    executor.commit(f).get();    //

    std::cout << "end..." << std::endl;
    return 0;
}
catch (std::exception& e) {
    std::cout << "some unhappy happened... " << e.what() << std::endl;
}

#include <iostream>
#include <future>
#include <functional>
#include <queue>

std::queue<std::function<void()>> tasks;

template<class Function, class ... Args>
auto enqueue(Function&& f, Args&& ... args) -> std::future<decltype(f(args...))>
{
    std::function<decltype(f(args...))()> func = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    std::function<void()> wrapper = [task_ptr]() //wrapper to match types of 'tasks'... ugly
    {
        (*task_ptr)();
    };

    tasks.push(wrapper);

    return task_ptr->get_future();
}

void indep() {}

struct A {
  int get(int i) const { return i; }
};

int main()
{
    enqueue(indep);

    A a;
    //enqueue(&A::get, a, 2); //wrong

    static auto wrapper_wrong = &A::get;
    //enqueue(wrapper_wrong, a, 2); //wrong again

    static std::function<int(const A&,int)> wrapper = &A::get;
    enqueue(wrapper, a, 2); //ok

    static auto autoptr = std::mem_fn(&A::get);
    enqueue(autoptr, a, 2); //ok again
}

#endif

#pragma once
#ifndef ILOVERS_THREAD_POOL_H
#define ILOVERS_THREAD_POOL_H

#include <iostream>
#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <QImage>
// 命名空间
namespace ilovers {
    class TaskExecutor;
}

class ilovers::TaskExecutor {
    using Task = std::function<void()>;
private:
    // 线程池
    std::vector<std::thread> pool;
    // 任务队列
    std::queue<Task> tasks;
    // 同步
    std::mutex m_task;
    std::condition_variable cv_task;
    // 是否关闭提交
    std::atomic<bool> stop;
    std::atomic<int> taskcount;
    std::atomic<int> taskingCount;
public:
    // 构造

    TaskExecutor(size_t size = 4) : stop{ false }, taskingCount{0} {
        size = size < 1 ? 1 : size;
        taskcount.store(0);
        for (size_t i = 0; i < size; ++i) {
            pool.emplace_back(&TaskExecutor::schedual, this);    // push_back(std::thread{...})
        }
    }
    void joinall() {//
        while (true) {
            std::unique_lock<std::mutex>ll( m_task,std::defer_lock );
            if (ll.try_lock()) {
                if (tasks.empty() && !taskingCount.load())break;
            }
            else {
                std::this_thread::yield();
            }
        }
    }
    // 析构
    ~TaskExecutor() {
        for (std::thread& thread : pool) {
            thread.detach();    // 让线程“自生自灭”
            //thread.join();        // 等待任务结束， 前提：线程一定会执行完
        }
    }

    // 停止任务提交
    void shutdown() {
        this->stop.store(true);
    }

    // 重启任务提交
    void restart() {
        this->stop.store(false);
    }
    int taskCount(){
        return taskcount.load();
    }
    // 提交一个任务
    template<class F, class... Args>
    auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))> {//用了右值引用不拷贝传进来的参数，给传进来的参数续命
        if (stop.load()) {    // stop == true ??
            throw std::runtime_error("task executor have closed commit.");
        }
        using ResType = decltype(f(args...));    // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
        auto task = std::make_shared<std::packaged_task<ResType()>>(//还有一个作用把任何形式的函数都包装成了一个无返回值无参数的函数，便于统一返回std::function<void()>
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)//注意和bind的结合使用
            );    // wtf !
        {    // 添加任务到队列
            std::lock_guard<std::mutex> lock{ m_task };
            tasks.emplace([task]() {   // push(Task{...}),,还有一个作用把任何形式的函数都包装成了一个无返回值无参数的函数，便于统一返回std::function<void()>
                (*task)();//用bind把函数参数绑定后就不需要调用的时候传递参数了,用lambda表达式把这个函数改成了void()形式函数
                });
            ++taskcount;
        }
        cv_task.notify_all();    // 唤醒线程执行

        std::future<ResType> future = task->get_future();//异步的等待packaged_task包装过的task的返回值
        return future;
    }

//    template<typename xClass, typename xReturn, typename...xParam>//用这个就可以特化成员函数版本了
//    auto commit( xReturn(xClass::* pfn)(xParam...), xClass* pThis, xParam...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//注意和bind的结合使用
//        return commit(std::bind(pfn, pThis, lp...));
//    }
//    template<typename xClass, typename xReturn, typename...xParam>//用这个就可以特化成员函数版本了
//    auto commit( xReturn(xClass::*(&&pfn))(xParam&&...), xClass* (&&pThis), xParam&&...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//注意和bind的结合使用
//        return commit(std::bind(std::forward(pfn), std::forward(pThis), std::forward<xParam>(lp)...));

//    }
//    template<typename xClass, typename xReturn, typename...xParam>//用这个就可以特化成员函数版本了
//    auto commit( xReturn(xClass::*pfn)(xParam&&...), xClass* pThis, xParam&&...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//注意和bind的结合使用
//        return commit(std::bind(pfn, pThis, std::forward<xParam>(lp)...));
//    }
private:
    // 获取一个待执行的 task
    Task get_one_task() {
        std::unique_lock<std::mutex> lock{ m_task };
        cv_task.wait(lock, [this]() { return !tasks.empty(); });    // wait 直到有 task
        Task task{ std::move(tasks.front()) };    // 取一个 task
        tasks.pop();
        --taskcount;
        return task;
    }

    // 任务调度
    void schedual() {
        while (true) {
            //std::cout << __LINE__ << std::endl;
            if (Task task = get_one_task()) {
                taskingCount++;
                task();    //
                taskingCount--;
            }
            else {
                std::this_thread::yield();
                // return;    // done
            }
        }
    }
};

#endif
