#ifndef _sequencer_h_
#define _sequencer_h_

#include "attribute.h"
#include "util.h"

#include <stdint.h>
#include <stdbool.h>

int			sequencer_get_current(void);
int			sequencer_get_start(void);
uint64_t	sequencer_get_current_end_time(void);
int			sequencer_get_repeats(void);
void		sequencer_get_status(bool *running, unsigned int *start, unsigned int *flash_size, unsigned int *flash_size_entries,
				unsigned int *flash_offset_flash0, unsigned int *flash_offset_flash1, unsigned int *flash_offset_mapped);
void		sequencer_run(void);
void		sequencer_init(void);
bool		sequencer_clear(void);
void		sequencer_start(unsigned int start, unsigned int repeats);
void		sequencer_stop(void);
bool		sequencer_set_entry(unsigned int entry, int io, int pin, uint32_t value, int duration);
bool		sequencer_get_entry(unsigned int entry, bool *active, int *io, int *pin, unsigned int *value, unsigned int *duration);
bool		sequencer_remove_entry(unsigned int entry);

#endif
