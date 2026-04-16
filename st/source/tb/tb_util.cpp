//
// [ TB_UTIL.CPP ]
//

#include "stdst.h"

#include <toolbar.h>
#include <events.h>
#include <hb_ids.h>
#include <calc.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

// --------------------------------------------------------------------------
//								UIW_CALC
// --------------------------------------------------------------------------

UIW_CALC::UIW_CALC(void) : UIW_WINDOW("W_CALC", defaultStorage)
{
	UIW_STRING *wOperation = (UIW_STRING *) this->Get("W_OP");
    wOperation->userFunction = compute;
    this->helpContext = H_CALC;
    windowManager->Center(this);
}

EVENT_TYPE UIW_CALC::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        eventManager->Put(UI_EVENT(S_CLOSE, 0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

EVENT_TYPE UIW_CALC::compute(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT && ccode != S_NON_CURRENT)
        return (ccode);
    CALC calc;
    UIW_STRING *wOperation = (UIW_STRING *) object->parent->Get("W_OP");
    UIW_BIGNUM *wResult  = (UIW_BIGNUM *) object->parent->Get("W_RES");
    UI_BIGNUM result(0.0);
    result.Import(calc.Evaluate(wOperation->DataGet()));
    wResult->DataSet(&result);
    return (ccode);
}

