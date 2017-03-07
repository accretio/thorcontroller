/*
 * Manual controller for thor
 *
 */

#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef TEST
#include <pigpio.h>
#endif

#include "gpiomapping.h"

#define RESPONSE_OK "OK"
#define RESPONSE_ERROR "ERROR"

#define DEFAULT_PAUSE 4000 

struct Request
{
  struct MHD_PostProcessor *pp;
  int device;
  short dir;
  short step;
  int pause;
  int active;
};


int
execute_operate(struct Request *request) {

  printf("executing request on device %d\n", request->device);
  
  if (request->device < 0 || request->device > 7) {
    return -1;
  }

  // set the motors position

  if (gpioWrite(DIRECTION_PIN(MOTOR_1(request->device)), request->dir)) {
    printf("couldn't write to the direction pin");
    return -1;
  }

  if (MOTOR_2(request->device)) {
    if (gpioWrite(DIRECTION_PIN(MOTOR_2(request->device)), DIFFERENTIAL(request->device, request->dir))) {
      printf("couldn't write to the direction pin");
      return -1;
    }
  }

  // now we step
  
  if (gpioWrite(STEP_PIN(MOTOR_1(request->device)), 1)) {
    printf("couldn't write to the step pin\n");
    return -1;
  }

  if (MOTOR_2(request->device)) {
    if (gpioWrite(STEP_PIN(MOTOR_2(request->device)), 1)) {
      printf("couldn't write to the step pin\n");
      return -1;
    }
  }

  if (gpioWrite(STEP_PIN(MOTOR_1(request->device)), 0)) {
    printf("couldn't write to the step pin\n");
    return -1;
  }
  
  if (MOTOR_2(request->device)) {
    if (gpioWrite(STEP_PIN(MOTOR_2(request->device)), 0)) {
      printf("couldn't write to the step pin\n");
      return -1;
    }
  }

  // and finally we pause

  gpioSleep(PI_TIME_RELATIVE, 0, DEFAULT_NEMA_PAUSE);
 
  return 0;
}


int
execute_admin(struct Request *request) {

  printf("execute admin request\n");

  switch(request->active) {
  case 0:
    printf("deactivating the steppers\n");
    /* if (gpioWrite(GPIO_ACTIVATE_PIN, 1)) {
       printf("couldn't write to the activate pin\n");
       return -1;
       } */
      break; 
  case 1:
    printf("activating the steppers\n");
      /*  if (gpioWrite(GPIO_ACTIVATE_PIN, 0)) {
          printf("couldn't write to the activate pin\n");
          return -1;
          } */
    break; 
  default:
    return -1; 
  }

  return 0;
}

static void
request_completed_callback (void *cls,
			    struct MHD_Connection *connection,
			    void **con_cls,
			    enum MHD_RequestTerminationCode toe)
{
  struct Request *request = *con_cls;
  if (NULL == request)
    return;
  if (NULL != request->pp)
    MHD_destroy_post_processor (request->pp);
  free (request);
}

static int
post_iterator (void *cls,
	       enum MHD_ValueKind kind,
	       const char *key,
	       const char *filename,
	       const char *content_type,
	       const char *transfer_encoding,
	       const char *data, uint64_t off, size_t size)
{
  struct Request *request = cls;

  
  if (0 == strcmp ("device", key))
    {
      request->device = atoi(data);
      return MHD_YES;
    }

  if (0 == strcmp ("dir", key))
    {
      request->dir = atoi(data);
      return MHD_YES;
    }

  if (0 == strcmp ("step", key))
    {
      request->step = atoi(data);
      return MHD_YES;
    }

  if (0 == strcmp ("pause", key))
    {
      request->pause = atoi(data);
      return MHD_YES;
    }

  return MHD_NO;
  
}

static int
post_iterator_admin (void *cls,
                     enum MHD_ValueKind kind,
                     const char *key,
                     const char *filename,
                     const char *content_type,
                     const char *transfer_encoding,
                     const char *data, uint64_t off, size_t size)
{
  struct Request *request = cls;

  
  if (0 == strcmp ("active", key))
    {
      request->active = atoi(data);
      return MHD_YES;
    }

  return MHD_NO;
  
}

