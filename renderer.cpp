#include "renderer.h"

Renderer::Renderer(int step_v,int width_v, int height_v) {
    this->step = step_v;
    this->width = width_v;    
    this->height = height_v;
}

Renderer::Renderer(const Renderer& orig) {
}

Renderer::~Renderer() {
}

void Renderer::renderFrameSet(int framesetId) {

    MPI_Status status;
    int skala;    
    
    double* xxtab = (double*) malloc(2000000 * sizeof (double));
    double* yytab = (double*) malloc(2000000 * sizeof (double));

    int* xramka = (int*) malloc(2000000 * sizeof (int));
    int* yramka = (int*) malloc(2000000 * sizeof (int));

    int count;

    memset(xramka, '\0', 2000000);
    memset(yramka, '\0', 2000000);

//    for (int k = 0; k < 20; k++) {
//        MPI_Recv(xxtab + k * 100000, 100000, MPI_DOUBLE, 0, 2 * k + 1, MPI_COMM_WORLD, &status);
//        MPI_Recv(yytab + k * 100000, 100000, MPI_DOUBLE, 0, 2 * k + 2, MPI_COMM_WORLD, &status);
//    }
    
        MPI_Recv(xxtab, 2000000, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(yytab, 2000000, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);    

    int yoffset = 10;
    int j, l;

    ///1000 ramek
    for (j = framesetId; j < 1000; j += step) {

        ///Generacja ramki

        l = 0;
        count = 2000000;
        for (int i = 0; i < count; i++) {
            int wspx = (int) skala * xxtab[i] + 500;
            int wspy = (int) skala * yytab[i] - yoffset;
            if (wspx > 0 && wspx < width && wspy > 0 && wspy < height) {
                xramka[l] = wspx;
                yramka[l] = wspy;
                l++;
            }
            if (l > 500000)
                break;
        }

        printf("Rendering zakończony!\n");

        ///Wysyłanie ramki
        for (int z = 0; z < 2000; z++) {
            MPI_Send(xramka + z * 1000, 1000, MPI_INT, 1, framesetId*10000 + 2*z, MPI_COMM_WORLD);
            MPI_Send(yramka + z * 1000, 1000, MPI_INT, 1, framesetId*10000 + 2*z, MPI_COMM_WORLD);
            if (z * 1000 > l) {
                break;
            }
        }

        printf("Wysłano ramke %d\n",framesetId);
        MPI_Send(&l, 1, MPI_INT, 1, framesetId*10, MPI_COMM_WORLD);

        skala = 50 + 4 * j;
        yoffset = 10 + 20 * j;

    }
    
}