#ifndef __D_EXT_ST_H
#define __D_EXT_ST_H

#if !defined(__DSTORAGE_H)
#include <dstorage.h>
#endif

#if !defined(__DXS_ENTR_H)
#include <dxs_entr.h>
#endif

#if !defined(__FILEHDR_H)
#include <filehdr.h>
#endif

class DB_EXT_STATISTICS
{
public:
    DB_EXT_STATISTICS(const char *path, const char *name, int readOnly = TRUE);
    ~DB_EXT_STATISTICS(void);
    //
    //
    enum STAT {
        OK        = 0x0000,
        NO_FILE   = 0x0001,
        BAD_FILE  = 0x0020,
        NEW       = 0x2000
    };
    //
    DXS_NON_CRITICAL_ENTRY *GetNonCriticalEntry(WORD extNum=0) const ;
    BOOL                PutNonCriticalEntry(WORD extNum, DXS_NON_CRITICAL_ENTRY& entry);
    DXS_CRITICAL_ENTRY *GetCritical(void) const;
    BOOL     PutCritical(DXS_CRITICAL_ENTRY& critical);
    BOOL     Store(WORD extNum);
    //
    void Flush(void);
    BOOL Archive(void);
    BOOL Repair(BinStorage *dBStorage, BOOL all = FALSE);
	BOOL Add(Receipt& receipt);
	BOOL Subtract(Receipt& receipt);
    WORD GetStatus(void)
    {
        return Status;
    }
    BOOL IsReadOnly(void)
    {
        return ReadOnly;
    }
private:
    DXS_NON_CRITICAL_ENTRY *NonCritical;
    DXS_CRITICAL_ENTRY     *Critical;
    //
    FILE_NAME Filename;
    int File;
    BOOL ReadOnly;
    WORD Status;
    //
    void Init(void);
};

#endif // __D_EXT_ST_H
