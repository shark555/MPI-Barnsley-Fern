#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL.h>
#include "mpi.h"

using namespace std;

int DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x,int y);
double* f(double x,double y); 
double* g(double x,double y); 
double* h(double x,double y); 
double* j(double x,double y); 


int rank,size,source,dest,tag,len;
char name[20];

int i;
double x = 10;
double y = 10;
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

////////////////////////


if(rank==0) {

double *xtab = (double*)malloc(2000000*sizeof(double));
double *ytab = (double*)malloc(2000000*sizeof(double));
double *tab = (double*)malloc(2*sizeof(double));
memset(xtab,'\0',2000000);
memset(ytab,'\0',2000000);

x=10;
y=10;

for(int i=0;i<2000000;i++) {

xtab[i] = x;
ytab[i] = y;

int los = rand()%100;
//printf("%d %d %lf %lf\n",i,los,x,y);

if(los<85) {tab = f(x,y);}
if(los>85 && los<92) {tab = g(x,y);}
if(los>92 && los<99) {tab = h(x,y);}
if(los==99) {tab = j(x,y);}

x = tab[0];
y = tab[1];

}

printf("Koniec generacji!\n");

for(int k=0;k<20;k++) {
MPI_Send(xtab+k*100000,100000,MPI_DOUBLE,1,2*k+1,MPI_COMM_WORLD);
MPI_Send(ytab+k*100000,100000,MPI_DOUBLE,1,2*k+2,MPI_COMM_WORLD);
}

for(int k=0;k<20;k++) {
MPI_Send(xtab+k*100000,100000,MPI_DOUBLE,3,2*k+1,MPI_COMM_WORLD);
MPI_Send(ytab+k*100000,100000,MPI_DOUBLE,3,2*k+2,MPI_COMM_WORLD);
}

for(int k=0;k<20;k++) {
MPI_Send(xtab+k*100000,100000,MPI_DOUBLE,4,2*k+1,MPI_COMM_WORLD);
MPI_Send(ytab+k*100000,100000,MPI_DOUBLE,4,2*k+2,MPI_COMM_WORLD);
}

for(int k=0;k<20;k++) {
MPI_Send(xtab+k*100000,100000,MPI_DOUBLE,5,2*k+1,MPI_COMM_WORLD);
MPI_Send(ytab+k*100000,100000,MPI_DOUBLE,5,2*k+2,MPI_COMM_WORLD);
}

printf("Wysłano fraktal!\n");

MPI_Finalize();

}


