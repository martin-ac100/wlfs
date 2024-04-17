#include <stdio.h>
#include <string.h>
#include "wlfs.h"

#define IMG "wlfs.img"
#define img_sector_size 16
#define img_total_sectors 8
#define img_file_offset 8

const char zeros[img_sector_size]={0};

wlfs_config_t wlfs_config;

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
	wlfs_config.start = img_file_offset;
	wlfs_config.sector_size = img_sector_size;
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
	return (int)fd;
}




uint32_t wlfs_read( uint32_t offset, void *dst, uint32_t len) {
	memcpy( dst, data+offset, len);
	return len;
}

int wlfs_erase(uint32_t start, uint32_t len) {
	int sectors = (len / img_sector_size) + 1;
	memset(data+start, 0, img_sector_size * sectors); 
	return len;
}


uint32_t wlfs_write( wlfs_rec_t *rec, const void *src) {
	if ( wlfs_erase(rec->offset, rec->header.len + sizeof(wlfs_rec_header_t)) ) {
		memcpy(data+rec->offset, &rec->header,sizeof(wlfs_rec_header_t));
		memcpy(data+rec->offset+sizeof(wlfs_rec_header_t), src, rec->header.len); 
	}
	return rec->header.len;
}


uint32_t wlfs_format_and_write( wlfs_rec_t *rec, const void *src) {
	if ( wlfs_erase(wlfs_config.start, wlfs_config.end - wlfs_config.start + 1) ) {
		memcpy(data+rec->offset, &rec->header,sizeof(wlfs_rec_header_t));
		memcpy(data+rec->offset+sizeof(wlfs_rec_header_t), src, rec->header.len); 
	}
	return rec->header.len;
}


int main( int argc, const char *argv[]) {
	char buf[8192];
	wlfs_open();
	printf("start offset: %d\nend: %d\nsector size: %d\n",wlfs_config.start,wlfs_config.end,wlfs_config.sector_size);
	printf("last record len: %d\n", wlfs_init(&wlfs_config));
	if (argc == 1) {
		wlfs_load( (void*)buf);
		puts(buf);
	}
	else if (argc == 2) {
		printf("Stored %d bytes of data\n", wlfs_store( (void *)argv[1], strlen(argv[1])));
	}
	wlfs_close();
}
