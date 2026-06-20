//
// [ PH_ENG.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>

#if !defined(__TEST__)
#include <info.h>
#endif

extern CFG *g_cfg;
//
static const char *PH_INFO_FILENAME   = "PH_INFO.BIN";
static const char *DDN_INF_FILENAME   = "DDN.INF";
static const char *DDI_INF_FILENAME   = "DDI.INF";
static const char *LOCAL_INF_FILENAME = "LOCAL.INF";
static const char *PH_PATCH_FILENAME  = "PH_PATCH.DAT";


PH_ENGINE::PH_ENGINE(void)
{
    LocalPlaces = new PLACE_INFO;
	DDNPlaces   = new PLACE_INFO;
    DDIPlaces   = new PLACE_INFO;
    //
    SetDefaultLockedNumbers();
    SetDefaultDDNTariffs();
    SetDefaultDDNSchedule();
    SetDefaultDDITariffs();
    SetDefaultDDISchedule();
}

PH_ENGINE::~PH_ENGINE()
{
    delete LocalPlaces;
    delete DDNPlaces;
    delete DDIPlaces;
}

BOOL PH_ENGINE::Inf2Dat(void)
{
    // optimize memory by converting one at a time
	BOOL ok = TRUE;
    //
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME filename;
    //
    // preserve existing PH_INFO.BIN data
    // except places.  Assume good information.
    //
    strcat(strcpy(filename, path), PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::in);
    // load
    if (file)
    {
        if (LoadHeader(file))
        {
            if (LoadLockedNumbers(file))
            {
                if (LoadDDNTariffs(file))
                {
					if (LoadDDITariffs(file))
                    {
                        if (LoadDDNSchedule(file))
                        {
                            if (LoadDDISchedule(file))
                            {}


                        }
                    }
                }
            }
        }
    }
    file.close();
    file.open(filename, ios::binary|ios::out); // new file !
    // save
    if (file)
    {
        if (ok &= SaveHeader(file))
		{
            if (ok &= SaveLockedNumbers(file))
            {
                if (ok &= SaveDDNTariffs(file))
                {
                    if (ok &= SaveDDITariffs(file))
                    {
                        if (ok &= SaveDDNSchedule(file))
                        {
                            if (ok &= SaveDDISchedule(file))
                            {}


                        }
                    }
                }
            }
        }
    }
    // load and save local
	strcat(strcpy(filename, path), LOCAL_INF_FILENAME);
    ok &= LocalPlaces->LoadFromInf(filename);
    ok &= LocalPlaces->Save(file);
    ok &= LocalPlaces->SaveToInf(filename);
    LocalPlaces->Flush(); // optimize
    // load and save DDN
    strcat(strcpy(filename, path), DDN_INF_FILENAME);
    ok &= DDNPlaces->LoadFromInf(filename);
    ok &= DDNPlaces->Save(file);
    ok &= DDNPlaces->SaveToInf(filename);
    DDNPlaces->Flush(); // optimize
    // load and save DDI
    strcat(strcpy(filename, path), DDI_INF_FILENAME);
    ok &= DDIPlaces->LoadFromInf(filename);
	ok &= DDIPlaces->Save(file);
    ok &= DDIPlaces->SaveToInf(filename);
    DDIPlaces->Flush(); // optimize
    //
    return ok;
}

BOOL PH_ENGINE::Dat2Inf(void)
{
    // optimize memory by converting one at a time
    BOOL ok = TRUE;
    //
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME filename;
    //
    // read existing PH_INFO.BIN data
    //
    strcat(strcpy(filename, path), PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::in);
	// load
    if (file)
    {
        if (LoadHeader(file))
        {
            if (LoadLockedNumbers(file))
            {
                if (LoadDDNTariffs(file))
                {
                    if (LoadDDITariffs(file))
                    {
                        if (LoadDDNSchedule(file))
                        {
                            if (LoadDDISchedule(file))
                            {}


                        }
                    }
                }
			}
        }
    }
    // load and save local
    strcat(strcpy(filename, path), LOCAL_INF_FILENAME);
    ok &= LocalPlaces->Load(file);
    ok &= LocalPlaces->SaveToInf(filename);
    LocalPlaces->Flush(); // optimize
    // load and save DDN
    strcat(strcpy(filename, path), DDN_INF_FILENAME);
    ok &= DDNPlaces->Load(file);
    ok &= DDNPlaces->SaveToInf(filename);
    DDNPlaces->Flush(); // optimize
    // load and save DDI
    strcat(strcpy(filename, path), DDI_INF_FILENAME);
    ok &= DDIPlaces->Load(file);
    ok &= DDIPlaces->SaveToInf(filename);
    DDIPlaces->Flush(); // optimize
    //
    return ok;
}

