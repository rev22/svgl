#include <svgl/screenshot.hpp>
#include <stdio.h>
#include <svgl/debug.hpp>

namespace svgl {

  void
  screenshot(unsigned int w, unsigned int h, const char* path, GLenum screenshotType)
  {
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    int pixel_len;
    int maxvalue;
    unsigned char* pixels;
    const char * pnm_type;
    const char *pam="pam";
    //const char *pgm="pgm", *ppm="ppm";
    const char* pnm_suffix=0;

    switch(screenshotType) {
    case GL_RGB:
      pnm_type = "P6";
      pixel_len=3;
      maxvalue=255;
      break;
    case GL_STENCIL_INDEX:
      pnm_type = "P5";
      pixel_len=1;
      maxvalue=4;
      break;
    case GL_RGBA:
      //pnm_type = "P7";
      pnm_type = "P8";
      pixel_len=4;
      maxvalue=255;
      //pnm_suffix=pam;
      break;
    default:
      std::cerr << "unknown screenshot type" << __FL__;
      return;
    }

    FILE* f=fopen(path,"w");
    if(!f) {
      fprintf(stderr, "unable to open %s\n", path);
      return;
    }

    int len=pixel_len*w*h;
    pixels = new unsigned char[len];
    glReadPixels(0,0, w,h, screenshotType, GL_UNSIGNED_BYTE, pixels);

    if(pnm_suffix==pam) {
      fprintf(f, "%s\nWIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL %d\nTUPLETYPE RGBA\nENDHDR\n",pnm_type, w,h, pixel_len, maxvalue);
    }
    else {
      fprintf(f, "%s %d %d %d\n",pnm_type, w,h, maxvalue);
    }
    // reversed
    //  fwrite( pixels, 1, len, f);

    for(int i=h-1; i>=0;--i) {
      fwrite( &pixels[i*pixel_len*w], pixel_len, w, f);
    }

    fclose(f);
    delete [] pixels;
  }

}
  
