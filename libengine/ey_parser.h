#ifndef EY_PARSER_H
#define EY_PARSER_H 1

struct ey_signature_file;
typedef struct ey_parser
{
	void *lexer;
	void *parser;
	char *filename;

	/*read buffer for lexer*/
	#define DEFAULT_BUFFER_SIZE 4096
	char *buffer;
	int buffer_size;
	int buffer_len;

	/*parser result*/
	struct ey_signature_file *signature_file;
}ey_parser_t;

struct ey_engine;
extern int ey_parse_file(struct ey_engine *eng, const char *filename);
extern int ey_parser_init(struct ey_engine *eng);
extern void ey_parser_finit(struct ey_engine *eng);
#endif
