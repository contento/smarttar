//	DRAW.H (PERSIST) - Draw function prototypes (compiler specific).
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__cplusplus) && !defined(__OS2__)
extern "C" {
#endif

extern void InitializeDisplay(void);
extern void RestoreDisplay(void);

extern void DrawCircle(short column, short line, short radius);
extern void DrawRectangle(short left, short top, short right, short bottom);
extern void DrawTriangle(short *triangle);

#if defined(__cplusplus) && !defined(__OS2__)
}
#endif

#if (defined(_MSC_VER) && !defined(WIN32)) || defined(__IBMCPP__) 
#	define	getch	_getch
#endif
