#ifndef __SSAVER_H
#define __SSAVER_H

#if !defined(UI_EVT_HPP)
#include <ui_evt.hpp>
#endif

class SSAVER: public UI_DEVICE
{
public:
    SSAVER(void);
    virtual ~SSAVER(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    //
    int IsActive(void)
    {
        return Active;
    }
protected:
    virtual void Poll(void);
private:
    int Active;
    int Time;
    int Left;
    int Top;
    int MaxCols;
    int MaxLines;
    int AsyncTrigger;
    //
    UCHAR *Bmp;
    short BmpWidth;
    short BmpHeight;
    //
    void InitSaver(void);
    void DoSaver(void);
};

#if defined(__TEST__)
struct CFG
{
    int SS_ID;
    int SS_TIME;
};
#endif

#endif // __SSAVER_H