#ifndef STL_INI_H
#define STL_INI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
typedef struct
{
	void *section;
	size_t cnt;
} stl_ini;
typedef enum
{
	STL_INI_FILE = 0,
	STL_INI_STRING
} stl_ini_type;
int stl_ini_init(stl_ini *si, char *file_or_string, stl_ini_type ini_type);
stl_ini *stl_ini_alloc(char *file_or_string, stl_ini_type ini_type);
char *stl_ini_search(stl_ini *si, char *section, char *key);
int stl_ini_put(stl_ini *si, char *section, char *key, char *val);
int stl_ini_del(stl_ini *si, char *section, char *key);
int stl_ini_dump(stl_ini *si, char *filename);
void stl_ini_deinit(stl_ini *si);
void stl_ini_destroy(stl_ini *si);

#endif
