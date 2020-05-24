#pragma once


#include "XCL.h"
#include<vector>
XCL_BEGIN
template <typename T, typename alloc = std::allocator<T>>
class ArrayList: public Collection<T>
{
	template<class T1, class alloc1 = std::allocator<T>>
	class ArrayList_Block
	{
	public:
		size_t index;
		std::vector<T1, alloc1> elements;
		ArrayList_Block()
		{
			this->index = 0;
		}
		ArrayList_Block(size_t index, size_t capacity)
		{
			this->index = index;
			elements.reserve(capacity);
		}
		ArrayList_Block(size_t index, std::vector<T, alloc>& data)
		{
			this->index = index;
			elements = data;
		}
	};

public:
	template<typename T1>
	class ArrayList_Iterator : public Iterator<T1>//, std::iterator<random_access_iterator_tag, long, long, long long> // not thread safe
	{
		friend ArrayList<T1>;
	public:
		ArrayList_Iterator()
		{
			m_vectorIndex = NegativeONE;
			m_indexInVector = NegativeONE;
			m_index = NegativeONE;
		}
		virtual bool hasNext()
		{
			return m_index + 1 < thisArrayList->m_size;
		}
		virtual bool hasPrev()
		{
			return m_index > 0 && m_index - 1 < thisArrayList->m_size;
		}
		virtual T& next()
		{
			if (m_vectorIndex < thisArrayList->m_indexOfBlocks.size()) // is indexOfBlicks[vectorIndex] exist? YES
			{
				m_index++;
				if (m_indexInVector++ < thisArrayList->m_indexOfBlocks[m_vectorIndex].elements.size()) // has NEXT in the vector?
				{
					return thisArrayList->m_indexOfBlocks[m_vectorIndex].elements[m_indexInVector];
				}
				else if (m_vectorIndex++ < thisArrayList->m_indexOfBlocks.size()) // Need to move to the next vector, and has next vector
				{
					m_indexInVector = 0;
					return thisArrayList->m_indexOfBlocks[m_vectorIndex].elements[m_indexInVector];
				}
				else //do not have next vector, or it is the end
				{
					//throw ArrayException(ArrayException.NoSuchElementException);
				}
			}
			else // is indexOfBlicks[vectorIndex] exist? NO
			{
				if (m_vectorIndex == NegativeONE && m_index == NegativeONE && m_indexInVector == NegativeONE) // before the first one
				{
					begin();
					if (thisArrayList->m_size > 0)
					{
						return thisArrayList->m_indexOfBlocks[0].elements[0];
					}
				}
			}
			//throw ArrayException(ArrayException.NoSuchElementException);
		}
		virtual void begin()
		{
			m_vectorIndex = 0;
			m_indexInVector = 0;
			m_index = 0;
		}
		virtual void end()
		{
			m_index = thisArrayList->m_size;
			if (thisArrayList->m_indexOfBlocks.back().elements.size() < thisArrayList->m_block_suggestCapacity)
			{
				m_vectorIndex = thisArrayList->m_indexOfBlocks.size() - 1;
				m_indexInVector = thisArrayList->m_indexOfBlocks[m_vectorIndex].elements.size();
			}
			else
			{
				m_vectorIndex = thisArrayList->m_indexOfBlocks.size();
				m_indexInVector = 0;
			}
		}
		virtual size_t getIndex()
		{
			return this->m_index;
		}
		virtual ArrayList_Iterator<T> operator++(int)
		{
			ArrayList_Iterator<T> ret = this;
			if (m_vectorIndex < thisArrayList->m_indexOfBlocks.size()) // is indexOfBlicks[vectorIndex] exist? YES
			{
				m_index++;
				if ((m_indexInVector++) < thisArrayList->m_indexOfBlocks[m_vectorIndex].elements.size()) // has NEXT in the vector?
				{
				}
				else if ((m_vectorIndex++) < thisArrayList->m_indexOfBlocks.size()) // Need to move to the next vector, and has next vector
				{
					m_indexInVector = 0;
				}
				else //do not have next vector, or it is the end
				{
					end();
				}
			}
			else // is indexOfBlicks[vectorIndex] exist? NO
			{
				if (m_vectorIndex == NegativeONE && m_index == NegativeONE && m_indexInVector == NegativeONE) // before the first one
				{
					begin();
				}
				else
					m_index++;
			}
			return ret;
		}
		virtual ArrayList_Iterator<T>& operator++()
		{
			if (m_vectorIndex < thisArrayList->m_indexOfBlocks.size()) // is indexOfBlicks[vectorIndex] exist? YES
			{
				m_index++;
				if ((m_indexInVector++) < thisArrayList->m_indexOfBlocks[m_vectorIndex].elements.size()) // has NEXT in the vector?
				{
				}
				else if ((m_vectorIndex++) < thisArrayList->m_indexOfBlocks.size()) // Need to move to the next vector, and has next vector
				{
					m_indexInVector = 0;
				}
				else //do not have next vector, or it is the end
				{
					end();
				}
			}
			else // is indexOfBlicks[vectorIndex] exist? NO
			{
				if (m_vectorIndex == NegativeONE && m_index == NegativeONE && m_indexInVector == NegativeONE) // before the first one
				{
					begin();
				}
				else
					m_index++;
			}
			return this;
		}
		virtual void operator--(int)
		{
			m_index--;
			if (m_vectorIndex < thisArrayList->m_indexOfBlocks.size()) // is indexOfBlicks[vectorIndex] exist? YES
			{
				if (m_indexInVector != 0) // has PREV in the vector?
				{
					m_indexInVector--;
				}
				else if (m_vectorIndex != 0) // Need to move to the previous vector, and has previous vector
				{
					m_vectorIndex--;
					m_indexInVector = thisArrayList->m_indexOfBlocks[m_vectorIndex].elements.size() - 1;
				}
				else //do not have previous vector, or it is the end
				{
					init();
				}
			}
		}
		virtual void operator+=(size_t offset)
		{
			m_index += offset;
			if (m_index > thisArrayList->size())
				end();
			m_vectorIndex = thisArrayList->inWhichVector(m_index);
			m_indexInVector = m_index - thisArrayList->m_indexOfBlocks[m_vectorIndex].index;
		}
		virtual void operator-=(size_t offset)
		{
			m_index -= offset;
			if (m_index >= 0)
			{
				m_vectorIndex = thisArrayList->inWhichVector(m_index);
				m_indexInVector = m_index - thisArrayList->m_indexOfBlocks[m_vectorIndex].index;
			}
			else
			{
				begin();
			}
		}
		virtual ArrayList_Iterator<T1> operator+(size_t offset)
		{
			return ArrayList_Iterator<T1>(m_index + offset, thisArrayList);
		}
		virtual ArrayList_Iterator<T1> operator-(size_t offset)
		{
			return ArrayList_Iterator<T1>(m_index - offset, thisArrayList);
		}
		virtual bool operator==(const ArrayList_Iterator<T1>& vi)
		{
			return m_index == vi.m_index;
		}
		virtual bool operator!=(const ArrayList_Iterator<T1>& vi)
		{
			return m_index != vi.m_index;
		}
		virtual bool operator>(const ArrayList_Iterator<T1>& vi)
		{
			return m_index > vi.m_index;
		}
		virtual bool operator>=(const ArrayList_Iterator<T1>& vi)
		{
			return m_index >= vi.m_index;
		}
		virtual bool operator<(const ArrayList_Iterator<T1>& vi)
		{
			return m_index < vi.m_index;
		}
		virtual bool operator<=(const ArrayList_Iterator<T1>& vi)
		{
			return m_index <= vi.m_index;
		}
		virtual bool operator==(Iterator<T1>& vi)
		{
			return m_index == vi.getIndex();
		}
		virtual bool operator!=(Iterator<T1>& vi)
		{
			return m_index != vi.getIndex();
		}
		virtual bool operator>(Iterator<T1>& vi)
		{
			return m_index > vi.getIndex();
		}
		virtual bool operator<(Iterator<T1>& vi)
		{
			return m_index < vi.getIndex();
		}
		virtual T1& operator[](size_t offset)
		{
			size_t vectorIndex = thisArrayList->inWhichVector(offset);
			return thisArrayList->m_indexOfBlocks[vectorIndex].elements[offset - thisArrayList->m_indexOfBlocks[vectorIndex].index];
		}
		virtual T1& operator*()
		{
			T1& ret = thisArrayList->m_indexOfBlocks.at(m_vectorIndex).elements[m_indexInVector];
			return ret;
		}
		virtual T1& get()
		{
			T1& ret = thisArrayList->m_indexOfBlocks.at(m_vectorIndex).elements[m_indexInVector];
			return ret;
		}

