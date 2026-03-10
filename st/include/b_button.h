#ifndef __B_BUTTON_H
#define __B_BUTTON_H
//
// note: these classes were introduced here to correct the problem concerning
//		 with the buttons in the tool bar and the groups if we were using
//       WOF_MINICELL without BTF_AUTO_SIZE
//

#ifdef __BTN__
const int TBUTTON_HEIGTH = 18;
class EXPORT UIW_TBUTTON : public UIW_BUTTON
{ // just for WOF_MINICELL buttons
public:
    UIW_TBUTTON(int left, int top, int width, char *text, BTF_FLAGS btFlags = BTF_NO_TOGGLE | BTF_AUTO_SIZE,
                WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER, USER_FUNCTION userFunction = NULL, EVENT_TYPE value = 0, char * bitmapName = NULL)
            : UIW_BUTTON(left, top+TBUTTON_HEIGTH, width, text, btFlags, woFlags, userFunction, value, bitmapName)
    {
        relative.top -= TBUTTON_HEIGTH;
    }
};
#else
const int TBUTTON_HEIGTH = 18;
#define UIW_TBUTTON UIW_BUTTON
#endif // __BTN__

#ifdef __BTN__
const int GBUTTON_HEIGHT = 8;
class EXPORT UIW_GBUTTON : public UIW_BUTTON
{ // just for WOF_MINICELL buttons
public:
    UIW_GBUTTON(int left, int top, int width, char *text, BTF_FLAGS btFlags = BTF_NO_TOGGLE | BTF_AUTO_SIZE,
                WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER, USER_FUNCTION userFunction = NULL, EVENT_TYPE value = 0, char * bitmapName = NULL)
            : UIW_BUTTON(left, top+GBUTTON_HEIGHT, width, text, btFlags, woFlags, userFunction, value, bitmapName)
    {
        relative.top -= GBUTTON_HEIGHT;
    }
};
#else
const int GBUTTON_HEIGHT = 8;
#define UIW_GBUTTON UIW_BUTTON
#endif // __BTN__

// end of correction !!!

#endif // __B_BUTTON_H