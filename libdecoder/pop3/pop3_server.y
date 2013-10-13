%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "pop3.h"
#include "pop3_detect.h"
#include "pop3_util.h"
#include "pop3_type.h"
#include "pop3_mem.h"
#include "pop3_client_parser.h"
#include "pop3_server_parser.h"
#include "pop3_client_lex.h"
#include "pop3_server_lex.h"
#include "pop3_private.h"

static int pop3_session_add_command(pop3_data_t *priv_data);
static int pop3_do_state_check(pop3_data_t *priv_data);
static void pop3_do_state_transfer(pop3_data_t *priv_data);

#ifdef YY_REDUCTION_CALLBACK
#undef YY_REDUCTION_CALLBACK
#endif
#define YY_REDUCTION_CALLBACK(name,val)											\
	do																			\
	{																			\
		if(pop3_do_rule_detect((pop3_data_t*)priv_data, name, (void*)val) < 0)	\
		{																		\
			pop3_attack(debug_pop3_ips, "find attack by signature\n");			\
			YYABORT;															\
		}																		\
	}while(0)
%}

%token TOKEN_SERVER_DOT
%token TOKEN_SERVER_OK
%token TOKEN_SERVER_ERROR
%token TOKEN_SERVER_STRING

%token TOKEN_SERVER_CONTINUE

%debug
%verbose
%defines "pop3_server_parser.h"
%output "pop3_server_parser.c"
%define api.prefix pop3_server_
%define api.pure full
%define api.push-pull push
%parse-param {void *priv_data}

%union
{
	struct
	{
		char *str;
		int str_len;
	}string;
	pop3_res_content_t content;
	pop3_response_t *response;
}

%type <string>		positive_response_line
					negative_response_line
%type <content>		positive_response_lines
					positive_response_message
%type <response>	positive_response
					negative_response
					response

%destructor
{
	if($$.str)
		pop3_free($$.str);
}positive_response_line negative_response_line

%destructor
{
	pop3_free_response_content(&$$);
}positive_response_lines positive_response_message

%destructor
{
	pop3_free_response($$);
}positive_response negative_response

%start response_list
%%
response_list:
	{
		pop3_data_t *data = (pop3_data_t*)priv_data;
		STAILQ_INIT(&data->response_list);
	}
	| response_list response
	{
		pop3_data_t *data = (pop3_data_t*)priv_data;
		STAILQ_INSERT_TAIL(&data->response_list, $2, next);
		
		/*DO state check*/
		if(!pop3_do_state_check(data))
			pop3_abnormal(debug_pop3_server, "Abnormal: pop3 state check failed\n");

		/*DO state transfer for positive response*/
		if($2->res_code == POP3_RESPONSE_OK)
			pop3_do_state_transfer(data);

		/*DO make pop3 command pair*/
		if(pop3_session_add_command(data))
		{
			pop3_debug(debug_pop3_server, "add pop3 command failed\n");
			YYABORT;
		}
	}
	;

response: positive_response
	{
		$$ = $1;

		/*for PASS command, do weak password check*/
		pop3_data_t *data = (pop3_data_t*)priv_data;
		if(pop3_do_weak_password_check(data))
			pop3_abnormal(debug_pop3_server, "Abnormal: detect weak password\n");
	}
	| negative_response
	{
		$$ = $1;

		pop3_data_t *data = (pop3_data_t*)priv_data;
		if(data->state == POP3_STATE_AUTHORIZATION && pop3_do_brute_force_check(data))
			pop3_attack(debug_pop3_server, "Attack: detect weak password\n");
	}
	;

positive_response: TOKEN_SERVER_OK positive_response_line positive_response_message
	{
		pop3_response_t *res = pop3_alloc_response(POP3_RESPONSE_OK, $2.str, $2.str_len, &$3);
		if(!res)
		{
			pop3_debug(debug_pop3_server, "failed to alloc positive response\n");
			YYABORT;
		}
		$$ = res;
	}
	;

positive_response_line: TOKEN_SERVER_STRING
	{
		char *data = NULL;
		int data_len = yylval.string.str_len;

		if(data_len)
		{
			data = (char*)pop3_malloc(data_len + 1);
			if(!data)
			{
				pop3_debug(debug_pop3_server, "failed to alloc positive response line data\n");
				YYABORT;
			}
			memcpy(data, yylval.string.str, data_len);
			data[data_len] = '\0';
		}
		$$.str = data;
		$$.str_len = data_len;
	}
	;