if(rank==1) {

double* xxtab = (double*)malloc(2000000*sizeof(double));
double* yytab = (double*)malloc(2000000*sizeof(double));

int* xramka = (int*)malloc(2000000*sizeof(int));
int* yramka = (int*)malloc(2000000*sizeof(int));

int count;

memset(xramka,'\0',2000000);
memset(yramka,'\0',2000000);


for(int k=0;k<20;k++) {
MPI_Recv(xxtab+k*100000,100000,MPI_DOUBLE,0,2*k+1,MPI_COMM_WORLD,&status);
MPI_Recv(yytab+k*100000,100000,MPI_DOUBLE,0,2*k+2,MPI_COMM_WORLD,&status);
}


int yoffset=10;
int j,l;

///1000 ramek
for(j=1;j<1000;j+=4) {

///Generacja ramki

l=0;
count = 2000000;
for(i=0;i<count;i++) {

int wspx = (int)skala*xxtab[i]+500;
int wspy = (int)skala*yytab[i]-yoffset;

if(wspx>0 && wspx<1024 && wspy>0 && wspy<768) {

xramka[l] = wspx;
yramka[l] = wspy;

l++;
} 

if(l>500000)
	break;

}

printf("Rendering zakończony!\n");

///Wysyłanie ramki
for(int z=0;z<2000;z++) {
MPI_Send(xramka+z*1000,1000,MPI_INT,2,600+2*z,MPI_COMM_WORLD);
MPI_Send(yramka+z*1000,1000,MPI_INT,2,601+2*z,MPI_COMM_WORLD);

if(z*1000>l) {break;}

}


printf("Wysłano ramke A\n");
MPI_Send(&l,1,MPI_INT,2,5000,MPI_COMM_WORLD);

skala=50+4*j;
yoffset=10+20*j;

}



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

//Rozmiar A
MPI_Recv(&l,1,MPI_INT,1,5000,MPI_COMM_WORLD,&status);
printf("Rozmiar ramki %d Ramka %d\n",l,4*i);

///Odbieranie ramki A 
for(int z=0;z<2000;z++) {
MPI_Recv(xramka+z*1000,1000,MPI_INT,1,600+2*z,MPI_COMM_WORLD,&status);
MPI_Recv(yramka+z*1000,1000,MPI_INT,1,601+2*z,MPI_COMM_WORLD,&status);

if(z*1000>l) {break;}

}
printf("Odebrano A\n");


////Rysowanie ramki A
for(int j=0;j<l;j++) {
DrawPixel(temp,0,255,0,xramka[j],yramka[j]);


}

SDL_BlitSurface(temp, NULL, screen,NULL);
SDL_Flip(screen);
SDL_FillRect(temp, NULL, 0);

printf("Narysowano A\n");


///Rozmiar B
MPI_Recv(&l,1,MPI_INT,3,20000,MPI_COMM_WORLD,&status);
printf("Rozmiar ramki %d Ramka %d\n",l,4*i+1);

///Odbieranie ramki B
for(int z=0;z<2000;z++) {
MPI_Recv(xramka+z*1000,1000,MPI_INT,3,6000+2*z,MPI_COMM_WORLD,&status);
MPI_Recv(yramka+z*1000,1000,MPI_INT,3,6001+2*z,MPI_COMM_WORLD,&status);

if(z*1000>l) {break;}

}
printf("Odebrano B\n");

//Rysowanie ramki B
for(int j=0;j<l;j++) {
DrawPixel(temp,0,255,0,xramka[j],yramka[j]);


}

SDL_BlitSurface(temp, NULL, screen,NULL);
SDL_Flip(screen);
SDL_FillRect(temp, NULL, 0);

printf("Narysowano B\n");

///Rozmiar C
MPI_Recv(&l,1,MPI_INT,4,70000,MPI_COMM_WORLD,&status);
printf("Rozmiar ramki %d Ramka %d\n",l,4*i+2);

///Odbieranie ramki C
for(int z=0;z<2000;z++) {
MPI_Recv(xramka+z*1000,1000,MPI_INT,4,8000+2*z,MPI_COMM_WORLD,&status);
MPI_Recv(yramka+z*1000,1000,MPI_INT,4,8001+2*z,MPI_COMM_WORLD,&status);

if(z*1000>l) {break;}


}
printf("Odebrano C\n");

//Rysowanie ramki C
for(int j=0;j<l;j++) {
DrawPixel(temp,0,255,0,xramka[j],yramka[j]);


}

SDL_BlitSurface(temp, NULL, screen,NULL);
SDL_Flip(screen);
SDL_FillRect(temp, NULL, 0);


///Rozmiar D
MPI_Recv(&l,1,MPI_INT,5,90000,MPI_COMM_WORLD,&status);
printf("Rozmiar ramki %d Ramka %d\n",l,4*i+3);

///Odbieranie ramki D
for(int z=0;z<2000;z++) {
MPI_Recv(xramka+z*1000,1000,MPI_INT,5,12000+2*z,MPI_COMM_WORLD,&status);
MPI_Recv(yramka+z*1000,1000,MPI_INT,5,12001+2*z,MPI_COMM_WORLD,&status);

if(z*1000>l) {break;}

}
printf("Odebrano D\n");

//Rysowanie ramki D
for(int j=0;j<l;j++) {
DrawPixel(temp,0,255,0,xramka[j],yramka[j]);


}

SDL_BlitSurface(temp, NULL, screen,NULL);
SDL_Flip(screen);
SDL_FillRect(temp, NULL, 0);
printf("Narysowano D\n");

}


}



if(rank==3) {

double* xxtab = (double*)malloc(2000000*sizeof(double));
double* yytab = (double*)malloc(2000000*sizeof(double));

int* xramka = (int*)malloc(2000000*sizeof(int));
int* yramka = (int*)malloc(2000000*sizeof(int));

int count;

memset(xramka,'\0',2000000);
memset(yramka,'\0',2000000);


for(int k=0;k<20;k++) {
MPI_Recv(xxtab+k*100000,100000,MPI_DOUBLE,0,2*k+1,MPI_COMM_WORLD,&status);
MPI_Recv(yytab+k*100000,100000,MPI_DOUBLE,0,2*k+2,MPI_COMM_WORLD,&status);
}


int yoffset=10;
int j,l;

///1000 ramek
for(j=2;j<1000;j+=4) {

///Generacja ramki


l=0;
count = 2000000;
for(i=0;i<count;i++) {

int wspx = (int)skala*xxtab[i]+500;
int wspy = (int)skala*yytab[i]-yoffset;

if(wspx>0 && wspx<1024 && wspy>0 && wspy<768) {

xramka[l] = wspx;
yramka[l] = wspy;

l++;
} 

if(l>500000)
	break;


}

printf("Rendering zakończony!\n");

///Wysyłanie ramki
for(int z=0;z<2000;z++) {
MPI_Send(xramka+z*1000,1000,MPI_INT,2,6000+2*z,MPI_COMM_WORLD);
MPI_Send(yramka+z*1000,1000,MPI_INT,2,6001+2*z,MPI_COMM_WORLD);

if(z*1000>l) {break;}

}


printf("Wysłano ramke B!!\n");
MPI_Send(&l,1,MPI_INT,2,20000,MPI_COMM_WORLD);

skala=50+4*j;
yoffset=10+20*j;

}

}

