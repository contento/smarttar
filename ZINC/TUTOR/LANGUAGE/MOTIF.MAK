PROGS = lang

CXXFLAGS= -g -I../../include -I/usr/include/X11R4 -I/usr/include/Motif1.1
LFLAGS=
LIBS=-L../../lib -l_mtf_zil -L/usr/lib/Motif1.1 -lXm -L/usr/lib/X11R4 -lXt -lX11 -lc

motif: lang

lang: lang.o lang_win.o
	$(CXX) $(LFLAGS) -o $@ lang.o lang_win.o $(LIBS)

clean:
	-rm -f lang *.o
