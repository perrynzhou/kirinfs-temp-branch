#include "stl_mutex.h"
#include <assert.h>
int main()
{
	stl_mutex mutex;
	assert(stl_mutex_init(&mutex) == 0);
	stl_mutex_lock(&mutex);
	stl_mutex_unlock(&mutex);
	assert(stl_mutex_term(&mutex) == 0);

	return 0;
}
