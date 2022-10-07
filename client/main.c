/*
 * File            : main.c
 * Author          : ZhangLe
 * CreateTime      : 2022-10-04 00:15:05
 * LastModified    : 2022-10-06 19:14:58
 * Vim             : ts=4, sw=4
 */

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

static struct options {
    const char *filename;
    const char *contents;
    char *mountpoint;
    int show_help;
} options;

struct fuse_session *se;
static const char* hello_str = "Hello world\n";
static const char* hello_name = "hello";

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
    OPTION("--name=%s", filename),
    OPTION("--contents=%s", contents),
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    FUSE_OPT_END
};

struct dirbuf{
    char *p;
    size_t size;
};

static int kirin_stat(fuse_ino_t ino, struct stat* stbuf){
    stbuf->st_ino = ino;
    switch (ino) {
        case 1:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;
        case 2:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(hello_str);
            break;
        default:
            return -1;
    }
    return 0;
}

static void kirin_lookup(fuse_req_t req, fuse_ino_t parent,
        const char *name) {
    struct fuse_entry_param e;

    if (parent != 1 || strcmp(name, hello_name) != 0)
        fuse_reply_err(req, ENOENT);
    else{
        memset(&e, 0, sizeof(e));
        e.ino = 2;
        e.attr_timeout = 1.0;
        e.entry_timeout = 1.0;
        kirin_stat(e.ino, &e.attr);

        fuse_reply_entry(req, &e);
    }
}

static void kirin_getattr(fuse_req_t req, fuse_ino_t ino,
        struct fuse_file_info *fi) {

    struct stat stbuf;
    (void) fi;

    memset(&stbuf, 0, sizeof(stbuf));
    if (kirin_stat(ino, &stbuf) == -1)
        fuse_reply_err(req, ENOENT);
    else
        fuse_reply_attr(req, &stbuf, 1.0);
}

static void dirbuf_add(fuse_req_t req, struct dirbuf *b, const char *name,
               fuse_ino_t ino)
{
    struct stat stbuf;
    size_t oldsize = b->size;
    b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
    b->p = (char *) realloc(b->p, b->size);
    memset(&stbuf, 0, sizeof(stbuf));
    stbuf.st_ino = ino;
    fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf,
              b->size);
}

#define min(x, y) ((x) < (y) ? (x) : (y))

static int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
                 off_t off, size_t maxsize)
{
    if (off < bufsize)
        return fuse_reply_buf(req, buf + off,
                      min(bufsize - off, maxsize));
    else
        return fuse_reply_buf(req, NULL, 0);
}

static void kirin_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
        off_t off, struct fuse_file_info *fi) {
    (void) fi;

    if(ino != 1)
        fuse_reply_err(req, ENOTDIR);
    else {
        struct dirbuf b;

        memset(&b, 0, sizeof(b));
        dirbuf_add(req, &b, ".", 1);
        dirbuf_add(req, &b, "..", 1);
        dirbuf_add(req, &b, hello_name, 2);
        reply_buf_limited(req, b.p, b.size, off, size);
        free(b.p);
    }
}

static void kirin_open(fuse_req_t req, fuse_ino_t ino,
              struct fuse_file_info *fi)
{
    if (ino != 2)
        fuse_reply_err(req, EISDIR);
    else if ((fi->flags & O_ACCMODE) != O_RDONLY)
        fuse_reply_err(req, EACCES);
    else
        fuse_reply_open(req, fi);
}

static void kirin_read(fuse_req_t req, fuse_ino_t ino, size_t size,
              off_t off, struct fuse_file_info *fi)
{
    (void) fi;

    assert(ino == 2);
    reply_buf_limited(req, hello_str, strlen(hello_str), off, size);
}

const static struct fuse_lowlevel_ops kirin_oper = {
    .lookup = kirin_lookup,
    .getattr = kirin_getattr,
    .readdir = kirin_readdir,
    .open = kirin_open,
    .read = kirin_read,
};

int main(int argc, char *argv[]){
    printf("hello world\n");
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_cmdline_opts opts;
    int ret = -1;

    options.mountpoint = strdup("/tmp/kirinfs_XXXXXX");
    if(mkdtemp(options.mountpoint) == NULL) {
        perror("mkdtemp");
        return -1;
    }

    fuse_lowlevel_version();

    se = fuse_session_new(&args, &kirin_oper, sizeof(kirin_oper), NULL);
    if (!se){
        goto err_out1;
    }

    if (fuse_set_signal_handlers(se) != 0)
        goto err_out2;

    if (fuse_session_mount(se, options.mountpoint) != 0)
        goto err_out3;

    fuse_daemonize(opts.foreground);

    if (opts.singlethread)
        ret = fuse_session_loop(se);
    else
        ret = fuse_session_loop_mt(se, opts.clone_fd);

    fuse_session_unmount(se);
    printf("bye bye\n");

err_out3:
    fuse_remove_signal_handlers(se);
err_out2:
    fuse_session_destroy(se);
err_out1:
    free(options.mountpoint);
    fuse_opt_free_args(&args);

    return ret ? 1 : 0;
}
