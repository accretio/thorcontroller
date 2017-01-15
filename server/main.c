/*
 * Manual controller for thor
 *
 */

#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pigpio.h>

#include "gpiomapping.h"

#define RESPONSE_OK "OK"
#define RESPONSE_ERROR "ERROR"

#define DEFAULT_PAUSE 4000

struct Request
{
  struct MHD_PostProcessor *pp;
  int motor;
  short dir;
  short step;
  int pause;
};


int
execute(struct Request *request) {

  printf("executing request with motor %d\n", request->motor);
  
  if (request->motor < 0 || request->motor > 6) {
    return -1;
  }
  
  if (gpioWrite(motor_dir_gpio[request->motor - 1], request->dir)) {
    printf("couldn't write to the direction pin");
    return -1;
  }
  
  if (gpioWrite(motor_step_gpio[request->motor - 1], 1)) {
    printf("couldn't write to the step pin\n");
    return -1;
  }
  
  if (gpioWrite(motor_step_gpio[request->motor - 1], 0)) {
    printf("couldn't write to the step pin\n");
    return -1;
  }
  
  gpioSleep(PI_TIME_RELATIVE, 0, DEFAULT_NEMA_PAUSE);
 
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

  
  if (0 == strcmp ("motor", key))
    {
      request->motor = atoi(data);
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


  if (0 != strcmp(method, MHD_HTTP_METHOD_POST))
    return MHD_NO; /* unexpected method */
  
  if (NULL == request)
    {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      request = calloc (1, sizeof (struct Request));
      
      request->pause = DEFAULT_PAUSE;
      request->motor = -1;
      request->dir = 0;
      request->step = 0;
      
      if (NULL == request)
        {
          /* Couldn't allocate the request */
          return MHD_NO;
        }
      *ptr = request;
      request->pp = MHD_create_post_processor (connection, 1024,
                                               &post_iterator, request);
      
      if (NULL == request->pp)
        {
          /* Couldn't allocate the post processor */
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
  

  
   
  if (0 != execute(request)) {
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
