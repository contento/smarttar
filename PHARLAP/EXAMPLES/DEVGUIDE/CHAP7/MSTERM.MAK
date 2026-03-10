# makefile for TERMP, TERMR, TERM2

# You can add -Zi here for debugging
COPT=-Ox -G2

# You can add /CO here for debugging
PLOPT=/far/noi
RLOPT=

# For MSC5.1, 6.0, uncomment next line
#SLIBPE=slibpe
# For MSC/C++ 7.0, uncomment next line
SLIBPE=

# If you renamed your compilers, you can specify that here
CL=cl
LINK=link

# NMAKE requires a single target (unlike old MS MAKE)
all: term2.exe termp.exe termr.exe


# terminal mainline to run entirely in real mode
termr.obj: term.c io.h queue.h
    $(CL) -c -Fotermr.obj $(COPT) term.c

# terminal mainline to run in prot mode with PassToProt handler
termp.obj: term.c io.h queue.h
    $(CL) -c -Fotermp.obj $(COPT) -Lp -DDOSX286 term.c

# terminal mainline to run with prot and real mode handlers
term2.obj: term.c io.h queue.h
    $(CL) -c -Foterm2.obj $(COPT) -Lp -DDOSX286 -DBOTHMODES term.c

# IRQ4 interrupt service routine for real mode containing I/O queues
irq4r.obj: irq4.c io.h queue.h
    $(CL) -c -Foirq4r.obj $(COPT) irq4.c

# IRQ4 interrupt service routine for prot mode containing I/O queues
irq4p.obj: irq4.c io.h queue.h
    $(CL) -c -Foirq4p.obj $(COPT) -Lp -DDOSX286 irq4.c

# IRQ4 interrupt service routine for prot mode using external I/O queues
irq4nqp.obj: irq4.c io.h queue.h
    $(CL) -c -Foirq4nqp.obj $(COPT) -Lp -DDOSX286 -DNOQUEUES irq4.c

# IRQ4 interrupt service routine for real mode DLL containing I/O queues
irq4dllr.obj: irq4.c io.h queue.h
    $(CL) -c -Foirq4dllr.obj $(COPT) -DDLL irq4.c

# Queue routines can run in both modes
queue.obj: queue.c queue.h
    $(CL) $(COPT) -c queue.c

termp.exe: termp.obj irq4p.obj queue.obj
    $(LINK) $(PLOPT) termp irq4p queue,termp,,/nod:slibce slibcep $(SLIBPE);
    bind286 termp

termr.exe: termr.obj irq4r.obj queue.obj
    $(LINK) $(RLOPT) termr irq4r queue,termr,,,;

termdll.dll: irq4dllr.obj queue.obj
    $(LINK) /nod irq4dllr queue,termdll.dll,,,termdll.def
    markreal termdll.dll

termdll.lib: termdll.dll
    implib termdll.lib termdll.def

term2.exe: term2.obj irq4nqp.obj queue.obj termdll.lib
    $(LINK) $(PLOPT) term2 irq4nqp queue,term2,,/nod:slibce slibcep $(SLIBPE) termdll;
    bind286 term2