	private:
		unsigned long long m_index;
		unsigned long long m_vectorIndex;
		unsigned long long m_indexInVector;
		const size_t NegativeONE = -1; //0xFFFFFFFFFFFFFFFF;
		ArrayList<T1>* thisArrayList; // the ptr of the arraylist that this iterator point to
		ArrayList_Iterator(size_t index, ArrayList<T1>* ptr)
		{
			if (thisArrayList->isValidIndex(index))
			{
				m_vectorIndex = ptr->inWhichVector(index);
				m_indexInVector = index - ptr->m_indexOfBlocks[m_vectorIndex].index;
				m_index = index;
				thisArrayList = ptr;
			}
			else
			{
				begin();
			}
		}
		ArrayList_Iterator(size_t index, size_t vectorIndex, size_t indexInVector, ArrayList<T1>* ptr) //does not check the validity.
		{
			m_vectorIndex = vectorIndex;
			m_indexInVector = indexInVector;
			m_index = index;
			thisArrayList = ptr;
		}
		ArrayList_Iterator(ArrayList<T1>* ptr) : thisArrayList(ptr) //this is the default constructor
		{
			init();
		}
		inline void init()
		{
			m_index = NegativeONE; //0xFFFFFFFFFFFFFFFF;
			m_vectorIndex = NegativeONE;
			m_indexInVector = NegativeONE; //0xFFFFFFFFFFFFFFFF;
		}
	};
	friend ArrayList_Iterator<T>;

private:
	const size_t m_default_block_minCapacity = 8;
	const size_t m_default_block_suggestCapacity = 64;
	const size_t m_default_splitPoint = 128;

public:
	ArrayList()
	{
		m_block_minCapacity = m_default_block_minCapacity;
		m_block_suggestCapacity = m_default_block_suggestCapacity;
		m_size = 0;
		m_splitPoint = m_default_splitPoint;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
	}
	ArrayList(void (*deleteMethod)(T)) // will automatically delete the new objects after it be removed from the list.
	{
		m_block_minCapacity = m_default_block_minCapacity;
		m_block_suggestCapacity = m_default_block_suggestCapacity;
		m_size = 0;
		m_splitPoint = m_default_splitPoint;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
		m_needDelete = true;
		this->deleteMethod = deleteMethod;
	}
	ArrayList(size_t block_suggestCapacity, bool autoBlockSize = false, void (*deleteMethod)(T) = nullptr)
	{
		m_autoBlockSize = autoBlockSize;
		m_block_minCapacity = block_suggestCapacity >> 2;
		m_block_suggestCapacity = block_suggestCapacity;
		m_size = 0;
		m_splitPoint = block_suggestCapacity << 1;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
		if (deleteMethod != nullptr)
		{
			m_needDelete = true;
			this->deleteMethod = deleteMethod;
		}
	}
	ArrayList(size_t block_minCapacity, size_t block_suggestCapacity, bool autoBlockSize = false, void (*deleteMethod)(T) = nullptr)
	{
		m_autoBlockSize = autoBlockSize;
		m_block_minCapacity = block_minCapacity;
		m_block_suggestCapacity = block_suggestCapacity;
		m_size = 0;
		m_splitPoint = block_suggestCapacity << 1;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
		if (deleteMethod != nullptr)
		{
			m_needDelete = true;
			this->deleteMethod = deleteMethod;
		}
	}

