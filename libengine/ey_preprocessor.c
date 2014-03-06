#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ey_engine.h"

static ey_preprocessor_t *ey_preprocessor_find(ey_engine_t *engine, const char *name)
{
	assert(engine != NULL && name != NULL);
	
	ey_preprocessor_t *pp = NULL;
	TAILQ_FOREACH(pp, &ey_preprocessor_list(engine), link)
	{
		if(!strcmp(pp->name, name))
			break;
	}
	return pp;
}

int ey_preprocessor_register(ey_engine_t *engine, ey_preprocessor_t *preprocessor)
{
	ey_preprocessor_t *ret = NULL;
	assert(engine != NULL && preprocessor != NULL);
	
	if(ey_preprocessor_find(engine, preprocessor->name))
	{
		engine_init_error("preprocessor %s is already registered\n");
		return -1;
	}

	ret = (ey_preprocessor_t*)engine_malloc(sizeof(ey_preprocessor_t));
	if(!ret)
	{
		engine_init_error("malloc preprocessor failed\n");
		return -1;
	}

	memset(ret, 0, sizeof(*ret));
	memcpy(ret, preprocessor, sizeof(ey_preprocessor_t));

	if(preprocessor->preprocessor_init && preprocessor->preprocessor_init(engine, ret))
	{
		engine_init_error("preprocessor init failed\n");
		engine_free(ret);
		return -1;
	}

	TAILQ_INSERT_TAIL(&ey_preprocessor_list(engine), ret, link);
	return 0;
}

static void ey_preprocessor_unregister(ey_engine_t *engine, ey_preprocessor_t *preprocessor)
{
	assert(engine != NULL && preprocessor != NULL);

	if(preprocessor->preprocessor_finit)
		preprocessor->preprocessor_finit(engine, preprocessor);
	
	engine_free(preprocessor);
}

void ey_preprocessor_finit(ey_engine_t *engine)
{
	assert(engine != NULL);

	ey_preprocessor_t *pp = NULL, *tmp = NULL;
	TAILQ_FOREACH_SAFE(pp, &ey_preprocessor_list(engine), link, tmp)
		ey_preprocessor_unregister(engine, pp);
	
	TAILQ_INIT(&ey_preprocessor_list(engine));
}

int ey_preprocessor_init(ey_engine_t *engine)
{
	assert(engine != NULL);

	TAILQ_INIT(&ey_preprocessor_list(engine));
	return 0;
}