#ifndef __MUTEX_H
#define __MUTEX_H

#define DECLARE_MUTEX(x) 					\
class x##Mutex 								\
{                        					\
public:                     				\
	x##Mutex::x##Mutex();             		\
	x##Mutex::~x##Mutex();         			\
private:									\
	static unsigned s_mutex;				\
};

DECLARE_MUTEX(RTReceiptQueue);
DECLARE_MUTEX(RTBoothClusters);

#endif // ifndef __RT_ENG_H
