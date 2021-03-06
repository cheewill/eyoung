#ifdef HTTP_MAIN

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libengine.h"
#include "http.h"
#include "html.h"

int parse_http_file(http_handler_t decoder, const char *filename)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	int ret = -1;
	http_work_t *work = NULL;
	int lines = 0;
	FILE *fp = fopen(filename, "r");
	if(!fp)
	{
		fprintf(stderr, "failed to open file %s\n", filename);
		goto failed;
	}

	work = http_work_create(decoder, 0);
	if(!work)
	{
		fprintf(stderr, "failed to alloc http private data\n");
		goto failed;
	}

	while ((read = getline(&line, &len, fp)) != -1)
	{
		lines++;
		if(read <=1)
		{
			fprintf(stderr, "invalid line(%d): %s\n", lines, line);
			goto failed;
		}
		if(toupper(line[0])=='C' && line[1]==':')
		{
			if(http_decode_data(work, line+2, read-2, 1, 0))
			{
				fprintf(stderr, "parse client failed, line(%d): %s\n", lines, line);
				goto failed;
			}
		}
		else if (toupper(line[0])=='S' && line[1]==':')
		{
			if(http_decode_data(work, line+2, read-2, 0, 0))
			{
				fprintf(stderr, "parse server failed, line(%d): %s\n", lines, line);
				goto failed;
			}
		}
		else
		{
			fprintf(stderr, "invalid line(%d): %s\n", lines, line);
			goto failed;
		}
	}

	/*give end flag to parser*/
	if(http_decode_data(work, "", 0, 1, 1))
	{
		fprintf(stderr, "parse client end flag failed");
		goto failed;
	}
	if(http_decode_data(work, "", 0, 0, 1))
	{
		fprintf(stderr, "parse server end flag failed");
		goto failed;
	}

	ret = 0;
	/*pass through*/
	fprintf(stderr, "parser OK!\n");

failed:
	if(work)
		http_work_destroy(work);
	if(line)
		free(line);
	if(fp)
		fclose(fp);
	return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	http_handler_t decoder = NULL;
	html_handler_t html_decoder = NULL;
	engine_t engine = NULL;
	if(argc != 4)
	{
		fprintf(stderr, "Usage: http_parser <http signature file> <html signature file> <message_file>\n");
		return -1;
	}
	debug_http_server_lexer = 1;
	debug_http_server_parser = 1;
	debug_http_client_lexer = 1;
	debug_http_client_parser = 1;
	debug_http_mem = 1;
	debug_http_detect = 1;

	debug_html_lexer = 1;
	debug_html_parser = 1;
	debug_html_mem = 1;
	debug_html_detect = 1;

	debug_engine_parser = 0;
	debug_engine_lexier = 0;
	debug_engine_init = 0;
	debug_engine_compiler = 0;
	debug_engine_runtime = 0;
	
	engine = ey_engine_create("http");
	if(!engine)
	{
		fprintf(stderr, "create http engine failed\n");
		ret = -1;
		goto failed;
	}
	
	if(ey_engine_load(engine, &argv[1], 2))
	{
		fprintf(stderr, "load http signature failed\n");
		ret = -1;
		goto failed;
	}
	
	html_decoder = html_decoder_init(engine);
	if(!html_decoder)
	{
		fprintf(stderr, "create html decoder failed\n");
		ret = -1;
		goto failed;
	}

	decoder = http_decoder_init(engine, html_decoder);
	if(!decoder)
	{
		fprintf(stderr, "create http decoder failed\n");
		ret = -1;
		goto failed;
	}

	ret = parse_http_file(decoder, argv[3]);

failed:
	if(decoder)
		http_decoder_finit(decoder);
	if(html_decoder)
		html_decoder_finit(html_decoder);
	if(engine)
		ey_engine_destroy(engine);
	return ret;
}
#endif
