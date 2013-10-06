#ifndef EY_ENGINE_H
#define EY_ENGINE_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <dlfcn.h>

#include "ey_memory.h"
#include "ey_event.h"
#include "ey_parser.h"
#include "ey_info.h"
#include "engine_mem.h"
#include "ey_signature.h"
#include "libutil.h"
#include "libjit.h"
#include "ey_compiler.h"
#include "ey_import.h"
#include "ey_elf.h"
#include "libmatch.h"
#include "ey_work.h"

typedef struct ey_engine
{
	char name[64];

	ey_fslab_t parser_fslab;

	ey_hash_t filename_hash;
	ey_hash_t signature_hash;
	ey_hash_t library_hash;
	ey_hash_t rhs_item_hash;
	ey_hash_t engine_work_hash;

	ey_signature_list_t signature_list;
	ey_code_list_t file_init_list;
	ey_code_list_t file_finit_list;

	ey_code_t *work_init_predefined;
	ey_code_t *work_finit_predefined;
	ey_code_t *work_init_userdefined;
	ey_code_t *work_finit_userdefined;

	ey_slab_t private_work_slab;
	ey_slab_t engine_work_slab;
	ey_slab_t engine_work_event_slab;

	#define EVENT_ARRAY_STEP	32
	ey_event_t *event_array;
	int event_size;
	int event_count;

	ey_parser_t *parser;
	ey_jit_t jit;

	unsigned long rhs_id;
	unsigned long *prefix_array;
	unsigned long *postfix_array;
}ey_engine_t;

#define ey_parser_fslab(eng) (((ey_engine_t*)(eng))->parser_fslab)
#define ey_filename_hash(eng) (((ey_engine_t*)(eng))->filename_hash)
#define ey_event_array(eng) (((ey_engine_t*)(eng))->event_array)
#define ey_event_size(eng) (((ey_engine_t*)(eng))->event_size)
#define ey_event_count(eng) (((ey_engine_t*)(eng))->event_count)
#define ey_signature_hash(eng) (((ey_engine_t*)(eng))->signature_hash)
#define ey_jit(eng) (((ey_engine_t*)(eng))->jit)
#define ey_library_hash(eng) (((ey_engine_t*)(eng))->library_hash)
#define ey_rhs_id(eng) (((ey_engine_t*)(eng))->rhs_id)
#define ey_rhs_item_hash(eng) (((ey_engine_t*)(eng))->rhs_item_hash)
#define ey_prefix_array(eng) (((ey_engine_t*)(eng))->prefix_array)
#define ey_postfix_array(eng) (((ey_engine_t*)(eng))->postfix_array)
#define ey_signature_list(eng) (((ey_engine_t*)(eng))->signature_list)
#define ey_file_init_list(eng) (((ey_engine_t*)(eng))->file_init_list)
#define ey_file_finit_list(eng) (((ey_engine_t*)(eng))->file_finit_list)
#define ey_work_init_predefined(eng) (((ey_engine_t*)(eng))->work_init_predefined)
#define ey_work_finit_predefined(eng) (((ey_engine_t*)(eng))->work_finit_predefined)
#define ey_work_init_userdefined(eng) (((ey_engine_t*)(eng))->work_init_userdefined)
#define ey_work_finit_userdefined(eng) (((ey_engine_t*)(eng))->work_finit_userdefined)
#define ey_engine_work_hash(eng) (((ey_engine_t*)(eng))->engine_work_hash)
#define ey_work_slab(eng) (((ey_engine_t*)(eng))->private_work_slab)
#define ey_engine_work_slab(eng) (((ey_engine_t*)(eng))->engine_work_slab)
#define ey_engine_work_event_slab(eng) (((ey_engine_t*)(eng))->engine_work_event_slab)

extern int ey_load_post_action(ey_engine_t *eng);
#endif
