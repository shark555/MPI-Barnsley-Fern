#include "drawer.h"
#include "sdldrawer.h"
#include "pngdrawer.h"

Drawer* Drawer::getInstance(char* name,int width,int height,int r,int g,int b) {
    if(strcmp(name,"SDL")==0)
        return new SDLDrawer(width,height,r,g,b);
    if(strcmp(name,"PNG")==0)
        return new PngDrawer(width,height,r,g,b);    
}
