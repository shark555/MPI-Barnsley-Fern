#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "mpi.h"
#include "barnsley.h"
#include "drawer.h"
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

        printf("Koniec generacji!\n");
        
        //Wysyłanie danych do kolejnych rendererów (startuje od 2 bo 0 i 1 to główne workery)
        for (int j = 2; j<size; j++) {
                MPI_Send(xtab, datasize, MPI_DOUBLE, j, 1, MPI_COMM_WORLD);
                MPI_Send(ytab, datasize, MPI_DOUBLE, j, 2, MPI_COMM_WORLD);            
        }

        printf("Wysłano fraktal!\n");

        MPI_Finalize();

    }

    //Rysowanie na SDLu
    if (rank == 1) {

        int* xramka = (int*) malloc(datasize * sizeof (int));
        int* yramka = (int*) malloc(datasize * sizeof (int));
        int l = 0;

        Drawer drawer = Drawer(640,480,0,255,100);

        for (int i = 0; i < framecount; i++) {
            
            int* senders = (int*) malloc((size-2) * sizeof (int));
            for(int k=2;k<size;k++) {
                senders[k-2] = k;
            }

            for (int k = 0; k<sizeof (senders) / sizeof (int); k++) {                
                //Rozmiar ramki
                MPI_Recv(&l, 1, MPI_INT, senders[k], senders[k]*10, MPI_COMM_WORLD, &status);
                //printf("Rozmiar ramki %d Ramka %d\n", l, 4 * i);
                ///Odbieranie ramki 
                //Zamienić to na pojedynczy recv całej tablicy
                for (int z = 0; z < datasize/1000; z++) {
                    MPI_Recv(xramka + z * 1000, 1000, MPI_INT, senders[k], senders[k]*100000 + 2 * z, MPI_COMM_WORLD, &status);
                    MPI_Recv(yramka + z * 1000, 1000, MPI_INT, senders[k], senders[k]*100000 + 2 * z, MPI_COMM_WORLD, &status);
                    if (z * 1000 > l) {
                        break;
                    }
                }                                           
                printf("Odebrano %d\n", senders[k]);
                drawer.drawFrame(xramka, yramka, l);
                printf("Narysowano %d\n", senders[k]);
            }
            ////////////////////////////////////////

        }


    }

    //Rendering
    if (rank > 1) {
        Renderer renderer = Renderer(size-2,640,480,framecount,datasize);
        renderer.renderFrameSet(rank);
    }

    system("sleep 100");

    MPI_Finalize();
    return 0;
}

