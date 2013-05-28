main: barnsley.o drawer.o renderer.o main.o
	mpic++ -lSDL -o main barnsley.o drawer.o renderer.o main.o
	rm -fr *.o      

barnsley.o: barnsley.cpp barnsley.h
	mpic++ -c barnsley.cpp   

drawer.o: drawer.cpp drawer.h
	mpic++ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -c drawer.cpp   

renderer.o: renderer.cpp renderer.h
	mpic++ -c renderer.cpp

main.o: main.cpp
	mpic++ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -c main.cpp

clean:
	@rm -fr *.o main
