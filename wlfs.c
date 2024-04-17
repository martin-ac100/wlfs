#include <string.h>
#include "wlfs.h"

#define SECTOR_ALIGN(x) (x+wlfs_config->sector_size-1) & ~(wlfs_config->sector_size-1)

static wlfs_rec_t cursor;
static wlfs_rec_t cursor_last;
static char magic[]="wlfs----";

static wlfs_config_t *wlfs_config;

int wlfs_find_rec() {
	cursor.offset = SECTOR_ALIGN(wlfs_config->start);
	cursor_last.header.version = 0;
	if (cursor.offset + sizeof(wlfs_rec_header_t) < wlfs_config->end) {
		wlfs_read(cursor.offset, &cursor.header, sizeof(wlfs_rec_header_t));
		while ( !strncmp(cursor.header.magic,"wlfs----",8) && cursor.header.version > cursor_last.header.version) {
			cursor_last = cursor;
			cursor.offset = SECTOR_ALIGN(cursor.offset + sizeof(wlfs_rec_header_t) + cursor.header.len);
			if (cursor.offset + sizeof(wlfs_rec_header_t) < wlfs_config->end) {
				wlfs_read(cursor.offset, &cursor.header, sizeof(wlfs_rec_header_t));
			}
		}
	}
	cursor_last.offset += sizeof(wlfs_rec_header_t);
	return cursor_last.header.version;
}

uint32_t wlfs_rec_len() {
	return wlfs_find_rec() ? cursor_last.header.len : 0;
}

uint32_t wlfs_init( wlfs_config_t *config) {
	wlfs_config = config;
	return wlfs_rec_len();
}

uint32_t wlfs_load(void *dst) {
	return wlfs_find_rec() ? wlfs_read( cursor_last.offset, dst, cursor_last.header.len ) : 0;
}

uint32_t wlfs_store(void *src, uint32_t len) {
	int ret_code = -1;
	if ( wlfs_find_rec() &&  sizeof(wlfs_rec_header_t) + len <= wlfs_config->end - cursor.offset ) {
		cursor.header.version = cursor_last.header.version + 1;
		cursor.header.len = len;
		memcpy(cursor.header.magic, magic, sizeof(magic)-1 );
		ret_code = wlfs_write(&cursor,src);
	}
	else {
		cursor.offset = SECTOR_ALIGN(wlfs_config->start);
		cursor.header.version = 1;
		cursor.header.len = len;
		memcpy(cursor.header.magic, magic, sizeof(magic)-1 );
		if ( sizeof(wlfs_rec_header_t) + len <= wlfs_config->end - cursor.offset ) {
			ret_code = wlfs_format_and_write(&cursor,src);
		}
	}
	return ret_code;
}
		


