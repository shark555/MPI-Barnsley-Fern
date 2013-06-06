#include "renderer.h"

Renderer::Renderer(int step_v,int width_v, int height_v,int framecount_v,int framesize_v) {
    this->step = step_v;
    this->width = width_v;    
    this->height = height_v;
    this->framecount = framecount_v;
    this->framesize = framesize_v;
}

Renderer::Renderer(const Renderer& orig) {
}

Renderer::~Renderer() {
}

void Renderer::renderFrameSet(int rank) {

    MPI_Status status;
    int skala;    
    
    double* xxtab = (double*) malloc(framesize * sizeof (double));
    double* yytab = (double*) malloc(framesize * sizeof (double));

    int* xramka = (int*) malloc(framesize * sizeof (int));
    int* yramka = (int*) malloc(framesize * sizeof (int));

    int count;

    memset(xramka, '\0', framesize);
    memset(yramka, '\0', framesize);
    
    MPI_Recv(xxtab, framesize, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(yytab, framesize, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);    

    int yoffset = 10;
    int j, l;

    for (j = rank-2; j < framecount; j += step) {

        //printf("%d -> %d\n",framesetId,j);
        
        ///Generacja ramki
        l = 0;
        for (int i = 0; i < framesize; i++) {
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

        //printf("Rendering zakończony!\n");
        ///Wysyłanie ramki
        
        MPI_Send(&l, 1, MPI_INT, 1, j, MPI_COMM_WORLD);        
        
        for (int z = 0; z < framesize/1000; z++) {
            MPI_Send(xramka + z * 1000, 1000, MPI_INT, 1, j*(framesize/1000)+z, MPI_COMM_WORLD);
            MPI_Send(yramka + z * 1000, 1000, MPI_INT, 1, j*(framesize/1000)+z, MPI_COMM_WORLD);
            printf("Wysyłam paczkę %d od %d\n",(rank-2)*(framesize/1000)+z,rank);
            if (z * 1000 > l) {
                break;
            }
        }

        printf("Wysłano ramke %d - wygenerował worker %d - ID:%d\n",j,rank,j);

        skala = 50 + 4 * j;
        yoffset = 10 + 20 * j;

    }
    
}