BOOL PH_ENGINE::LoadFromInfs(void)
{
#pragma warn -pia
    //
    // preserve existing PH_INFO.BIN data
    // except places
    //
    FILE_NAME filename;
    _GetAppPath(filename);
    strcat(filename, PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::in);
    if (file)
    {
        if (LoadHeader(file))
        {
            if (LoadLockedNumbers(file))
            {
                if (LoadDDNTariffs(file))
				{
                    if (LoadDDITariffs(file))
                    {
                        if (LoadDDNSchedule(file))
                        {
                            if (LoadDDISchedule(file))
                            {}


                        }
                    }
                }
            }
        }
    }
    // load places
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME fnLocal;
    strcat(strcpy(fnLocal, path), LOCAL_INF_FILENAME);
	FILE_NAME fnDDN  ;
    strcat(strcpy(fnDDN, path)  , DDN_INF_FILENAME);
    FILE_NAME fnDDI  ;
    strcat(strcpy(fnDDI, path)  , DDI_INF_FILENAME);
    return (
               LocalPlaces->LoadFromInf(fnLocal) &&
               DDNPlaces->LoadFromInf(fnDDN)     &&
               DDIPlaces->LoadFromInf(fnDDI)
           );
#pragma warn +pia
}

BOOL PH_ENGINE::SaveToInfs(void)
{
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME fnLocal;
    strcat(strcpy(fnLocal, path), LOCAL_INF_FILENAME);
    FILE_NAME fnDDN  ;
    strcat(strcpy(fnDDN, path)  , DDN_INF_FILENAME);
	FILE_NAME fnDDI  ;
    strcat(strcpy(fnDDI, path)  , DDI_INF_FILENAME);
    return (
               LocalPlaces->SaveToInf(fnLocal) &&
               DDNPlaces->SaveToInf(fnDDN)     &&
               DDIPlaces->SaveToInf(fnDDI)
           );
}

BOOL PH_ENGINE::Load(void)
{
#pragma warn -pia
    FILE_NAME filename;
    _GetAppPath(filename);
    strcat(filename, PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::in);
    BOOL ok = FALSE;
    if (file)
    {
        if (ok = LoadHeader(file))
		{
            if (ok = LoadLockedNumbers(file))
            {
                if (ok = LoadDDNTariffs(file))
                {
                    if (ok = LoadDDITariffs(file))
                    {
                        if (ok = LoadDDNSchedule(file))
                        {
                            if (ok = LoadDDISchedule(file))
                            {
                                if (ok = LocalPlaces->Load(file))
                                {
                                    if (ok = DDNPlaces->Load(file))
                                    {
                                        if (ok = DDIPlaces->Load(file))
                                        {}


                                    }
								}
                            }
                        }
                    }
                }
            }
        }
    }
    return ok;
#pragma warn +pia
}

