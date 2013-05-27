#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL.h>
#include "mpi.h"
#include "barnsley.h"

using namespace std;

int DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x,int y);
void renderFrameSet(int framesetId,int step,MPI_Status status);

int rank,size,source,dest,tag,len;
char name[20];

int i;
int skala;

SDL_Surface *screen;
SDL_Surface *temp;


int main(int argc, char *argv[])
{

 MPI_Status status;
 MPI_Init(&argc,&argv);
 MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 MPI_Comm_size(MPI_COMM_WORLD,&size);
 MPI_Get_processor_name(name,&len);
 srand(time(0));

if(rank==0) {

double *xtab;
double *ytab;
double** fractal;
    
Barnsley barnsley = Barnsley();
fractal = barnsley.generate();  
xtab = fractal[0];
ytab = fractal[1];

printf("Koniec generacji!\n");

//Wysyłanie fraktala(jako double) do rendererów
//To powinno być konfigurowalne

int* senders = (int*)malloc(4*sizeof(int));
senders[0] = 1;
senders[1] = 3;
senders[2] = 4;
senders[3] = 5;

for(int j=0;j<sizeof(senders)/sizeof(int);j++) {
        for(int k=0;k<20;k++) {
                MPI_Send(xtab+k*100000,100000,MPI_DOUBLE,senders[j],2*k+1,MPI_COMM_WORLD);
                MPI_Send(ytab+k*100000,100000,MPI_DOUBLE,senders[j],2*k+2,MPI_COMM_WORLD);
        }
}

printf("Wysłano fraktal!\n");

MPI_Finalize();

}

if(rank==2) {

int* xramka = (int*)malloc(2000000*sizeof(int));
int* yramka = (int*)malloc(2000000*sizeof(int));
int l=0;

memset(xramka,'\0',2000000);
memset(yramka,'\0',2000000);

    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

screen = SDL_SetVideoMode(1024, 768, 16, SDL_HWSURFACE |SDL_DOUBLEBUF);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }

temp = SDL_CreateRGBSurface(SDL_HWSURFACE, 1024, 768, 16,
                                   NULL, NULL, NULL, NULL);


for(int i=0;i<1000;i++) {

int* senders = (int*)malloc(4*sizeof(int));
senders[0] = 1;
senders[1] = 3;
senders[2] = 4;
senders[3] = 5;

for(int k=0;k<sizeof(senders)/sizeof(int);k++) {

//Rozmiar ramki
MPI_Recv(&l,1,MPI_INT,senders[k],senders[k]*10,MPI_COMM_WORLD,&status);
printf("Rozmiar ramki %d Ramka %d\n",l,4*i);

///Odbieranie ramki 
for(int z=0;z<2000;z++) {
        MPI_Recv(xramka+z*1000,1000,MPI_INT,senders[k],senders[k]*10000+2*z,MPI_COMM_WORLD,&status);
        MPI_Recv(yramka+z*1000,1000,MPI_INT,senders[k],senders[k]*10000+2*z,MPI_COMM_WORLD,&status);
        if(z*1000>l) {break;}
}
printf("Odebrano %d\n",senders[k]);

////Rysowanie ramki
for(int j=0;j<l;j++) {
        DrawPixel(temp,0,255,0,xramka[j],yramka[j]);
}

SDL_BlitSurface(temp, NULL, screen,NULL);
SDL_Flip(screen);
SDL_FillRect(temp, NULL, 0);

printf("Narysowano %d\n",senders[k]);

}
////////////////////////////////////////

}


}

if(rank!=0 && rank!=2) {
    renderFrameSet(rank,4,status);
}
 
system("sleep 500");

MPI_Finalize();
return 0;
}

int DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x,int y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return 0;
        }
    }
            Uint16 *bufp;
            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
    SDL_UpdateRect(screen, x, y, 1, 1);

    return 0;
}

void renderFrameSet(int framesetId,int step,MPI_Status status) {

    double* xxtab = (double*) malloc(2000000 * sizeof (double));
    double* yytab = (double*) malloc(2000000 * sizeof (double));

    int* xramka = (int*) malloc(2000000 * sizeof (int));
    int* yramka = (int*) malloc(2000000 * sizeof (int));

    int count;

    memset(xramka, '\0', 2000000);
    memset(yramka, '\0', 2000000);

    for (int k = 0; k < 20; k++) {
        MPI_Recv(xxtab + k * 100000, 100000, MPI_DOUBLE, 0, 2 * k + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(yytab + k * 100000, 100000, MPI_DOUBLE, 0, 2 * k + 2, MPI_COMM_WORLD, &status);
    }

    int yoffset = 10;
    int j, l;

    ///1000 ramek
    for (j = framesetId; j < 1000; j += step) {

        ///Generacja ramki

        l = 0;
        count = 2000000;
        for (i = 0; i < count; i++) {
            int wspx = (int) skala * xxtab[i] + 500;
            int wspy = (int) skala * yytab[i] - yoffset;
            if (wspx > 0 && wspx < 1024 && wspy > 0 && wspy < 768) {
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
            MPI_Send(xramka + z * 1000, 1000, MPI_INT, 2, framesetId*10000 + 2*z, MPI_COMM_WORLD);
            MPI_Send(yramka + z * 1000, 1000, MPI_INT, 2, framesetId*10000 + 2*z, MPI_COMM_WORLD);
            if (z * 1000 > l) {
                break;
            }
        }

        printf("Wysłano ramke %d\n",framesetId);
        MPI_Send(&l, 1, MPI_INT, 2, framesetId*10, MPI_COMM_WORLD);

        skala = 50 + 4 * j;
        yoffset = 10 + 20 * j;

    }


}