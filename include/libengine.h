#ifndef ENGINE_H
#define ENGINE_H 1

typedef void* engine_t;
typedef void *engine_work_t;

typedef struct engine_work_event
{
	unsigned int id;
	const char *raw_data;
	size_t raw_data_len;
	void *value;
}engine_work_event_t;

typedef struct engine_action
{
	unsigned int action;
}engine_action_t;

extern engine_t ey_engine_create(const char *name);
extern void ey_engine_destroy(engine_t engine);
extern int ey_engine_load(engine_t engine, char *files[], int files_num);

extern engine_work_t ey_engine_work_create(engine_t engine);
extern void ey_engine_work_destroy(engine_t engine, engine_work_t work);
extern int ey_engine_work_detect(engine_t eng, engine_work_t work, engine_work_event_t *event, engine_action_t *action);

extern int debug_engine_parser;
extern int debug_engine_lexier;
extern int debug_engine_init;

#endif