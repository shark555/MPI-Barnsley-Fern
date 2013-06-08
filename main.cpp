#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <jansson.h> 
#include "mpi.h"
#include "barnsley.h"
#include "drawer.h"
#include "renderer.h"

using namespace std;

int rank, size, source, dest, tag, len;
int i;
char name[20];

//OPTIONS
int width,height;
int color_r,color_g,color_b;
char* drawerAlgo;

void populateOptions();

int main(int argc, char *argv[]) {
        
    populateOptions();        
    
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

    //Rysowanie
    if (rank == 1) {
        
        int* xramka = (int*) malloc(datasize * sizeof (int));
        int* yramka = (int*) malloc(datasize * sizeof (int));
        int l = 0;

        Drawer* drawer = Drawer::getInstance(drawerAlgo,width,height,color_r,color_g,color_b,framecount);         
        
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
                                
                drawer->drawFrame(xramka, yramka, l,i+k-2);                
                printf("Narysowano %d\n", i+k-2);                
            }
            ////////////////////////////////////////

        }
        
    }

    //Rendering
    if (rank > 1) {
        Renderer renderer = Renderer(size-2,width,height,framecount,datasize);
        renderer.renderFrameSet(rank);
    }

    system("sleep 1");

    MPI_Finalize();
    return 0;
}

void populateOptions() {
    
    json_error_t error;
    json_t *json,*width_ob,*height_ob,*drawer_ob,*color_ob,*color_r_ob,*color_g_ob,*color_b_ob;
    
    json = json_load_file("config.json", 0, &error);

    if(json) {
        //Dimensions
        width_ob = json_object_get(json,"width");
        width = (int)json_integer_value(width_ob);        
        height_ob = json_object_get(json,"height");
        height = (int)json_integer_value(height_ob);  
        //Drawer algorithm
        drawer_ob = json_object_get(json,"drawer");
        drawerAlgo = (char*)json_string_value(drawer_ob);        
        //Color
        color_ob = json_object_get(json,"color"); 
        color_r_ob = json_object_get(color_ob,"r");
        color_g_ob = json_object_get(color_ob,"g");
        color_b_ob = json_object_get(color_ob,"b");  
        color_r = (int)json_integer_value(color_r_ob);
        color_g = (int)json_integer_value(color_g_ob);  
        color_b = (int)json_integer_value(color_b_ob);          
    }

}
