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

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &len);
    srand(time(0));
    printf("%s\n", name);

    if (rank == 0) {

        double *xtab;
        double *ytab;
        double** fractal;

        Barnsley barnsley = Barnsley();
        fractal = barnsley.generate();
        xtab = fractal[0];
        ytab = fractal[1];

        printf("Koniec generacji!\n");

        for (int j = 2; j<size; j++) {
                MPI_Send(xtab, 2000000, MPI_DOUBLE, j, 1, MPI_COMM_WORLD);
                MPI_Send(ytab, 2000000, MPI_DOUBLE, j, 2, MPI_COMM_WORLD);            
        }

        printf("Wysłano fraktal!\n");

        MPI_Finalize();

    }

    if (rank == 1) {

        int* xramka = (int*) malloc(2000000 * sizeof (int));
        int* yramka = (int*) malloc(2000000 * sizeof (int));
        int l = 0;

        memset(xramka, '\0', 2000000);
        memset(yramka, '\0', 2000000);

        Drawer drawer = Drawer(1024,768,0,255,100);

        for (int i = 0; i < 1000; i++) {
            
            int* senders = (int*) malloc(4 * sizeof (int));
            for(int k=2;k<size;k++) {
                senders[k-2] = k;
            }

            for (int k = 0; k<sizeof (senders) / sizeof (int); k++) {

                //Rozmiar ramki
                MPI_Recv(&l, 1, MPI_INT, senders[k], senders[k]*10, MPI_COMM_WORLD, &status);
                printf("Rozmiar ramki %d Ramka %d\n", l, 4 * i);

                ///Odbieranie ramki 
                //Zamienić to na pojedynczy recv całej tablicy
                for (int z = 0; z < 2000; z++) {
                    MPI_Recv(xramka + z * 1000, 1000, MPI_INT, senders[k], senders[k]*10000 + 2 * z, MPI_COMM_WORLD, &status);
                    MPI_Recv(yramka + z * 1000, 1000, MPI_INT, senders[k], senders[k]*10000 + 2 * z, MPI_COMM_WORLD, &status);
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

    if (rank > 1) {
        Renderer renderer = Renderer(4,1024,768);
        renderer.renderFrameSet(rank);
    }

    system("sleep 500");

    MPI_Finalize();
    return 0;
}

