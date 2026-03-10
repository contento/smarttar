#ifndef __ST_BIDS_H
#define __ST_BIDS_H

#if !defined(__ARRAYS_H)
#include <classlib\arrays.h>
#endif // !defined(__ARRAYS_H)

#if !defined(__DLISTIMP_H)
#include <classlib\dlistimp.h>
#endif // !defined(__DLISTIMP_H)

typedef BI_ArrayAsVector<long> LArray;
typedef BI_ArrayAsVectorIterator<long> LArrayIterator;

typedef BI_SArrayAsVector<long> LongSortedArray;
typedef BI_SArrayAsVectorIterator<long> LongSortedArrayIterator;

typedef BI_SDoubleListImp<long> LSDList;
typedef BI_DoubleListIteratorImp<long> LSDListIterator;


/////////////////////////////////////////////////////////////////////
// algorithms
/////////////////////////////

template <class TIterator>
int g_Count(TIterator it)
{
    it.restart();

    int nCount = 0;
    while (it)
    {
    	++nCount;
    	++it;
    }

    it.restart();

    return nCount;
}

#endif // __ST_BIDS_H