	ArrayList(const ArrayList<T>& copy)
	{
		this->m_indexOfBlocks = copy.m_indexOfBlocks;
		this->m_block_minCapacity = copy.m_block_minCapacity;
		this->m_block_suggestCapacity = copy.m_block_suggestCapacity;
		this->m_size = copy.m_size;
		this->m_splitPoint = copy.m_splitPoint;
		this->m_needDelete = copy.m_needDelete;
		this->deleteMethod = copy.deleteMethod;
		this->m_autoBlockSize = copy.m_autoBlockSize;
	}
	ArrayList(Collection<T>& anArray, size_t block_suggestCapacity = m_default_block_suggestCapacity, bool autoBlockSize = false, void (*deleteMethod)(T) = nullptr)
	{
		if (deleteMethod != nullptr)
		{
			m_needDelete = true;
			this->deleteMethod = deleteMethod;
		}
		m_autoBlockSize = autoBlockSize;
		if (anArray.size() <= m_minBlockSizeNeedRestructure)
		{
			m_block_minCapacity = m_default_block_minCapacity;
			m_block_suggestCapacity = m_default_block_suggestCapacity;
			m_size = 0;
			m_splitPoint = m_default_splitPoint;
		}
		else
		{
			size_t block_suggestCapacity = anArray.size() >> 5;
			restructure(block_suggestCapacity, true);
		}
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
		addAll_nts(anArray);
	}

	void add(const T& e)
	{
		Collection<T>::m_aLock.lock();
		if (m_indexOfBlocks.back().elements.size() < m_block_suggestCapacity)
		{
			m_indexOfBlocks.back().elements.emplace_back(e);
		}
		else
		{
			if (m_autoBlockSize)restructure();
			m_indexOfBlocks.emplace_back(ArrayList_Block<T>(m_size, m_block_minCapacity));//create a new block
			m_indexOfBlocks.back().elements.emplace_back(e);
		}
		m_size++;
		Collection<T>::m_aLock.unlock();
	}

