//	VALIDATE.CPP (VALIDATE) - Validate example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

//  Define the validate class.
class EXPORT VALIDATE : public UIW_WINDOW
{
public:
	VALIDATE(int left, int top, int helpContext);
	virtual ~VALIDATE(void) {}

private:
	
	// Define the field pointers.
	UIW_BIGNUM *inputA;
	UIW_BIGNUM *inputB;
	UIW_BIGNUM *inputC;
	UIW_BIGNUM *totalField;

	//	The following is a description of the method used in this program
	//	to resolve the C++ code limitations that static class members do not
	//	have access to non static class members and only static class members
	//	can have their address passed as a parameter, as is
	//	needed for validate and user functions:

	//	The address of the static member function, Validation, is passed to
	//	the UIW_BIGNUM constructor when the UIW_BIGNUM objects are created.  When 
	//	called, this function is only a	dummy function to call the non-static
	//	function, MemberValidate, which	has access to all of the non-static
	//	members of VALIDATE.
	//	
	//	To help understand the function of the two lines in the dummy
	//	Validation function, note that these lines could be replaced with the
	//  following three lines:
	//			UIW_BIGNUM *number = (UIW_BIGNUM *)object;
	//			VALIDATE *validateThis = (VALIDATE *)number->parent;
	//          return validateThis->MemberValidate(object, event, ccode);
	//  	
	static EVENT_TYPE VALIDATE::Validation(UI_WINDOW_OBJECT *object, 
       UI_EVENT &event, EVENT_TYPE ccode)
	{
		return (((VALIDATE *)object->parent)->MemberValidate(object, event, ccode));
	}
	
	//  Define MemberValidate as a non static member function.
	virtual EVENT_TYPE MemberValidate(UI_WINDOW_OBJECT *object, UI_EVENT &event, 
       EVENT_TYPE ccode);
};

// Constructor.
VALIDATE::VALIDATE(int left, int top, int helpContext) :
	UIW_WINDOW(left, top, 40, 12, WOF_NO_FLAGS, WOAF_NO_FLAGS, helpContext)
{
	//  Initialize the value for the UIW_NUMERIC fields.
	UI_BIGNUM value;

	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ UIW_SYSTEM_BUTTON::Generic()
		+ new UIW_TITLE("Validate Numeric Function")
		+ new UIW_PROMPT(3, 0, "Accepted Input Range is 0 - 999", WOF_NO_FLAGS)
		+ new UIW_PROMPT(3, 2, "Input A", WOF_NO_FLAGS)
		+ (inputA = new UIW_BIGNUM(13, 2, 10, &value, NULL,
			NMF_DECIMAL(0), WOF_BORDER, VALIDATE::Validation))
		+ new UIW_PROMPT(25, 2, "Range Edit", WOF_NO_FLAGS)
		+ new UIW_PROMPT(6, 3, "+", WOF_NO_FLAGS)
		+ new UIW_PROMPT(3, 4, "Input B", WOF_NO_FLAGS)
		+ (inputB = new UIW_BIGNUM(13, 4, 10, &value, NULL,
			NMF_DECIMAL(0), WOF_BORDER, VALIDATE::Validation))
		+ new UIW_PROMPT(25, 4, "Val. Function", WOF_NO_FLAGS)
	 	+ new UIW_PROMPT(6, 5, "+", WOF_NO_FLAGS)
		+ new UIW_PROMPT(3, 6, "Input C", WOF_NO_FLAGS)
		+ (inputC = new UIW_BIGNUM(13, 6, 10, &value, NULL,
			NMF_DECIMAL(0), WOF_BORDER, VALIDATE::Validation))
		+ new UIW_PROMPT(25, 6, "Val. Function", WOF_NO_FLAGS)
		+ new UIW_PROMPT(6, 7, "=", WOF_NO_FLAGS)
		+ new UIW_PROMPT(13, 7, "__________", WOF_NO_FLAGS)
		+ new UIW_PROMPT(3, 8, "Total D", WOF_NO_FLAGS)
		+ (totalField = new UIW_BIGNUM(13, 8, 10, &value, NULL,
			NMF_DECIMAL(0), WOF_BORDER | WOF_NON_SELECTABLE));
}

//	Note that MemberValidate is the desired processing validate function 
//	called by the dummy function Validation.	The dummy validate function is
//  used to satisfy C++ code requirements. (For explanation, see comments 
//	preceeding the function Validation.)
EVENT_TYPE VALIDATE::MemberValidate(UI_WINDOW_OBJECT *object, UI_EVENT &, 
   EVENT_TYPE ccode)
{
	if (ccode == S_CURRENT)
		return (0);

	// Make sure the value entered is not outside the absolute range of UI_BIGNUM.
	if (object->Validate(FALSE) == NMI_OUT_OF_RANGE)						// BUG.2143
	{
		errorSystem->ReportError(totalField->windowManager, WOS_NO_STATUS,
			"The value entered is too large.  It must be in the range "
			"0 through 999.  "
			"This error window is created in the validate "
			"function.");
		return (0);
	}

	UI_BIGNUM *bignum = ((UIW_BIGNUM *)object)->DataGet();
	ibignum value;
	bignum->Export(&value);
		
	//  All fields are edited in this range test.  However, if a range
	//  test in UIW_BIGNUM failed, the error system would have
	//  been called without this member function being called.
	if (value >=  0 && value <= 999)
	{	
	    ibignum totalValue = 0;						   	
		bignum = inputA->DataGet();
		bignum->Export(&value);
		totalValue += value;

		bignum = inputB->DataGet(); 
		bignum->Export(&value);
		totalValue += value;

		bignum = inputC->DataGet();
		bignum->Export(&value);
		totalValue += value;

		bignum = totalField->DataGet();
		bignum->Import(totalValue);
		totalField->DataSet(bignum);
		
		return (0);
	}
	else
	{
		errorSystem->ReportError(totalField->windowManager, WOS_NO_STATUS,
			"%d is not valid.  The value must be in the range "
			"0 through 999.  "
			"This error window is created in the validate "
			"function.", value);

		value = 0;
		bignum->Import(value);
		((UIW_BIGNUM *)object)->DataSet(bignum);
		return (-1);
	}
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Add 2 user windows to the window manager.
	*windowManager
  		+ new VALIDATE(50, 0, NO_HELP_CONTEXT)
		+ new VALIDATE(0, 0, NO_HELP_CONTEXT);

	// Initialize the error window system.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	
	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}
