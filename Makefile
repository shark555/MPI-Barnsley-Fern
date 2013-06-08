main: barnsley.o drawer.o sdldrawer.o pngdrawer.o renderer.o main.o
	mpic++ -lSDL -lSDL_ttf -lpng -ljansson -o main barnsley.o drawer.o sdldrawer.o pngdrawer.o renderer.o main.o
	rm -fr *.o      

barnsley.o: barnsley.cpp barnsley.h
	mpic++ -c barnsley.cpp   

drawer.o: drawer.cpp drawer.h
	mpic++ -I/usr/include/SDL -c drawer.cpp

sdldrawer.o: sdldrawer.cpp sdldrawer.h 
	mpic++ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -c sdldrawer.cpp   

pngdrawer.o: pngdrawer.cpp pngdrawer.h
	mpic++ -lpng -c pngdrawer.cpp

renderer.o: renderer.cpp renderer.h
	mpic++ -c renderer.cpp

main.o: main.cpp
	mpic++ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -c main.cpp

clean:
	@rm -fr *.o main
