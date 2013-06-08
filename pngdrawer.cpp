#include "pngdrawer.h"

PngDrawer::PngDrawer(int width_v,int height_v,int r_v,int g_v,int b_v) {
    width = width_v;
    height = height_v;
    r = r_v;
    g = g_v;
    b = b_v;
    buffer = (float*)malloc(width*height*sizeof(float));
}

PngDrawer::PngDrawer(const PngDrawer& orig) {
}

PngDrawer::~PngDrawer() {
    free(buffer);
}

int PngDrawer::writeImage(char* filename)
{
	int code = 0;
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;
	
	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0 ; y<height ; y++) {
		for (x=0 ; x<width ; x++) {
			if(buffer[y*width + x]==1) {
                            row[3*x] = r;
                            row[3*x+1] = g;
                            row[3*x+2] = b;                            
                        } else {
                            row[3*x] = 0;
                            row[3*x+1] = 0;
                            row[3*x+2] = 0;                            
                        }
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);
                
	return code;
}

float* PngDrawer::drawPixel(int x,int y,float color)
{
    buffer[y*width+x] = color;
    return buffer;
}


int PngDrawer::drawFrame(int* xframe,int* yframe,int size,int frame_id) {
        int count =  width*height;
        for(int i=count-1;i>=0;i--) {
            buffer[i] = 0.0;
        }
        for(int i=0;i<size;i++) {
                drawPixel(xframe[i],yframe[i],1);
        }
        char* filename = (char*)malloc(10*sizeof(char));                
        sprintf(filename,"images/%d.png",frame_id);                
        writeImage(filename);        
        return 0;
}

