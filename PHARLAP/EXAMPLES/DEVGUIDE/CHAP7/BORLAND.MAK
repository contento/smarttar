
all: sig.exe ctrl_c.exe criterr.exe timeloop.exe int33p.exe \
      termr.exe termp.exe term2.exe

sig.exe: sig.c
	bcc286 sig.c

ctrl_c.exe: ctrl_c.c
	bcc286 ctrl_c.c 

criterr.exe: criterr.c
	bcc286 criterr.c 

timeloop.exe: timeloop.c
	bcc286 timeloop.c

int33p.exe: int33p.c
	bcc286 int33p.c

# makefile for TERMP, TERMR, TERM2

# You can add -v here for debugging
COPT=

# You can add /CO here for debugging
PLOPT=
RLOPT=

# terminal mainline to run entirely in real mode
termr.obj: term.c io.h queue.h
    bcc -c -ml -otermr.obj $(COPT) term.c

# terminal mainline to run in prot mode with PassToProt handler
termp.obj: term.c io.h queue.h
    bcc286 -c -otermp.obj $(COPT) term.c

# terminal mainline to run with prot and real mode handlers
term2.obj: term.c io.h queue.h
    bcc286 -c -oterm2.obj $(COPT) -DBOTHMODES term.c

# IRQ4 interrupt service routine for real mode containing I/O queues
irq4r.obj: irq4.c io.h queue.h
    bcc -c -ml -oirq4r.obj $(COPT) irq4.c

# IRQ4 interrupt service routine for prot mode containing I/O queues
irq4p.obj: irq4.c io.h queue.h
    bcc286 -c -oirq4p.obj $(COPT) irq4.c

# IRQ4 interrupt service routine for prot mode using external I/O queues
irq4nqp.obj: irq4.c io.h queue.h
    bcc286 -c -oirq4nqp.obj $(COPT) -DNOQUEUES irq4.c

# IRQ4 interrupt service routine for real mode DLL containing I/O queues
irq4dllr.obj: irq4.c io.h queue.h
    bcc -c -ml -oirq4dllr.obj $(COPT) -DDLL irq4.c

# Queue routines can run in both modes
queue.obj: queue.c queue.h
    bcc -c -ml $(COPT) queue.c

termr.exe: termr.obj irq4r.obj queue.obj
    tlink $(RLOPT) c0l termr irq4r queue,termr,,cl,;

termp.exe: termp.obj irq4p.obj queue.obj
    bcc286 termp.obj irq4p.obj queue.obj 

termdll.dll: irq4dllr.obj queue.obj
    tlink /nod irq4dllr queue,termdll.dll,,cl,termdll.def
    markreal termdll

termdll.lib: termdll.dll
    implib termdll.lib termdll.def

term2.exe: term2.obj irq4nqp.obj queue.obj termdll.lib
    bcc286 term2.obj irq4nqp.obj queue.obj termdll.lib 
