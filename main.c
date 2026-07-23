#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "funk.h"
#define MY_INITIAL_BUFFER_SIZE (1)

int main(int argc, char **argv)
{
    CURLcode result;
    struct my_buffer buffer;
    char curl_error[CURL_ERROR_SIZE];

    buffer.memory = malloc(MY_INITIAL_BUFFER_SIZE);
    buffer.size = MY_INITIAL_BUFFER_SIZE;
    buffer.used = 0;

    result = get_url("https://www.googleapis.com/youtube/v3/search?part=snippet&amp;channelId=[YOUR_CHANNEL_ID]&amp;eventType=live&type=video&amp;key=[YOUR_API_KEY]", &amp;buffer, curl_error);
    if (result == 0)
	{
	char * inhalt = NULL;
	inhalt = malloc(1 + buffer.used);
	memcpy(inhalt, buffer.memory, buffer.used);
	inhalt[buffer.used] = '\0';
	if (inhalt)
	{
		int pos = strpos(inhalt, "\"videoId\": \"", 0);
		if (pos == -1) {}
		int endpos = strpos (inhalt, "\"", pos+10);
		char vid[12];
		substring(vid, inhalt, endpos+1, endpos - pos);
		FILE * fp;
		fp = fopen ("/var/www/html/url.php", "w");
		fprintf(fp, "<script> jsvid = \"https://www.youtube.com/watch?v=%s%s", vid, "\";</script>");
		fclose(fp);
		char line[75];
		char * ssh = "ssh uhla@sborka.dev -p 12345 echo ";
		char * fname = " \\>\\> /var/www/html/url.php";
		char * command;
		asprintf(&amp;command, "%s%s%s", ssh, vid, fname);
		puts(command);
		FILE *cmd = popen(command, "r");
		// popen("ssh uhla@elabu.ga -p 12036 pidof ffmpeg", "r");
		fgets(line, 75, cmd);
		pid_t pid = strtoul(line, NULL, 10);
		printf("pid: %d", pid);
		pclose(cmd);
    	}
	else
	{
       		puts(curl_error);
	}
	} // result == 0
   return 0;
} // main