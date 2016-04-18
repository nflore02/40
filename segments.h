/* Homework 6 UM - Nick Flores & Daniel Rodrigues
 * segments.h
 * 
 * This is the header file for our segments implementation.
 * It manages the manipulation and access of the segments in memory
 */

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct memory {
        Seq_T segments;
        Seq_T unmapped_ids;
        uint32_t total_ids;
} memory;

/*
 * return the uint32_t word at the passed in id/offset
 */
uint32_t Seg_load(memory *mem, uint32_t Seg_id, uint32_t offset);

/*
 * returns 1 or 0 depending if the value is properly stored or not
 */
bool Seg_store(memory *mem, uint32_t value, uint32_t Seg_id, int offset);

/*
 * returns id of the segment that was mapped
 * and stores it in mapped ID sequence
 */
uint32_t Seg_map(memory *mem, int size);

/*
 * returns id of the segment that was unmapped
 * and stores it in unmapped ID sequence
 */
void Seg_unmap(memory *mem, uint32_t Seg_id);

/*
 *
 *
 */
void Seg_replace(memory *mem, uint32_t Seq_id);

void Seg_free(memory *mem);
#endif