positive_response_message: 
	{
		STAILQ_INIT(&$$);
	}
	| TOKEN_SERVER_DOT
	{
		STAILQ_INIT(&$$);
	}
	| positive_response_lines TOKEN_SERVER_DOT
	{
		STAILQ_CONCAT(&$$, &$1);
	}
	;

positive_response_lines: TOKEN_SERVER_STRING
	{
		char *data = NULL;
		int data_len = yylval.string.str_len;

		if(data_len)
		{
			data = (char*)pop3_malloc(data_len + 1);
			if(!data)
			{
				pop3_debug(debug_pop3_server, "failed to alloc positive response line data\n");
				YYABORT;
			}
			memcpy(data, yylval.string.str, data_len);
			data[data_len] = '\0';
		}
		
		pop3_line_t *line = pop3_alloc_response_line(data, data_len);
		if(!line)
		{
			pop3_debug(debug_pop3_server, "failed to alloc positive response line\n");
			pop3_free(data);
			YYABORT;
		}
		STAILQ_INSERT_TAIL(&$$, line, next);
	}
	| positive_response_lines TOKEN_SERVER_STRING
	{
		char *data = NULL;
		int data_len = yylval.string.str_len;

		if(data_len)
		{
			data = (char*)pop3_malloc(data_len + 1);
			if(!data)
			{
				pop3_debug(debug_pop3_server, "failed to alloc positive response line data\n");
				YYABORT;
			}
			memcpy(data, yylval.string.str, data_len);
			data[data_len] = '\0';
		}
		
		pop3_line_t *line = pop3_alloc_response_line(data, data_len);
		if(!line)
		{
			pop3_debug(debug_pop3_server, "failed to alloc positive response line\n");
			pop3_free(data);
			YYABORT;
		}
		STAILQ_INSERT_TAIL(&$1, line, next);
		STAILQ_CONCAT(&$$, &$1);
	}
	;

negative_response: TOKEN_SERVER_ERROR negative_response_line negative_response_message
	{
		pop3_response_t *res = pop3_alloc_response(POP3_RESPONSE_ERROR, $2.str, $2.str_len, NULL);
		if(!res)
		{
			pop3_debug(debug_pop3_server, "failed to alloc negative response\n");
			YYABORT;
		}
		$$ = res;
	}
	;

negative_response_line: TOKEN_SERVER_STRING
	{
		char *data = NULL;
		int data_len = yylval.string.str_len;

		if(data_len)
		{
			data = (char*)pop3_malloc(data_len + 1);
			if(!data)
			{
				pop3_debug(debug_pop3_server, "failed to alloc negative response line data\n");
				YYABORT;
			}
			memcpy(data, yylval.string.str, data_len);
			data[data_len] = '\0';
		}
		$$.str = data;
		$$.str_len = data_len;
	}
	;

negative_response_message:
	| negative_response_lines
	;

negative_response_lines: TOKEN_SERVER_STRING
	| negative_response_lines TOKEN_SERVER_STRING
	;
%%
static int pop3_session_add_command(pop3_data_t *priv_data)
{
	pop3_response_t *res = STAILQ_FIRST(&priv_data->response_list);
	pop3_request_t *req = STAILQ_FIRST(&priv_data->request_list);
	assert(res != NULL);

	pop3_cmd_t *cmd = pop3_alloc_cmd(req, res);
	if(!cmd)
	{
		pop3_debug(debug_pop3_server, "failed to alloc command\n");
		return 1;
	}

	if(res)
		STAILQ_REMOVE_HEAD(&priv_data->response_list, next);
	if(req)
		STAILQ_REMOVE_HEAD(&priv_data->request_list, next);
	STAILQ_INSERT_TAIL(&priv_data->cmd_list, cmd, next);
	return 0;
}

