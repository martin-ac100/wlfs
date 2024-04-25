#include <string.h>
#include "wlfs.h"
#include "stdio.h"

static wlfs_header_t cursor, cursor_last;
static uint32_t cursor_offset, cursor_last_offset;
static char magic[]="wlfs----";
static int HEADER_LEN;

static wlfs_config_t *wlfs_config;

int wlfs_find_rec() {
	cursor_offset = wlfs_config->start;
	memcpy(cursor.magic, magic, sizeof(magic)-1 );
	cursor_last.version=0;
	while ( cursor_offset+sizeof(wlfs_header_t) <= wlfs_config->end ) {
		wlfs_read(cursor_offset, &cursor, sizeof(wlfs_header_t));
		if ( strncmp(cursor.magic,"wlfs----",8) || cursor.version <= cursor_last.version) {
			break;
		}
		cursor_last = cursor;
		cursor_last_offset = cursor_offset;
		cursor_offset = SECTOR_ALIGN((cursor_offset + HEADER_LEN + cursor.len));
	}
	cursor_last_offset += HEADER_LEN;
	return cursor_last.version;
}

uint32_t wlfs_rec_len() {
	return wlfs_find_rec() ? cursor_last.len : 0;
}

void wlfs_init( wlfs_config_t *config) {
	wlfs_config = config;
	HEADER_LEN = WRITE_ALIGN(sizeof(wlfs_header_t));
	printf("HEADER_LEN = %d\n",HEADER_LEN);
	printf("SECTOR_ALIGN = %d\n",SECTOR_ALIGN(16 + HEADER_LEN + 5));
}

uint32_t wlfs_load(void *dst) {
	if (wlfs_find_rec() && (cursor_last_offset + cursor_last.len) <= wlfs_config->end) {
		return wlfs_read( cursor_last_offset, dst, cursor_last.len );
	}
	else {
		return 0;
	}
}


uint32_t wlfs_store(void *src, uint32_t len) {
	int ret_code = 0;
	if ( wlfs_find_rec() &&  (cursor_offset + HEADER_LEN + len) <= wlfs_config->end ) {
		cursor.version = cursor_last.version + 1;
		cursor.len = len;
		memcpy(cursor.magic, magic, sizeof(magic)-1 );
		if ( wlfs_config->erase_before_write ) {
			wlfs_erase(cursor_offset,HEADER_LEN + len);
		}
		wlfs_write(cursor_offset, &cursor, sizeof(cursor));
		cursor_offset += HEADER_LEN;
		ret_code = wlfs_write(cursor_offset, src, len);
	}
	else {
		cursor_offset = wlfs_config->start;
		cursor.version = 1;
		cursor.len = len;
		memcpy(cursor.magic, magic, sizeof(magic)-1 );
		if ( (cursor_offset + HEADER_LEN + len) <= wlfs_config->end ) {
			wlfs_erase(wlfs_config->start, wlfs_config->end - wlfs_config->start + 1);
			wlfs_write(cursor_offset, &cursor, sizeof(cursor));
			cursor_offset += HEADER_LEN;
			ret_code = wlfs_write(cursor_offset, src, len);
		}
	}
	return ret_code;
}
