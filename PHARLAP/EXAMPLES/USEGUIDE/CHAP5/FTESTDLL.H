/* TESTDLL.H */
#define EXPORT      _far _loadds _export
#define P_EXPORT    EXPORT _pascal
#define C_EXPORT    EXPORT _cdecl

void P_EXPORT put(double y);
void C_EXPORT get_half(double *xp);
