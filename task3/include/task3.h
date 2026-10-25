#pragma once
#include<iostream>
#include"task3.h"
class shape
{
public:
    virtual double area()=0;
    virtual double perimeter()=0;
    virtual ~shape()=default;
};
class triangle:public shape
{
public:
    triangle(double a,double b,double c);
    virtual double area() override;
    virtual double perimeter() override;
private:
    double a,b,c;
};
class rectangle:public shape
{
public:
    rectangle(double l1,double l2);
    virtual double area() override;
    virtual double perimeter() override;
private:
    double l1,l2;
};
class circle:public shape
{
public:
    circle(double r);
    virtual double area() override;
    virtual double perimeter() override;
private:
    double r;
};
