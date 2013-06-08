#include "sdldrawer.h"

SDLDrawer::SDLDrawer(int width,int height,int r_v,int g_v,int b_v,int framecount_v) {
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
    TTF_Init();
    font = TTF_OpenFont( "Vera.ttf", 15 );   
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 16,NULL, NULL, NULL, NULL); 
    SDL_WM_SetCaption( "Paproć Barnsleya", NULL );
    this->r = r_v;
    this->g = g_v;
    this->b = b_v;   
    this->framecount = framecount_v;
}

SDLDrawer::~SDLDrawer() {
        SDL_FreeSurface( message );
        TTF_CloseFont( font ); 
        TTF_Quit();
        SDL_Quit();
}

int SDLDrawer::drawFrame(int* xframe,int* yframe,int size,int frame_id) {           
    for(int i=0;i<size;i++) {
                drawPixel(r,g,b,xframe[i],yframe[i]);
    }
    SDL_BlitSurface(surface, NULL, screen,NULL);   
    SDL_Flip(screen);        
    SDL_FillRect(surface, NULL, 0);
    drawFrameInfo(frame_id,size);
    
    if (framecount - 1 == frame_id) {             
        bool quit = false;
        SDL_Event event;
        while (quit == false) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
            }
        }
    }
    
    return 0;
}

int SDLDrawer::drawPixel(Uint8 R, Uint8 G, Uint8 B, int x,int y)
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

void SDLDrawer::apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, NULL, destination, &offset );
}

void SDLDrawer::drawFrameInfo(int frame_id,int size) {
     char* text  = (char*)malloc(10*sizeof(char)); 
     sprintf(text,"%d - %d",frame_id,size);
     message = TTF_RenderText_Solid(font,text ,{255,255,255});  
     apply_surface( 5, 5, message, surface );     
}

