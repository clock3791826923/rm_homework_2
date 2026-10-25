#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include<condition_variable>

// 一个数据包：编号、发送时间、5 个传感器读数。
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
            std::lock_guard<std::mutex> lock(mutex_);
            packets_.push_back(packet);
        }
        cv_.notify_one();
    }
    bool pop(SensorPacket& packet) 
    {
    // 必须换成 unique_lock
        std::unique_lock<std::mutex> lock(mutex_); 
    //  新增：如果没数据且没关闭，就在这里睡死，直到被叫醒
        cv_.wait(lock, [this]() {
        return head_ < packets_.size() || closed_;});
        if (head_ >= packets_.size()) {
        return false; // 没数据了，且被唤醒（说明生产者 close 了）
    }
    packet = packets_[head_];
    ++head_;
    return true;
}

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }

    bool isFinished() {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_ && head_ >= packets_.size();
    }

private:
    std::vector<SensorPacket> packets_;
    std::size_t head_ = 0;
    bool closed_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
};

PacketQueue g_queue;
std::size_t g_processedCount = 0;

// 采集线程：生成 20 组，每组 5 个数，范围 0~100。
void producer() {
    for (std::uint64_t id = 0; id < 20; ++id) {
        SensorPacket packet;
        packet.id = id;

        for (int i = 0; i < 5; ++i) {
            // 用简单公式生成数据，避免再引入随机数库。
            const int value = static_cast<int>((id * 17 + i * 23 + 7) % 101);
            packet.values.push_back(value);
        }

        // 先记录时间，再入队，所以耗时包含排队等待时间。
        packet.send_time = std::chrono::steady_clock::now();
        g_queue.push(packet);

        if (id + 1 < 20) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    g_queue.close();
}

// 打印一组数据，例如 [7, 30, 53, 76, 99]。
void pv(const std::vector<int>& values) {
    std::cout << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << values[i];
    }
    std::cout << ']';
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
