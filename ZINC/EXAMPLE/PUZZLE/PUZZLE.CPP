//	PUZZLE.CPP (PUZZLE) - Sample fifteens puzzle using buttons.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// This sample program illustrates using buttons in a window.

#include <stdlib.h>
#include <time.h>
#include <ui_win.hpp>

class EXPORT PUZZLE : public UIW_WINDOW
{
public:
	PUZZLE(int left, int top);
	~PUZZLE(void) {}

	static EVENT_TYPE ButtonFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

	UIW_BUTTON *blankButton;
};

PUZZLE::PUZZLE(int left, int top) :
	UIW_WINDOW(left, top, 19, 8, WOF_NO_FLAGS, WOAF_NO_SIZE)
{
	int i, j;
	int count = 16;
	static char letter[2] = " ";											// BUG.IBM
	UIW_BUTTON *button;
	UI_LIST list;

	// Add border and title to the window.
	*this
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Restore", MNIF_RESTORE)
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("Mi&nimize", MNIF_MINIMIZE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_TITLE("Puzzle", WOF_JUSTIFY_CENTER);

	// Add the buttons to a temporary list.
	blankButton = new UIW_BUTTON(0, 0, 3, letter, BTF_NO_TOGGLE,
		WOF_JUSTIFY_CENTER, PUZZLE::ButtonFunction);

#if defined(ZIL_MSDOS) || defined(ZIL_MOTIF)
	UIF_FLAGS btFlags = BTF_NO_3D;
	blankButton->Information(SET_FLAGS, &btFlags, ID_BUTTON);
#endif

	(void *)&(list + blankButton);
	for (i = 0; i < 15; i++)
	{
		letter[0] = 'A' + i;
		(void *)&(list + new UIW_BUTTON(0, 0, 3, letter, BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER,
					PUZZLE::ButtonFunction));
	}

	// Randomly pick from the list and add the buttons to the window.
	struct tm *sTime;
	time_t lTime;
	time(&lTime);
	sTime = localtime(&lTime);
	int seed = sTime->tm_sec;
	srand(seed);
	for (i = 1; i <= 16; i += 4)
		for (j = 1; j <= 4; j += 1)
		{
			button = (UIW_BUTTON *)list.Get( (rand() &0x7FFF) / (32767 / count--) );	// BUG.SUNOS4
			(void *)&(list - button);
			button->relative.left = i;
			button->relative.top = j;
			button->relative.right = i + 3;
			button->relative.bottom = j;
			*this + button;
		}
}

EVENT_TYPE PUZZLE::ButtonFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Update only on exit from the field.
	if (ccode != L_SELECT)
		return (0);

	UIW_BUTTON *button = (UIW_BUTTON *)object;
	PUZZLE *puzzle = (PUZZLE *)button->parent;
	UIW_BUTTON *empty = puzzle->blankButton;

	// Calculate distance to blank button.
	int xDistance = abs(empty->true.left - button->true.left);
	int yDistance = abs(empty->true.top - button->true.top);

	// If distance is less than one square away, update text.
	if ((abs(xDistance) == puzzle->display->cellWidth * 4 && !yDistance) ||
		(abs(yDistance) == puzzle->display->cellHeight * 1 && !xDistance))
	{
		// Change the text associated with the buttons.
		empty->DataSet(button->DataGet());
		button->DataSet(" ");

#if defined(ZIL_MSDOS) || defined(ZIL_MOTIF)
		// Change the 3D look buttons.
		UIF_FLAGS btFlags = BTF_NO_3D;
		empty->Information(CLEAR_FLAGS, &btFlags, ID_BUTTON);
		button->Information(GET_FLAGS, &btFlags, ID_BUTTON);
		btFlags |= BTF_NO_3D;
		button->Information(SET_FLAGS, &btFlags, ID_BUTTON);

		event.type = S_CREATE;
		empty->Event(event);
		button->Event(event);

		// Redisplay the pieces.
		event.type = S_REDISPLAY;
		empty->Event(event);
		button->Event(event);
#endif
		puzzle->blankButton = button;
	}

	return (0);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

 	// Add the puzzle to the window manager.
	*windowManager + new PUZZLE(0, 0);

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
