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

# main.c编译
```shell
$ gcc -Wall main.c -D_FILE_OFFSET_BITS=64 `pkg-config fuse3 --cflags --libs` -o main
```
