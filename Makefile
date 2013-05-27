main: barnsley.o main.o
	mpic++ -lSDL -o main barnsley.o main.o
	rm -fr *.o      

barnsley.o: barnsley.cpp barnsley.h
	mpic++ -c barnsley.cpp   

main.o: main.cpp
	mpic++ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -c main.cpp

clean:
	@rm -fr *.o main
