#include "stl_ini.h"

#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define STL_INI_MAX_LINE_LEN 4096
#define STL_INI_SECTION_MAX_ITEM_SIZE 4096
#define STL_INI_SECTION_MAX_SIZE 4096
typedef struct
{
	char *key;
	char *val;
} stl_ini_item;
typedef struct
{
	char *name;
	size_t cnt;
	stl_ini_item **items;
} stl_ini_section;

static int stl_ini_section_init(stl_ini_section *sis, char *name)
{
	if (sis && name)
	{
		sis->name = strdup(name);
		sis->cnt = 0;
		sis->items = calloc(STL_INI_SECTION_MAX_ITEM_SIZE, sizeof(stl_ini_item *));
		assert(sis->items);
		return 0;
	}
	return -1;
}
static int stl_ini_section_put_conf(stl_ini_section *sis, char *key, char *value)
{
	int ret = -1;
	if (sis && key && value)
	{
		ret = 0;
		sis->items[sis->cnt] = (stl_ini_item *)calloc(1, sizeof(stl_ini_item));
		sis->items[sis->cnt]->key = strdup(key);
		if (sis->items[sis->cnt]->val != NULL)
		{
			free(sis->items[sis->cnt]->val);
		}
		sis->items[sis->cnt++]->val = strdup(value);
	}
	return ret;
}
static int stl_ini_section_del_conf(stl_ini_section *sis, char *key)
{
	int ret = -1;
	if (sis && key)
	{
		size_t key_len = strlen(key);
		size_t i = 0;
		for (; i < sis->cnt; i++)
		{
			if (strncmp(sis->items[i]->key, key, key_len) == 0)
			{
				free(sis->items[i]->key);
				free(sis->items[i]->val);
				free(sis->items[i]);
				break;
			}
		}
		for (; i < sis->cnt; i++)
		{
			sis->items[i] = sis->items[i + 1];
		}
		sis->cnt--;
	}
	return ret;
}
static int stl_ini_section_deinit(stl_ini_section *sis)
{
	if (sis)
	{
		for (size_t i = 0; i < sis->cnt; i++)
		{
			free(sis->items[i]->key);
			free(sis->items[i]->val);
		}
		free(sis->items);
		free(sis->name);
		sis->cnt = 0;
		return 0;
	}
	return -1;
}

int stl_ini_on_item_func(void *arg, int line, char *section, char *key,
						 char *value)
{

	stl_ini *ini = (stl_ini *)arg;
	stl_ini_section *sections = (stl_ini_section *)ini->section;
	size_t section_len = strlen(section);
	stl_ini_section *cur_section = NULL;

	bool found = false;
	for (size_t i = 0; i < ini->cnt; i++)
	{
		if (strncmp(sections[i].name, section, section_len) == 0)
		{
			found = true;
			cur_section = &sections[i];
		}
	}
	if (!found)
	{
		stl_ini_section_init(&sections[ini->cnt], section);
		cur_section = &sections[ini->cnt];
		ini->cnt++;
	}

	stl_ini_section_put_conf(cur_section, key, value);
	printf("Line : %d, Section : %s, Key : %s, Value : %s \n", line,
		   section, key, value);

	return 0;
}

static char *trim_space(char *str)
{
	char *end;

	while (isspace((unsigned char)*str))
	{
		str++;
	}

	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
	{
		end--;
	}

	end[1] = '\0';

	return str;
}

static char *trim_comment(char *str)
{
	char *s = str;

	if (*s == '\0' || *s == ';' || *s == '#')
	{
		*s = '\0';
		return str;
	}

	while (*s && (s = strchr(s, ' ')) != NULL)
	{
		s++;
		if (*s == ';' || *s == '#')
		{
			*s = '\0';
			break;
		}
	}

	return str;
}

static char *trim_bom(char *str)
{
	unsigned char *p = (unsigned char *)str;

	if (strlen(str) >= 3)
	{
		if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
		{
			str += 3;
		}
	}

	return str;
}

int stl_ini_parse(void *arg, void *arg1, char *(*next_line)(void *, char *, size_t))
{
	int rc = 0, line = 0;
	char buf[STL_INI_MAX_LINE_LEN];
	char section[256] = {0}, key[256] = {0};
	char *head, *end;

	while ((head = next_line(arg1, buf, sizeof(buf) - 1)) != NULL)
	{
		if (++line == 1)
		{
			head = trim_bom(head);
		}

		head = trim_space(trim_comment(head));
		if (*head == '\0')
		{
			continue;
		}

		if (head > buf && *key)
		{
			rc = stl_ini_on_item_func(arg, line, section, key, head);
		}
		else if (*head == '[')
		{
			if ((end = strchr(head, ']')) == NULL)
			{
				return line;
			}

			*key = '\0';
			*end = '\0';
			strncpy(section, head + 1, sizeof(section) - 1);
		}
		else
		{
			if ((end = strpbrk(head, "=:")) == NULL)
			{
				return line;
			}

			*end = '\0';
			trim_space(head);
			strncpy(key, head, sizeof(key) - 1);
			rc = stl_ini_on_item_func(arg, line, section, head, trim_space(end + 1));
		}

		if (rc != 0)
		{
			return line;
		}
	}

	return 0;
}

static inline char *stl_ini_file_line(void *p, char *buf, size_t size)
{
	return fgets(buf, (int)size, (FILE *)p);
}

static inline char *stl_ini_string_line(void *p, char *buf, size_t size)
{
	size_t len, diff;
	char *t, *str = (*(char **)p);

	if (str == NULL || *str == '\0')
	{
		return NULL;
	}

	t = strchr(str, '\n');
	if (t == NULL)
	{
		t = str + strlen(str);
	}

	diff = (size_t)(t - str);
	len = diff < size ? diff : size;
	memcpy(buf, str, len);
	buf[len] = '\0';

	*(char **)p = (*t == '\0') ? '\0' : t + 1;

	return buf;
}

