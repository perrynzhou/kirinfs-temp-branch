# 1. 编译步骤
gcc -Wall kfuse.c MonMap.h MonMap.c -D_FILE_OFFSET_BITS=64 -o kfuse