	template<template<typename _T, typename _Alloc = std::allocator> class Container, typename _A>
	void addAll(const Container<T, _A>& container)
	{
		Collection<T>::m_aLock.lock();
		_addAll(container.begin(), container.size());
		Collection<T>::m_aLock.unlock();
	}

	template<class _Iter>
	void addAll(_Iter& from, const size_t length)
	{
		Collection<T>::m_aLock.lock();
		_addAll(from, length);
		Collection<T>::m_aLock.unlock();
	}

	template<class _Iter>
	void addAll(_Iter& from, _Iter& to)
	{
		Collection<T>::m_aLock.lock();
		_addAll(from, to);
		Collection<T>::m_aLock.unlock();
	}

	void addAll(T* anArray, const size_t length)
	{
		Collection<T>::m_aLock.lock();
		_addAll(anArray, length);
		Collection<T>::m_aLock.unlock();
	}

private:
	template<class _Iter>
	void _addAll(_Iter& from, const size_t length) // argument "to" is not necessary
	{
		trim_split(m_indexofBlocks.size() - 1);
		size_t len = m_block_suggestCapacity - this->m_indexOfBlocks.back().elements.size();
		if (len > length)
			len = length;
		for (size_t i = 0; i < len; i++, from++)
		{
			this->m_indexOfBlocks.back().elements.emplace_back(*from);
		}
		size_t numBlockAdd = (length - len) / m_block_suggestCapacity;
		len += numBlockAdd * m_block_suggestCapacity;
		for (size_t i = 0; i < numBlockAdd; i++)
		{
			m_indexOfBlocks.emplace_back(ArrayList::ArrayList_Block<T, alloc>(
				m_indexOfBlocks.back().elements.size() + m_indexOfBlocks.back().index, m_block_suggestCapacity));
			auto& lastVector = m_indexOfBlocks.back().elements;
			for (j = 0; j < m_block_suggestCapacity; j++, from++)
			{
				lastVector.elements.emplace_back(*from);
			}
		}
		if (len != length)
		{
			m_indexOfBlocks.emplace_back(ArrayList::ArrayList_Block<T, alloc>(
				m_indexOfBlocks.back().elements.size() + m_indexOfBlocks.back().index, m_block_minCapacity));
			auto& lastVector = m_indexOfBlocks.back().elements;
			while (len != length)
			{
				lastVector.elements.emplace(*from);
				len++;
				from++;
			}
		}
		size += length;
		if (m_autoBlockSize) restructure();
	}

	template<class _Iter>
	void _addAll(_Iter& from, _Iter& to)
	{
		size_t count = 0;
		trim_split(m_indexOfBlocks.size() - 1);

		{
			auto& lastVector = m_indexOfBlocks.back().elements;
			for (size_t i = lastVector.size(), i < m_block_suggestCapacity&& from != to; i++, from++, count++)
			{
				lastVector.emplace_back(*from);
			}
		}
		while (from != to)
		{
			m_indexOfBlocks.emplace_back(ArrayList::ArrayList_Block<T, alloc>(
				m_indexOfBlocks.back().elements.size() + m_indexOfBlocks.back().index, m_block_minCapacity));
			auto& lastVector = m_indexOfBlocks.back();
			for (i = 0; i < m_block_suggestCapacity && from != to; i++, from++, count++)
			{
				lastVector.emplace_back(*from);
			}
		}
		m_size += count;
		if (m_autoBlockSize) restructure();
	}

private:

	bool trim_split(size_t vectorIndex) // return true if trimed the vector
	{
		if (m_indexOfBlocks[vectorIndex].elements.size() < m_splitPoint) return false;
		
		size_t previousVectorSize = 0;
		size_t nextVectorSize = 0;
		if (vectorIndex > 0) previousVectorSize = m_indexOfBlocks[vectorIndex - 1].elements.size();
		if (vectorIndex < m_indexOfBlocks.size() - 1) m_indexOfBlocks[vectorIndex + 1].elements.size();
		if ()

		return true;
	}

	bool trim_force_toSuggestSize(size_t vectorIndex)
	{

	}


private:
	template<typename _Iter>
	ArrayList_Iterator<T> _addAll(ArrayList_Iterator<T>& iter, _Iter& from, size_t length)
	{
		
	}

