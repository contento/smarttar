//
// [ TEST.CPP ]
//

#include "stdst.h"

#include <rt_eng.h>
#include <conio.h>

const int PHONE_COL      = 1;
const int TIME_COL       = 20;
const int PAID_FLAG_COL  = 30;
const int PAID_TIME_COL  = 40;
const int PAID_VALUE_COL = 50;

CFG *g_cfg;
WORD g_numOfSemWait;

int main()
{
	g_cfg = new CFG;
	g_cfg->Load();

	RT_ENGINE rtEngine(g_cfg->CLUSTERS);

	clrscr();
	_setcursortype(_NOCURSOR);
	cprintf("SmartTar Real Time Engine 3.00.  MicroDise¤o Ltda.\n");

	gotoxy(PHONE_COL     , 3);
	cprintf("Phone Number");
	gotoxy(TIME_COL      , 3);
	cprintf("Time");
	gotoxy(PAID_FLAG_COL , 3);
	cprintf("Pre-Flag");
	gotoxy(PAID_TIME_COL , 3);
	cprintf("Pre-Time");
	gotoxy(PAID_VALUE_COL, 3);
	cprintf("Pre-Paid");

	gotoxy(1, 23);
	cprintf("Press any key to exit...");
	for (;;)
	{
		if (kbhit())
		{
			if (!getch())
				getch();
			break;
		}
		else
		{
			rtEngine.Show();
		}
	}
	_setcursortype(_NORMALCURSOR);
	//
	g_cfg->Save();
	delete g_cfg;
	return 0;
}

void RT_ENGINE::Show(void)
{
	register y;
	const char *PAD = "________________";
	for (register cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; cNum++)
	{
		for (register bNum=0; bNum < CLUSTER_SIZE; bNum++)
		{
			y = 4+cNum*CLUSTER_SIZE+bNum+1*cNum;
			// phone
			gotoxy(PHONE_COL, y);
			if (Clusters[cNum].Phones[bNum][0])
				cprintf("%s", Clusters[cNum].Phones[bNum]);
			else
				cprintf("%s", PAD);
			// time
			gotoxy(TIME_COL, y);
			cprintf("%lu", Clusters[cNum].ElapsedCounts[bNum]/1000);
			// PrePaid flag
			gotoxy(PAID_FLAG_COL, y);
			cprintf("%s", (Clusters[cNum].PrePaid[bNum])?"TRUE":"FALSE");
			// PrePaid Time
			gotoxy(PAID_TIME_COL, y);
			cprintf("%lu", Clusters[cNum].PreTime[bNum]);
			// PreValue
			gotoxy(PAID_VALUE_COL, y);
			cprintf("%8.2f", Clusters[cNum].PreValue[bNum]);
			// collisions
			if (g_numOfSemWait)
			{
				gotoxy(40, 3);
				cprintf("[ Waiting %u times ...]", g_numOfSemWait);
			}
		}
	}
}
