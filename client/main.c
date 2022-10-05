/*
 * File            : main.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-04 00:15:05
 * LastModified    : 2022-10-04 01:40:53
 * Vim             : ts=4, sw=4
 */

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

static struct options {
    const char *filename;
    const char *contents;
    int show_help;
} options;

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
    OPTION("--name=%s", filename),
    OPTION("--contents=%s", contents),
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    FUSE_OPT_END
};

static void *kirin_init(struct fuse_conn_info* conn,
        struct fuse_config* cfg){
    (void) conn;
    cfg->kernel_cache = 1;
    return NULL;
}

static int kirin_open(const char* path, struct fuse_file_info *fi){
    if(strcmp(path + 1, options.filename) != 0)
        return -ENOENT;

    if((fi->flags & O_ACCMODE) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int kirin_read(const char* path, char* buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path + 1, options.filename) != 0)
        return -ENOENT;

    len = strlen(options.contents);
    if(offset < len) {
        if(offset + size < len)
            size = len -offset;
        memcpy(buf, options.contents + offset, size);
    }else
        size = 0;
    return size;
}

static int kirin_getattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi){
    (void) fi;
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path, "/") == 0){
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if(strcmp(path + 1, options.filename) == 0){
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(options.contents);
    } else
        res = -ENOENT;

    return res;
}

static int kirin_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf, options.filename, NULL, 0, 0);

	return 0;
}

static struct fuse_operations kirin_oper = {
    .init = kirin_init,
    .getattr = kirin_getattr,
    .readdir = kirin_readdir,
    .open = kirin_open,
    .read = kirin_read,
};

int main(int argc, char *argv[]){
    printf("hello kirinfs\n");
    int ret;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    options.filename = strdup("hello");
    options.contents = strdup("hello kirinfs");

    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return -1;

    ret = fuse_main(args.argc, args.argv, &kirin_oper, NULL);
    return ret;
}
