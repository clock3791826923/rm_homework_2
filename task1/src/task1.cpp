#include <yaml-cpp/yaml.h>
#include <filesystem> 
#include <iostream>
#include <optional>    
#include <string> 
#include <thread>// std::this_thread::sleep_for
#include <chrono>// std::chrono::milliseconds
struct config
{
    int a=0;
    int b=0;
};
class cm//读取并检验
{
public:
    cm(std::filesystem::path path):path_(path) {}
    bool load(config &c,std::string &mis)
    {
        mis="";
        try 
        {
            YAML::Node root=YAML::LoadFile(path_.string());
            if (!root.IsMap())
            {
                mis="YAML必须是键值对";
                return false;
            }
            YAML::Node an=root["a"];
            YAML::Node bn=root["b"];
            if (!an)
            {
                mis="config中缺少参数a";
                return false;
            }
            if (!bn)
            {
                mis="config中缺少参数b";
                return false;
            } 
            try 
            {
                c.a=an.as<int>();
            } 
            catch (const YAML::Exception&) 
            {
                mis="参数a必须是整数";
                return false;
            }
            try 
            {
                c.b=bn.as<int>();
            } 
            catch (const YAML::Exception&) 
            {
                mis="参数b必须是整数";
                return false;
            }
            return true;
        } 
        catch(YAML::BadFile) 
        {
            mis="找不到文件";
        }
        catch(YAML::ParserException)
        {
            mis="格式错误";
        }
        catch(YAML::Exception)
        {
            mis="其它错误";
        }
        return false;
    }
    std::filesystem::path& path()
    {
        return path_;
    }
private:
    std::filesystem::path path_;
};

void printresult(config c) //输出
{
    int s=c.a+c.b;
    std::cout << "a+b ="<<s<< std::endl;
}

// 返回文件的最后修改时间,文件不存在或读不到时返回 std::nullopt，表示没有时间
std::optional<std::filesystem::file_time_type> gettime(const std::filesystem::path& path) 
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) //判断文件是否存在
    {
        return std::nullopt;
    }
    const auto time=std::filesystem::last_write_time(path, ec);
    if (ec) //报错里面有东西
    {
        return std::nullopt;
    }
    return time;
}
int runhot(cm& p) //热更新
{
    config c;
    std::string mis;
    auto lasttime=gettime(p.path());
    if (p.load(c, mis)) 
    {
        printresult(c);
    } 
    else 
    {
        std::cerr <<"错误："<<mis<<std::endl;
        std::cerr << "程序没有退出，修好配置文件后会自动重试。" << std::endl;
    }
    std::cout << "正在查看" << p.path().string()<< "，修改文件后会自动重新读取" << std::endl;
    while (true) 
    {
        // 每200毫秒检查一次
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        const auto ct=gettime(p.path());
        // 修改时间没变，说明文件没有被保存过，继续等待
        if (ct==lasttime) 
        {
            continue;
        }
        lasttime=ct;
        // 文件刚刚被删除
        if (!ct) 
        {
            std::cerr<<"配置文件消失"<<std::endl;
            continue;
        }
        // 文件有变化，重新读取。
        if (p.load(c, mis)) 
        {
            std::cout<< "检测到修改";
            printresult(c);
        } 
        else 
        {
            std::cerr<< "错误："<<mis<<std::endl;
            std::cerr<< "这次修改没有生效，等待下一次保存" <<std::endl;
        }
    }
}
int main()
{
    const std::filesystem::path pt="../config/config.yaml";
    cm p(pt);
    return runhot(p);
}
