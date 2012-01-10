#include "mainFrame.h"

size_t curl_write_func( char *ptr, size_t size, size_t nmemb, void *userdata)
{
  mainFrame *temp = (mainFrame *) userdata;
  temp->curl_write(ptr);
  return size;
}


