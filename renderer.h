#ifndef RENDERER_H
#define	RENDERER_H
#include "mpi.h"
#include <stdlib.h>

class Renderer {
public:
    Renderer(int step_v,int width_v,int height_v,int framecount_v,int framesize_v);
    Renderer(const Renderer& orig);
    virtual ~Renderer();
    void renderFrameSet(int framesetId);    
private:
    int step;
    int width;
    int height;
    int framecount;
    int framesize;
};

#endif	/* RENDERER_H */

