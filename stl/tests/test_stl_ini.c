#include "../src/stl_ini.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

char *example_ini = "# My configuration\n"
			  "[Network] \n"
			  "hostname = github.com \n"
			  "port = 443 \n"
			  "protocol = https \n"
			  "repo = any";


void file_example(void)
{
	int rc;

	printf(" \n Parse file \n");

	stl_ini ini;
	rc=stl_ini_init(&ini,"my_config.ini",STL_INI_FILE);
	stl_ini_dump(&ini,"./dump.ini");
	assert(rc == 0);
}

void string_example(void)
{
	int rc;

	printf(" \n Parse string \n");

		stl_ini ini;
	rc=stl_ini_init(&ini,example_ini,STL_INI_STRING);
	stl_ini_dump(&ini,"./dump.ini");
}

int main()
{
	string_example();
	//file_example();
}
