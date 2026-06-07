//
// [ TEST.CPP ]
//
// Feb 20 1995
// GCC/gcc
//

#include <string.h>
#include <ctype.h>
#include <iostream.h>
#include <iomanip.h>
#include <constream.h>
#include <fstream.h>
#include <db_eng.h>
#include <cfg.h>
#include <st_util.h>

static char GetOption(const char *options);

CFG *g_cfg;

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

char *errorMemory;

void NewHandler(void);

int main(void)
{
	errorMemory = new char[0x200];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;

	g_cfg = new CFG;
	g_cfg->Load();

	DB_ENGINE dbEngine;

	BOOL ok = TRUE, doPause = TRUE;

	do
	{
		clrscr();
		cout << "[ SmartTar DBEngine ]" << endl << endl;

		cout
			<< "   Size: " << sizeof(Receipt) << " bytes per record."<< endl
			<< "Records: " << dbEngine.GetEntries() << endl
			<< "   From: " << dbEngine.GetLowerNumber() << endl
			<< "     To: " << dbEngine.GetHigherNumber() << endl
			<< "  Cache: " << g_cfg->CACHE_SIZE << " entries."<< endl
			<< "   Dups: " << g_cfg->CHECK_DUPS << endl
			<< endl
		;
		cout
			<< "[A]rchive"             << endl
			<< "[I]nsert 1000 records" << endl
			<< "inser[T] 10 records"   << endl
			<< "[G]et a record"        << endl
			<< "[M]odify a record"     << endl
			<< "[F]lush"               << endl
			<< "[R]epair"              << endl
			<< "[L]ist records"        << endl
			<< "List r[e]cords to file"        << endl
			<< "[B]ooth list"          << endl
			<< "[D]isplay archived DB" << endl
			<< "Random [U]pdate"       << endl
			<< "[Q]uit (Esc)"          << endl
			<< endl
		;

		cout << "Option: ";

		int n = 10;
		switch (GetOption("AIEFGLBMRTUDQ\x1B"))
		{
		case 'I':
			n = 1000;
		case 'T':
			{
				cout << endl;
				doPause = FALSE;

				randomize();
				for (int i=0; i < n; ++i)
				{
					DynamicReceipt dynReceipt;

					dynReceipt.m_r.Tag = Receipt::TEL;

					int n = random(2);
					if (n)
					{
						strcpy(dynReceipt.m_r.City, "Armenia (Qui)");
						strcpy(dynReceipt.m_r.Phone, "0967448412");
						dynReceipt.m_r.ElapsedTime = 100000L;
					}
					else
					{
						strcpy(dynReceipt.m_r.City, "Medellin (Ant)");
						strcpy(dynReceipt.m_r.Phone, "0943415600");
						dynReceipt.m_r.ElapsedTime = 50000L;
					}

					dynReceipt.m_r.MagicNumber = DB_STORAGE::MAGIC_NUMBER;
					dynReceipt.m_r.Stat.Paid = PAID_CALL;
					dynReceipt.Attr_.Storable  = TRUE;
					dynReceipt.Attr_.Countable = TRUE;
					dynReceipt.m_r.Number = g_cfg->N_RECEIPT++;
					dynReceipt.m_r.BoothNumber = random(32);
					dynReceipt.m_r.Date = _GetSysDate();
					dynReceipt.m_r.Time = _GetSysTime();

					if (!dbEngine.Add(dynReceipt))
					{
						cerr << "Couldn't add: " << dynReceipt.m_r.Number << endl;
						doPause = TRUE;
					}
				}
				break;
			}
		case 'G':
			{
                long number;
                cout << endl << "Record: ";
				cin >> number;
				Receipt receipt;
				if (dbEngine.Get(receipt, number))
                {
                    cout
					<< " ["  << receipt.BoothNumber + 1 << "] "
					<< receipt.Number << ' '
					<< receipt.City << ' '
					<< receipt.ElapsedTime
                    << endl
                    ;
				}
                else
                {
                    cout << number << " Not found ..." << endl;
                }
				break;
			}
        case 'M':
            {
				long number;
                cout << endl << "Record: ";
                cin >> number;
				Receipt receipt;
				if (dbEngine.Get(receipt, number))
				{
                    cout
						<< " ["  << receipt.BoothNumber + 1 << "] "
						<< receipt.Number << ' '
						<< receipt.City << ' '
						<< receipt.ElapsedTime
						<< endl
                    ;
                    cout << "City: ";
					DynamicReceipt tmpDynRec;
					tmpDynRec.m_r = receipt;
					cin >> tmpDynRec.m_r.City;
					if (!dbEngine.Update(tmpDynRec))
                    	cerr << "No se pudo actualizar !!!" << endl;
				}
				else
				{
					cout << number << " Not found ..." << endl;
				}
				break;
			}
		case 'U':
			{
				cout << endl;

				int n = dbEngine.GetEntries();

				if (!n)
				{
					cout  << "No records...";
					break;
				}

				cout << "Updating (press any key to stop)..." << endl;

				randomize();
				long number;

				long lower = dbEngine.GetLowerNumber();
				do
				{
					number = lower + random(n);

					Receipt receipt;
					if (dbEngine.Get(receipt, number))
					{
						DynamicReceipt dynReceipt;
						dynReceipt.m_r = receipt;

						dynReceipt.m_r.extendedStat.nonProcessed = FALSE;

						if (!dbEngine.Update(dynReceipt))
							cerr  << number << " Not updated." << endl;
					}
				}
				while (!kbhit());
				getch();

				break;
			}
		case 'F':
            {
                cout << endl << "Flushing ...";
				dbEngine.Flush();
                doPause = FALSE;
				break;
            }
        case 'R':
			{
				cout << endl << "Rebuilding ...";
				if (!dbEngine.Repair())
					cerr << "Error repairing database ..." << endl;
				break;
			}
		case 'L':
			{
				long fromNumber;
				cout << endl << "From record: ";
				cin >> fromNumber;

				clrscr();

				DB_STORAGE::Iterator it(dbEngine.GetDBStorage());
				fromNumber = it.Restart(fromNumber);
				int i = 0;
				while (it)
				{
					Receipt receipt;

					long number = it.Current();

					if (dbEngine.Get(receipt, number))
					{
						WORD year, month, day;
						_UnpackDate(receipt.Date, year, month, day);
						cout
							<< setw(5) << fromNumber + i << ": "
							<< setw(5) << receipt.Number << ' '
							<< " ["    << setw(2) << receipt.BoothNumber + 1 << "] "
							<< receipt.City << ' '
							<< receipt.ElapsedTime << ' '
							<< day << '/' << month  << '/' << year
							<< endl;
					}
					else
					{
						cerr << number << " Not found ..." << endl;
					}

					i++;

					it++;
				}

				cout << "No More ...";
				break;
			}
		case 'E':
			{
				long fromNumber;
				cout << endl << "From record: ";
				cin >> fromNumber;

				clrscr();
				cout << "Listing to file RX.LST ...";

				ofstream file("rx.lst");

				file
					<< "   Size: " << sizeof(Receipt) << " bytes per record."<< endl
					<< "Records: " << dbEngine.GetEntries() << endl
					<< "   From: " << dbEngine.GetLowerNumber() << endl
					<< "     To: " << dbEngine.GetHigherNumber() << endl
					<< endl;

				DB_STORAGE::Iterator it(dbEngine.GetDBStorage());
				fromNumber = it.Restart(fromNumber);
				int i = 0;
				while (it)
				{
					Receipt receipt;

					long number = it.Current();

					if (dbEngine.Get(receipt, number))
					{
						WORD year, month, day;
						_UnpackDate(receipt.Date, year, month, day);
						WORD hh, mm, ss;
						_UnpackTime(receipt.Time, hh, mm, ss);
						file
							<< setw(5) << fromNumber + i << ": "
							<< setw(5) << receipt.Number << ' '
							<< " [" << setw(2) << receipt.BoothNumber + 1 << "] "
							<< receipt.City << ' '
							<< receipt.ElapsedTime << ' '
							<< day << '/' << month  << '/' << year << ' '
							<< hh << ':' << mm  << ':' << ss
							<< endl;
					}
					else
					{
						cerr << number << " Not found ..." << endl;
					}

					i++;

					it++;
				}

				break;
            }
		case 'B':
			{
				int boothNumber = -1;
				cout << endl << "Booth: ";
				cin >> boothNumber;
				boothNumber--;

				clrscr();

				Receipt receipt;

				DB_STORAGE::Iterator it(dbEngine.GetDBStorage());
				long fromNumber = it.Restart();
                int i = 0;
				while (it)
				{
					Receipt receipt;

					long number = it.Current();

					if (dbEngine.Get(receipt, number, boothNumber))
					{
						WORD year, month, day;
						_UnpackDate(receipt.Date, year, month, day);
						cout
							<< setw(5) << fromNumber + i << ": "
							<< setw(5) << receipt.Number << ' '
							<< " ["    << setw(2) << receipt.BoothNumber + 1 << "] "
							<< receipt.City << ' '
							<< receipt.ElapsedTime << ' '
							<< day << '/' << month  << '/' << year
							<< endl;
					}

					i++;

					it++;
				}

				cout << "No More ...";
				break;
			}
		case 'A':
			{
				dbEngine.Archive();
				doPause = FALSE;
				break;
			}
		case 'D':
			{
				UINT turn = 1, date = _GetSysDate();
				cout << endl << "Turn: ";
				cin >> turn;
				if (!dbEngine.LoadArcDB(date, turn))
				{
					cout << "No such file...";
					break;
				}

				clrscr();

				DB_STORAGE::Iterator it(dbEngine.GetArcDBStorage());
				long fromNumber = it.Restart(fromNumber);
				int i = 0;
				while (it)
				{
					Receipt receipt;

					long number = it.Current();

					if (dbEngine.Get(receipt, number))
					{
						WORD year, month, day;
						_UnpackDate(receipt.Date, year, month, day);
						cout
							<< setw(5) << fromNumber + i << ": "
							<< setw(5) << receipt.Number << ' '
							<< " ["    << setw(2) << receipt.BoothNumber + 1 << "] "
							<< receipt.City << ' '
							<< receipt.ElapsedTime << ' '
							<< day << '/' << month  << '/' << year
							<< endl;
					}
					else
					{
						cerr << number << " Not found ..." << endl;
					}

					i++;

					it++;
				}

				cout << "No More ...";

				dbEngine.UnloadArcDB();
				break;
			}
		case '\x1B':
		case 'Q' :
			ok = FALSE;
			doPause = FALSE;
            break;
		}

        if (doPause)
        {
            cout << endl << endl << "Press any key to continue..." ;
            getch();
        }
		doPause = TRUE;
    }
	while (ok);

	g_cfg->Save();

    return (0);
}

char GetOption(const char *options)
{
    char byte;
    do
    {
        if ((byte = toupper(getch())) == 0)
            byte = getch();
    }
    while (!strchr(options, byte));
    return byte;
}

void NewHandler(void)
{
    if (errorMemory)
        delete [] errorMemory;
    cout
		<< "No hay memoria disponible" << endl
		<< "por favor reporte este problema ..."
		<< endl
    ;
    exit(1);
}

