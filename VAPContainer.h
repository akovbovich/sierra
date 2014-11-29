#include <algorithm>

/****************************************************************************/
// s_VolumeAtPriceV2

struct s_VolumeAtPriceV2
{
	int PriceInTicks;
	unsigned int Volume;
	unsigned int BidVolume;
	unsigned int AskVolume;
	unsigned int NumberOfTrades;

	s_VolumeAtPriceV2();
	s_VolumeAtPriceV2(unsigned int p_Volume, unsigned int p_BidVolume, unsigned int p_AskVolume, unsigned int p_NumberOfTrades);

	s_VolumeAtPriceV2& operator -= (const s_VolumeAtPriceV2& Rhs);
	s_VolumeAtPriceV2& operator += (const s_VolumeAtPriceV2& Rhs);

	bool operator < (const s_VolumeAtPriceV2& Rhs) const;
};

/*==========================================================================*/
inline s_VolumeAtPriceV2::s_VolumeAtPriceV2()
	//: Volume(0)
	//, BidVolume(0)
	//, AskVolume(0)
	//, NumberOfTrades(0)
{
}

/*==========================================================================*/
inline s_VolumeAtPriceV2::s_VolumeAtPriceV2(unsigned int p_Volume, unsigned int p_BidVolume, unsigned int p_AskVolume, unsigned int p_NumberOfTrades)
	:  Volume(p_Volume)
	, BidVolume(p_BidVolume)
	, AskVolume(p_AskVolume)
	, NumberOfTrades(p_NumberOfTrades)
{
}

/*==========================================================================*/
inline s_VolumeAtPriceV2& s_VolumeAtPriceV2::operator -= (const s_VolumeAtPriceV2& Rhs)
{
	if (Rhs.Volume > Volume)
		Volume = 0;
	else
		Volume -= Rhs.Volume;

	if (Rhs.BidVolume > BidVolume)
		BidVolume = 0;
	else
		BidVolume -= Rhs.BidVolume;

	if (Rhs.AskVolume > AskVolume)
		AskVolume = 0;
	else
		AskVolume -= Rhs.AskVolume;

	NumberOfTrades -= Rhs.NumberOfTrades;

	return *this;
}

/*==========================================================================*/
inline s_VolumeAtPriceV2& s_VolumeAtPriceV2::operator += (const s_VolumeAtPriceV2& Rhs)
{
	Volume += Rhs.Volume;
	BidVolume += Rhs.BidVolume;
	AskVolume += Rhs.AskVolume;
	NumberOfTrades += Rhs.NumberOfTrades;

	return *this;
}

/*==========================================================================*/
inline bool s_VolumeAtPriceV2::operator < (const s_VolumeAtPriceV2& Rhs) const
{
	if(this->PriceInTicks < Rhs.PriceInTicks)
		return true;
	else
		return false;
}

/****************************************************************************/
// c_VAPContainer

class c_VAPContainer
{
	public:
		c_VAPContainer(unsigned int InitialAllocationElements = 1024);
		c_VAPContainer(const c_VAPContainer& Src);
		~c_VAPContainer();

		c_VAPContainer& operator = (const c_VAPContainer& Rhs);

		void Clear();
		void ClearFromBarIndexToEnd(unsigned int BarIndex);

		unsigned int GetNumberOfBars() const;

		unsigned int GetSizeAtBarIndex(unsigned int BarIndex) const;

		template<typename T>
		static void InternalSwap(T& a, T& b);

		void Swap(c_VAPContainer& That);

		bool GetVAPElement(int PriceInTicks, unsigned int BarIndex, s_VolumeAtPriceV2** p_VAP, bool AllocateIfNeeded = false);
		bool GetVAPElementAtIndex(unsigned int BarIndex, int VAPDataIndex, s_VolumeAtPriceV2** p_VAP) const;
		bool GetVAPElementForPriceTickAndBarIndex(unsigned int BarIndex, int PriceInTicks, s_VolumeAtPriceV2** p_VAP);

		bool GetNextHigherVAPElement(unsigned int BarIndex, int& PriceInTicks, const s_VolumeAtPriceV2** p_VAP) const;
		bool GetNextLowerVAPElement(unsigned int BarIndex, int& PriceInTicks, const s_VolumeAtPriceV2** p_VAP) const;

