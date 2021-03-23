#pragma once
#include <vector>
#include <algorithm>
using namespace std;

template<class T>
class SortedArray
{
public:
	SortedArray();			//构造函数，初始化大小
	~SortedArray();

	void ReSort();				//重排
	void Insert(const T& elem);	//插入
	void Erase(int index);		//下标删除
	void Erase(const T& elem);	//元素删除

	T& operator[] (int i);
	const T& operator[] (int i) const;

private:
	vector<T>* data;
	int size;
};

