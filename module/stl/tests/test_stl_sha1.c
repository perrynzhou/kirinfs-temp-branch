/*
 * File            : test_stl_sha1.c
 * Author          : ZhangLe
 * CreateTime      : 2022-09-24 20:31:26
 * LastModified    : 2022-09-24 22:34:38
 * Vim             : ts=4, sw=4
 */

#include "../src/stl_sha1.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int check_ctx(stl_sha1 *ctx){
    if(ctx){
        printf("bytes = %ld\n", ctx->bytes);
        printf("a = %ld\n", ctx->a);
        printf("b = %ld\n", ctx->b);
        printf("c = %ld\n", ctx->c);
        printf("d = %ld\n", ctx->d);
        printf("e = %ld\n", ctx->e);
        printf("f = %ld\n", ctx->f);
    }else{
        fprintf(stderr, "ctx is null");
        return -1;
    }
    return 0;
}

void test_demo() {
    int res;
    char sha1_value[20] = {'\0'};
    char *str = "hello world";

    stl_sha1* ctx = stl_sha1_alloc();

    res = stl_sha1_init(ctx);
    if (res != 0){
        fprintf(stderr, "init stl sha1 has been failed, err code: %d", res);
    }
    check_ctx(ctx);

    stl_sha1_update(ctx, str, strlen(str));

    stl_sha1_final(sha1_value, ctx);
    fprintf(stdout, "sha value: %s\n", (char *)&sha1_value);

    res = stl_sha1_deinit(ctx);
    if (res != 0){
        fprintf(stderr, "deinit stl sha1 has been failed, err code: %d", res);
    }
    check_ctx(ctx);

    stl_sha1_destroyt(ctx);
}

void test_file(){
    int i, n, fd, len;
    int res;
    char buffer[2048];
    char sha1_value[20] = {'\0'};

    stl_sha1* ctx = stl_sha1_alloc();
    res = stl_sha1_init(ctx);
    if(res != 0){
        fprintf(stderr, "init stl sha1 has been failed, err code: %d", res);
    }

    fd = open("file1", O_RDONLY, S_IRUSR);
    if (-1 == fd){
        fprintf(stderr, "open file has been failed");
    }

    while(0 < (len = read(fd, buffer, 2048))){
        stl_sha1_update(ctx, buffer, len);
        memset(buffer, 0, sizeof(buffer));
    }
    stl_sha1_final(sha1_value, ctx);
    fprintf(stdout, "sha value: %s\n", (char *)&sha1_value);

    close(fd);
    stl_sha1_destroyt(ctx);
}

int main(int argc, char* argv[]){
    test_demo();
    test_file();
    return 0;
}
