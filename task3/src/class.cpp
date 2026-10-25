#include"../include/task3.h"
#include<cmath>
#include<iostream>
triangle::triangle(double a,double b,double c)
{
    this->a=a;
    this->b=b;
    this->c=c;
}
double triangle::area()
{
    double p=(a+b+c)/2.0;
    return std::sqrt(p*(p-a)*(p-b)*(p-c));
}
double triangle::perimeter()
{
    return a+b+c;
}
rectangle::rectangle(double l1,double l2)
{
    this->l1=l1;
    this->l2=l2;
}
double rectangle::area()
{
    return l1*l2;
}
double rectangle::perimeter()
{
    return 2*(l1+l2);
}
circle::circle(double r)
{
    this->r=r;
}
double circle::area()
{
    return M_PI*r*r;
}
double circle::perimeter()
{
    return 2*M_PI*r;
}
