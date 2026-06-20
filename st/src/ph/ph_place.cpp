//
// [ PH_PLACE.CPP ]
//
#include "stdst.h"

#include <parser.h>
#include <ph_eng.h>

PH_ENGINE::PLACE_INFO::PLACE_INFO()
{
    Count = 0;
    for (int slot=0; slot<MAX_INFO_SLOTS; slot++)
        Info[slot].Count = 0;
}

PH_ENGINE::PLACE_INFO::~PLACE_INFO()
{
    Flush();
}

BOOL PH_ENGINE::PLACE_INFO::LoadFromInf(const char *filename)
{
    ifstream file(filename);
    TEXT_FILE_LINE line;
    while (file.getline(line, sizeof(TEXT_FILE_LINE)))
        Add(line);
    return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::SaveToInf(const char *filename)
{
    BOOL ok = FALSE;
    ofstream file(filename);
    if (file)
    {
        SetInfHeader(file);
        for (int slot = 0; slot < MAX_INFO_SLOTS; slot++)
        {
			PLACE_ENTRY_LIST_ITERATOR it(GetPlaceList(slot));
            SetInfSlotSeparator(file, slot);
			while (it)
            {
				ToInfLine(file, it) << endl;
				++it;
            }
        }
		ok = TRUE;
	}
	return ok;
}

PH_ENGINE::PLACE_ENTRY_LIST& PH_ENGINE::PLACE_INFO::GetPlaceList(int slot)
{
	return Info[slot].Entries;
}

ostream& PH_ENGINE::PLACE_INFO::ToInfLine(ostream& os, PLACE_ENTRY_LIST_ITERATOR it)
{
	return ToInfLine(os, it.current());
}

ostream& PH_ENGINE::PLACE_INFO::ToInfLine(ostream& os, PLACE_ENTRY const &entry)
{
	os
		<< setiosflags(ios::left)  << setw(sizeof(CITY_NAME)-1) << entry.Place
		<< " : "
		<< setiosflags(ios::right) << setw(2) << entry.TariffNum
/* $$$230
		<< ", "
		<< setiosflags(ios::right) << setw(5) << setprecision(1) << it.current().m_minutes
		<< ", "
		<< setiosflags(ios::right) << setw(5) << setprecision(1) << it.current().m_percentage
*/
		<< " = ";

	FormatNumbers(os, entry);

	return os;
}

ostream& PH_ENGINE::PLACE_INFO::EntryToLine(ostream& os, PLACE_ENTRY const & entry)
{
	os << entry.Place << ":" << entry.TariffNum	<< "=";

	FormatNumbers(os, entry);

	return os;
}

ostream& PH_ENGINE::PLACE_INFO::FormatNumbers(ostream& os, PLACE_ENTRY const &entry)
{
	PHONE_NUMBER number;
	for (int i = 0; i < entry.NumEntries.Count; i++)
	{
		number = entry.NumEntries.Numbers[i];
		if (number & RANGE_INDICATOR_MASK)
		{ // ranges ?
			number &= ~RANGE_INDICATOR_MASK; // no range
			os << number << '-';
		}
		else
		{
			os << number;
			if (i < entry.NumEntries.Count-1) // last item ?
				os << ',';
		}
	}

	return os;
}

ostream& PH_ENGINE::PLACE_INFO::SetInfHeader(ostream& os)
{
	return os
		   << "; *******************************************" << endl
		   << ";   Arbol de Numeraci¢n"                       << endl
		   << "; "                                            << endl
		   << ";     Entradas: " << Count                     << endl
		   << "; "                                            << endl
/* $$$230
		   << ";   Formato: "								  << endl
		   << ";     lugar : tarifa, minutos, porcentaje = numeraci¢n"  << endl
		   << "; "                                            << endl
*/
		   << ";   Utilice INF2DAT para producir PH_INFO.BIN"   << endl
		   << "; *******************************************" << endl
		   << "  "                                            << endl
		   ;
}

ostream& PH_ENGINE::PLACE_INFO::SetInfSlotSeparator(ostream& os, int slot)
{
	return os << ";[ " << slot << " ]" << endl;
}

BOOL PH_ENGINE::PLACE_INFO::Load(fstream& file)
{
	PLACE_ENTRY entry;
	if (!LoadCount(file))
		return FALSE;

	for (int slot = 0; slot < MAX_INFO_SLOTS; slot++)
	{
		if (!Info[slot].LoadCount(file))
			return FALSE;

		for (int i = 0; i < Info[slot].Count; i++)
		{
			if (!LoadEntry(file, entry))
				return FALSE;

			Info[slot].Entries.add(entry);
		}
	}

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::Save(fstream& file)
{
	if (!SaveCount(file))
		return FALSE;

	for (int slot = 0; slot < MAX_INFO_SLOTS; slot++)
	{
		if (!Info[slot].SaveCount(file))
			return FALSE;

		PLACE_ENTRY_LIST_ITERATOR it(Info[slot].Entries);

		while (it)
		{
			if (!SaveEntry(file, it.current()))
				return FALSE;

			++it;
		}
	}

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::Search(const PHONE& phone, PLACE_ENTRY& entry)
{
	if (!isdigit(phone[0]))
		return FALSE;

	BOOL found = FALSE;
	PHONE_NUMBER number;
	PHONE partialPhone;

	int len = strlen((char *)&phone);
	int slot = phone[0] - '0';

	for (int i = 0; i < len && i < (int)sizeof(partialPhone)-1 && !found; i++)
	{
		partialPhone[i] = phone[i];
		partialPhone[i+1] = '\0';
		number = atol(partialPhone);
		found = PartialSearch(number, slot, entry);
	}

	return found;
}

BOOL PH_ENGINE::PLACE_INFO::SearchPlace(CITY_NAME const & place, PLACE_ENTRY_LIST& placeList)
{
	BOOL found = FALSE;

	CITY_NAME upperPlace, tmpUpperPlace;
	strcpy(upperPlace, (char *)&place); strupr(upperPlace);

	char *pPlace;

	for (int slot = 0; slot < MAX_INFO_SLOTS; ++slot)
	{
		PLACE_ENTRY_LIST_ITERATOR it(GetPlaceList(slot));
		while (it)
		{
			// non uppercase sensitive
			strcpy(tmpUpperPlace, it.current().Place); strupr(tmpUpperPlace);
			pPlace = strstr(tmpUpperPlace, upperPlace);
			// find occurrence: first characters
			if (pPlace && pPlace == tmpUpperPlace)
			{
				placeList.add(it.current());
				found = TRUE; // at least one
			}

			++it;
		}

	}

	return found;
}

BOOL PH_ENGINE::PLACE_INFO::PartialSearch(PHONE_NUMBER number, WORD slot, PLACE_ENTRY& entry)
{
	int   numCount;
	PHONE_NUMBER leftNumber, rightNumber;
	PHONE_NUMBER  *numbers;
	BOOL found = FALSE;
	PLACE_ENTRY_LIST_ITERATOR it(Info[slot].Entries);
	while (it && !found)
	{
		numCount = it.current().NumEntries.Count;
		numbers  = it.current().NumEntries.Numbers;
		for (int i = 0; i < numCount && !found; i++)
		{
			leftNumber = numbers[i];
			if ((leftNumber & RANGE_INDICATOR_MASK) && (i+1) < numCount)
			{
				leftNumber  &= ~RANGE_INDICATOR_MASK; // no range
				rightNumber  = numbers[i+1];
                if (leftNumber <= number && number <= rightNumber)
                {
					entry = it.current();
                    found = TRUE;
                }
            }
            else
            {
                if (leftNumber == number)
                {
					entry = it.current();
                    found = TRUE;
                }
            }
        }
		++it;
    }
    return found;
}

BOOL PH_ENGINE::PLACE_INFO::Add(const char *line)
{
    BOOL ok = FALSE;
    //
	CITY_NAME place;
	WORD      tariffNum;
	double    minutes;
	double    percentage;
	PLACE_INFO::NUMBERS_PER_LINE *numbers = new PLACE_INFO::NUMBERS_PER_LINE[MAX_INFO_SLOTS];
	memset(numbers, 0, MAX_INFO_SLOTS*sizeof(PLACE_INFO::NUMBERS_PER_LINE));
	//
	if (Translate(line, place, tariffNum, minutes, percentage, numbers))
	{
		PLACE_ENTRY entry;

		strcpy(entry.Place, place); // copy place
		entry.TariffNum 	= tariffNum;
/* $$$230
		entry.m_minutes    	= minutes;
		entry.m_percentage	= percentage;
*/
		// numbers
		for (int slot=0; slot<MAX_INFO_SLOTS; slot++)
		{
			entry.NumEntries.Count = numbers[slot].Count;
			// copy numbers
			if (numbers[slot].Count)
			{
				entry.NumEntries.Numbers = new PHONE_NUMBER[numbers[slot].Count];
				memcpy(entry.NumEntries.Numbers, numbers[slot].Numbers, numbers[slot].Count*sizeof(PHONE_NUMBER));
				Info[slot].Entries.add(entry);
				// store place info
				Info[slot].Count++;
				Count++;
			}
		}
		ok = TRUE;
	}

	delete [] numbers;

	return ok;
}

#pragma argsused
BOOL PH_ENGINE::PLACE_INFO::Translate(
	const char *line,
	CITY_NAME& place,
	WORD& tariffNum,
	double& minutes,
	double& percentage,
	NUMBERS_PER_LINE *numbers)
{
	//
	Parser::Tokens tokens;
	Parser parser(line, tokens);
	Parser::Iterator it(tokens);

	if (!it)
		return FALSE;

	if (TokenIs(it, ";"))
		return FALSE; // is a comment

	//
	// Format:
	// Place : Tariff, minutes, percentage = numbers
	//
	if (!GetPlace(it, place))
		return FALSE;

	if (!GetTariffNum(it, tariffNum))
		return FALSE;
/* $$$230
	if (!GetMinutes(it, minutes))
		return FALSE;

	if (!GetPercentage(it, percentage))
		return FALSE;
*/

   if (!GetNumbers(it, numbers))
		return FALSE;

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::GetPlace(Parser::Iterator& it, CITY_NAME& place)
{
	TEXT_FILE_LINE tmpPlace; // big cities !!!
	tmpPlace[0] = '\0';
	while (
		it    &&
		!TokenIs(it, ":") &&
		strlen(tmpPlace) + strlen(it.current()) < sizeof(TEXT_FILE_LINE))
	{
		strcat(tmpPlace, it.current());
		++it;
	}
	if (!it)
		return FALSE;

	// trim trailing spaces and tabs (back and front)
	_RTrim(tmpPlace);
	strrev(tmpPlace);
	_RTrim(tmpPlace);
	strrev(tmpPlace);

	// truncate names with lenght great than CITY_WIDTH
	tmpPlace[sizeof(CITY_NAME)-1] = '\0';
	strcpy((char *)place, (char *)&tmpPlace);

	++it; // bypass ':'

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::GetTariffNum(Parser::Iterator& it, WORD& tariffNum)
{
	SkipSpaces(it);

	if (!it)
		return FALSE; // bad format ?

	tariffNum = atoi(it.current());

	if (!tariffNum && !TokenIs(it, "0"))
		return FALSE; // bad number

	++it; // bypass tariff number

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::GetMinutes(Parser::Iterator& it, double& minutes)
{
	// find comma
	while (it && !TokenIs(it, ","))
	{
		++it;
	}

	if (!it)
		return FALSE; // bad format ?

	++it; // skip comma

	SkipSpaces(it);

	if (!it)
		return FALSE; // bad format ?

	// compose number
	char szNumber[0x20];
	szNumber[0] = '\0';

	if (TokenIs(it, "-"))
	{
		strcat(szNumber, it.current());
		++it; // skip
		if (!it)
			return FALSE; // bad format
	}

	strcat(szNumber, it.current());
	++it;

	if (!it)
		return FALSE; // bad format

	if (TokenIs(it, "."))
	{
		strcat(szNumber, it.current());
		++it;  // skip
		if (!it)
			return FALSE; // bad format ?

		strcat(szNumber, it.current());
		++it;

		if (!it)
			return FALSE; // bad format

	}

	// convert
	if (!sscanf(szNumber, "%lf", &minutes))
		return FALSE; // bad format

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::GetPercentage(Parser::Iterator& it, double& percentage)
{
	// find comma
	while (it && !TokenIs(it, ","))
	{
		++it;
	}

	if (!it)
		return FALSE; // bad format ?

	++it; // skip comma

	SkipSpaces(it);

	if (!it)
		return FALSE; // bad format ?

	// compose number
	char szNumber[0x20];
	szNumber[0] = '\0';

	if (TokenIs(it, "-"))
	{
		strcat(szNumber, it.current());
		++it; // skip
		if (!it)
			return FALSE; // bad format
	}

	strcat(szNumber, it.current());
	++it;

	if (!it)
		return FALSE; // bad format

	if (TokenIs(it, "."))
	{
		strcat(szNumber, it.current());
		++it;  // skip
		if (!it)
			return FALSE; // bad format ?

		strcat(szNumber, it.current());
		++it;

		if (!it)
			return FALSE; // bad format

	}

	// convert
	if (!sscanf(szNumber, "%lf", &percentage))
		return FALSE; // bad format

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::GetNumbers(Parser::Iterator& it, NUMBERS_PER_LINE *numbers)
{
	// find equal
	while (it && !TokenIs(it, "="))
	{
		++it;
	}

	if (!it)
		return FALSE; // bad format

	++it; // skip equal sign ?

	SkipSpaces(it);

	if (!it)
		return FALSE; // bad format

	// process numbers and ranges
	BOOL isRange = FALSE;
	WORD slot, prevSlot=0;
	DWORD number;

	while (it)
	{
		SkipSpaces(it);

		if (TokenIs(it, ","))
		{ // comma ?
			; // skip comma
		}
		else if (TokenIs(it, "-"))
		{
			isRange = TRUE; // indicate range
		}
		else
		{
			if (!IsValidPhoneItem(it.current()))
				return FALSE;

			PHONE phone;
			strcpy(phone, it.current());
			number = atol(phone);

			if (!isdigit(phone[0]))
				return FALSE;

			slot = phone[0] - '0';
			// ---
			if (numbers[slot].Count >= MAX_NUMBERS_PER_LINE)
				return FALSE; // bad format: too many numbers on this line
			if (isRange)
			{
				if (!numbers[prevSlot].Count)
					return FALSE; // bad format

				// has prevSlot a token ?
				if (slot != prevSlot)
					return FALSE; // bad format

				// sign prevSlot number To indicate a range
				numbers[slot].Numbers[numbers[slot].Count-1] |= RANGE_INDICATOR_MASK;
				numbers[slot].Numbers[numbers[slot].Count++] = number;
				isRange = FALSE;
			}
			else
				numbers[slot].Numbers[numbers[slot].Count++] = number;
			prevSlot = slot;
		}

		++it;
	}

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::IsValidPhoneItem(const String& line)
{
	int len = strlen(line);
	if (len >= sizeof(PHONE))
		return FALSE;

	int i=0;
	while (isdigit(*(line+i)) && i < len)
	{
		i++;
	}

	return (i == len);
}

void PH_ENGINE::PLACE_INFO::SkipSpaces(Parser::Iterator& it)
{
	while (it && (TokenIs(it, " ") || TokenIs(it, "\t")))
	{
		++it;
	}
}

void PH_ENGINE::PLACE_INFO::Flush(void)
{
	for (int slot = 0; slot < MAX_INFO_SLOTS; slot++)
    {
        // flush phone numbers
		PLACE_ENTRY_LIST_ITERATOR it(Info[slot].Entries);
		while (it)
		{
			if (it.current().NumEntries.Numbers)
				delete [] it.current().NumEntries.Numbers;
			++it;
		}
		Info[slot].Entries.flush();
	}
}

BOOL PH_ENGINE::PLACE_INFO::LoadEntry(fstream& file, PLACE_ENTRY& entry)
{
	file.read(entry.Place, sizeof(CITY_NAME));
	if (file.gcount() != sizeof(CITY_NAME))
		return FALSE;

	file.read((char *)&entry.TariffNum, sizeof(WORD));
	if (file.gcount() != sizeof(WORD))
		return FALSE;

/* $$$230
	file.read((char *)&entry.m_minutes, sizeof(double));
	if (file.gcount() != sizeof(double))
		return FALSE;

	file.read((char *)&entry.m_percentage, sizeof(double));
	if (file.gcount() != sizeof(double))
		return FALSE;
*/

	file.read((char *)&entry.NumEntries.Count, sizeof(WORD));
	if (file.gcount() != sizeof(WORD))
		return FALSE;

	entry.NumEntries.Numbers = new PHONE_NUMBER[entry.NumEntries.Count];
	file.read((char *)entry.NumEntries.Numbers, entry.NumEntries.Count*sizeof(PHONE_NUMBER));
	if (file.gcount() != entry.NumEntries.Count*sizeof(PHONE_NUMBER))
	{
		entry.NumEntries.Count = 0;
		delete [] entry.NumEntries.Numbers;

		return FALSE;
	}

	return TRUE;
}

BOOL PH_ENGINE::PLACE_INFO::SaveEntry(fstream& file, PLACE_ENTRY& entry)
{
	file.write(entry.Place, sizeof(CITY_NAME));
	file.write((char *)&entry.TariffNum, sizeof(WORD));
/* $$$230
	file.write((char *)&entry.m_minutes, sizeof(double));
	file.write((char *)&entry.m_percentage, sizeof(double));
*/
	file.write((char *)&entry.NumEntries.Count, sizeof(WORD));
    file.write((char *)entry.NumEntries.Numbers, entry.NumEntries.Count*sizeof(PHONE_NUMBER));
    return TRUE;
}

