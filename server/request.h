#ifndef REQUEST_H
#define REQUEST_H

#include <microhttpd.h>

struct Request
{
  struct MHD_PostProcessor *pp;
  int device;
  short dir;
  short step;
  int pause;
  int active;
};


#endif