		// This was the original function signature.
		bool GetHighAndLowPriceTicksForBarIndex(unsigned int BarIndex, int& r_HighPriceInTicks, int& r_LowPriceInTicks) const;
		// Overloaded function allows for optional return parameters.
		bool GetHighAndLowPriceTicksForBarIndex(unsigned int BarIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const;
		bool GetHighAndLowPriceTicksForBarIndexRange(unsigned int FirstBarIndex, unsigned int LastBarIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const;

		const s_VolumeAtPriceV2& GetVAPElementAtPrice(unsigned int BarIndex, int PriceInTicks) const;
		unsigned int GetVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const;
		unsigned int GetBidVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const;
		unsigned int GetAskVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const;

	private:
		void Initialize(unsigned int InitialAllocationElements = -1);

		unsigned int GetFirstDataElementIndexForBar(unsigned int BarIndex) const;
		bool GetBeginEndIndexesForBarIndex(unsigned int BarIndex, unsigned int* p_BeginIndex, unsigned int* p_EndIndex) const;

		void GetHighAndLowPriceTicks(unsigned int BeginIndex, unsigned int EndIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const;

		bool InitialAllocation();

		void SortContainer() const;

	private:
		// This points to an array with an element for each bar.  The value
		// at each element is the index of the first element in the VAP data
		// array that contains data for the bar.
		unsigned int* m_p_BarIndexToFirstElementIndexArray;

		// The number of elements that are currently used in the above array.
		unsigned int m_NumberOfBars;

		// The number of elements that are currently allocated for the above
		// array.  The elements allocated is usually greater than the
		// elements used so that the memory for the array does not need to be
		// reallocated for every new element.
		unsigned int m_NumElementsAllocated;

		// This points to an array of all of the Volume at Price data
		// elements that are stored in this container.
		s_VolumeAtPriceV2* m_p_VAPDataElements;

		// The number of elements that are currently used in the above array.
		unsigned int m_NumElementsUsed;

		// The number of elements that are currently allocated for the above
		// array.
		unsigned int m_NumBarsAllocated;

		// This is the number of bars that are initially allocated when the
		// arrays for the container are initially allocated.
		unsigned int m_InitialAllocationElements;

		// These members are used to determine how much, if any, new data has
		// been added since the container was last sorted.
		mutable int m_LastSortedBarIndex;
		mutable int m_LastSortedBarSize;
};

/*==========================================================================*/
inline c_VAPContainer::c_VAPContainer(unsigned int InitialAllocationElements)
{
	Initialize(InitialAllocationElements);
}

/*==========================================================================*/
inline c_VAPContainer::c_VAPContainer(const c_VAPContainer& Src)
{
	Initialize(Src.m_InitialAllocationElements);
}

/*==========================================================================*/
inline c_VAPContainer::~c_VAPContainer()
{
	Clear();
}

/*==========================================================================*/
inline c_VAPContainer& c_VAPContainer::operator = (const c_VAPContainer& Rhs)
{
	Clear();
	Initialize();

	return *this;
}

/*==========================================================================*/
inline void c_VAPContainer::Clear()
{
	if (m_p_VAPDataElements != NULL)
		free(m_p_VAPDataElements);

	if (m_p_BarIndexToFirstElementIndexArray != NULL)
		free(m_p_BarIndexToFirstElementIndexArray);

	Initialize();
}

/*==========================================================================*/
inline void c_VAPContainer::ClearFromBarIndexToEnd(unsigned int BarIndex)
{
	if (m_NumberOfBars == 0)
		return;

	if (BarIndex > m_NumberOfBars - 1)
		return;

	// Clear the VAP data array
	unsigned int BeginIndex = m_p_BarIndexToFirstElementIndexArray[BarIndex];

	memset(m_p_VAPDataElements + BeginIndex, 0, (m_NumElementsUsed - BeginIndex) * sizeof(s_VolumeAtPriceV2));
	m_NumElementsUsed = BeginIndex;

	// Clear the StartingVAPIndexesForBarIndexes array

	int NumElementsToErase = m_NumberOfBars - BarIndex;

	memset(m_p_BarIndexToFirstElementIndexArray + BarIndex, 0, NumElementsToErase * sizeof(unsigned int));

	m_NumberOfBars = BarIndex;
}

/*==========================================================================*/
inline unsigned int c_VAPContainer::GetNumberOfBars() const 
{
	return m_NumberOfBars;
}

/*============================================================================
	Returns the number of Volume at Price elements for the bar at the given
	BarIndex.
----------------------------------------------------------------------------*/
inline unsigned int c_VAPContainer::GetSizeAtBarIndex(unsigned int BarIndex) const
{
	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return 0;

	return EndIndex - BeginIndex;
}

/*============================================================================
	Static
----------------------------------------------------------------------------*/
template<typename T>
inline void c_VAPContainer::InternalSwap(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}

/*==========================================================================*/
inline void c_VAPContainer::Swap(c_VAPContainer& That)
{
	InternalSwap(m_p_BarIndexToFirstElementIndexArray, That.m_p_BarIndexToFirstElementIndexArray);
	InternalSwap(m_NumberOfBars, That.m_NumberOfBars);
	InternalSwap(m_NumBarsAllocated, That.m_NumBarsAllocated);
	InternalSwap(m_p_VAPDataElements, That.m_p_VAPDataElements);
	InternalSwap(m_NumElementsUsed, That.m_NumElementsUsed);
	InternalSwap(m_NumElementsAllocated, That.m_NumElementsAllocated);
}

/*============================================================================
	This function is for adding new elements or modifying existing elements.
----------------------------------------------------------------------------*/
inline bool c_VAPContainer::GetVAPElement(int PriceInTicks, unsigned int BarIndex, s_VolumeAtPriceV2 ** p_VAP, bool AllocateIfNeeded)
{
	*p_VAP = NULL;

	// Increase size of arrays to accommodate request if AllocatIfNeeded is
	// true.  This function will always increase arrays if AllocateIfNeeded
	// is true, there is no need to make a separate call to add elements.

	// When accessing Bar Indexes which are greater than allocated, they must
	// be done in sequence, one step at a time.  Otherwise, there will be an
	// error.

	// Make certain the arrays are initially allocated.
	if (!InitialAllocation())
		return false;

	// Do not allow the array to grow by more than one element.
	//if (BarIndex > m_NumberOfBars)
	//	return false;

	// We need to increase the array holding the pointers to the beginning of
	// data into the m_p_VAPDataForBars array, for each bar.
	if (BarIndex >= m_NumberOfBars)
	{
		if (!AllocateIfNeeded)
			return false;

		int NumberOfBarsAdded = BarIndex + 1 - m_NumberOfBars;

		m_NumberOfBars = BarIndex + 1;

		if (BarIndex >= m_NumBarsAllocated)
		{
			int OldSizeInElements = m_NumBarsAllocated;

			m_NumBarsAllocated = m_NumBarsAllocated * 2;

			if (BarIndex >= m_NumBarsAllocated)
				m_NumBarsAllocated = m_NumberOfBars;

			// If there is not enough available memory to expand the block to
			// the given size, the original block is left unchanged, and NULL
			// is returned.
			void* p_StartingVAPIndexesForBarIndexesArray
				= realloc(
					m_p_BarIndexToFirstElementIndexArray,
					m_NumBarsAllocated * sizeof(int)
				);

			if (p_StartingVAPIndexesForBarIndexesArray == NULL)
			{
				Clear();
				return false;
			}

			m_p_BarIndexToFirstElementIndexArray
				= static_cast<unsigned int*>(p_StartingVAPIndexesForBarIndexesArray);

			int SizeDifferenceInElements = m_NumBarsAllocated - OldSizeInElements;

			// Zero New elements.
			memset(m_p_BarIndexToFirstElementIndexArray+OldSizeInElements, 0, SizeDifferenceInElements * sizeof(int));
		}

		for (int NewIndex = (int)BarIndex; NewIndex > (int)BarIndex - NumberOfBarsAdded; --NewIndex)
		{
			int VAPIndex = (m_p_VAPDataElements + m_NumElementsUsed) - m_p_VAPDataElements;
			m_p_BarIndexToFirstElementIndexArray[NewIndex] = VAPIndex;
		}
	}

	if (!GetVAPElementForPriceTickAndBarIndex(BarIndex, PriceInTicks, p_VAP))
	{
		if (!AllocateIfNeeded)
			return false;

		// We cannot add because we are not at the last bar index segment in
		// the VAP array.
		if (BarIndex + 1 < m_NumberOfBars)
			return false;

		++m_NumElementsUsed;

		// The array needs to be increased in size.
		if (m_NumElementsUsed > m_NumElementsAllocated)
		{
			int OldSizeInElements = m_NumElementsAllocated;

			m_NumElementsAllocated = (int)(m_NumElementsAllocated * 1.25);

			// If there is not enough available memory to expand the block to
			// the given size, the original block is left unchanged, and NULL
			// is returned.
			void* p_VAPDataForBars
				= realloc(
					m_p_VAPDataElements,
					m_NumElementsAllocated * sizeof(s_VolumeAtPriceV2)
				);

			if (p_VAPDataForBars == NULL)
			{
				Clear();
				return false;
			}

			m_p_VAPDataElements = static_cast<s_VolumeAtPriceV2*>(p_VAPDataForBars);

			int SizeDifferenceInElements = 	m_NumElementsAllocated - OldSizeInElements;

			// Zero New elements.
			memset(m_p_VAPDataElements + OldSizeInElements, 0, SizeDifferenceInElements * sizeof(s_VolumeAtPriceV2));
		}

		m_p_VAPDataElements[m_NumElementsUsed - 1].PriceInTicks = PriceInTicks;

		*p_VAP = &m_p_VAPDataElements[m_NumElementsUsed - 1];
	}

	//_CrtCheckMemory();

	return true;
}

/*============================================================================
	VAPDataIndex is zero based, and is relative to the data for BarIndex.
----------------------------------------------------------------------------*/
inline bool c_VAPContainer::GetVAPElementAtIndex(unsigned int BarIndex, int VAPDataIndex, s_VolumeAtPriceV2** p_VAP) const
{
	*p_VAP = NULL;

	SortContainer();

	unsigned int VAPSegmentBeginIndex, VAPSegmentEndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &VAPSegmentBeginIndex, &VAPSegmentEndIndex))
		return false;

	int NumberOfElements = VAPSegmentEndIndex - VAPSegmentBeginIndex;

	if (VAPDataIndex < 0)
		VAPDataIndex = 0;
	if (VAPDataIndex > NumberOfElements - 1)
		VAPDataIndex = NumberOfElements - 1;

	*p_VAP = m_p_VAPDataElements + VAPSegmentBeginIndex + VAPDataIndex;

	return true;
}

/*============================================================================
	This function is for getting a specific Volume at Price element to modify.
----------------------------------------------------------------------------*/
inline bool c_VAPContainer::GetVAPElementForPriceTickAndBarIndex(unsigned int BarIndex, int PriceInTicks, s_VolumeAtPriceV2** p_VAP)
{
	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return false;

	for (unsigned int CurrentIndex = BeginIndex; CurrentIndex < EndIndex; ++CurrentIndex)
	{
		if (m_p_VAPDataElements[CurrentIndex].PriceInTicks == PriceInTicks)
		{
			*p_VAP = &m_p_VAPDataElements[CurrentIndex];
			return true;
		}
	}

	return false;
}
	
/*==========================================================================*/
inline bool c_VAPContainer::GetNextHigherVAPElement(unsigned int BarIndex, int& PriceInTicks, const s_VolumeAtPriceV2** p_VAP) const
{
	*p_VAP = NULL;

	// Set PriceInTicks to lowest value on first iteration.
	if (PriceInTicks == INT_MIN)
	{
		int LowPriceInTicks;
		if (!GetHighAndLowPriceTicksForBarIndex(BarIndex, NULL, &LowPriceInTicks))
			return false;

		PriceInTicks = LowPriceInTicks - 1;
	}

	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return false;

	int MinPriceInTicksDistanceFound = INT_MAX;
	unsigned int IndexOfNextHigherVAPElement = UINT_MAX;
	int PriceInTicksDistance;

	for (unsigned int CurrentIndex = BeginIndex; CurrentIndex < EndIndex; ++CurrentIndex)
	{
		PriceInTicksDistance = m_p_VAPDataElements[CurrentIndex].PriceInTicks - PriceInTicks;

		if (PriceInTicksDistance == 1)
		{
			*p_VAP = &m_p_VAPDataElements[CurrentIndex];

			// Adjust price value,  so the next value can be passed in again
			// to be used in a loop
			PriceInTicks = m_p_VAPDataElements[CurrentIndex].PriceInTicks;

			return true;
		}

		if (PriceInTicksDistance <= 0)
			continue;

		if (PriceInTicksDistance < MinPriceInTicksDistanceFound)
		{
			MinPriceInTicksDistanceFound = PriceInTicksDistance;
			IndexOfNextHigherVAPElement = CurrentIndex;
		}
	}

	// No higher element found.
	if (IndexOfNextHigherVAPElement == UINT_MAX)
		return false;

	*p_VAP = &m_p_VAPDataElements[IndexOfNextHigherVAPElement];
	PriceInTicks = m_p_VAPDataElements[IndexOfNextHigherVAPElement].PriceInTicks;

	return true;
}

/*==========================================================================*/
inline bool c_VAPContainer::GetNextLowerVAPElement(unsigned int BarIndex, int& PriceInTicks, const  s_VolumeAtPriceV2** p_VAP) const
{
	*p_VAP = NULL;

	// Set PriceInTicks to highest value on first iteration.
	if (PriceInTicks == INT_MIN)
	{
		int HighPriceInTicks;

		if (!GetHighAndLowPriceTicksForBarIndex(BarIndex, &HighPriceInTicks, NULL))
			return false;

		PriceInTicks = HighPriceInTicks + 1;
	}

	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return false;

	int MinPriceInTicksDistanceFound = INT_MAX;
	unsigned int IndexOfNextLowerVAPElement = UINT_MAX;
	int PriceInTicksDistance;

	for (unsigned int CurrentIndex = BeginIndex; CurrentIndex < EndIndex; ++CurrentIndex)
	{
		PriceInTicksDistance = PriceInTicks - m_p_VAPDataElements[CurrentIndex].PriceInTicks;

		if (PriceInTicksDistance == 1)
		{
			*p_VAP = &m_p_VAPDataElements[CurrentIndex];

			// Adjust price value, so the next value can be passed in again
			// to be used in a loop.
			PriceInTicks =m_p_VAPDataElements[CurrentIndex].PriceInTicks;

			return true;
		}

		if (PriceInTicksDistance <= 0)
			continue;

		if (PriceInTicksDistance < MinPriceInTicksDistanceFound)
		{
			MinPriceInTicksDistanceFound = PriceInTicksDistance;
			IndexOfNextLowerVAPElement = CurrentIndex;
		}
	}

	// No higher element found.
	if (IndexOfNextLowerVAPElement == UINT_MAX)
		return false;

	*p_VAP = &m_p_VAPDataElements[IndexOfNextLowerVAPElement];
	PriceInTicks = m_p_VAPDataElements[IndexOfNextLowerVAPElement].PriceInTicks;

	return true;
}

/*==========================================================================*/
inline bool c_VAPContainer::GetHighAndLowPriceTicksForBarIndex(unsigned int BarIndex, int& r_HighPriceInTicks, int& r_LowPriceInTicks) const
{
	return GetHighAndLowPriceTicksForBarIndex(BarIndex, &r_HighPriceInTicks, &r_LowPriceInTicks);
}

/*==========================================================================*/
inline bool c_VAPContainer::GetHighAndLowPriceTicksForBarIndex(unsigned int BarIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const
{
	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return false;

	GetHighAndLowPriceTicks(BeginIndex, EndIndex, p_HighPriceInTicks, p_LowPriceInTicks);

	return true;
}

/*==========================================================================*/
inline bool c_VAPContainer::GetHighAndLowPriceTicksForBarIndexRange(unsigned int FirstBarIndex, unsigned int LastBarIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const
{
	unsigned int BeginIndex = GetFirstDataElementIndexForBar(FirstBarIndex);
	unsigned int EndIndex = GetFirstDataElementIndexForBar(LastBarIndex + 1);

	if (BeginIndex >= EndIndex)
		return false;

	GetHighAndLowPriceTicks(BeginIndex, EndIndex, p_HighPriceInTicks, p_LowPriceInTicks);

	return true;
}

/*============================================================================
	Returns an element with all zeros if there is no element for the
	requested BarIndex and PriceInTicks.
----------------------------------------------------------------------------*/
inline const s_VolumeAtPriceV2& c_VAPContainer::GetVAPElementAtPrice(unsigned int BarIndex, int PriceInTicks) const
{
	static const s_VolumeAtPriceV2 ZeroVAP(0, 0, 0, 0);

	unsigned int BeginIndex, EndIndex;
	if (!GetBeginEndIndexesForBarIndex(BarIndex, &BeginIndex, &EndIndex))
		return ZeroVAP;

	for (unsigned int CurrentIndex = BeginIndex; CurrentIndex < EndIndex; ++CurrentIndex)
	{
		if (m_p_VAPDataElements[CurrentIndex].PriceInTicks == PriceInTicks)
			return m_p_VAPDataElements[CurrentIndex];
	}

	return ZeroVAP;
}

/*============================================================================
	Returns 0 if there is no element for the requested BarIndex and
	PriceInTicks.
----------------------------------------------------------------------------*/
inline unsigned int c_VAPContainer::GetVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const
{
	return GetVAPElementAtPrice(BarIndex, PriceInTicks).Volume;
}

/*============================================================================
	Returns 0 if there is no element for the requested BarIndex and
	PriceInTicks.
----------------------------------------------------------------------------*/
inline unsigned int c_VAPContainer::GetBidVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const
{
	return GetVAPElementAtPrice(BarIndex, PriceInTicks).BidVolume;
}

/*============================================================================
	Returns 0 if there is no element for the requested BarIndex and
	PriceInTicks.
----------------------------------------------------------------------------*/
inline unsigned int c_VAPContainer::GetAskVolumeAtPrice(unsigned int BarIndex, int PriceInTicks) const
{
	return GetVAPElementAtPrice(BarIndex, PriceInTicks).AskVolume;
}

/*==========================================================================*/
inline void c_VAPContainer::Initialize(unsigned int InitialAllocationElements)
{
	m_p_VAPDataElements = NULL;
	m_p_BarIndexToFirstElementIndexArray = NULL;
	m_NumBarsAllocated = 0;
	m_NumberOfBars = 0;

	if (InitialAllocationElements != -1)
		m_InitialAllocationElements = InitialAllocationElements;

	m_NumElementsUsed = 0;
	m_NumElementsAllocated = 0;

	m_LastSortedBarIndex = -1;
	m_LastSortedBarSize = 0;
}

/*============================================================================
	Returns the index of the first element in the VAP data array for the bar 
	at the given BarIndex.  If the bar is not within the container, the 'end'
	index, or the size of the VAP data array, is returned.
----------------------------------------------------------------------------*/
inline unsigned int c_VAPContainer::GetFirstDataElementIndexForBar(unsigned int BarIndex) const
{
	if (BarIndex >= m_NumberOfBars)
		return m_NumElementsUsed;

	if (m_p_BarIndexToFirstElementIndexArray == NULL)
		return m_NumElementsUsed;

	return m_p_BarIndexToFirstElementIndexArray[BarIndex];
}

/*============================================================================
	Sets the given *p_BeginIndex and *p_EndIndex parameters to the indexes of
	the first element and one index beyond the last element of VAP data for
	the bar at the given BarIndex.  *p_BeginIndex and *p_EndIndex are only
	set when the function returns true, and the function only returns true if
	there is data for the bar.
----------------------------------------------------------------------------*/
inline bool c_VAPContainer::GetBeginEndIndexesForBarIndex(unsigned int BarIndex, unsigned int* p_BeginIndex, unsigned int* p_EndIndex) const
{
	unsigned int BeginIndex = GetFirstDataElementIndexForBar(BarIndex);
	unsigned int EndIndex = GetFirstDataElementIndexForBar(BarIndex + 1);

	if (BeginIndex >= EndIndex)
		return false;

	if (p_BeginIndex != NULL)
		*p_BeginIndex = BeginIndex;

	if (p_EndIndex != NULL)
		*p_EndIndex = EndIndex;

	return true;
}

/*==========================================================================*/
inline void c_VAPContainer::GetHighAndLowPriceTicks(unsigned int BeginIndex, unsigned int EndIndex, int* p_HighPriceInTicks, int* p_LowPriceInTicks) const
{
	if (m_p_VAPDataElements == NULL)
		return;

	if (p_HighPriceInTicks == NULL && p_LowPriceInTicks == NULL)
		return;

	if (EndIndex > m_NumElementsUsed)
		EndIndex = m_NumElementsUsed;

	if (p_HighPriceInTicks != NULL)
		*p_HighPriceInTicks = INT_MIN;

	if (p_LowPriceInTicks != NULL)
		*p_LowPriceInTicks = INT_MAX;

	for (unsigned int CurrentIndex = BeginIndex; CurrentIndex < EndIndex; ++CurrentIndex)
	{
		int PriceInTicks = m_p_VAPDataElements[CurrentIndex].PriceInTicks;

		if (p_HighPriceInTicks != NULL && *p_HighPriceInTicks < PriceInTicks)
			*p_HighPriceInTicks = PriceInTicks;

		if (p_LowPriceInTicks != NULL && *p_LowPriceInTicks > PriceInTicks)
			*p_LowPriceInTicks = PriceInTicks;
	}
}

/*==========================================================================*/
inline bool c_VAPContainer::InitialAllocation()
{
	if (m_p_BarIndexToFirstElementIndexArray == NULL)
	{
		m_NumBarsAllocated = m_InitialAllocationElements;

		m_p_BarIndexToFirstElementIndexArray
			= static_cast<unsigned int*>(
				realloc(
					m_p_BarIndexToFirstElementIndexArray,
					m_NumBarsAllocated * sizeof(int)
				)
			);

		if (m_p_BarIndexToFirstElementIndexArray == NULL)
			return false;

		// Zero New elements.
		memset(m_p_BarIndexToFirstElementIndexArray, 0, m_NumBarsAllocated  * sizeof(int));
	}

	if (m_p_VAPDataElements == NULL)
	{
		m_NumElementsAllocated = m_InitialAllocationElements*10;

		m_p_VAPDataElements
			= static_cast<s_VolumeAtPriceV2*>(
				realloc(
					m_p_VAPDataElements,
					m_NumElementsAllocated * sizeof(s_VolumeAtPriceV2)
				)
			);

		if (m_p_VAPDataElements == NULL)
			return false;

		// Zero New elements.
		memset(m_p_VAPDataElements, 0, m_NumElementsAllocated * sizeof(s_VolumeAtPriceV2));
	}

	return true;
}

/*============================================================================
	This function should be called any time before accessing the data which
	requires a sorted order.  This will only sort the data that needs to be
	sorted.  If nothing needs to be done, then it will not do anything.
----------------------------------------------------------------------------*/
inline void c_VAPContainer::SortContainer() const
{
	// Determine if the container needs to be sorted.

	if (m_NumberOfBars == 0)
		return;

	if (m_LastSortedBarIndex + 1 == m_NumberOfBars)
	{
		unsigned int SizeAtIndex = GetSizeAtBarIndex(m_LastSortedBarIndex);

		if (m_LastSortedBarSize == SizeAtIndex)
			return;
	}


	// Sort the container.

	if (m_LastSortedBarIndex == -1)
		m_LastSortedBarIndex = 0;

	for (unsigned int BarIndex = m_LastSortedBarIndex; BarIndex < m_NumberOfBars; ++BarIndex)
	{
		unsigned int VAPSegmentBeginIndex, VAPSegmentEndIndex;
		if (!GetBeginEndIndexesForBarIndex(BarIndex, &VAPSegmentBeginIndex, &VAPSegmentEndIndex))
			continue;

		s_VolumeAtPriceV2* p_VAPSegmentBegin = m_p_VAPDataElements + VAPSegmentBeginIndex;
		s_VolumeAtPriceV2* p_VAPSegmentEnd = m_p_VAPDataElements + VAPSegmentEndIndex;

		std::sort(p_VAPSegmentBegin, p_VAPSegmentEnd);
	}

	m_LastSortedBarSize = GetSizeAtBarIndex(m_NumberOfBars - 1);
	m_LastSortedBarIndex = m_NumberOfBars - 1;
}

/*==========================================================================*/