static int pop3_do_state_check(pop3_data_t *priv_data)
{
	static int check_table[POP3_STATE_MAX][POP3_COMMAND_MAX] = 
	{
				/*USER, PASS, APOP, LIST, RETR, DELE, UIDL, TOP, STAT, QUIT, NOOP, RSET, UNKNOWN*/
	/*INIT*/	{  0,    0,    0,    0,    0,    0,    0,    0,   0,    0,    0,    0,     1},
	/*AUTH*/	{  1,    1,    1,    0,    0,    0,    0,    0,   0,    1,    0,    0,     1},
	/*TRAN*/	{  0,    0,    0,    1,    1,    1,    1,    1,   1,    1,    1,    1,     1},
	/*UPDT*/	{  0,    0,    0,    0,    0,    0,    0,    0,   0,    0,    0,    0,     1}
	};

	assert((unsigned int)priv_data->state < (unsigned int)POP3_STATE_MAX);

	pop3_request_t *req = STAILQ_FIRST(&priv_data->request_list);
	if(req)
	{
		assert((unsigned int)req->req_code < (unsigned int)POP3_COMMAND_MAX);
		return check_table[priv_data->state][req->req_code];
	}
	else if(priv_data->state == POP3_STATE_INIT)
	{
		return 1;
	}
	return 1;
}

static void pop3_do_state_transfer(pop3_data_t *priv_data)
{
	static int transfer_table[POP3_COMMAND_MAX][POP3_STATE_MAX] = 
	{
				/*POP3_STATE_INIT		POP3_STATE_AUTHORIZATION		POP3_STATE_TRANSACTION		POP3_STATE_UPDATE*/
	/*USER*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*PASS*/	{POP3_STATE_INIT,		POP3_STATE_TRANSACTION,			POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*APOP*/	{POP3_STATE_INIT,		POP3_STATE_TRANSACTION,			POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*LIST*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*RETR*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*DELE*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},	
	/*UIDL*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},	
	/*TOP*/		{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},	
	/*STAT*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*QUIT*/	{POP3_STATE_INIT,		POP3_STATE_INIT,				POP3_STATE_UPDATE,			POP3_STATE_UPDATE},
	/*NOOP*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*RSET*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE},
	/*UNKW*/	{POP3_STATE_INIT,		POP3_STATE_AUTHORIZATION,		POP3_STATE_TRANSACTION,		POP3_STATE_UPDATE}
	};
	pop3_response_t *res = STAILQ_FIRST(&priv_data->response_list);
	assert((unsigned int)priv_data->state < (unsigned int)POP3_STATE_MAX);
	assert(res != NULL && res->res_code==POP3_RESPONSE_OK);
	
	pop3_request_t *req = STAILQ_FIRST(&priv_data->request_list);
	if(req)
	{
		assert((unsigned int)req->req_code < (unsigned int)POP3_COMMAND_MAX);
		int new_state = transfer_table[req->req_code][priv_data->state];
		pop3_debug(debug_pop3_server, "transfer state from %d to %d\n", priv_data->state, new_state);
		priv_data->state = new_state;
	}
	else if(priv_data->state == POP3_STATE_INIT)
	{
		pop3_debug(debug_pop3_server, "transfer state from %d to %d\n", POP3_STATE_INIT, POP3_STATE_AUTHORIZATION);
		priv_data->state = POP3_STATE_AUTHORIZATION;
	}
}

int parse_pop3_server_stream(pop3_data_t *priv, const char *buf, size_t buf_len, int last_frag)
{
	pop3_server_pstate *parser = (pop3_server_pstate*)priv->response_parser.parser;
	yyscan_t lexier = (yyscan_t)priv->response_parser.lexier;
	YY_BUFFER_STATE input = NULL;
	int token = 0, parser_ret = 0;
	POP3_SERVER_STYPE value;

	yydebug = debug_pop3_server;
	priv->response_parser.last_frag = last_frag;
	input = pop3_server_scan_stream(buf, buf_len, priv);
	if(!input)
	{
		pop3_debug(debug_pop3_server, "create pop3 server stream buffer failed\n");
		return 1;
	}

	while(1)
	{
		token = pop3_server_lex(&value, lexier);
		if(token == TOKEN_SERVER_CONTINUE)
			break;
		parser_ret = pop3_server_push_parse(parser, token, &value, (void*)priv);
		if(parser_ret != YYPUSH_MORE)
			break;
	}
	pop3_server__delete_buffer(input, lexier);

	if(parser_ret != YYPUSH_MORE && parser_ret != 0)
	{
		pop3_debug(debug_pop3_server, "find error while parsing pop3 server stream\n");
		return 2;
	}
	return 0;
}