static void
add_cors_headers (struct MHD_Response *response)
{
  MHD_add_response_header (response,
                           "Access-Control-Allow-Origin",
                           "*");
  MHD_add_response_header (response,
                           "Access-Control-Allow-Methods",
                           "GET, POST, OPTIONS");
  // TODO: pull the headers from the request?
  MHD_add_response_header (response,
                           "Access-Control-Allow-Headers",
                           "Accept, Accept-CH, Accept-Charset, Accept-Datetime, Accept-Encoding, Accept-Ext, Accept-Features, Accept-Language, Accept-Params, Accept-Ranges, Access-Control-Allow-Credentials, Access-Control-Allow-Headers, Access-Control-Allow-Methods, Access-Control-Allow-Origin, Access-Control-Expose-Headers, Access-Control-Max-Age, Access-Control-Request-Headers, Access-Control-Request-Method, Age, Allow, Alternates, Authentication-Info, Authorization, C-Ext, C-Man, C-Opt, C-PEP, C-PEP-Info, CONNECT, Cache-Control, Compliance, Connection, Content-Base, Content-Disposition, Content-Encoding, Content-ID, Content-Language, Content-Length, Content-Location, Content-MD5, Content-Range, Content-Script-Type, Content-Security-Policy, Content-Style-Type, Content-Transfer-Encoding, Content-Type, Content-Version, Cookie, Cost, DAV, DELETE, DNT, DPR, Date, Default-Style, Delta-Base, Depth, Derived-From, Destination, Differential-ID, Digest, ETag, Expect, Expires, Ext, From, GET, GetProfile, HEAD, HTTP-date, Host, IM, If, If-Match, If-Modified-Since, If-None-Match, If-Range, If-Unmodified-Since, Keep-Alive, Label, Last-Event-ID, Last-Modified, Link, Location, Lock-Token, MIME-Version, Man, Max-Forwards, Media-Range, Message-ID, Meter, Negotiate, Non-Compliance, OPTION, OPTIONS, OWS, Opt, Optional, Ordering-Type, Origin, Overwrite, P3P, PEP, PICS-Label, POST, PUT, Pep-Info, Permanent, Position, Pragma, ProfileObject, Protocol, Protocol-Query, Protocol-Request, Proxy-Authenticate, Proxy-Authentication-Info, Proxy-Authorization, Proxy-Features, Proxy-Instruction, Public, RWS, Range, Referer, Refresh, Resolution-Hint, Resolver-Location, Retry-After, Safe, Sec-Websocket-Extensions, Sec-Websocket-Key, Sec-Websocket-Origin, Sec-Websocket-Protocol, Sec-Websocket-Version, Security-Scheme, Server, Set-Cookie, Set-Cookie2, SetProfile, SoapAction, Status, Status-URI, Strict-Transport-Security, SubOK, Subst, Surrogate-Capability, Surrogate-Control, TCN, TE, TRACE, Timeout, Title, Trailer, Transfer-Encoding, UA-Color, UA-Media, UA-Pixels, UA-Resolution, UA-Windowpixels, URI, Upgrade, User-Agent, Variant-Vary, Vary, Version, Via, Viewport-Width, WWW-Authenticate, Want-Digest, Warning, Width, X-Content-Duration, X-Content-Security-Policy, X-Content-Type-Options, X-CustomHeader, X-DNSPrefetch-Control, X-Forwarded-For, X-Forwarded-Port, X-Forwarded-Proto, X-Frame-Options, X-Modified, X-OTHER, X-PING, X-PINGOTHER, X-Powered-By, X-Requested-With, X-Api-Key");
  MHD_add_response_header (response,
                           "Access-Control-Max-Age",
                           "86400");
}

static int ahc_op(void * cls,
                  struct MHD_Connection * connection,
                  const char * url,
                  const char * method,
                  const char * version,
                  const char * upload_data,
                  size_t * upload_data_size,
                  void ** ptr) {

  struct Request *request;
  struct MHD_Response * response;
  int code;
  int ret; 
  request = *ptr;
  
  if (0 == strcmp (MHD_HTTP_METHOD_OPTIONS, method))
    {
      
      response = MHD_create_response_from_buffer (0, NULL,
                                                  MHD_RESPMEM_PERSISTENT);
      add_cors_headers(response);
      ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
      MHD_destroy_response (response);
      return ret;
    }
  
  if (0 != strcmp(method, MHD_HTTP_METHOD_POST))
    return MHD_NO; /* unexpected method */
  
  if (NULL == request)
    {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      request = calloc (1, sizeof (struct Request));
      
      request->pause = DEFAULT_PAUSE;
      request->device = -1;
      request->dir = 0;
      request->step = 0;
      
      if (NULL == request)
        {
          /* Couldn't allocate the request */
          return MHD_NO;
        }
      *ptr = request;

      if (0 == strcmp(url, "/operate")) {
        request->pp = MHD_create_post_processor (connection, 1024,
                                                 &post_iterator, request);
      } else if (0 == strcmp(url, "/admin")) {
        request->pp = MHD_create_post_processor (connection, 1024, &post_iterator_admin, request); 
      }
      
      if (NULL == request->pp)
        {
          /* Couldn't allocate the post processor */
          printf("couldn't allocate the post processor for url %s\n", url);
          return MHD_NO;
        }
      
      return MHD_YES;
    }

  /* process the post data */
  
  MHD_post_process (request->pp,
                    upload_data,
                    *upload_data_size);

  if (0 != *upload_data_size)
    {
      *upload_data_size = 0;
      return MHD_YES;
    }
  
  
  if (0 == strcmp(url, "/operate")) {
    ret = execute_operate(request);
  } else if (0 == strcmp(url, "/admin")) {
    ret = execute_admin(request);
  }
 
  if (0 != ret) {
    response = MHD_create_response_from_buffer (strlen(RESPONSE_ERROR),
                                                (void*) RESPONSE_ERROR,
                                                MHD_RESPMEM_PERSISTENT);   
    code = MHD_HTTP_INTERNAL_SERVER_ERROR;
      
  } else {
    response = MHD_create_response_from_buffer (strlen(RESPONSE_OK),
                                                (void*) RESPONSE_OK,
                                                MHD_RESPMEM_PERSISTENT);
    code = MHD_HTTP_OK;
  }
  
  MHD_add_response_header (response,
                           "Access-Control-Allow-Origin", 
                           "*"); 
  
  ret = MHD_queue_response(connection,
                           code,
                           response);
   
  MHD_destroy_response(response);
  return ret;
}

int main(int argc,
	 char ** argv) {
  struct MHD_Daemon * d;
  if (argc != 2) {
    printf("%s PORT\n",
	   argv[0]);
    return 1;
  }

  d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
                       atoi(argv[1]),
                       NULL,
                       NULL,
                       &ahc_op,
                       NULL,
                       MHD_OPTION_CONNECTION_LIMIT, 20,
                       MHD_OPTION_NOTIFY_COMPLETED, &request_completed_callback, NULL,
                       MHD_OPTION_END);

  if (d == NULL)
    return 1;

  if (gpioInitialise() < 0) {
    printf("couldn't initialize the gpio lib\n");
    return -1;
  }
  
  (void) getc (stdin);
  MHD_stop_daemon(d);
  return 0;
}
