#include "SortedArray.h"
#include <algorithm>

using namespace std;

template<class T>
SortedArray<T>::SortedArray()
{
	data = new vector<T>();
}

template<class T>
SortedArray<T>::~SortedArray()
{
	delete data;
}

template<class T>
void SortedArray<T>::ReSort()
{
	sort(data->begin(), data->end());
}


template<class T>
T& SortedArray<T>::operator[](int i)
{
	return (*data)[i];
}

template<class T>
const T& SortedArray<T>::operator[](int i) const
{
	return data->at(i);
}

template<class T>
void SortedArray<T>::Insert(const T& elem)
{
	int l = 0, r = size - 1;
	int target = -1;
	while (l <= r)
	{
		int mid = (l + r) >> 1;
		if (data->at(mid) >= elem)
		{
			target = mid;
			r = mid - 1;
		}
		else
		{
			l = mid + 1;
		}
	}

	if (target == -1) data->push_back(elem);
	else data->insert(data->begin() + target, elem);
	++size;
}

template<class T>
void SortedArray<T>::Erase(int index)
{
	data->erase(data->begin() + index);
	--size;
}

template<class T>
void SortedArray<T>::Erase(const T& elem)
{
	int l = 0, r = size - 1;
	int target = -1;
	while (l <= r)
	{
		int mid = (l + r) >> 1;
		if (data->at(mid) >= elem)
		{
			target = mid;
			r = mid - 1;
		}
		else
		{
			l = mid + 1;
		}
	}
	if (data->at(target) == elem) data->erase(data->begin() + target);
	--size;
}