#ifndef DRAWER_H
#define	DRAWER_H
#include<string.h>

class Drawer {
public:
        virtual int drawFrame(int* xframe,int* yframe,int size,int frame_id) = 0;
        static Drawer* getInstance(char* name,int width,int height,int r,int g,int b,int framecount);
};

#endif	/* DRAWER_H */

