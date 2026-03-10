/* INT33.C - Sample INT 33H protected mouse driver for 286|DOS-Extender */

/************************************************************************/
/*	Copyright (C) 1986-1990 Phar Lap Software, Inc.			*/
/*	Unpublished - rights reserved under the Copyright Laws of the	*/
/*	United States.  Use, duplication, or disclosure by the 		*/
/*	Government is subject to restrictions as set forth in 		*/
/*	subparagraph (c)(1)(ii) of the Rights in Technical Data and 	*/
/*	Computer Software clause at 252.227-7013.			*/
/*	Phar Lap Software, Inc., 60 Aberdeen Ave., Cambridge, MA 02138	*/
/************************************************************************/

#include <phapi.h>


/*

Basic constants

*/

#define NULL 0
#define TRUE 1
#define FALSE 0


/*

Far pointer type and macros

*/

typedef void _far *FARPTR;

#define FP_OFF(fp) ((UINT)(ULONG)(fp))
#define FP_SEL(fp) ((UINT)((ULONG)(fp) >> 16))
#define FP_MAKE(off, sel) (((_segment)(sel)) :> (void _based(void) *)(off))


/*

Register macros

*/

#define AX (regs.ax)
#define BX (regs.bx)
#define CX (regs.cx)
#define DX (regs.dx)
#define SI (regs.si)
#define DI (regs.di)
#define BP (regs.bp)
#define SP (regs.sp)
#define DS (regs.ds)
#define ES (regs.es)


/*

Global data

*/

PIHANDLER old_int33_handp = {NULL};  	/* Address of old INT 33H handler */

extern USHORT _far real_size;		/* Size of the real mode segment */

USHORT real_para = {0};			/* Paragraph address of the real
					   mode segment */

USHORT real_sel = {0};			/* Protected mode selector of the
					   real mode segment */

extern USHORT xfer_rm_seg;		/* Real mode paragraph address of
					   the 286|DOS-Extender transfer
					   buffer */

extern USHORT xfer_pm_sel;		/* Protected mode selector of the
					   286|DOS-Extender transfer 
					   buffer */

extern USHORT xfer_size;		/* Size of the transfer buffer */

extern FARPTR passup_protp;	     	/* Pass up protected mode pointer */

#define PASSUP_PROTP (*(FARPTR *)FP_MAKE(FP_OFF(&passup_protp), real_sel))

/*

Function declarations

*/

extern void _interrupt int33_hand();
extern INT mousav_size();
extern passup_real();


/*

int33_init - INT 33H initialization routine

*/

_loadds int33_init()

{
	
	INT rc;				/* Return code */
 
	/* Move the real mode segment down to real mode memory */

	rc = DosAllocRealSeg(real_size, &real_para, &real_sel);
	if(rc != 0)
		return 0;
	memcpy(FP_MAKE(0, real_sel), FP_MAKE(0, FP_SEL(passup_real)),
	       real_size);

	/* Take over protected mode INT 33H */

	rc = DosSetProtVec(0x33, (PIHANDLER)int33_hand, &old_int33_handp);
	if(rc != 0)
		return 0;

	/* Return */

	return 1;

}


/*

int33_hand - Mouse driver INT 33H handler

*/

void _interrupt int33_hand(REGS16 regs)

{

	INT rc;				/* Return code */
	FARPTR old_passup_protp;	/* Old pass-up protected mode
					   pointer */
	INT n;				/* Size of the mouse save area */
 	USHORT oDX;			/* Original DX */
 	USHORT oES;			/* Original ES */

	/* Switch on the function code in AX */

	oES = ES;
	oDX = DX;
	switch(AX)
	{

	/* Most functions are passed down directly to real mode */

	default:
		DosRealIntr(0x33, &regs, 0L, 0);
		break;

	/* Set pointer shape */

	case 0x09:
		memcpy(FP_MAKE(0, xfer_pm_sel), FP_MAKE(oDX, oES), 64);
		DX = 0;
		ES = xfer_rm_seg;
		DosRealIntr(0x33, &regs, 0L, 0);
		DX = oDX;
		break;

	/* Set mouse even handler */

	case 0x0C:
		PASSUP_PROTP = FP_MAKE(oDX, oES);
		DX = FP_OFF(passup_real);
		ES = real_para;
		DosRealIntr(0x33, &regs, 0L, 0);
		DX = oDX;
		break;

	/* Swap mouse even handler */

	case 0x14:
		old_passup_protp = PASSUP_PROTP;
		PASSUP_PROTP = FP_MAKE(oDX, oES);
		DX = FP_OFF(passup_real);
		ES = real_para;
		DosRealIntr(0x33, &regs, 0L, 0);
		DX = FP_OFF(old_passup_protp);
		oES = FP_SEL(old_passup_protp);
		break;

	/* Save mouse driver state */

	case 0x16:
		n = mousav_size();
		if(n > xfer_size)
			break;
		ES = xfer_rm_seg;
		DX = 0;
		DosRealIntr(0x33, &regs, 0L, 0);
		memcpy(FP_MAKE(oDX, oES), FP_MAKE(0, xfer_pm_sel), n);
		DX = oDX;
		break;

	/* Restore mouse driver state */

	case 0x17:
		n = mousav_size();
		if(n > xfer_size)
			break;
		memcpy(FP_MAKE(0, xfer_pm_sel), FP_MAKE(oDX, oES), n);
		ES = xfer_rm_seg;
		DX = 0;
		DosRealIntr(0x33, &regs, 0L, 0);
		DX = oDX;
		break;
	
	/* Set alternate mouse event handler */

	case 0x18:
		break;
	
	/* Get alternate mouse event handler */

	case 0x19:
		break;

	}
	ES = oES;

	/* Return */
	
ret1:	return;

}


/*

mousav_size - Get size of the mouse save area

*/

INT mousav_size()

{

	REGS16 regs;		/* Registers */
	
	regs.ax = 0x15;
	memset(&regs, 0, sizeof(regs));
	DosRealIntr(0x33, &regs, 0L, 0);

	return regs.bx;

}
