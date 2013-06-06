#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mpi.h"
#include "barnsley.h"
#include "drawer.h"
#include "pngdrawer.h"
#include<string.h>
#include "renderer.h"

using namespace std;

int rank, size, source, dest, tag, len;
char name[20];

int i;

int main(int argc, char *argv[]) {

    /*
    int datasize = 2000000;  
    int framecount = 1000;
    */   
    int datasize = atoi(argv[1]);  
    int framecount = atoi(argv[2]);  
 
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &len);
    srand(time(0));
    printf("%s\n", name);

    ///Generacja fraktala
    if (rank == 0) {

        double *xtab;
        double *ytab;
        double** fractal;
        
        Barnsley barnsley = Barnsley(datasize);
        fractal = barnsley.generate();
        xtab = fractal[0];
        ytab = fractal[1];

        //printf("Koniec generacji!\n");
        
        //Wysyłanie danych do kolejnych rendererów (startuje od 2 bo 0 i 1 to główne workery)
        for (int j = 2; j<size; j++) {
                MPI_Send(xtab, datasize, MPI_DOUBLE, j, 1, MPI_COMM_WORLD);
                MPI_Send(ytab, datasize, MPI_DOUBLE, j, 2, MPI_COMM_WORLD);            
        }

        //printf("Wysłano fraktal!\n");

    }

    //Rysowanie na SDLu
    if (rank == 1) {

        int* xramka = (int*) malloc(datasize * sizeof (int));
        int* yramka = (int*) malloc(datasize * sizeof (int));
        int l = 0;

        Drawer drawer = Drawer(640,480,0,255,100);
//        PngDrawer drawer = PngDrawer(1024,768);            
        
        int allframes = 0;
        for (int i = 0; i < framecount; i=i+(size-2)) {
                       
            for (int k = 2; k<size; k++) {                
                //Rozmiar ramki
                MPI_Recv(&l, 1, MPI_INT, k, i+k-2, MPI_COMM_WORLD, &status);
                printf("Odbieram ramkę od %d o ID:%d\n",k, i+k-2);                
                printf("Rozmiar ramki %d - %d\n",i+k-2, l);
                
                ///Odbieranie ramki 
                for (int z = 0; z < datasize/1000; z++) {
                    MPI_Recv(xramka + z * 1000, 1000, MPI_INT, k, (i+k-2)*(datasize/1000)+z, MPI_COMM_WORLD, &status);
                    MPI_Recv(yramka + z * 1000, 1000, MPI_INT, k, (i+k-2)*(datasize/1000)+z, MPI_COMM_WORLD, &status);
                    if (z * 1000 > l) {
                        break;
                    }
                } 
                                
                drawer.drawFrame(xramka, yramka, l);
                //char* filename = (char*)malloc(10*sizeof(char));                
                //sprintf(filename,"images/%d.png",allframes);
                
                //drawer.writeImage(filename);
                //printf("Narysowano %d\n", allframes);                
                //printf("Narysowano %d\n", senders[k]);
                allframes++;                
            }
            ////////////////////////////////////////

        }

        
    }

    //Rendering
    if (rank > 1) {
        Renderer renderer = Renderer(size-2,640,480,framecount,datasize);
        renderer.renderFrameSet(rank);
    }

    system("sleep 1");

    MPI_Finalize();
    return 0;
}

