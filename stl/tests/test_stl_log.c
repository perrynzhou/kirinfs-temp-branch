
#include "stl_log.h"
int main(void) {

	for(size_t i=0;i<10;i++)
	{
		stl_log_trace("output value=%d",i);
	}
	return 0;
}