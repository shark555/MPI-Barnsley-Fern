#ifndef SDLDRAWER_H
#define	SDLDRAWER_H
#include <SDL.h>
#include "SDL/SDL_ttf.h"
#include "drawer.h"

class SDLDrawer : public Drawer {
public:
    SDLDrawer(int width,int height,int r,int g,int b);
    virtual ~SDLDrawer();
    int drawFrame(int* xframe,int* yframe,int size,int frame_id);
private:
    int r,g,b;
    SDL_Surface *screen,*surface,*message;
    TTF_Font *font = NULL;
    int drawPixel(Uint8 R, Uint8 G, Uint8 B, int x,int y);
    void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination);
    void drawFrameInfo(int frame_id,int size);
};

#endif	/* SDLDRAWER_H */

