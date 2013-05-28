#include "drawer.h"

Drawer::Drawer(int width,int height,int r_v,int g_v,int b_v) {
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    screen = SDL_SetVideoMode(width, height, 16, SDL_HWSURFACE |SDL_DOUBLEBUF);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set %dx%d video: %s\n",width,height,SDL_GetError());
        exit(1);
    }
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 1024, 768, 16,NULL, NULL, NULL, NULL); 
    this->r = r_v;
    this->g = g_v;
    this->b = b_v;    
}

int Drawer::drawFrame(int* xframe,int* yframe,int size) {        
        for(int i=0;i<size;i++) {
                drawPixel(r,g,b,xframe[i],yframe[i]);
        }
        SDL_BlitSurface(surface, NULL, screen,NULL);   
        SDL_Flip(screen);        
        SDL_FillRect(surface, NULL, 0);
        return 0;
}

int Drawer::drawPixel(Uint8 R, Uint8 G, Uint8 B, int x,int y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return 0;
        }
    }
    Uint16 *bufp;
    bufp = (Uint16 *)surface->pixels + y*surface->pitch/2 + x;
    *bufp = color;
    if ( SDL_MUSTLOCK(surface) ) {
        SDL_UnlockSurface(surface);
    }
    SDL_UpdateRect(surface, x, y, 1, 1);

    return 0;
}