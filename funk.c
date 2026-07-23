#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "funk.h"

#define MY_MAXIMUM_BUFFER_SIZE (4 * 1024 * 1024)

/* калбак */
extern size_t my_curl_write(char *ptr, size_t size, size_t nmemb, void *userdata){
    struct my_buffer *buffer = userdata;
    size_t needed = size * nmemb;

    if (needed > (buffer->size - buffer->used)) {
        unsigned char *new_memory;
        size_t new_size = 2 * buffer->size;
        while (needed > (new_size - buffer->used)) {
            new_size *= 2;
            if (new_size > (MY_MAXIMUM_BUFFER_SIZE)) {
                return 0;
            }
        }
        new_memory = realloc(buffer->memory, new_size);
        if (!new_memory) {
            return 0;
        }
        buffer->memory = new_memory;
        buffer->size = new_size;
    }
    memcpy(buffer->memory + buffer->used, ptr, needed);
    buffer->used += needed;
    return needed;
}

extern CURLcode get_url(const char *url, struct my_buffer *buffer, char *curl_error) {
    CURLcode result;
    CURL *my_curl = curl_easy_init();

    curl_easy_setopt(my_curl, CURLOPT_ERRORBUFFER, curl_error);
    curl_easy_setopt(my_curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(my_curl, CURLOPT_URL, url);
    curl_easy_setopt(my_curl, CURLOPT_WRITEFUNCTION, my_curl_write);
    curl_easy_setopt(my_curl, CURLOPT_WRITEDATA, buffer);

    result = curl_easy_perform(my_curl);

    curl_easy_cleanup(my_curl);

    return result;
}

int strpos(char *hay, char *needle, int offset)
{
   char haystack[strlen(hay)];
   strncpy(haystack, hay+offset, strlen(hay)-offset);
   char *p = strstr(haystack, needle);
   if (p)
      return p - haystack+offset;
   return -1;
}

char* substring(char *destination, const char *source, int beg, int n)
{
	while (n > 0)
	{
		*destination = *(source + beg);

		destination++;
		source++;
		n--;
	}

	// null terminate destination string
	*destination = '\0';

	return destination;
}