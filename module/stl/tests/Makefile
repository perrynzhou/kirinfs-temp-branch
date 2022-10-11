all:
	# gcc -g -O0 ../src/stl_string.h ../src/stl_string.c test_stl_string.c
	#gcc -g -O0 ../src/stl_md5.h ../src/stl_md5.c test_stl_md5.c
	#gcc -g -O0 ../src/stl_thread_pool.h ../src/stl_thread_pool.c test_stl_thread_pool.c  -lpthread
	#gcc -g -O0 ../src/stl_ini.h ../src/stl_ini.c ./test_stl_ini.c
	#gcc -g -O0 ../src/stl_hash.h ../src/stl_hash.c ../src/stl_dict.h ../src/stl_dict.c  ./test_stl_dict.c
	#gcc -g -O0 ../src/stl_rand.h ../src/stl_rand.c ./test_stl_rand.c  -o test_stl_rand
	#gcc -DTEST -g -O0 ../src/stl_socket.h ../src/stl_socket.c ../src/stl_epoll.h ../src/stl_epoll.c -o test_epoll
	#gcc -DTEST -g -O0 ../src/stl_socket.h ../src/stl_socket.c  -o test_socket
	#gcc -DTEST -g -O0 ../src/stl_slab_pool.h ../src/stl_slab_pool.c   test_stl_slab_pool.c  -o test_stl_slab_pool
	#gcc -DTEST -g -O0 ../src/stl_sha1.h ../src/stl_sha1.c test_stl_sha1.c  -o test_sha1
	gcc -DTEST -g -O0 -std=c99 ../src/stl_epoll.h ../src/stl_epoll.c ../src/stl_socket.h ../src/stl_socket.c test_stl_epoll_service.c -lpthread -o test_epoll_server
	gcc -DTEST -g -O0 -std=c99 ../src/stl_epoll.h ../src/stl_epoll.c ../src/stl_socket.h ../src/stl_socket.c test_stl_epoll_client.c -lpthread -o test_epoll_client


clean:
	#rm -rf test_stl_string
	#rm -rf test_stl_md5
	#rm -rf test_stl_thread_pool
	#rm -rf test_stl_dict
	#rm -rf stl_slab_pool
	#rm -rf test_sha1
	rm -rf test_epoll_server
	rm -rf test_epoll_client
