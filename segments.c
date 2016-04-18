/* 
 * Homework 6 UM - Nick Flores & Daniel Rodrigues
 * segments.c
 * 
 * This is the implementatino file for our segments header file.
 * It manages the manipulation and access of the segments in memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "seq.h"
#include "segments.h"

/*
 * Seq_load:
 *      grabs the segment at the given ID and returns
 *      the value at the given offset
 */
uint32_t Seg_load(memory *mem, uint32_t Seg_id, uint32_t offset)
{
        Seq_T segment = (Seq_T)Seq_get(mem->segments, Seg_id);
        return (uint32_t)(uintptr_t)Seq_get(segment, offset);
}

/*
 * Seg_store:
 *      grabs the segment at the given ID
 *      adds high if offset >= length of the sequence
 *      adds low if offset is 0
 *      or simply puts at the offset
 *      returns true if succesfully stored
 */
bool Seg_store(memory *mem, uint32_t value, uint32_t Seg_id, int offset)
{
        Seq_T segment = (Seq_T)Seq_get(mem->segments, Seg_id);
        if ((int)offset == Seq_length(segment)) {
                Seq_addhi(segment, (void *)(uintptr_t)value);
        } else {
                Seq_put(segment, offset, (void *)(uintptr_t)value);
        }
        return true;
}

/*
 * Seg_map:
 *      removes low if length of unmpaped sequence is > 0
 *      adds low if ID is 0 otherwise adds high
 *      returns the ID
 */
uint32_t Seg_map(memory *mem, int size)
{
        uint32_t id;
        uint32_t init = 0;
        int i, k;
        if (Seq_length(mem->unmapped_ids) > 0) {
                id = (uint32_t)(uintptr_t)Seq_remlo(mem->unmapped_ids);
                Seq_T old_seg = Seq_get(mem->segments, id);
                int length = Seq_length(old_seg);
                for (k = 0; k < size; k++) {
                        if (k < length) {
                                Seq_put(old_seg, k, (void *)(uintptr_t)init);
                        } else {
                                Seq_addhi(old_seg, (void *)(uintptr_t)init);
                        }
                }
                return id;
        } else {
                id = mem->total_ids;
                mem->total_ids++;
                Seq_T newSeg = Seq_new(size);
                for (i = 0; i < size; i++) {
                        Seq_addlo(newSeg, (void *)(uintptr_t)init);
                }

                if (id == 0) {
                        Seq_addlo(mem->segments, newSeg);
                } else {
                        Seq_addhi(mem->segments, newSeg);
                }
                return id;
        }
}

/*
 * Seg_unmap:
 *      adds low from unmapped ids sequence at given ID
 */
void Seg_unmap(memory *mem, uint32_t Seg_id)
{
        Seq_addlo(mem->unmapped_ids, (void *)(uintptr_t)Seg_id);
}

/*
 * Seg_replace:
 *      removes low from segments sequence
 *      creates a new sequence and duplicates the
 *      segments sequence
 *      loops through duplicate sequence and grabs each word
 *      then adds high the word into the new sequence
 *      then adds low from the new sequence into segments
 */
void Seg_replace(memory *mem, uint32_t Seg_id)
{
        Seq_T new_seq = Seq_new(0);

        Seq_T duplicate_seq = Seq_get(mem->segments, Seg_id);
        int i;
        for (i = 0; i < Seq_length(duplicate_seq); i++) {
                uint32_t word = (uint32_t)(uintptr_t)Seq_get(duplicate_seq, i);
                Seq_addhi(new_seq, (void *)(uintptr_t)word);
        }
        Seq_T elem = Seq_put(mem->segments,0,  new_seq);
        Seq_free(&elem);
}

/*
 * Seg_free:
 *      loops through the segments sequence and frees them
 *      using Seq_free
 *      frees unmapped_ids and the segments sequence as a whole
 */
void Seg_free(memory *mem)
{
        int i;
        int length = Seq_length(mem->segments);
        for (i = 0; i < length; i++) {
                Seq_T elem = Seq_remlo(mem->segments);
                Seq_free(&elem);
        }
        Seq_free(&(mem->unmapped_ids));
        Seq_free(&(mem->segments));
}
