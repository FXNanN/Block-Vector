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
		//virtual bool isEmpty() = 0;
		//virtual void clear() = 0;
		//virtual const T& get(size_t index) = 0;
		//virtual void add(const T& e) = 0;
		//virtual void addAll(const T* e, const size_t length) = 0;
		//virtual void addAll(Collection<T>& collection) = 0;
		//virtual T remove(size_t index) = 0;
		//virtual T set(size_t index, const T& e) = 0;
		//virtual void toArray(T* anArray) = 0;
		//virtual void swap(size_t e1wIndex, size_t e2Index) = 0;
		//virtual void iterator(Iterator<T>& iterator) = 0;

		//virtual void lockReading() = 0; //prevent other thread write in a period, and DO NOT write! 
		//virtual void unlockReading() = 0;//prevent other thread write in a period---unlock
		//virtual void lock() = 0; //prevent other thread read or write in a period---lock
		//virtual void unlock() = 0;//prevent other thread read or write in a period---unlock

	protected:
		std::shared_mutex m_aLock;
	};

	template<typename T>
	class Iterator//: public std::iterator<std::bidirectional_iterator_tag, T>
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef std::ptrdiff_t difference_type;

		virtual bool hasNext() = 0;
		virtual T& next() = 0;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual size_t getIndex() = 0;

		//virtual Iterator operator++(int) = 0; // return the original iterator
		//virtual Iteraor& operator++() = 0; // return the current iterator
		virtual bool operator==(Iterator& ref) = 0;
		virtual bool operator!=(Iterator& ref) = 0;
		virtual bool operator<(Iterator& ref) = 0;
		virtual bool operator>(Iterator& ref) = 0;
		virtual T& get() = 0;
		virtual T& operator*() = 0;
		//virtual T* operator->() const = 0;

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


}


#include "XCL_ArrayList.h"