/*= Archive Library v2.12 ================================================
 |
 |  H\ALGAUGE.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  ALGAUGE.H and ALGAUGE.CPP are derived from Microsoft's zYzGauge
 |  file posted on CompuServe.
 |
 |  You have to include this header file if you are going to use the
 |  percentage gauge file.  The percentage gauge is not a C++ class, it
 |  is some straight C Windows code that Microsoft posted on Compuserve.
 |  The end user has to create a control and assign it class "ALGauge"
 |  in Resource Workshop or App Studio.  The initialization function
 |  defined in this header file then has to be called to set up the
 |  control.  After that, it is simply a matter of sending a number
 |  between 0 and 100 to the control, using the ALGaugeSetPosition
 |  command. The actual format used for this control is:
 |
 |   SendMessage(window_handle, ALGaugeSetPosition, ratio, 0)
 |
 +- Functions ------------------------------------------------------------
 |  ALGaugeInit()
 |
 +- Enumerated Types -----------------------------------------------------
 |  ALGaugeMessages
 |  ALGaugeOrientation
 |
 +=======================================================================*/

#ifndef __ALGAUGE_H__
#define __ALGAUGE_H__

// These are all the different messages you can send to
// an ALGauge object.  Our demo programs don't use any of these
// except ALGaugeSetPosition, so most of these aren't even tested.

// Tag public type
typedef enum _ALGaugeMessages {
    ALGaugeSetRange = WM_USER,
    ALGaugeGetRange,
    ALGaugeSetPosition,
    ALGaugeGetPosition,
    ALGaugeSetOrientation,
    ALGaugeGetOrientation,
    ALGaugeSetForegroundColor,
    ALGaugeGetForegroundColor,
    ALGaugeSetBackgroundColor,
    ALGaugeGetBackgroundColor,
    ALGaugeSetDeltaPosition
} ALGaugeMessages;

// Send one of these with ALGaugeSetOrientation, to determined which
// direction the flood fill progresses.

// Tag public type
typedef enum _ALGaugeOrientation {
    ALGaugeOrientLeftToRight,
    ALGaugeOrientRightToLeft,
    ALGaugeOrientBottomToTop,
    ALGaugeOrientTopToBottom
} ALGaugeOrientation;

// public function prototypes

#ifdef __cplusplus
extern "C"
#endif
AL_LINKAGE int AL_FUNCTION ALGaugeInit(HINSTANCE hInstance, HINSTANCE hPrevInstance);

#endif // #ifndef __ALGAUGE_H__
