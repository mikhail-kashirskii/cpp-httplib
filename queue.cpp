
#include <iostream>

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

    template <typename T>
    class SharedQueue
    {
    public:
        SharedQueue();
        ~SharedQueue();

        T& front();
        void pop_front();

        void push_back(const T& item);
        void push_back(T&& item);

        int size();

    private:
        std::deque<T> queue_;
        std::mutex mutex_;
        std::condition_variable cond_;
    }; 

    template <typename T>
    SharedQueue<T>::SharedQueue(){}

    template <typename T>
    SharedQueue<T>::~SharedQueue(){}

    template <typename T>
    T& SharedQueue<T>::front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        return queue_.front();
    }

    template <typename T>
    void SharedQueue<T>::pop_front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        queue_.pop_front();
    }     

    template <typename T>
    void SharedQueue<T>::push_back(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(item);
        mlock.unlock();     // unlock before notificiation to minimize mutex con
        cond_.notify_one(); // notify one waiting thread

    }

    template <typename T>
    void SharedQueue<T>::push_back(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(std::move(item));
        mlock.unlock();     // unlock before notificiation to minimize mutex con
        cond_.notify_one(); // notify one waiting thread

    }

    template <typename T>
    int SharedQueue<T>::size()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        int size = queue_.size();
        mlock.unlock();
        return size;
    }



using namespace std;

//template<class T>
void Producer(SharedQueue<int>& in, SharedQueue<int>& out) {
  for (size_t i=0; i<10; i++)
    in.push_back(i);
  for (size_t i=0; i<10; i++) {
    cout<< out.front() << endl;
    out.pop_front();
  }
}

//template<class T>
void Worker(SharedQueue<int>& in, SharedQueue<int>& out) {
    while(true) {
      auto fr = in.front();
      in.pop_front();
      cout<< fr << endl;
      out.push_back(fr);
    }
}

int main()
{
    cout<<"started"<<endl;
    SharedQueue<int> in,out;
    std::thread writer(Producer, std::ref(in),std::ref(out));
    std::thread reader(Worker, std::ref(in),std::ref(out));

    reader.join();
    writer.join();
    
    return 0;
}
