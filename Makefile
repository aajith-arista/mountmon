all: mountmon

mountmon: mountmon.c libmount.a libblkid.a libselinux.a
	gcc mountmon.c -static -o mountmon -L . -lmount -lblkid -lselinux
clean:
	rm mountmon *.o
