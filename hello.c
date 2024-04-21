#include <stdio.h>
#include "esp_ota_ops.h"
#include "esp_app_format.h"

void app_main(void)
{
	puts("Hello world!\n");
	const esp_partition_t *boot_part;
	boot_part=esp_ota_get_boot_partition();
	printf("boot partition: type %x, subtype %x, size %ld, label %s\n", boot_part->type, boot_part->subtype, boot_part->size, boot_part->label);

	char *data[sizeof(esp_image_header_t)];
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
	

}


