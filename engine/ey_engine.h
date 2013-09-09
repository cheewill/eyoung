#ifndef EY_ENGINE_H
#define EY_ENGINE_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "ey_memory.h"
#include "ey_event.h"
#include "ey_parser.h"
#include "ey_info.h"
#include "engine_mem.h"

typedef struct ey_engine
{
	char name[64];

	ey_fslab_t filename_fslab;
	ey_hash_t filename_hash;
	ey_parser_t *parser;

	ey_hash_t event_hash;
}ey_engine_t;

#define ey_filename_fslab(eng) (((ey_engine_t*)(eng))->filename_fslab)
#define ey_filename_hash(eng) (((ey_engine_t*)(eng))->filename_hash)
#define ey_event_hash(eng) (((ey_engine_t*)(eng))->event_hash)

#endif
