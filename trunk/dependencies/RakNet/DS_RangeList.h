#ifndef __RANGE_LIST_H
#define __RANGE_LIST_H

#include "DS_OrderedList.h"
#include "BitStream.h"
#include "RakMemoryOverride.h"
#include "RakAssert.h"

namespace DataStructures
{
    template <class range_type>
	struct RangeNode
    {
        RangeNode() {}
        ~RangeNode() {}
        RangeNode(range_type min, range_type max) {minIndex=min; maxIndex=max;}
        range_type minIndex;
        range_type maxIndex;
    };


    template <class range_type>
    int RangeNodeComp(const range_type &a, const RangeNode<range_type> &b)
    {
        if (a<b.minIndex)
            return -1;
        if (a==b.minIndex)
            return 0;
        return 1;
    }

	template <class range_type>
	class RAK_DLL_EXPORT RangeList
	{
	public:
		RangeList();
		~RangeList();
		void Insert(range_type index);
		void Clear(void);
		unsigned Size(void) const;
		unsigned RangeSum(void) const;
		BitSize_t Serialize(RakNet::BitStream *in, BitSize_t maxBits, bool clearSerialized);
		bool Deserialize(RakNet::BitStream *out);

		DataStructures::OrderedList<range_type, RangeNode<range_type> , RangeNodeComp<range_type> > ranges;
	};

	template <class range_type>
	BitSize_t RangeList<range_type>::Serialize(RakNet::BitStream *in, BitSize_t maxBits, bool clearSerialized)
	{
		RakAssert(ranges.Size() < (unsigned short)-1);
		RakNet::BitStream tempBS;
		BitSize_t bitsWritten;
		unsigned short countWritten;
		unsigned i;
		countWritten=0;
		bitsWritten=0;
		for (i=0; i < ranges.Size(); i++)
		{
			if ((int)sizeof(unsigned short)*8+bitsWritten+(int)sizeof(range_type)*8*2+1>maxBits)
				break;
			tempBS.Write(ranges[i].minIndex==ranges[i].maxIndex);
			tempBS.Write(ranges[i].minIndex);
			bitsWritten+=sizeof(range_type)*8+1;
			if (ranges[i].minIndex!=ranges[i].maxIndex)
			{
				tempBS.Write(ranges[i].maxIndex);
				bitsWritten+=sizeof(range_type)*8;
			}
			countWritten++;
		}

		BitSize_t before=in->GetWriteOffset();
		in->WriteCompressed(countWritten);
		bitsWritten+=in->GetWriteOffset()-before;
	//	RAKNET_DEBUG_PRINTF("%i ", in->GetNumberOfBitsUsed());
		in->Write(&tempBS, tempBS.GetNumberOfBitsUsed());
	//	RAKNET_DEBUG_PRINTF("%i %i \n", tempBS.GetNumberOfBitsUsed(),in->GetNumberOfBitsUsed());

		if (clearSerialized && countWritten)
		{
			unsigned rangeSize=ranges.Size();
			for (i=0; i < rangeSize-countWritten; i++)
			{
				ranges[i]=ranges[i+countWritten];
			}
			ranges.RemoveFromEnd(countWritten);
		}

		return bitsWritten;
	}
	template <class range_type>
	bool RangeList<range_type>::Deserialize(RakNet::BitStream *out)
	{
		ranges.Clear();
		unsigned short count;
		out->ReadCompressed(count);
		unsigned short i;
		range_type min,max;
		bool maxEqualToMin=false;

		for (i=0; i < count; i++)
		{
			out->Read(maxEqualToMin);
			if (out->Read(min)==false)
				return false;
			if (maxEqualToMin==false)
			{
				if (out->Read(max)==false)
					return false;
				if (max<min)
					return false;
			}
			else
				max=min;

			ranges.InsertAtEnd(RangeNode<range_type>(min,max));
		}
		return true;
	}

	template <class range_type>
	RangeList<range_type>::RangeList()
	{
		RangeNodeComp<range_type>(0, RangeNode<range_type>());
	}

	template <class range_type>
	RangeList<range_type>::~RangeList()
	{
		Clear();
	}

	template <class range_type>
	void RangeList<range_type>::Insert(range_type index)
	{
		if (ranges.Size()==0)
		{
			ranges.Insert(index, RangeNode<range_type>(index, index), true);
			return;
		}

		bool objectExists;
		unsigned insertionIndex=ranges.GetIndexFromKey(index, &objectExists);
		if (insertionIndex==ranges.Size())
		{
			if (index == ranges[insertionIndex-1].maxIndex+1)
				ranges[insertionIndex-1].maxIndex++;
			else if (index > ranges[insertionIndex-1].maxIndex+1)
			{
				// Insert at end
				ranges.Insert(index, RangeNode<range_type>(index, index), true);
			}

			return;
		}

		if (index < ranges[insertionIndex].minIndex-1)
		{
			// Insert here
			ranges.InsertAtIndex(RangeNode<range_type>(index, index), insertionIndex);

			return;
		}
		else if (index == ranges[insertionIndex].minIndex-1)
		{
			// Decrease minIndex and join left
			ranges[insertionIndex].minIndex--;
			if (insertionIndex>0 && ranges[insertionIndex-1].maxIndex+1==ranges[insertionIndex].minIndex)
			{
				ranges[insertionIndex-1].maxIndex=ranges[insertionIndex].maxIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}

			return;
		}
		else if (index >= ranges[insertionIndex].minIndex && index <= ranges[insertionIndex].maxIndex)
		{
			// Already exists
			return;
		}
		else if (index == ranges[insertionIndex].maxIndex+1)
		{
			// Increase maxIndex and join right
			ranges[insertionIndex].maxIndex++;
			if (insertionIndex<ranges.Size()-1 && ranges[insertionIndex+1].minIndex==ranges[insertionIndex].maxIndex+1)
			{
				ranges[insertionIndex+1].minIndex=ranges[insertionIndex].minIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}

			return;
		}
	}

	template <class range_type>
	void RangeList<range_type>::Clear(void)
	{
		ranges.Clear();
	}

	template <class range_type>
	unsigned RangeList<range_type>::Size(void) const
	{
		return ranges.Size();
	}

	template <class range_type>
	unsigned RangeList<range_type>::RangeSum(void) const
	{
		unsigned sum=0,i;
		for (i=0; i < ranges.Size(); i++)
			sum+=ranges[i].maxIndex-ranges[i].minIndex+1;
        return sum;
	}

}

#endif
