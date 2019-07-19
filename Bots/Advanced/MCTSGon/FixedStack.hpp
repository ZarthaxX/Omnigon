#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <stdio.h>

namespace MCTSGon {

	template <class T>
	class FixedStack {
	public:
		inline FixedStack(int _size);
		inline FixedStack(const FixedStack& f);
		~FixedStack();
		inline FixedStack& operator=(const FixedStack& f);
		inline void push(const T& a);
		inline void pop();
		inline const T& top() const;
		inline int size() const;
		inline bool empty() const;
		inline void clear();
	private:
		int first;
		int sz;
		int array_sz;
		T* data;
	};
	
	template<class T>
	inline FixedStack<T>::FixedStack(int _size)
	{
		data = new T[_size];
		array_sz = _size;
		sz = 0;
	}

	template<class T>
	inline FixedStack<T>::FixedStack(const FixedStack & fq)
	{
		*this = fq;
	}

	template<class T>
	inline FixedStack<T>::~FixedStack()
	{
		delete data;
	}


	template<class T>
	inline FixedStack<T>& FixedStack<T>::operator=(const FixedStack & fq)
	{
		delete data;

		sz = fq.size;
		array_sz = fq.array_sz;
		data = new T[array_sz];
		memset(data, fq.data, array_sz * sizeof(T));
		return *this;
	}

	template<class T>
	inline void FixedStack<T>::push(const T& a)
	{
		if (array_sz == sz) {
			std::cerr << "Exceeded stack capacity: " << array_sz << std::endl;
			throw - 1;
		}

		data[sz] = a;
		sz++;
	}

	template<class T>
	inline void FixedStack<T>::pop()
	{
		if (sz == 0)
			throw - 1;

		sz--;
	}

	template<class T>
	inline const T & FixedStack<T>::top() const
	{
		return data[sz - 1];
	}

	template<class T>
	inline int FixedStack<T>::size() const
	{
		return sz;
	}

	template<class T>
	inline bool FixedStack<T>::empty() const
	{
		return sz == 0;
	}

	template<class T>
	inline void FixedStack<T>::clear()
	{
		first = 0;
		sz = 0;
	}
}
