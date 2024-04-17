#include <stdint.h>
// platform independent
typedef struct wlfs_config_t {
	uint32_t sector_size;
	uint32_t start;
	uint32_t end;
} wlfs_config_t;

typedef struct wlfs_rec_header_t {
	char magic[8];
	uint32_t version;
	uint32_t len;
} wlfs_rec_header_t;

typedef struct wlfs_rec_t {
	uint32_t offset;
	wlfs_rec_header_t header;
} wlfs_rec_t;

uint32_t wlfs_init( wlfs_config_t *config);
uint32_t wlfs_rec_len();
uint32_t wlfs_load(void *dst);
uint32_t wlfs_store(void *src, uint32_t len);

// platform specific functions - to be implemented externaly
uint32_t wlfs_read(uint32_t offset, void *dst, uint32_t len);
uint32_t wlfs_write( wlfs_rec_t *rec, const void *src);
uint32_t wlfs_format_and_write( wlfs_rec_t *rec, const void *src);
