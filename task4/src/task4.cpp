#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include<condition_variable>
struct SensorPacket
{
    std::uint64_t id = 0;
    std::chrono::steady_clock::time_point send_time;
    std::vector<int> values;
};
class PacketQueue 
{
public:
    void push(const SensorPacket& packet) 
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            p.push_back(packet);
        }
        cv.notify_one();
    }
    bool pop(SensorPacket& packet) 
    {
    // 必须换成 unique_lock
        std::unique_lock<std::mutex> lock(mutex); 
        //如果没数据且没关闭，就在这里睡死，直到被叫醒
        cv.wait(lock, [this]() {return head < p.size() || closed;});
        if (head>=5) 
        {
            return false; 
        }
        packet=p[head];
        head++;
        return true;
    }
    void close() 
    {
        std::lock_guard<std::mutex> lock(mutex);
        closed = true;
    }
private:
    std::vector<SensorPacket> p;
    std::size_t head = 0;
    bool closed = false;
    std::mutex mutex;
    std::condition_variable cv;
};
PacketQueue g_queue;
void producer() 
{
    for (std::uint64_t id = 0; id < 20; ++id) 
    {
        SensorPacket packet;
        packet.id=id;
        for (int i = 0; i < 5; i++) 
        {
            int value = static_cast<int>((id * 17 + i * 23 + 7) % 101);
            packet.values.push_back(value);
        }
        // 先记录时间，再入队
        packet.send_time=std::chrono::steady_clock::now();
        g_queue.push(packet);
        if (id+1<20) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    g_queue.close();
}
void pv(const std::vector<int>& values) 
{
    std::cout << '[';
    for (std::size_t i=0; i <5; i++) 
    {
        if (i!=0) 
        {
            std::cout<<", ";
        }
        std::cout<<values[i];
    }
    std::cout<<']';
}

void consumer() 
{
    SensorPacket p;
    while (g_queue.pop(p)) 
    {
        int min=p.values[0];
        int max=p.values[0];
        int sum = 0;
        for (int i=0; i<5; i++) 
        {
            if (p.values[i]<min) 
            {
                min=p.values[i];
            }
            if (p.values[i]>max) 
            {
                max=p.values[i];
            }
            sum+=p.values[i];
        }
        double average=sum/5.0;
        auto ft= std::chrono::steady_clock::now();
        double hs=std::chrono::duration<double, std::milli>(ft- p.send_time).count();
        std::cout<<"[结果] id="<<p.id<<"，数据=";
        pv(p.values);
        std::cout<<"，最小值="<< min<<"，最大值="<<max<<std::fixed<< std::setprecision(2)<<"，平均值="<<average<< "，耗时=" <<hs<< " ms\n";
        continue;
    }
}
int main() 
{
    std::cout << "小洁的传感器监测程序已启动" << std::endl;
    // 先启动消费者，再启动生产者。
    std::thread consumerThread(consumer);
    std::thread producerThread(producer);
    // 等采集结束，再等处理线程把剩余数据处理完。
    producerThread.join();
    consumerThread.join();
    std::cout<< "采集与处理完成，共处理 "<<20<< " 组数据，程序退出" << std::endl;
    return 0;
}