int stl_ini_parse_file(void *arg, char *filename)
{
	int rc;
	FILE *file;

	file = fopen(filename, "rb");
	if (!file)
	{
		return -1;
	}

	rc = stl_ini_parse(arg, file, stl_ini_file_line);
	if (rc == 0)
	{
		rc = ferror(file) != 0 ? -1 : 0;
	}

	fclose(file);

	return rc;
}

int stl_ini_parse_string(void *arg, char *str)
{
	char *ptr = (char *)str;
	return stl_ini_parse(arg, &ptr, stl_ini_string_line);
}

int stl_ini_init(stl_ini *si, char *file_or_string, stl_ini_type ini_type)
{
	int ret = -1;
	if (si && file_or_string && (ini_type >= STL_INI_FILE && ini_type <= STL_INI_STRING))
	{
		stl_ini_section *section = (stl_ini_section *)calloc(STL_INI_SECTION_MAX_SIZE, sizeof(stl_ini_section));
		assert(section != NULL);
		si->cnt = 0;
		si->section = section;
		switch (ini_type)
		{
		case STL_INI_FILE:
			stl_ini_parse_file(si, file_or_string);
			break;
		case STL_INI_STRING:
			stl_ini_parse_string(si, file_or_string);
			break;
		default:
			break;
		}
	}
	return ret;
}
stl_ini *stl_ini_alloc(char *file_or_string, stl_ini_type ini_type)
{
	stl_ini *ini = NULL;
	if (file_or_string && (ini_type >= STL_INI_FILE && ini_type <= STL_INI_STRING))
	{
		ini = (stl_ini *)calloc(1, sizeof(stl_ini));
		assert(ini != NULL);
		if (stl_ini_init(ini, file_or_string, ini_type) != 0)
		{
			free(ini);
			ini = NULL;
		}
	}
	return ini;
}

char *stl_ini_search(stl_ini *si, char *section, char *key)
{
	char *value = NULL;
	if (si && section && key)
	{
		size_t section_len = strlen(section);
		stl_ini_section *sections = (stl_ini_section *)si->section;
		for (size_t i = 0; i < si->cnt; i++)
		{
			if (strncmp(sections[i].name, section, section_len) == 0)
			{
				stl_ini_item **items = sections[i].items;
				size_t key_len = strlen(key);
				for (size_t j = 0; j < sections[i].cnt; j++)
				{
					if (strncmp(items[j]->key, key, key_len) == 0)
					{
						value = items[j]->val;
						break;
					}
				}
			}
		}
	}
	return value;
}
int stl_ini_dump(stl_ini *si, char *filename)
{
	if (si && filename)
	{
		stl_ini_section *sections = (stl_ini_section *)si->section;
		int fd = open(filename, O_RDWR | O_TRUNC | O_CREAT);
		for (size_t i = 0; i < si->cnt; i++)
		{
			char buf[4096] = {'\0'};
			snprintf((char *)&buf, 4096, "[%s]\n", sections[i].name);
			write(fd, (char *)&buf, strlen((char *)&buf));
			stl_ini_item **items = sections[i].items;
			for (size_t j = 0; j < sections[i].cnt; j++)
			{
				snprintf((char *)&buf, 4096, "%s=%s\n", items[j]->key, items[j]->val);
				write(fd, (char *)&buf, strlen((char *)&buf));
			}
		}
		return 0;
	}
	return -1;
}
void stl_ini_deinit(stl_ini *si)
{
	if (si)
	{
		stl_ini_section *sections = (stl_ini_section *)si->section;
		for (size_t i = 0; i < si->cnt; i++)
		{

			stl_ini_item **items = sections[i].items;
			for (size_t j = 0; j < sections[i].cnt; j++)
			{
				free(items[i]->key);
				free(items[i]->val);
			}
		}
		free(si->section);
		si->cnt = 0;
	}
}
void stl_ini_destroy(stl_ini *si)
{
	if (si)
	{
		stl_ini_deinit(si);
		free(si);
		si = NULL;
	}
}

int stl_ini_put(stl_ini *si, char *section, char *key, char *val)
{
	int ret = -1;
	if (si && section && key && val)
	{
		size_t section_len = strlen(section);
		stl_ini_section *sections = (stl_ini_section *)si->section;
		stl_ini_section *cur_section = NULL;
		for (size_t i = 0; i < si->cnt; i++)
		{

			if (strncmp(sections[i].name, section, section_len) == 0)
			{
				cur_section = &sections[i];
				break;
			}
		}
		if (cur_section != NULL)
		{
			stl_ini_item **items = cur_section->items;
			size_t key_len = strlen(key);
			for (size_t j = 0; j < cur_section->cnt; j++)
			{
				if (strncmp(items[j]->key, key, key_len) == 0)
				{
					ret = stl_ini_section_put_conf(cur_section, key, val);
					break;
				}
			}
		}
	}
	return ret;
}
int stl_ini_del(stl_ini *si, char *section, char *key)
{
	int ret = -1;
	if (si && section && key)
	{
		size_t section_len = strlen(section);
		stl_ini_section *sections = (stl_ini_section *)si->section;
		stl_ini_section *cur_section = NULL;
		for (size_t i = 0; i < si->cnt; i++)
		{

			if (strncmp(sections[i].name, section, section_len) == 0)
			{
				cur_section = &sections[i];
				break;
			}
		}
		if (cur_section != NULL)
		{
			ret = stl_ini_section_del_conf(cur_section, key);
		}
		return ret;
	}
}