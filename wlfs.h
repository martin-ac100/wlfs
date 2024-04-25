#include <stdint.h>
// platform independent

#define SECTOR_ALIGN(x) (x+wlfs_config->sector_size-1) & ~(wlfs_config->sector_size-1)
#define WRITE_ALIGN(x) (x+wlfs_config->write_align-1) & ~(wlfs_config->write_align-1)

typedef struct wlfs_config_t {
	int sector_size;
	int write_align;
	int erase_before_write;
	uint32_t start;
	uint32_t end;
} wlfs_config_t;

typedef struct wlfs_header_t {
	char magic[8];
	int version;
	uint32_t len;
} wlfs_header_t;


void wlfs_init( wlfs_config_t *config);
uint32_t wlfs_rec_len();
uint32_t wlfs_load(void *dst);
uint32_t wlfs_store(void *src, uint32_t len);

// platform specific functions - to be implemented externaly
uint32_t wlfs_read(uint32_t offset, void *dst, uint32_t len);
uint32_t wlfs_write(uint32_t offset, const void *src, uint32_t len);
uint32_t wlfs_erase(uint32_t offset, uint32_t len);
