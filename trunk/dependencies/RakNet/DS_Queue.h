/// \file
/// \brief \b [Internal] A queue used by RakNet.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __QUEUE_H
#define __QUEUE_H

// Template classes have to have all the code in the header file
#include "RakAssert.h"
#include "Export.h"
#include "RakMemoryOverride.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	/// \brief A queue implemented as an array with a read and write index.
	template <class queue_type>
	class RAK_DLL_EXPORT Queue
	{
	public:
		Queue();
		~Queue();
		Queue( Queue& original_copy );
		bool operator= ( const Queue& original_copy );
		void Push( const queue_type& input );
		void PushAtHead( const queue_type& input, unsigned index=0 );
		queue_type& operator[] ( unsigned int position ) const; // Not a normal thing you do with a queue but can be used for efficiency
		void RemoveAtIndex( unsigned int position ); // Not a normal thing you do with a queue but can be used for efficiency
		inline queue_type Peek( void ) const;
		inline queue_type PeekTail( void ) const;
		inline queue_type Pop( void );
		inline unsigned int Size( void ) const;
		inline bool IsEmpty(void) const;
		inline unsigned int AllocationSize( void ) const;
		inline void Clear( void );
		void Compress( void );
		bool Find ( queue_type q );
		void ClearAndForceAllocation( int size ); // Force a memory allocation to a certain larger size

	private:
		queue_type* array;
		unsigned int head;  // Array index for the head of the queue
		unsigned int tail; // Array index for the tail of the queue
		unsigned int allocation_size;
	};


	template <class queue_type>
		inline unsigned int Queue<queue_type>::Size( void ) const
	{
		if ( head <= tail )
			return tail -head;
		else
			return allocation_size -head + tail;
	}

	template <class queue_type>
	inline bool Queue<queue_type>::IsEmpty(void) const
	{
		return head==tail;
	}

	template <class queue_type>
	inline unsigned int Queue<queue_type>::AllocationSize( void ) const
	{
		return allocation_size;
	}

	template <class queue_type>
		Queue<queue_type>::Queue()
	{
		//allocation_size = 16;
		//array = RakNet::OP_NEW_ARRAY<queue_type>(allocation_size, __FILE__, __LINE__ );
		allocation_size = 0;
		array=0;
		head = 0;
		tail = 0;
	}

	template <class queue_type>
		Queue<queue_type>::~Queue()
	{
		if (allocation_size>0)
			RakNet::OP_DELETE_ARRAY(array, __FILE__, __LINE__);
	}

	template <class queue_type>
		inline queue_type Queue<queue_type>::Pop( void )
	{
#ifdef _DEBUG
		RakAssert( allocation_size > 0 && Size() >= 0 && head != tail);
#endif
		//head=(head+1) % allocation_size;

		if ( ++head == allocation_size )
			head = 0;

		if ( head == 0 )
			return ( queue_type ) array[ allocation_size -1 ];

		return ( queue_type ) array[ head -1 ];
	}

	template <class queue_type>
		void Queue<queue_type>::PushAtHead( const queue_type& input, unsigned index )
	{
		RakAssert(index <= Size());

		// Just force a reallocation, will be overwritten
		Push(input);

		if (Size()==1)
			return;

		unsigned writeIndex, readIndex, trueWriteIndex, trueReadIndex;
		writeIndex=Size()-1;
		readIndex=writeIndex-1;
		while (readIndex >= index)
		{
			if ( head + writeIndex >= allocation_size )
				trueWriteIndex = head + writeIndex - allocation_size;
			else
				trueWriteIndex = head + writeIndex;

			if ( head + readIndex >= allocation_size )
				trueReadIndex = head + readIndex - allocation_size;
			else
				trueReadIndex = head + readIndex;

			array[trueWriteIndex]=array[trueReadIndex];

			if (readIndex==0)
				break;
			writeIndex--;
			readIndex--;
		}

		if ( head + index >= allocation_size )
			trueWriteIndex = head + index - allocation_size;
		else
			trueWriteIndex = head + index;

		array[trueWriteIndex]=input;
	}


	template <class queue_type>
		inline queue_type Queue<queue_type>::Peek( void ) const
	{
#ifdef _DEBUG
		RakAssert( head != tail );
		RakAssert( allocation_size > 0 && Size() >= 0 );
#endif

		return ( queue_type ) array[ head ];
	}

	template <class queue_type>
		inline queue_type Queue<queue_type>::PeekTail( void ) const
		{
#ifdef _DEBUG
			RakAssert( head != tail );
			RakAssert( allocation_size > 0 && Size() >= 0 );
#endif
			if (tail!=0)
				return ( queue_type ) array[ tail-1 ];
			else
				return ( queue_type ) array[ allocation_size-1 ];
		}

	template <class queue_type>
		void Queue<queue_type>::Push( const queue_type& input )
	{
		if ( allocation_size == 0 )
		{
			array = RakNet::OP_NEW_ARRAY<queue_type>(16, __FILE__, __LINE__ );
			head = 0;
			tail = 1;
			array[ 0 ] = input;
			allocation_size = 16;
			return ;
		}

		array[ tail++ ] = input;

		if ( tail == allocation_size )
			tail = 0;

		if ( tail == head )
		{
			//  unsigned int index=tail;

			// Need to allocate more memory.
			queue_type * new_array;
			new_array = RakNet::OP_NEW_ARRAY<queue_type>(allocation_size * 2, __FILE__, __LINE__ );
#ifdef _DEBUG
			RakAssert( new_array );
#endif
			if (new_array==0)
				return;

			for ( unsigned int counter = 0; counter < allocation_size; ++counter )
				new_array[ counter ] = array[ ( head + counter ) % ( allocation_size ) ];

			head = 0;

			tail = allocation_size;

			allocation_size *= 2;

			// Delete the old array and move the pointer to the new array
			RakNet::OP_DELETE_ARRAY(array, __FILE__, __LINE__);

			array = new_array;
		}

	}

	template <class queue_type>
		Queue<queue_type>::Queue( Queue& original_copy )
	{
		// Allocate memory for copy

		if ( original_copy.Size() == 0 )
		{
			allocation_size = 0;
		}

		else
		{
			array = RakNet::OP_NEW_ARRAY<queue_type >( original_copy.Size() + 1 , __FILE__, __LINE__ );

			for ( unsigned int counter = 0; counter < original_copy.Size(); ++counter )
				array[ counter ] = original_copy.array[ ( original_copy.head + counter ) % ( original_copy.allocation_size ) ];

			head = 0;

			tail = original_copy.Size();

			allocation_size = original_copy.Size() + 1;
		}
	}

	template <class queue_type>
		bool Queue<queue_type>::operator= ( const Queue& original_copy )
	{
		if ( ( &original_copy ) == this )
			return false;

		Clear();

		// Allocate memory for copy
		if ( original_copy.Size() == 0 )
		{
			allocation_size = 0;
		}

		else
		{
			array = RakNet::OP_NEW_ARRAY<queue_type >( original_copy.Size() + 1 , __FILE__, __LINE__ );

			for ( unsigned int counter = 0; counter < original_copy.Size(); ++counter )
				array[ counter ] = original_copy.array[ ( original_copy.head + counter ) % ( original_copy.allocation_size ) ];

			head = 0;

			tail = original_copy.Size();

			allocation_size = original_copy.Size() + 1;
		}

		return true;
	}

	template <class queue_type>
		inline void Queue<queue_type>::Clear ( void )
	{
		if ( allocation_size == 0 )
			return ;

		if (allocation_size > 32)
		{
			RakNet::OP_DELETE_ARRAY(array, __FILE__, __LINE__);
			allocation_size = 0;
		}

		head = 0;
		tail = 0;
	}

	template <class queue_type>
		void Queue<queue_type>::Compress ( void )
	{
		queue_type* new_array;
		unsigned int newAllocationSize;
		if (allocation_size==0)
			return;

		newAllocationSize=1;
		while (newAllocationSize <= Size())
			newAllocationSize<<=1; // Must be a better way to do this but I'm too dumb to figure it out quickly :)

		new_array = RakNet::OP_NEW_ARRAY<queue_type >(newAllocationSize, __FILE__, __LINE__ );

		for (unsigned int counter=0; counter < Size(); ++counter)
			new_array[counter] = array[(head + counter)%(allocation_size)];

		tail=Size();
		allocation_size=newAllocationSize;
		head=0;

		// Delete the old array and move the pointer to the new array
		RakNet::OP_DELETE_ARRAY(array, __FILE__, __LINE__);
		array=new_array;
	}

	template <class queue_type>
		bool Queue<queue_type>::Find ( queue_type q )
	{
		if ( allocation_size == 0 )
			return false;

		unsigned int counter = head;

		while ( counter != tail )
		{
			if ( array[ counter ] == q )
				return true;

			counter = ( counter + 1 ) % allocation_size;
		}

		return false;
	}

	template <class queue_type>
		void Queue<queue_type>::ClearAndForceAllocation( int size )
	{
		RakNet::OP_DELETE_ARRAY(array, __FILE__, __LINE__);
		array = RakNet::OP_NEW_ARRAY<queue_type>(size, __FILE__, __LINE__ );
		allocation_size = size;
		head = 0;
		tail = 0;
	}

	template <class queue_type>
		inline queue_type& Queue<queue_type>::operator[] ( unsigned int position ) const
	{
#ifdef _DEBUG
		RakAssert( position < Size() );
#endif
		//return array[(head + position) % allocation_size];

		if ( head + position >= allocation_size )
			return array[ head + position - allocation_size ];
		else
			return array[ head + position ];
	}

	template <class queue_type>
	void Queue<queue_type>::RemoveAtIndex( unsigned int position )
	{
#ifdef _DEBUG
		RakAssert( position < Size() );
		RakAssert( head != tail );
#endif

		if ( head == tail || position >= Size() )
			return ;

		unsigned int index;

		unsigned int next;

		//index  = (head + position) % allocation_size;
		if ( head + position >= allocation_size )
			index = head + position - allocation_size;
		else
			index = head + position;

		//next = (index + 1) % allocation_size;
		next = index + 1;

		if ( next == allocation_size )
			next = 0;

		while ( next != tail )
		{
			// Overwrite the previous element
			array[ index ] = array[ next ];
			index = next;
			//next = (next + 1) % allocation_size;

			if ( ++next == allocation_size )
				next = 0;
		}

		// Move the tail back
		if ( tail == 0 )
			tail = allocation_size - 1;
		else
			--tail;
	}
} // End namespace

#endif

