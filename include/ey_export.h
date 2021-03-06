#ifndef EY_EXPORT_H
#define EY_EXPORT_H 1

#define EY_EXPORT_TYPE_IDENT			1
#define EY_EXPORT_TYPE_TYPE				2
#define EY_EXPORT_TYPE_INIT				3
#define EY_EXPORT_TYPE_FINIT			4

#define EY_IDENT_SECTION				".eyoung_ident"
#define EY_TYPE_SECTION					".eyoung_type"
#define EY_INIT_SECTION					".eyoung_init"
#define EY_FINIT_SECTION				".eyoung_finit"

typedef struct ey_extern_symbol
{
	char *name;
	void *value;
	char *decl;
	char *file;
	int line;
	int type;
}ey_extern_symbol_t;

#define EY_EXPORT_IDENT(name, decl)								\
	static const ey_extern_symbol_t __eyoung_ident_##name		\
	__attribute__((section(EY_IDENT_SECTION), unused)) =		\
	{ 															\
		#name,													\
		&name, 													\
		decl,													\
		__FILE__, 												\
		__LINE__, 												\
		EY_EXPORT_TYPE_IDENT									\
	};

#define EY_EXPORT_TYPE(name,decl)								\
	static const ey_extern_symbol_t __eyoung_type_##name		\
	__attribute__((section(EY_TYPE_SECTION), unused)) = 		\
	{															\
		#name, 													\
		(void*)0, 												\
		decl, 													\
		__FILE__, 												\
		__LINE__,												\
		EY_EXPORT_TYPE_IDENT									\
	};

#define EY_EXPORT_INIT(name)									\
	static const ey_extern_symbol_t __eyoung_type_##name		\
	__attribute__((section(EY_INIT_SECTION), unused)) = 		\
	{															\
		#name, 													\
		name, 													\
		(void*)0, 												\
		__FILE__, 												\
		__LINE__,												\
		EY_EXPORT_TYPE_INIT										\
	};

#define EY_EXPORT_FINIT(name)									\
	static const ey_extern_symbol_t __eyoung_type_##name		\
	__attribute__((section(EY_FINIT_SECTION), unused)) = 		\
	{															\
		#name, 													\
		name, 													\
		(void*)0, 												\
		__FILE__, 												\
		__LINE__,												\
		EY_EXPORT_TYPE_FINIT									\
	};
#endif 
