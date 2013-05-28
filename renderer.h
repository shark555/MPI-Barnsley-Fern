#ifndef RENDERER_H
#define	RENDERER_H
#include "mpi.h"
#include <stdlib.h>

class Renderer {
public:
    Renderer(int step_v,int width,int height);
    Renderer(const Renderer& orig);
    virtual ~Renderer();
    void renderFrameSet(int framesetId);    
private:
    int step;
    int width;
    int height;
};

#endif	/* RENDERER_H */

