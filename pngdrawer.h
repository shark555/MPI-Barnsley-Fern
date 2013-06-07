#include <stdio.h> 
#include <math.h> 
#include <malloc.h> 
#include <png.h>
#ifndef PNGDRAWER_H
#define PNGDRAWER_H

class PngDrawer {
public:
    PngDrawer(int width,int height);
    PngDrawer(const PngDrawer& orig);
    virtual ~PngDrawer();
    float* drawPixel(int width, int height,float color);
    int writeImage(char* filename);
    int drawFrame(int* xframe,int* yframe,int size,int frame_id);    
private:
    int width;
    int height;
    float *buffer;
    void  setRGB(png_byte *ptr, float val);
};

#endif  /* PNGDRAWER_H */

