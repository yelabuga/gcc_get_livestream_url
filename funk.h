#ifndef FUNK_H
# define FUNK_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct my_buffer {
    unsigned char *memory;
    size_t size;
    size_t used;
};

size_t my_curl_write(char *ptr, size_t size, size_t nmemb, void *userdata);
CURLcode get_url(const char *url, struct my_buffer *buffer, char *curl_error);
int strpos(char *hay, char *needle, int offset);
char* substring(char *destination, const char *source, int beg, int n);

#endif