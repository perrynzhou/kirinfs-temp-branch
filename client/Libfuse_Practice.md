# libfuse实验
## libfuse编译

```shell
# gcc -9 安装
$ yum install -y devtoolset-9-gcc-c++
$ source /opt/rh/devtoolset-9/enable
$ mkdir bld; cd bld
$ meson ..
$ ninja
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