	template<typename _Iter>
	ArrayList_Iterator<T> _addAll(ArrayList_Iterator<T>& iter, _Iter& from, _Iter& to)
	{
		size_t count = 0;

		m_size += count;
	}

public:
	size_t size()
	{
		Collection<T>::m_aLock.lock_shared();
		size_t ret = m_size;
		Collection<T>::m_aLock.unlock_shared();
		return ret;
	}




	///////////////////////
	void iterator(Iterator<T> iterator)
	{
		iterator = ArrayList_Iterator<T>(this);
		//return static_cast<Iterator<T>>(ai);
	}
	ArrayList_Iterator<T> getIteratorAt(size_t index)
	{
		return ArrayList_Iterator<T>(index, this);
	}
	ArrayList_Iterator<T> begin() noexcept
	{
		return ArrayList_Iterator<T>(0, 0, 0, this);
	}
	ArrayList_Iterator<T> end() noexcept
	{
		Collection<T>::m_aLock.lock_shared();
		ArrayList_Iterator<T> i = ArrayList_Iterator<T>(m_size, m_indexOfBlocks.size() - 1, m_indexOfBlocks.back().elements.size(), this);
		Collection<T>::m_aLock.unlock_shared();
		return i;
	}
	const T& back() const noexcept
	{
		Collection<T>::m_aLock.lock_shared();
		const T& ret = m_indexOfBlocks.back().elements.back();
		Collection<T>::m_aLock.unlock_shared();
		return ret;
	}



private:
	std::vector<ArrayList_Block<T>, alloc> m_indexOfBlocks;
	size_t m_size;

	bool m_autoBlockSize = true; // auto restructure, change the block suggest capacity
	size_t m_block_minCapacity;
	size_t m_block_suggestCapacity;// the capacity better be this value, but it can still be larger than it. 
	size_t m_splitPoint; // should be 2 * block_suggestCapacity
	bool m_needDelete = false;// = false;
	void (*deleteMethod)(T e) = 0;

	size_t inWhichVector(size_t index)
	{
		if (m_indexOfBlocks.back().index <= index) return m_indexOfBlocks.size() - 1;
		size_t leftIndex = 0;
		size_t rightIndex = m_indexOfBlocks.size(); // not include in the range
		size_t cursor = index / m_block_suggestCapacity; // optimize the initial start point
		if (cursor >= rightIndex)
			cursor = rightIndex - 1;
		while (leftIndex + 1 < rightIndex)
		{
			if (m_indexOfBlocks[cursor].index < index)
			{
				leftIndex = cursor;
				cursor = (leftIndex + rightIndex) >> 1;
			}
			else if (m_indexOfBlocks[cursor].index > index)
			{
				rightIndex = cursor;
				cursor = (leftIndex + rightIndex) >> 1;
			}
			else
				return cursor;
		}
		return leftIndex;
	}


private:
	const size_t m_minBlockSizeNeedRestructure = 1024;
	size_t m_maxReachedSize = m_minBlockSizeNeedRestructure;
	const size_t m_maxBlockSize = 4096;
public:
	void restructure()// adjust the size of one block based on the arrayList size
	{
		if (m_size < m_maxReachedSize || m_block_suggestCapacity > m_maxBlockSize)
			return;
		m_maxReachedSize = m_size;
		size_t block_suggestCapacity = m_size >> 5;
		if (block_suggestCapacity > m_maxBlockSize)
			block_suggestCapacity = m_maxBlockSize;
		restructure(block_suggestCapacity >> 2, block_suggestCapacity, block_suggestCapacity << 1);
	}
	void restructure(size_t block_suggestCapacity, bool autoBlockSize)
	{
		m_block_minCapacity = block_suggestCapacity >> 3 + 1;
		m_block_suggestCapacity = block_suggestCapacity;
		m_splitPoint = block_suggestCapacity << 1;
		m_autoBlockSize = autoBlockSize;
	}
	void restructure(size_t block_minCapacity, size_t block_suggestCapacity, size_t splitPoint, bool autoBlockSize)
	{
		m_block_minCapacity = block_minCapacity;
		m_block_suggestCapacity = block_suggestCapacity;
		m_splitPoint = splitPoint;
		m_autoBlockSize = autoBlockSize;
	}
	void restructure(size_t block_minCapacity, size_t block_suggestCapacity, size_t splitPoint)
	{
		m_block_minCapacity = block_minCapacity;
		m_block_suggestCapacity = block_suggestCapacity;
		m_splitPoint = splitPoint;
	}

public:
	bool isValidIndex(size_t index)
	{
		//lockReading();
		bool ret = index >= 0 && index < m_size;
		//unlockReading();
		return ret;
	}

};

XCL_END