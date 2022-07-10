#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <mutex>
#include <vector>
#include <queue>

using std::string;
using std::cout;
using std::endl;

class Message {
public:
        int data1;
};

class EventQueue {
private:
        std::queue<Message> mQ;
        std::condition_variable cv;
        std::condition_variable cvFull;
        std::mutex mu;
        const int mSizeQ = 5;

public:
        void push(Message &m) {
                std::unique_lock<std::mutex> lock(mu);
                if (mQ.empty()) {
                        mQ.push(std::move(m));
                        cv.notify_all();
                }
                else if (mQ.size() == mSizeQ) {
                        cvFull.wait(lock);
                        mQ.push(std::move(m));
                } else {
                        mQ.push(std::move(m));
                }
        }

        Message& pop() {
                std::unique_lock<std::mutex> lock(mu);
                if (mQ.empty()) {
                        cv.wait(lock);
                        Message &ret = mQ.front();
                        mQ.pop();
                        return ret;
                } else if (mQ.size() == mSizeQ) {
                        Message &ret = mQ.front();
                        mQ.pop();
                        cvFull.notify_all();
                        return ret;
                } else {
                        Message &ret = mQ.front();
                        mQ.pop();
                        return ret;
                }
        }
};

class MessageProducer {
public:
        void operator()(EventQueue &ev) {
                static int i = 0;
                while(1) {
                        Message m1;
                        m1.data1 = i++;
                        if (i == 100)
                                i = 0;
                        ev.push(std::ref(m1));
                        int tim = (int)std::rand() % 4;
                        std::this_thread::sleep_for(std::chrono::milliseconds(100 * tim + 1));
                        cout << "Pushing "<< tim << endl;
                }
        }
};

class MessageConsumer {
public:
        void operator()(EventQueue &ev) {
                while (1) {
                        Message &m = ev.pop();
                        cout << m.data1 << endl;
                        int tim = (int)std::rand() % 4;
                        std::this_thread::sleep_for(std::chrono::milliseconds(100 * tim + 1));
                }
        }
};

int main()
{
        EventQueue ev;
        MessageProducer mp;
        MessageConsumer mc;
        std::thread t1(mp, std::ref(ev));
        std::thread t2(mc, std::ref(ev));

        t1.join();
        t2.join();
        return 0;
}
