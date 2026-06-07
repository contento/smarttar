#include <iostream.h>
#include <classlib\dlistimp.h>

typedef BI_SDoubleListImp<long> LongList;
typedef BI_DoubleListIteratorImp<long> LongListIterator;

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

void main()
{
	LongList list;

    list.add(3);
    list.add(7);
    list.add(2);
    list.add(5);
    list.add(8);

    LongListIterator it(list);

    cout << "Items: " << g_Count(it) << endl;

    while (it)
    {
    	cout << it.current() << endl;

        ++it;
    }

    cout << "Reversed " << endl;
    it.restartAtTail();
    while (it)
    {
    	cout << it.current() << endl;

        --it;
    }
}
