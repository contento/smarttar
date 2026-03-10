/* PTRACE.H */

/* DosPTrace commands */
#define READ_MEM_I_SPACE        0x0001  // read instruction-space (code)
#define READ_MEM_D_SPACE        0x0002  // read data-space
#define READ_REGISTERS          0x0003
#define WRITE_MEM_I_SPACE       0x0004
#define WRITE_MEM_D_SPACE       0x0005
#define WRITE_REGISTERS         0x0006
#define GO                      0x0007
#define TERMINATE_CHILD         0x0008
#define SINGLE_STEP             0x0009
#define STOP_CHILD              0x000A
#define FREEZE_CHILD            0x000B
#define RESUME_CHILD            0x000C
#define SEG_NUM_TO_SELECTOR     0x000D // turn .MAP segnum into selector
#define GET_FLOATINGPT_REGS     0x000E
#define SET_FLOATINGPT_REGS     0x000F
#define GET_DLL_NAME            0x0010
#define THREAD_STATUS           0x0011
#define MAP_READONLY_ALIAS      0x0012
#define MAP_READWRITE_ALIAS     0x0013
#define UNMAP_ALIAS             0x0014

/* DosPTrace events */
#define EVENT_SUCCESS           0
#define EVENT_ERROR             -1
#define EVENT_SIGNAL            -2
#define EVENT_SINGLESTEP        -3
#define EVENT_BREAKPOINT        -4
#define EVENT_PARITYERROR       -5
#define EVENT_DYING             -6
#define EVENT_GP_FAULT          -7
#define EVENT_LOAD_DLL          -8
#define EVENT_FLOATPT_ERROR     -9
#define EVENT_THREAD_DEAD       -10
#define EVENT_ASYNC_STOP        -11
#define EVENT_NEW_PROCESS       -12
#define EVENT_ALIAS_FREE        -13

/* DosPTrace error types */
#define ERROR_BAD_COMMAND       1
#define ERROR_CHILD_NOTFOUND    2
#define ERROR_UNTRACEABLE       5

/* Thread states */
#define THREAD_RUNNABLE         0
#define THREAD_SUSPENDED        1
#define THREAD_BLOCKED          2
#define THREAD_CRITSEC          3

/* Thread debug states */
#define THREAD_THAWED           0
#define THREAD_FROZEN           1

#if 1
// these were left out of the printed version
typedef struct PTRACEBUF {
    unsigned pid, tid;
    unsigned cmd, value, offv, segv, mte;
    unsigned rAX, rBX, rCX, rDX, rSI, rDI, rBP;
    unsigned rDS, rES;
    unsigned rIP, rCS, rF;
    unsigned rSP, rSS;
    } PTRACEBUF, far *PPTRACEBUF;

USHORT APIENTRY DosPTrace(PPTRACEBUF pPtraceBuf);
#endif