if(rank==4) {

double* xxtab = (double*)malloc(2000000*sizeof(double));
double* yytab = (double*)malloc(2000000*sizeof(double));

int* xramka = (int*)malloc(2000000*sizeof(int));
int* yramka = (int*)malloc(2000000*sizeof(int));

int count;

memset(xramka,'\0',2000000);
memset(yramka,'\0',2000000);



for(int k=0;k<20;k++) {
MPI_Recv(xxtab+k*100000,100000,MPI_DOUBLE,0,2*k+1,MPI_COMM_WORLD,&status);
MPI_Recv(yytab+k*100000,100000,MPI_DOUBLE,0,2*k+2,MPI_COMM_WORLD,&status);
}


int yoffset=10;
int j,l;

///1000 ramek
for(j=3;j<1000;j+=4) {

///Generacja ramki

l=0;
count = 6000000;
for(i=0;i<count;i++) {

int wspx = (int)skala*xxtab[i]+500;
int wspy = (int)skala*yytab[i]-yoffset;

if(wspx>0 && wspx<1024 && wspy>0 && wspy<768) {

xramka[l] = wspx;
yramka[l] = wspy;

l++;
} 

if(l>500000)
	break;

}

printf("Rendering zakończony!\n");

///Wysyłanie ramki
for(int z=0;z<2000;z++) {
MPI_Send(xramka+z*1000,1000,MPI_INT,2,8000+2*z,MPI_COMM_WORLD);
MPI_Send(yramka+z*1000,1000,MPI_INT,2,8001+2*z,MPI_COMM_WORLD);

if(z*1000>l) {break;}

}


printf("Wysłano ramke C!!\n");
MPI_Send(&l,1,MPI_INT,2,70000,MPI_COMM_WORLD);

skala=50+4*j;
yoffset=10+20*j;

}

}


if(rank==5) {

double* xxtab = (double*)malloc(2000000*sizeof(double));
double* yytab = (double*)malloc(2000000*sizeof(double));

int* xramka = (int*)malloc(2000000*sizeof(int));
int* yramka = (int*)malloc(2000000*sizeof(int));

int count;

memset(xramka,'\0',2000000);
memset(yramka,'\0',2000000);



for(int k=0;k<20;k++) {
MPI_Recv(xxtab+k*100000,100000,MPI_DOUBLE,0,2*k+1,MPI_COMM_WORLD,&status);
MPI_Recv(yytab+k*100000,100000,MPI_DOUBLE,0,2*k+2,MPI_COMM_WORLD,&status);
}


int yoffset=10;
int j,l;

///1000 ramek
for(j=4;j<1000;j+=4) {

///Generacja ramki

l=0;
count = 2000000;
for(i=0;i<count;i++) {

int wspx = (int)skala*xxtab[i]+500;
int wspy = (int)skala*yytab[i]-yoffset;

if(wspx>0 && wspx<1024 && wspy>0 && wspy<768) {

xramka[l] = wspx;
yramka[l] = wspy;

l++;
} 

if(l>500000)
	break;

}

printf("Rendering zakończony!\n");

///Wysyłanie ramki
for(int z=0;z<2000;z++) {
MPI_Send(xramka+z*1000,1000,MPI_INT,2,12000+2*z,MPI_COMM_WORLD);
MPI_Send(yramka+z*1000,1000,MPI_INT,2,12001+2*z,MPI_COMM_WORLD);

if(z*1000>l) {break;}

}


printf("Wysłano ramke D!!\n");
MPI_Send(&l,1,MPI_INT,2,90000,MPI_COMM_WORLD);

skala=50+4*j;
yoffset=10+20*j;

}

}




system("sleep 100");

MPI_Finalize();
return 0;
}





double* f(double x,double y) {
double* out;
out = (double*)malloc(2*sizeof(double));

out[0] = 0.85*x + 0.04*y; 
out[1] = -0.04*x + 0.85*y + 1.6;

return out;
}


double* g(double x,double y) {
double* out;
out = (double*)malloc(2*sizeof(double));

out[0] = -0.15*x + 0.28*y; 
out[1] = 0.26*x + 0.24*y + 0.44;

return out;
}

double* h(double x,double y) {
double* out;
out = (double*)malloc(2*sizeof(double));

out[0] = 0.2*x - 0.26*y; 
out[1] = 0.23*x + 0.22*y + 1.6;

return out;
}

double* j(double x,double y) {
double* out;
out = (double*)malloc(2*sizeof(double));

out[0] = 0;
out[1] = 0.16*y;

return out;
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
