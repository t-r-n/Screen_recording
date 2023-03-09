#if 0
//�÷�
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
    executor.restart();    // ��������
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
// �����ռ�
namespace ilovers {
    class TaskExecutor;
}

class ilovers::TaskExecutor {
    using Task = std::function<void()>;
private:
    // �̳߳�
    std::vector<std::thread> pool;
    // �������
    std::queue<Task> tasks;
    // ͬ��
    std::mutex m_task;
    std::condition_variable cv_task;
    // �Ƿ�ر��ύ
    std::atomic<bool> stop;
    std::atomic<int> taskcount;
    std::atomic<int> taskingCount;
public:
    // ����

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
    // ����
    ~TaskExecutor() {
        for (std::thread& thread : pool) {
            thread.detach();    // ���̡߳���������
            //thread.join();        // �ȴ���������� ǰ�᣺�߳�һ����ִ����
        }
    }

    // ֹͣ�����ύ
    void shutdown() {
        this->stop.store(true);
    }

    // ���������ύ
    void restart() {
        this->stop.store(false);
    }
    int taskCount(){
        return taskcount.load();
    }
    // �ύһ������
    template<class F, class... Args>
    auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))> {//������ֵ���ò������������Ĳ��������������Ĳ�������
        if (stop.load()) {    // stop == true ??
            throw std::runtime_error("task executor have closed commit.");
        }
        using ResType = decltype(f(args...));    // typename std::result_of<F(Args...)>::type, ���� f �ķ���ֵ����
        auto task = std::make_shared<std::packaged_task<ResType()>>(//����һ�����ð��κ���ʽ�ĺ�������װ����һ���޷���ֵ�޲����ĺ���������ͳһ����std::function<void()>
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)//ע���bind�Ľ��ʹ��
            );    // wtf !
        {    // ������񵽶���
            std::lock_guard<std::mutex> lock{ m_task };
            tasks.emplace([task]() {   // push(Task{...}),,����һ�����ð��κ���ʽ�ĺ�������װ����һ���޷���ֵ�޲����ĺ���������ͳһ����std::function<void()>
                (*task)();//��bind�Ѻ��������󶨺�Ͳ���Ҫ���õ�ʱ�򴫵ݲ�����,��lambda���ʽ����������ĳ���void()��ʽ����
                });
            ++taskcount;
        }
        cv_task.notify_all();    // �����߳�ִ��

        std::future<ResType> future = task->get_future();//�첽�ĵȴ�packaged_task��װ����task�ķ���ֵ
        return future;
    }

//    template<typename xClass, typename xReturn, typename...xParam>//������Ϳ����ػ���Ա�����汾��
//    auto commit( xReturn(xClass::* pfn)(xParam...), xClass* pThis, xParam...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//ע���bind�Ľ��ʹ��
//        return commit(std::bind(pfn, pThis, lp...));
//    }
//    template<typename xClass, typename xReturn, typename...xParam>//������Ϳ����ػ���Ա�����汾��
//    auto commit( xReturn(xClass::*(&&pfn))(xParam&&...), xClass* (&&pThis), xParam&&...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//ע���bind�Ľ��ʹ��
//        return commit(std::bind(std::forward(pfn), std::forward(pThis), std::forward<xParam>(lp)...));

//    }
//    template<typename xClass, typename xReturn, typename...xParam>//������Ϳ����ػ���Ա�����汾��
//    auto commit( xReturn(xClass::*pfn)(xParam&&...), xClass* pThis, xParam&&...lp) {
//        //std::bind(std::forward<F>(f), std::forward<Args>(args)...)//ע���bind�Ľ��ʹ��
//        return commit(std::bind(pfn, pThis, std::forward<xParam>(lp)...));
//    }
private:
    // ��ȡһ����ִ�е� task
    Task get_one_task() {
        std::unique_lock<std::mutex> lock{ m_task };
        cv_task.wait(lock, [this]() { return !tasks.empty(); });    // wait ֱ���� task
        Task task{ std::move(tasks.front()) };    // ȡһ�� task
        tasks.pop();
        --taskcount;
        return task;
    }

    // �������
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
