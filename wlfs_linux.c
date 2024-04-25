#include <stdio.h>
#include <string.h>
#include "wlfs.h"

#define IMG "wlfs.img"
#define img_sector_size 16
#define img_write_align 1
#define img_total_sectors 8
#define img_file_offset 8

const char zeros[img_sector_size]={0};

wlfs_config_t wlfs_config;
wlfs_config_t *p_wlfs_config = &wlfs_config;

#define img_len img_sector_size * img_total_sectors + img_file_offset
char data[img_len];

uint32_t wlfs_open() {
	FILE *fd;
	uint32_t bytes_read = 0;
	fd = fopen(IMG, "rb");
	wlfs_config.end = 0;
	if (fd) {
		fread(data,1,img_len,fd);
		wlfs_config.end = img_len-1;
		fclose(fd);
	}
	wlfs_config.sector_size = img_sector_size;
	wlfs_config.write_align = img_write_align;
	wlfs_config.start = SECTOR_ALIGN(p_wlfs_config, img_file_offset);
	wlfs_init(&wlfs_config);
	return wlfs_config.end;
}

int wlfs_close() {
	FILE *fd;
	fd = fopen(IMG,"wb");
	if (fd) {
		fwrite(data,1,img_len,fd);
		fclose(fd);
	}
	return fd ? 0 : -1 ;
}




uint32_t wlfs_read( uint32_t offset, void *dst, uint32_t len) {
	memcpy( dst, data+offset, len);
	return len;
}

uint32_t wlfs_erase(uint32_t start, uint32_t len) {
	int sectors = (len / img_sector_size) + 1;
	memset(data+start, 0, img_sector_size * sectors); 
	return len;
}


uint32_t wlfs_write( uint32_t offset, const void *src, uint32_t len) {
	memcpy( data+offset, src, len);
	return len;
}


int main( int argc, const char *argv[]) {
	char buf[8192];
	wlfs_open();
	printf("start offset: %d\nend: %d\nsector size: %d\n",wlfs_config.start,wlfs_config.end,wlfs_config.sector_size);
	wlfs_init(&wlfs_config);
	printf("last record len: %d\n", wlfs_rec_len());
	if (argc == 1) {
		wlfs_load( (void*)buf);
		puts(buf);
	}
	else if (argc == 2) {
		printf("Stored %d bytes of data\n", wlfs_store( (void *)argv[1], strlen(argv[1])));
	}
	wlfs_close();
}