BOOL PH_ENGINE::Save(void)
{
#pragma warn -pia
    FILE_NAME filename;
    _GetAppPath(filename); // NULL implies the EXE path
    strcat(filename, PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::out);
    BOOL ok = FALSE;
	if (file)
    {
        if (ok = SaveHeader(file))
        {
            if (ok = SaveLockedNumbers(file))
            {
                if (ok = SaveDDNTariffs(file))
                {
                    if (ok = SaveDDITariffs(file))
                    {
                        if (ok = SaveDDNSchedule(file))
                        {
                            if (ok = SaveDDISchedule(file))
                            {
                                if (ok = LocalPlaces->Save(file))
                                {
                                    if (ok = DDNPlaces->Save(file))
                                    {
                                        if (ok = DDIPlaces->Save(file))
										{}
									}
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return ok;
#pragma warn +pia
}

BOOL PH_ENGINE::GetCallAttr(const PHONE& phone, CALL_ATTR& attr, int numOfDigits)
{
    static BOOL analized;
    analized = FALSE;
    attr = 0; // clear
	if (!numOfDigits)
    {
        numOfDigits = strlen((char *)&phone);
    }
    if (numOfDigits > 0)
    { // check for number of digits
        if (!(analized = AnalizeFirstDigit(phone, attr)))
        {
            if (numOfDigits > 1)
            {
                if (!(analized = AnalizeSecondDigit(phone, attr)))
                {
                    if (numOfDigits > 2)
                    {
                        if (!(analized = AnalizeThirdDigit(phone, attr)))
                        {
#if defined(__EDA__) // just EDA force to 4th digit under new version
							if (g_cfg->ACCESS_LEVELS == 3)
                            {
                                if (numOfDigits > 3)
								{
                                    analized = AnalizeFourthDigit(phone, attr);
                                }
                            }
#endif
                        }
                    }
				}
            }
        }
	}

    return analized;
}

BOOL PH_ENGINE::GetCallParms(PHONE const &phone, CALL_PARAMETERS& parameters)
{
	// TRUE if the call could be classified
    static BOOL got;
    got = GetCallAttr(phone, parameters.Attr);
    // Warning: the NOT_INCLUDE_CALL_MASK attrib is independent of the other ones.
	strcpy((char *)&parameters.AccessHeader, "");
    parameters.AreaCode = 0; // local call or so
    if (got)
    {
		static int len;
        len = strlen((char *)&phone);
		if (parameters.Attr & LOCAL_DIAL_MASK)
		{
			; // nothing to do !!!
		}
		else if (parameters.Attr & DDI_DIAL_MASK)
		{
			strncpy((char *)&parameters.AccessHeader, (char *)&phone, g_cfg->ACCESS_LEVELS);
			parameters.AccessHeader[g_cfg->ACCESS_LEVELS] = '\0';
			if (len > g_cfg->ACCESS_LEVELS)
				parameters.AreaCode =  phone[g_cfg->ACCESS_LEVELS] - '0';
		}
		else
		{
			if (len > g_cfg->ACCESS_LEVELS-1)
			{
				parameters.AreaCode =  phone[g_cfg->ACCESS_LEVELS-1] - '0';
				// composite header 091
				strncpy((char *)&parameters.AccessHeader, (char *)&phone, g_cfg->ACCESS_LEVELS);
				parameters.AccessHeader[g_cfg->ACCESS_LEVELS] = '\0';
			}
			else
			{
				// simple header 09
				strncpy((char *)&parameters.AccessHeader, (char *)&phone, g_cfg->ACCESS_LEVELS-1);
				parameters.AccessHeader[g_cfg->ACCESS_LEVELS-1] = '\0';
			}
		}
	}
	return got;
}

BOOL PH_ENGINE::Search(PHONE const &phone, PLACE_ENTRY& entry, CALL_PARAMETERS& parameters)
{
	BOOL found = FALSE;
	// default info for entry
	strcpy(entry.Place, ""); // don't use NULL. use constant string !!!
	entry.TariffNum = 0;
	PHONE tmpPhone;
	//
	// Warning: the NOT_INCLUDE_CALL_MASK attrib is independent of the other ones.
	//
	found = GetCallParms(phone, parameters);
	if (found)
	{
		// begin 2.22 build 25
		if (parameters.Attr & NOT_INCLUDED_CALL_MASK)
		{
			strcpy(entry.Place, "--- No Incluída ---");
			if (!(parameters.Attr & INTERNATIONAL_CALL_MASK))
			{ // only for DDN
				if (!(parameters.Attr & LOCAL_DIAL_MASK))
				{
					// v.2.20.7 uses tariff 9
					if (parameters.AreaCode> 0 && parameters.AreaCode < 10)
						entry.TariffNum = g_cfg->DEF_TARIFFS[parameters.AreaCode];
				}
			}
		}
		// end 2.22 build 25
		else
		{
			if (parameters.Attr & LOCAL_DIAL_MASK)
			{
				strcpy(tmpPhone, (char *)&phone[0]); // don't use ACCESS codes
				found = LocalPlaces->Search(tmpPhone, entry);
			}
			else if (parameters.Attr & DDI_DIAL_MASK)
			{
				strcpy(tmpPhone, (char *)&phone[g_cfg->ACCESS_LEVELS]); // don't use ACCESS codes
				found = DDIPlaces->Search(tmpPhone, entry);
			}
			else
			{
				strcpy(tmpPhone, (char *)&phone[g_cfg->ACCESS_LEVELS-1]); // don't use ACCESS codes
				found = DDNPlaces->Search(tmpPhone, entry);
			}
			if (!found)
			{
				parameters.Attr |= NOT_INCLUDED_CALL_MASK;
				strcpy(entry.Place, "--- No Incluída ---");
				if (!(parameters.Attr & INTERNATIONAL_CALL_MASK))
				{ // only for DDN
					if (!(parameters.Attr & LOCAL_DIAL_MASK))
					{
						// v.2.20.7 uses tariff 9
						if (parameters.AreaCode> 0 && parameters.AreaCode < 10)
							entry.TariffNum = g_cfg->DEF_TARIFFS[parameters.AreaCode];
					}
				}
			}
		}
	}

	return found;
}

BOOL PH_ENGINE::SearchPlace(CITY_NAME const & place, PLACE_ENTRY_LIST& placeList, int nFromSource)
{
	BOOL bFound = FALSE;

	if (nFromSource == ALL_SOURCES)
	{
		bFound  = LocalPlaces->SearchPlace(place, placeList);
		bFound |= DDNPlaces->SearchPlace(place, placeList);
		bFound |= DDIPlaces->SearchPlace(place, placeList);
	}
	else
		bFound = GetPlaces(nFromSource)->SearchPlace(place, placeList);

	return bFound;
}

BOOL PH_ENGINE::Add(int from, const char *line)
{
	return GetPlaces(from)->Add(line);
}

PH_ENGINE::PLACE_ENTRY_LIST& PH_ENGINE::GetPlaceList(int from, int slot)
{
    return GetPlaces(from)->GetPlaceList(slot);
}

ostream& PH_ENGINE::ToInfLine(ostream& os, PH_ENGINE::PLACE_ENTRY_LIST_ITERATOR iterator)
{
    return LocalPlaces->ToInfLine(os, iterator); // it's the same for all
}

ostream& PH_ENGINE::ToInfLine(ostream& os, PH_ENGINE::PLACE_ENTRY const &entry)
{
	return LocalPlaces->ToInfLine(os, entry); // it's the same for all
}

ostream& PH_ENGINE::EntryToLine(ostream& os, PH_ENGINE::PLACE_ENTRY const &entry)
{
	return LocalPlaces->EntryToLine(os, entry); // it's the same for all
}

PH_ENGINE::PLACE_INFO *PH_ENGINE::GetPlaces(int from)
{
	switch (from)
	{
	case LOCAL_SOURCE:
		return LocalPlaces;
	case DDN_SOURCE  :
		return DDNPlaces;
	case DDI_SOURCE  :
		return DDIPlaces;
	}

	return NULL;
}

BOOL PH_ENGINE::CreatePatch(void)
{
    BOOL ok = TRUE;
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME filename;
    strcat(strcpy(filename, path), PH_INFO_FILENAME);
    fstream file(filename, ios::binary|ios::in);
    // load
    if (file)
    {
        if (ok &= LoadHeader(file))
        {
            if (ok &= LoadLockedNumbers(file))
            {
                if (ok &= LoadDDNTariffs(file))
                {
                    if (ok &= LoadDDITariffs(file))
                    {
                        if (ok &= LoadDDNSchedule(file))
                        {
                            if (ok &= LoadDDISchedule(file))
                            {}


                        }
                    }
                }
            }
        }
    }
    else
    {
        ok = FALSE;
    }
    file.close();
    //
    if (FileVersion == FILEHDR_VER)
    {  // same generation ?
        // save
        strcat(strcpy(filename, path), PH_PATCH_FILENAME);
        file.open(filename, ios::binary|ios::out); // new file !
        if (file)
        {
            if (ok &= SaveHeader(file))
            {
                if (ok &= SaveLockedNumbers(file))
                {
                    if (ok &= SaveDDNTariffs(file))
                    {
                        if (ok &= SaveDDITariffs(file))
                        {
                            if (ok &= SaveDDNSchedule(file))
                            {
                                if (ok &= SaveDDISchedule(file))
                                {}


                            }
                        }
                    }
                }
            }
        }
        else
        {
            ok = FALSE;
        }
    }
    else
    {
        ok = FALSE;
    }
    return ok;
}

BOOL PH_ENGINE::ApplyPatch(void)
{
    BOOL ok = TRUE;
    FILE_NAME path;
    _GetAppPath(path);
    FILE_NAME filename;
    if (ok &= Load())
    { // load all
        if (FileVersion == FILEHDR_VER)
        {  // same generation ?
            // load patch
            strcat(strcpy(filename, path), PH_PATCH_FILENAME);
            fstream file(filename, ios::binary|ios::in);
            if (file)
            {
                if (ok &= LoadHeader(file))
                {
                    if (FileVersion == FILEHDR_VER)
                    {  // same generation ?
                        if (ok &= LoadLockedNumbers(file))
                        {
                            if (ok &= LoadDDNTariffs(file))
                            {
                                if (ok &= LoadDDITariffs(file))
                                {
                                    if (ok &= LoadDDNSchedule(file))
                                    {
                                        if (ok &= LoadDDISchedule(file))
                                        {}


                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        ok = FALSE;
                    }
                }
            }
            else
            {
                ok = FALSE;
            }
            file.close();
            if (ok)
            {
                ok &= Save(); // save all
            }


        }
        else
        {
            ok = FALSE;
        }
    }
    return ok;
}