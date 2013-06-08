#ifndef PNGDRAWER_H
#define PNGDRAWER_H
#include <stdio.h> 
#include <math.h> 
#include <malloc.h> 
#include <png.h>
#include "drawer.h"

class PngDrawer : public Drawer {
public:
    PngDrawer(int width,int height,int r,int g,int b);
    PngDrawer(const PngDrawer& orig);
    virtual ~PngDrawer();
    float* drawPixel(int width, int height,float color);
    int drawFrame(int* xframe,int* yframe,int size,int frame_id);    
private:
    int width;
    int height;
    int r,g,b;
    float *buffer;
    int writeImage(char* filename);    
};

#endif  /* PNGDRAWER_H */

