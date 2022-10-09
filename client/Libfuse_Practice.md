# libfuse实验
## libfuse编译

```shell
# gcc -9 安装
$ yum install -y devtoolset-9-gcc-c++
$ source /opt/rh/devtoolset-9/enable
$ mkdir bld; cd bld
$ meson ..
$ ninja
$ ninja install
[0/1] Installing files.
Installing lib/libfuse3.so.3.8.0 to /usr/local/lib64
Installing util/fusermount3 to /usr/local/bin
Installing util/mount.fuse3 to /usr/local/sbin
Installing /zhongyl/libfuse/include/fuse.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/include/fuse_common.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/include/fuse_lowlevel.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/include/fuse_opt.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/include/cuse_lowlevel.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/include/fuse_log.h to /usr/local/include/fuse3
Installing /zhongyl/libfuse/doc/fusermount3.1 to /usr/local/share/man/man1
Installing /zhongyl/libfuse/doc/mount.fuse3.8 to /usr/local/share/man/man8
Installing /zhongyl/libfuse/bld/meson-private/fuse3.pc to /usr/local/lib64/pkgconfig
Running custom install script '/zhongyl/libfuse/util/install_helper.sh /usr/local/etc /usr/local/bin /usr/lib/udev/rules.d true'
== FURTHER ACTION REQUIRED ==
Make sure that your init system will start the /etc/init.d/fuse3 init script
$ cp -f /usr/local/lib64/pkgconfig/fuse3.pc /usr/lib64/pkgconfig/
```

## example实验
```shell
$ pwd
/zhongyl/libfuse/bld/example
$ mkdir dir
$ ./hello dir
$ cd dir
$ cat hello
hello world!
$ ls -l hello
-r--r--r-- 1 root root 13 Dec 31  1969 hello
$ rm -rf hello
rm: cannot remove ‘hello’: Function not implemented
```

## main.c编译
```shell
$ gcc -Wall main.c -D_FILE_OFFSET_BITS=64 `pkg-config fuse3 --cflags --libs` -o main
```

## gdb单步调试
```shell
# 通过gcc的-g选项生成调试信息
$ gcc -Wall -g main.c -D_FILE_OFFSET_BITS=64 `pkg-config fuse3 --cflags --libs` -o main
# 启动
$ gdb ./main
# 设置断点
$ b kirin_lookup
$ b kirin_getattr
$ b kirin_readdir
$ b kirin_open
$ b kirin_read
# info break确认断点信息
$ info break
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000400e75 in kirin_lookup at main.c:67
2       breakpoint     keep y   0x0000000000400f4a in kirin_getattr at main.c:86
3       breakpoint     keep y   0x0000000000401176 in kirin_readdir at main.c:122
4       breakpoint     keep y   0x000000000040123a in kirin_open at main.c:139
5       breakpoint     keep y   0x00000000004012a5 in kirin_read at main.c:152
# ls 挂载点，显示栈帧
# 1. getattr
$ bt
#0  kirin_getattr (req=0x603510, ino=1, fi=0x0) at main.c:86
#1  0x00007ffff7bb4645 in do_getattr () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 2. readdir
$ bt
#0  kirin_readdir (req=0x603510, ino=1, size=4096, off=0, fi=0x7fffffffe290) at main.c:122
#1  0x00007ffff7bb3e4a in do_readdir () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 3. lookup
$ bt
#0  kirin_lookup (req=0x603510, parent=1, name=0x7ffff7edc038 "hello") at main.c:70
#1  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#3  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 4. readdir
$  bt
#0  kirin_readdir (req=0x603510, ino=1, size=4096, off=96, fi=0x7fffffffe290) at main.c:122
#1  0x00007ffff7bb3e4a in do_readdir () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# cat 文件
# 1. getattr
$ bt
#0  kirin_getattr (req=0x603510, ino=1, fi=0x0) at main.c:87
#1  0x00007ffff7bb4645 in do_getattr () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 2. lookup
#0  kirin_lookup (req=0x603510, parent=1, name=0x7ffff7edc038 "hello") at main.c:67
#1  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#3  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 3. open
#0  kirin_open (req=0x603510, ino=2, fi=0x7fffffffe290) at main.c:144
#1  0x00007ffff7bb4bbc in do_open () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 4. getattr
#0  kirin_getattr (req=0x603510, ino=2, fi=0x0) at main.c:86
#1  0x00007ffff7bb4645 in do_getattr () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 5. read
#0  kirin_read (req=0x603510, ino=2, size=4096, off=0, fi=0x7fffffffe290) at main.c:152
#1  0x00007ffff7bb42b2 in do_read () from /lib64/libfuse3.so.3
#2  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#4  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
# 6. getattr
#0  kirin_stat (ino=2, stbuf=0x7fffffffe1f0) at main.c:46
#1  0x0000000000400f7c in kirin_getattr (req=0x603510, ino=2, fi=0x7fffffffe290) at main.c:87
#2  0x00007ffff7bb4645 in do_getattr () from /lib64/libfuse3.so.3
#3  0x00007ffff7bb64f6 in fuse_session_process_buf_int () from /lib64/libfuse3.so.3
#4  0x00007ffff7bb158d in fuse_session_loop () from /lib64/libfuse3.so.3
#5  0x00000000004013f4 in main (argc=1, argv=0x7fffffffe5e8) at main.c:192
```
