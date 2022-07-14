#pragma once
// À´×ÔFFplay.cÔ´Âë
#include <stdint.h>

typedef struct{
	uint8_t* buffer;

	size_t elem_size, nb_elems;
	size_t offset_r, offset_w;
	// distinguishes the ambiguous situation offset_r == offset_w
	int    is_empty;

	unsigned int flags;
	size_t       auto_grow_limit;
}AVFifo;


typedef int AVFifoCB(void* opaque, void* buf, size_t* nb_elems);
size_t av_fifo_can_read(const AVFifo* f);
void av_fifo_drain2(AVFifo* f, size_t size);
void av_fifo_freep2(AVFifo** f);
int av_fifo_read(AVFifo* f, void* buf, size_t nb_elems);
AVFifo* av_fifo_alloc2(size_t elems, size_t elem_size,
	unsigned int flags);
int av_fifo_write(AVFifo* f, const void* buf, size_t nb_elems);

