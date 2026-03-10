#ifndef __CQUEUE_H
#define __CQUEUE_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

// a circular queue using an array
template <class T>
class CIRCULAR_QUEUE
{
public:
    CIRCULAR_QUEUE(WORD size = 0x100);
    ~CIRCULAR_QUEUE();
    //
    BOOL Get(T& object);
    BOOL Put(const T object);
    BOOL Get2(T& object);
    BOOL HasSpaceFor(WORD numOfObjects)
    {
        return ((Count+numOfObjects)<Size);
    }
    WORD GetCount(void)
    {
        return Count;
    }
private:
    T    *Data;
    WORD Size;
    WORD Front, Rear;
    WORD Count;
};

template <class T>
CIRCULAR_QUEUE<T>::CIRCULAR_QUEUE(WORD size)
{
    Size = size;
    Data = new T[Size];
    Rear = Front = Count = 0;
}

template <class T>
CIRCULAR_QUEUE<T>::~CIRCULAR_QUEUE()
{
    delete [] Data;
}

template <class T>
BOOL CIRCULAR_QUEUE<T>::Get(T& object)
{
    if (Front != Rear)
    {	 /* wrap around */
        object = Data[Front];
        Front = (Front+1)%Size;
        Count--;
        return TRUE;
    }
    return FALSE;
}
//
// this non destructive "Get2" let to get an object but don't extract it
//
template <class T>
BOOL CIRCULAR_QUEUE<T>::Get2(T& object)
{
    if (Front != Rear)
    {	 /* wrap around */
        object = Data[Front];
        // this the difference between Get and Get2
        // Front = (Front+1)%Size;
        return TRUE;
    }
    return FALSE;
}

template <class T>
BOOL CIRCULAR_QUEUE<T>::Put(const T object)
{
    if ((Rear+1)%Size != Front)
    { 	/* wrap around */
        Data[Rear] = object;
        Rear = (Rear+1)%Size;
        Count++;
        return TRUE;
    }
    return FALSE;
}

#endif // __CQUEUE_H
