#ifndef DRAWER_H
#define	DRAWER_H
#include <SDL.h>

class Drawer {
public:
    Drawer(int width,int height,int r,int g,int b);
    int drawFrame(int* xframe,int* yframe,int size);
private:
    int r,g,b;
    SDL_Surface *screen,*surface;
    int drawPixel(Uint8 R, Uint8 G, Uint8 B, int x,int y);
};

#endif	/* DRAWER_H */

