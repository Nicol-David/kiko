//作者：shao chuang
//QQ群：1020789851
//时间：2020.04.29


#pragma once
//此类的作用是让继承与他的子类，不能被拷贝构造，也不能拷贝赋值
class noncopyable{
protected://受保护的构造函数和析构函数，可以让其子类对其进行构造和析构
          //而其自身不可以构造自己
	noncopyable() = default;
	~noncopyable() = default;
	noncopyable(const noncopyable&) = delete;//将拷贝构造、拷贝赋值删除
	const noncopyable& operator=(const noncopyable&) = delete;
};

