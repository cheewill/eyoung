#ifndef HTTP_PRIVATE_H
#define HTTP_PRIVATE_H 1

#include "ey_memory.h"
#include "libengine_type.h"
#include "http_type.h"
#include "http_client_parser.h"
#include "http_server_parser.h"
#include "http_client_lex.h"
#include "http_server_lex.h"

struct yy_buffer_state;
extern struct yy_buffer_state* http_client_scan_stream(const char *new_buf, size_t new_buf_len, http_data_t *priv);
extern struct yy_buffer_state* http_server_scan_stream(const char *new_buf, size_t new_buf_len, http_data_t *priv);

extern int parse_http_client_stream(http_data_t *priv, const char *buf, size_t buf_len, int last_frag);
extern int parse_http_server_stream(http_data_t *priv, const char *buf, size_t buf_len, int last_frag);

#define DEFAULT_VALUE_LENGTH	128
typedef struct http_decoder
{
	engine_t engine;

	/*for request*/
	ey_fslab_t http_request_value_fslab;
	ey_slab_t http_request_first_line_slab;
	ey_slab_t http_request_header_slab;
	ey_slab_t http_request_string_part_slab;
	ey_slab_t http_request_chunk_part_slab;
	ey_slab_t http_request_body_slab;
	ey_slab_t http_request_slab;
	
	/*for response*/
	ey_fslab_t http_response_value_fslab;
	ey_slab_t http_response_first_line_slab;
	ey_slab_t http_response_header_slab;
	ey_slab_t http_response_string_part_slab;
	ey_slab_t http_response_chunk_part_slab;
	ey_slab_t http_response_body_slab;
	ey_slab_t http_response_slab;

	ey_slab_t http_data_slab;
	ey_slab_t http_cmd_slab;
}http_decoder_t;

extern void http_server_register(http_decoder_t *decoder);
extern void http_client_register(http_decoder_t *decoder);
extern int http_element_detect(http_data_t *http_data, const char *event_name, int event_id, void *event,
	char *cluster_buffer, size_t cluster_buffer_len);

/*memory mgt system init api for system initializing*/
extern int http_mem_init(http_decoder_t *decoder);
extern void http_mem_finit(http_decoder_t *decoder);

/*slab mgt api*/
extern http_data_t* http_alloc_priv_data(http_decoder_t *decoder, int greedy);
extern void http_free_priv_data(http_decoder_t *decoder, http_data_t *priv_data);

extern http_cmd_t* http_alloc_cmd(http_decoder_t *decoder, http_request_t *request, http_response_t *response);
extern void http_free_cmd(http_decoder_t *decoder, http_cmd_t *cmd);
extern void http_free_cmd_list(http_decoder_t *decoder, http_cmd_list_t *cmd_list);

/*
 * FOR COMMON DATA
 * */
extern char* http_alloc_string(http_decoder_t *decoder, const char *i_str, size_t i_len, http_string_t *o_str, int from_client);
#define http_client_alloc_string(decoder,i_str,i_len,o_str) http_alloc_string(decoder,i_str,i_len,o_str,1)
#define http_server_alloc_string(decoder,i_str,i_len,o_str) http_alloc_string(decoder,i_str,i_len,o_str,0)

extern void http_free_string(http_decoder_t *decoder, ey_string_t *string, int from_client);
#define http_client_free_string(decoder,string) http_free_string(decoder,string,1)
#define http_server_free_string(decoder,string) http_free_string(decoder,string,0)

extern http_string_list_part_t* http_alloc_string_list_part(http_decoder_t *decoder, const http_string_t *src, int from_client);
#define http_client_alloc_string_list_part(decoder,string) http_alloc_string_list_part(decoder,string,1)
#define http_server_alloc_string_list_part(decoder,string) http_alloc_string_list_part(decoder,string,0)

extern void http_free_string_list_part(http_decoder_t *decoder, http_string_list_part_t *src, int from_client);
#define http_client_free_string_list_part(decoder,src) http_free_string_list_part(decoder,src,1)
#define http_server_free_string_list_part(decoder,src) http_free_string_list_part(decoder,src,0)

