//	PERSIST8.CPP (PERSIST) - Abstract read from disk into Microsoft Windows.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#define XTFUNCPROTO
#if defined(__DVX__)
struct _XDisplay;
struct XKeytrans;
struct _XrmHashBucketRec;
struct _XSQEvent;
#endif
extern "C"
{
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
}

// Persistent code definition files.
#include "draw.h"
#include "persist.hpp"

/*	_drawingArea is the blank widget we use for drawing. */
extern "C"
{
Widget _drawingArea;
}

GRAPHIC_OBJECT::JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
{
	{ ID_CIRCLE, 	CIRCLE::New },
	{ ID_RECTANGLE,	RECTANGLE::New },
	{ ID_TRIANGLE,	TRIANGLE::New },
	{ 0, NULL }		// end of array.
};

#define byteSwap(val)	((val) = (unsigned short)(val) >> 8 | (val) << 8)

GRAPHIC_OBJECT *GRAPHIC_OBJECT::New(FILE *file)
{
	// Read the type from disk.
	short type;
	int fileObject = fread((char *)&type, sizeof(type), 1, file);
	if (!fileObject)
		return (NULL);
	byteSwap(type);

	// Get the New pointer and create the object.
	for (int i = 0; _jumpTable[i].newFunction; i++)
		if (type == _jumpTable[i].type)
			return (_jumpTable[i].newFunction(file, L_SKIP_TYPE));
	return (NULL);
}

void ExposeCallback(Widget, XtPointer, XtPointer callData)
{

	if (((XmDrawingAreaCallbackStruct *)callData)->event->xexpose.count > 0)
		return;		// Ignore multiple exposure events.

	FILE *file = fopen("persist.dat", "r");
	if (file)
	{
		int fileObjects = 1;
		do
		{
			GRAPHIC_OBJECT *object = GRAPHIC_OBJECT::New(file);
			if (object)
			{
				object->Draw();
				delete object;
			}
			else
				fileObjects = 0;
		} while (fileObjects);
		fclose(file);
	}
	else
		printf("ERROR: The \"persist.dat\" file could not be found.\n");
}

main(int argc, char **argv)
{
	// Initialize the Motif application
	XtAppContext appContext;
	Arg args[5]; int nargs;

	Widget topLevel = XtAppInitialize (&appContext, "Persistence", NULL, 0,
#if (XmVersion <= 1001) && !defined(_IBM_RS6000)
		(Cardinal *)&argc,
#else
		&argc,
#endif
		argv, NULL, NULL, 0);

	// Create the drawing widget.
	nargs = 0;
	XtSetArg(args[nargs], XmNwidth, 500); nargs++;
	XtSetArg(args[nargs], XmNheight, 300); nargs++;
	_drawingArea = XmCreateDrawingArea(topLevel, "Persist", args, nargs);
	Display *xDisplay = XtDisplay(_drawingArea);
	GC xGc = XDefaultGC(xDisplay, DefaultScreen(xDisplay));
	XSetForeground(xDisplay, xGc, BlackPixelOfScreen(XtScreen(_drawingArea)));
	XtAddCallback(_drawingArea, XmNexposeCallback, ExposeCallback, NULL);
	XtManageChild(_drawingArea);

	// Realize the tree and enter the loop.
	XtRealizeWidget(topLevel);
	XtAppMainLoop(appContext);

    return (0);
}
