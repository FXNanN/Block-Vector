#include<vector>
#include<shared_mutex>
#include<iterator>
#include<memory>
#include<exception>


// Never try to understand this. If you want to edit it, I suggest you rewrite this file.  :)

namespace XCL
{
	using std::size_t;

	template<typename T> class Iterator;

	template <typename T>
	class Collection //thread safe
	{
	public:
		virtual size_t size() = 0;
		virtual bool isEmpty() = 0;
		virtual void clear() = 0;
		virtual const T& get(size_t index) = 0;
		virtual void add(const T& e) = 0;
		virtual void addAll(const T* e, const size_t length) = 0;
		virtual void addAll(Collection<T>& collection) = 0;
		virtual T remove(size_t index) = 0;
		virtual T set(size_t index, const T& e) = 0;
		virtual void toArray(T* anArray) = 0;
		virtual void swap(size_t e1wIndex, size_t e2Index) = 0;
		virtual void iterator(Iterator<T>& iterator) = 0;

		virtual void lockReading() = 0; //prevent other thread write in a period, and DO NOT write! 
		virtual void unlockReading() = 0;//prevent other thread write in a period---unlock
		virtual void lock() = 0; //prevent other thread read or write in a period---lock
		virtual void unlock() = 0;//prevent other thread read or write in a period---unlock

	protected:
		std::shared_mutex m_aLock;
	};

	template<typename T>
	class Iterator//: public std::iterator<std::bidirectional_iterator_tag, T>
	{
	public:
		virtual bool hasNext() = 0;
		virtual T& next() = 0;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual size_t getIndex() = 0;

		virtual void operator++(int) = 0;
		virtual bool operator==(Iterator& ref) = 0;
		virtual bool operator!=(Iterator& ref) = 0;
		virtual bool operator<(Iterator& ref) = 0;
		virtual bool operator>(Iterator& ref) = 0;
		virtual T& get() = 0;
		virtual T& operator*() = 0;
		virtual T* operator->() const = 0;

	};

	class ArrayException : public std::exception
	{
		//public:
		static const int NoSuchElementException = 0;
		static const int OutOfRangeException = 1;
		static const int InvalidRangeException = 2;

	public:
		int exceptionType;
		ArrayException(int exceptionType)
		{
			this->exceptionType = exceptionType;
		}
	};



	template <typename T, class alloc = std::allocator<T>>
	class ArrayList: public Collection<T>;

}

template <typename T, class alloc = std::allocator<T>>
class ArrayList
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
					throw ArrayException(ArrayException.NoSuchElementException);
				}
			}
			else // is indexOfBlicks[vectorIndex] exist? NO
			{
				if (m_vectorIndex == NegativeONE && m_index == NegativeONE && m_indexInVector = NegativeONE) // before the first one
				{
					begin();
					if (thisArrayList->m_size > 0)
					{
						return thisArrayList->m_indexOfBlocks[0].elements[0];
					}
				}
			}
			throw ArrayException(ArrayException.NoSuchElementException);
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
				m_indexInVector = thisArrayList->m_indexOfBlocks[m_vectorIndex].size();
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
		virtual void operator++(int)
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
				if (m_vectorIndex == NegativeONE && m_index == NegativeONE && m_indexInVector = NegativeONE) // before the first one
				{
					begin();
				}
				else
					m_index++;
			}
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
			if (isValidIndex(index))
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
			m_index = NegativeONE; //0xFFFFFFFFFFFFFFFF;
			m_vectorIndex = NegativeONE;
			m_indexInVector = NegativeONE; //0xFFFFFFFFFFFFFFFF;
		}
		void init()
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
	ArrayList(size_t block_suggestCapacity)
	{
		m_autoBlockSize = false;
		m_block_minCapacity = block_suggestCapacity >> 3 + 1;
		m_block_suggestCapacity = block_suggestCapacity;
		m_size = 0;
		m_splitPoint = block_suggestCapacity * 2;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
	}
	ArrayList(size_t block_suggestCapacity, bool needDelete)
	{
		m_autoBlockSize = false;
		m_block_minCapacity = block_suggestCapacity / 4;
		m_block_suggestCapacity = block_suggestCapacity;
		m_size = 0;
		m_splitPoint = block_suggestCapacity * 2;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
		m_needDelete = needDelete;
	}
	ArrayList(size_t block_minCapacity, size_t block_suggestCapacity)
	{
		m_autoBlockSize = false;
		m_block_minCapacity = block_minCapacity;
		m_block_suggestCapacity = block_suggestCapacity;
		m_size = 0;
		m_splitPoint = block_suggestCapacity * 2;
		m_indexOfBlocks.emplace_back(ArrayList_Block<T>(0, m_block_minCapacity));
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
	ArrayList(Collection<T>& anArray)
	{
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

private:
	std::vector<ArrayList_Block<T>, alloc> m_indexOfBlocks;
	size_t m_size;

	bool m_autoBlockSize = true; // auto restructure, change the block suggest capacity
	size_t m_block_minCapacity;
	size_t m_block_suggestCapacity;// the capacity better be this value, but it can still be larger than it. 
	//size_t m_reserveSpace;//should not be use when element add to the end of the array, but insert. 
	size_t m_splitPoint; // should be 2 * block_suggestCapacity
	bool m_needDelete;// = false;
	void (*deleteMethod)(T e) = 0;

	size_t inWhichVector(size_t index)
	{
		if (m_indexOfBlocks.back().index <= index) return m_indexOfBlocks.size() - 1;
		size_t leftIndex = 0;
		size_t rightIndex = m_indexOfBlocks.size(); // not include in the range
		size_t cursor = index / m_block_suggestCapacity; // optimize the initial start point
		if (cursor >= rightIndex)
			cursor = rightIndex - 1��
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
	void restructure(size_t block_minCapacity, size_t block_suggestCapacity, size_t splitPoint)
	{
		m_block_minCapacity = block_minCapacity;
		m_block_suggestCapacity = block_suggestCapacity;
		m_splitPoint = splitPoint;
	}
};