extern void http_free_string_list(http_decoder_t *decoder, http_string_list_t *list_head, int from_client);
#define http_client_free_string_list(decoder,list) http_free_string_list(decoder,list,1)
#define http_server_free_string_list(decoder,list) http_free_string_list(decoder,list,0)

extern http_chunk_body_part_t* http_alloc_chunk_body_part(http_decoder_t *decoder, int from_client);
#define http_client_alloc_chunk_body_part(decoder) http_alloc_chunk_body_part(decoder,1)
#define http_server_alloc_chunk_body_part(decoder) http_alloc_chunk_body_part(decoder,0)

extern void http_free_chunk_body_part(http_decoder_t *decoder, http_chunk_body_part_t *src, int from_client);
#define http_client_free_chunk_body_part(decoder,src) http_free_chunk_body_part(decoder,src,1)
#define http_server_free_chunk_body_part(decoder,src) http_free_chunk_body_part(decoder,src,0)

extern void http_free_chunk_body_list(http_decoder_t *decoder, http_chunk_body_list_t *list, int from_client);
#define http_client_free_chunk_body_list(decoder,list) http_free_chunk_body_list(decoder,list,1)
#define http_server_free_chunk_body_list(decoder,list) http_free_chunk_body_list(decoder,list,0)

extern void http_free_chunk_body(http_decoder_t *decoder, http_chunk_body_t *body, int from_client);
#define http_client_free_chunk_body(decoder,body) http_free_chunk_body(decoder,body,1)
#define http_server_free_chunk_body(decoder,body) http_free_chunk_body(decoder,body,0)

extern http_body_t* http_alloc_body(http_decoder_t *decoder, int from_client);
#define http_client_alloc_body(decoder) http_alloc_body(decoder,1)
#define http_server_alloc_body(decoder) http_alloc_body(decoder,0)

extern void http_free_body(http_decoder_t *decoder, http_body_t *body, int from_client);
#define http_client_free_body(decoder,body) http_free_body(decoder,body,1)
#define http_server_free_body(decoder,body) http_free_body(decoder,body,0)
/*
 * FOR REQUEST
 * */
/*alloc/free request*/
extern http_request_t* http_client_alloc_request(http_decoder_t *decoder,
										  http_request_first_line_t *first_line,
										  http_request_header_list_t *header_list,
										  http_body_t *body);
extern void http_client_free_request(http_decoder_t *decoder, http_request_t*request);
extern void http_client_free_request_list(http_decoder_t *decoder, http_request_list_t*request_list);

/*alloc/free first_line*/
extern http_request_first_line_t *http_client_alloc_first_line(http_decoder_t *decoder,
										  http_request_method_t method,
										  http_string_t *uri,
										  http_version_t version);
extern void http_client_free_first_line(http_decoder_t *decoder, http_request_first_line_t *first_line);

/*alloc/free header*/
extern http_request_header_t* http_client_alloc_header(http_decoder_t *decoder,
										  http_request_header_type_t type,
										  http_string_t *value);
extern void http_client_free_header(http_decoder_t *decoder, http_request_header_t *header);
extern void http_client_free_header_list(http_decoder_t *decoder, http_request_header_list_t *header_list);

/*
 * FOR RESPONSE
 * */
/*alloc/free response*/
extern http_response_t* http_server_alloc_response(http_decoder_t *decoder,
										  http_response_first_line_t *first_line,
										  http_response_header_list_t *header_list,
										  http_body_t *body);
extern void http_server_free_response(http_decoder_t *decoder, http_response_t *response);
extern void http_server_free_response_list(http_decoder_t *decoder, http_response_list_t *response_list);

/*alloc/free first_line*/
extern http_response_first_line_t *http_server_alloc_first_line(http_decoder_t *decoder,
										  http_version_t version,
										  http_response_code_t code,
										  http_string_t *message);
extern void http_server_free_first_line(http_decoder_t *decoder, http_response_first_line_t *first_line);

/*alloc/free header*/
extern http_response_header_t* http_server_alloc_header(http_decoder_t *decoder,
										  http_response_header_type_t type,
										  http_string_t *value);
extern void http_server_free_header(http_decoder_t *decoder, http_response_header_t *header);
extern void http_server_free_header_list(http_decoder_t *decoder, http_response_header_list_t *header_list);
#endif