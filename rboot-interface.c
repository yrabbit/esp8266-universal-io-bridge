#include "rboot-interface.h"
#include "util.h"
#include "sys_string.h"
#include "sdk.h"

#include <stdint.h>
#include <stdbool.h>

static unsigned int cache_map_entry[2] = { 0xffff, 0xffff };

typedef union
{
	rboot_if_rtc_config_t	data;
	uint32_t 				overlay[4];
} rboot_rtc_data_overlay_t;

_Static_assert(sizeof(rboot_if_rtc_config_t) < sizeof(uint32_t[4]), "RTC RAM struct overlay too small");

void Cache_Read_Enable_New(void);
iram attr_used void Cache_Read_Enable_New(void)
{
	extern uint32_t	SPIRead(uint32_t, void *, uint32_t);
	extern void		Cache_Read_Enable(uint32_t, uint32_t, uint32_t);

			rboot_if_config_t config;
	const	rboot_rtc_data_overlay_t *rtc_in_iospace;
			rboot_rtc_data_overlay_t rtc_in_dram;
			uint32_t cache_address;
			unsigned int ix;

	if(cache_map_entry[0] == 0xffff)
	{
		SPIRead(OFFSET_OTA_RBOOT_CFG, &config, sizeof(config));

		rtc_in_iospace = (const rboot_rtc_data_overlay_t *)(0x60001100 + (rboot_if_rtc_address << 2));

		for(ix = 0; ix < sizeof(rtc_in_dram.overlay) / sizeof(rtc_in_dram.overlay[0]); ix++)
			rtc_in_dram.overlay[ix] = rtc_in_iospace->overlay[ix];

		// Don't check for next_mode == RBOOT_TEMP_ROM and neither use next_slot
		// 		because they already have been reset by rboot at this point.
		// Trust rboot to have selected the correct rom slot instead.

		if(rtc_in_dram.data.magic == rboot_if_rtc_magic)
			cache_address = config.slots[rtc_in_dram.data.last_slot];
		else
			cache_address = config.slots[config.slot_current];

		cache_address /= 0x100000;

		cache_map_entry[0] = cache_address % 2;
		cache_map_entry[1] = cache_address / 2;
	}

	Cache_Read_Enable(cache_map_entry[0], cache_map_entry[1], 1);
}

attr_const unsigned int rboot_if_mapped_slot(void)
{
	unsigned int rv = 0;

	if(cache_map_entry[0])
		rv |= 1 << 0;

	if(cache_map_entry[1])
		rv |= 1 << 1;

	return(rv);
}

attr_const const char *rboot_if_boot_mode(unsigned int index)
{
	if(index == rboot_if_conf_mode_standard)
		return("standard");

	if(index == rboot_if_conf_mode_temp_rom)
		return("temp_rom");

	return("unknown");
}

bool rboot_if_write_config(const rboot_if_config_t *config, string_t *string_buffer)
{
	uint32_t *buffer = (uint32_t *)(void *)string_buffer_nonconst(string_buffer);

	if(string_size(string_buffer) < SPI_FLASH_SEC_SIZE)
		return(false);

	if(spi_flash_read(OFFSET_OTA_RBOOT_CFG, buffer, SPI_FLASH_SEC_SIZE) != SPI_FLASH_RESULT_OK)
		return(false);

	memcpy(buffer, config, sizeof(*config));

	if(spi_flash_erase_sector(OFFSET_OTA_RBOOT_CFG / SPI_FLASH_SEC_SIZE) != SPI_FLASH_RESULT_OK)
		return(false);

	if(spi_flash_write(OFFSET_OTA_RBOOT_CFG, buffer, SPI_FLASH_SEC_SIZE) != SPI_FLASH_RESULT_OK)
		return(false);

	return(true);
}

bool rboot_if_read_config(rboot_if_config_t *config)
{
	if(spi_flash_read(OFFSET_OTA_RBOOT_CFG, (uint32_t *)config, sizeof(*config)) != SPI_FLASH_RESULT_OK)
		return(false);

	if(config->magic != rboot_if_conf_magic)
		return(false);

	return(true);
}

static uint8_t checksum(const uint8_t *start, const uint8_t *end)
{
    uint8_t cs;

	for(cs = rboot_if_rtc_checksum_init; start != end; start++)
        cs ^= *start;

    return(cs);
}

bool rboot_if_write_rtc_ram(rboot_if_rtc_config_t *config)
{
	config->checksum = checksum((const uint8_t *)config, (const uint8_t *)&config->checksum);

	if(!system_rtc_mem_write(rboot_if_rtc_address, config, sizeof(*config)))
		return(false);

	return(true);
}

bool rboot_if_read_rtc_ram(rboot_if_rtc_config_t *config)
{
	if(!system_rtc_mem_read(rboot_if_rtc_address, config, sizeof(*config)))
		return(false);

	if(config->magic != rboot_if_rtc_magic)
		return(false);

	return(config->checksum == checksum((const uint8_t *)config, (const uint8_t *)&config->checksum));
}

void rboot_if_info(string_t *dst)
{
	rboot_if_config_t config;
	rboot_if_rtc_config_t rrtc;

	string_format(dst, ">\n> OTA image information:\n");

	if(rboot_if_read_config(&config))
		string_format(dst,
				">  rboot magic number: 0x%08x\n"
				">  rboot version: %u\n"
				">  boot mode: %s\n"
				">  current slot: %u\n"
				">  mapped slot: %u\n"
				">  slot count: %u\n"
				">  slot 0: 0x%06lx\n"
				">  slot 1: 0x%06lx\n",
				config.magic,
				config.version,
				rboot_if_boot_mode(config.boot_mode),
				config.slot_current,
				rboot_if_mapped_slot(),
				config.slot_count,
				config.slots[0],
				config.slots[1]);
	else
		string_format(dst, ">  rboot config unavailable\n");

	string_format(dst, ">\n> OTA RTC RAM boot config information:\n");

	if(rboot_if_read_rtc_ram(&rrtc))
		string_format(dst,
				">   magic number: 0x%08lx\n"
				">   current boot mode: %s\n"
				">   current slot: %u\n"
				">   start once boot mode: %s\n"
				">   start once rom slot: %u\n"
				">   struct checksum: %x\n",
			rrtc.magic,
			rboot_if_boot_mode(rrtc.last_mode),
			rrtc.last_slot,
			rboot_if_boot_mode(rrtc.next_mode),
			rrtc.temporary_slot,
			rrtc.checksum);
	else
		string_append(dst, "\n> rboot RTC RAM boot config unvailable\n");
}
