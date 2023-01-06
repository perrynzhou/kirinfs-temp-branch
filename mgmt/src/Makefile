all:
	gcc -DTEST -g -O0 -std=gnu99 -w \
	../../module/stl/src/stl_string.h ../../module/stl/src/stl_string.c  \
	../../module/stl/src/stl_hash.h ../../module/stl/src/stl_hash.c  \
	../../module/stl/src/stl_socket.h   ../../module/stl/src/stl_socket.c \
	../../module/stl/src/stl_epoll.h   ../../module/stl/src/stl_epoll.c \
	../../module/stl/src/stl_dict.h   ../../module/stl/src/stl_dict.c \
	service_node.h service_node.c \
	mgmt_service.h mgmt_service.c \
	mgmt_cmd.h mgmt_cmd.c  mgmt_main.c -o test_mgmt_server -lpthread -lrt

	gcc -DTEST -g -O0 -std=gnu99 -w \
	../../module/stl/src/stl_string.h ../../module/stl/src/stl_string.c  \
	../../module/stl/src/stl_socket.h   ../../module/stl/src/stl_socket.c \
	mgmt_cmd.h mgmt_cmd.c mgmt_client.c -o test_mgmt_client


clean:
	rm -rf test_mgmt_server
	rm -rf test_mgmt_client


