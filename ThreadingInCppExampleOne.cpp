#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <mutex>
#include <vector>

using std::string;
using std::cout;
using std::endl;

class MyFunctor {
public:
        void operator()() {
                cout << "this is from thread functor" << endl;
        }
};

void thread_function()
{
        cout << "thread function\n";
}

void thread_function_reference_passing(string &s) {
        cout << "thread_function_reference_passing: sting is :" << s << endl;
        cout << "thread_function_reference_passing: sting after mod is :" << s << endl;
        s = "Black Dog";
}

//------------Sharing : Mutex -------------------//
class MutexExample {
public:
        std::mutex mu;
        void shared_function(string &msg, int i) {
                std::unique_lock<std::mutex> muLock (mu);
                cout << msg << " : " << i << endl;
        }

        void thread_func_one() {
                string s = "thread 1";
                for (int i = 0; i <= 100; i++) {
                        shared_function(std::ref(s), i);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
        }
        void thread_func_two() {
               string s = "thread 2";
               for (int i = -1; i >= -100; i--) {
                        shared_function(std::ref(s), i);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
               }
        }
        void mutex_example_driver() {
                std::thread t1(&MutexExample::thread_func_one,this);
                std::thread t2(&MutexExample::thread_func_two,this);
                t1.join();
                t2.join();
        }
};

// condition variable example
class MyCondVar {
public:
        std::vector<std::thread> threadArray;
        std::condition_variable cv;
        std::mutex mu;

        void print_thread_id() {
                std::unique_lock<std::mutex> mtx (mu);
                cv.wait(mtx);
                std::thread::id this_id = std::this_thread::get_id();
                cout << "Hi from thread id: " << this_id << endl;
        }

        void cv_driver() {
                for (int i = 0; i < 10 ; i++) {
                        std::thread t(&MyCondVar::print_thread_id, this);
                        threadArray.push_back(std::move(t));
                }

                cout << "Now 10 threads will introduce" << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                cv.notify_all();
                for (auto &i : threadArray) {
                        i.join();
                }
        }
};



int main()
{
    // std::thread t1(&thread_function);   // t starts running

    // note  that this will not work
    // std::thread t(MyFunctor());
    // MyFunctor mft;
    // std::thread t2(mft);

    // string s = "Pikachu";
    // std::thread t3(&thread_function_reference_passing, std::ref(s));
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    // cout <<  "main: string after modification: " << s << endl;

    // cout << "main thread\n";
    // t1.join();
    // t2.join();
    // t3.join();

    // MutexExample ex;
    // ex.mutex_example_driver();

    MyCondVar mv;
    mv.cv_driver();

    return 0;
}
