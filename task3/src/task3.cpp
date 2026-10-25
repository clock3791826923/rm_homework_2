#include<iostream>
#include"../include/task3.h"
int main()
{
    triangle t(3,4,5);
    rectangle r(3,4);
    circle c(2);
    std::cout<<"triangle"<<std::endl;
    std::cout<<"area"<<t.area()<<std::endl;
    std::cout<<"perimeter"<<t.perimeter()<<std::endl;
    std::cout<<"rectangle"<<std::endl;
    std::cout<<"area:"<<r.area()<<std::endl;
    std::cout<<"perimeter"<<r.perimeter()<<std::endl;
    std::cout<<"circle"<<std::endl;
    std::cout<<"area:"<<c.area()<<std::endl;
    std::cout<<"perimeter:"<<c.perimeter()<<std::endl;
    return 0;
}