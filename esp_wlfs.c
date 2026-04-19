#include <stdio.h>
#include <string.h>
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "wlfs.h"


wlfs_config_t wlfs_config = {.sector_size=512, .write_align=16, .erase_before_write=1};
wlfs_config_t *p_wlfs_config = &wlfs_config;
const esp_partition_t *boot_part;

uint32_t wlfs_read(uint32_t offset, void *dst, uint32_t len) {
	return esp_partition_read(boot_part, (size_t)offset, dst, (size_t)len) == ESP_OK ? len : 0;
}

uint32_t wlfs_write(uint32_t offset, const void *src, uint32_t len){
	return esp_partition_write(boot_part, (size_t)offset, src, (size_t)len) == ESP_OK ? len : 0;
}

uint32_t wlfs_erase(uint32_t offset, uint32_t len) {
	return esp_partition_erase_range(boot_part, (size_t)offset, (size_t)len) == ESP_OK ? len : 0;
}

void app_main(void)
{

	puts("Started\n");
	boot_part=esp_ota_get_boot_partition();
	printf("boot partition: type %x, subtype %x, size %ld, label %s\n", boot_part->type, boot_part->subtype, boot_part->size, boot_part->label);

	char data[512];
	esp_image_header_t *img=(esp_image_header_t  *)&data;
	esp_image_segment_header_t *segment=(esp_image_segment_header_t *)&data;
	uint8_t segment_count;
	size_t offset=0;
	esp_partition_read(boot_part, offset, data, sizeof(esp_image_header_t));
	segment_count=img->segment_count;
	printf("magic: %hx, # segments: %u, hash_appended %d\n",img->magic, segment_count, img->hash_appended);
	offset += sizeof(esp_image_header_t);

	for (int i=0; i < segment_count; i++) {
		esp_partition_read(boot_part, offset, data, sizeof(esp_image_segment_header_t));
		printf("segment #%d, paddr=%x, load_addr %lx, data_len %lx\n", i, offset , segment->load_addr, segment->data_len);
		offset += sizeof(esp_image_segment_header_t) + segment->data_len;
	}
	printf("partition free space offset = %x\n",offset);
	
	wlfs_config.start = SECTOR_ALIGN(p_wlfs_config,offset);
	wlfs_config.end = boot_part->size-1;
	wlfs_init(&wlfs_config);
	memset(data,0,sizeof(data));
	if ( wlfs_rec_len() ) {
		printf("Loaded data, LEN = %lu, %s\n",wlfs_load(data),data);
	}
	strcat(data,"Hello world! ");
	wlfs_store(data,strlen( (const char *)data));

}	





