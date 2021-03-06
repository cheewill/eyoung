#ifndef LIBENGINE_FUNCTION_H
#define LIBENGINE_FUNCTION_H 1

#include "libengine_type.h"
#include "libengine_export.h"
extern engine_t ey_engine_create(const char *name);
extern void ey_engine_destroy(engine_t engine);
extern int ey_engine_load(engine_t engine, char *files[], int files_num);
extern int ey_engine_find_event(engine_t engine, const char *event_name);

extern engine_work_t *ey_engine_work_create(engine_t engine);
extern void ey_engine_work_destroy(engine_work_t *work);
extern engine_work_event_t *ey_engine_work_create_event(engine_work_t *work, unsigned long event_id, engine_action_t *action);
extern int ey_engine_work_detect_data(engine_work_t *work, const char *data, size_t data_len, int from_client);
extern int ey_engine_work_detect_event(engine_work_event_t *event, void *predefined);
extern void ey_engine_work_destroy_event(engine_work_event_t *event);

extern int debug_engine_parser;
extern int debug_engine_lexier;
extern int debug_engine_init;
extern int debug_engine_compiler;
extern int debug_engine_runtime;

#endif
