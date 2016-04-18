/*
 * Homework 6 UM - Nick Flores & Daniel Rodrigues
 *
 * segment_test.c
 * This is our testing file to test only our segment.c
 * implementation. It includes the segments.h header file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "segments.h"

void test_load()
{
        uint32_t test_id = 0;
        uint32_t value = 0;
        uint32_t offset = 0;
        value = Seg_load(test_id, offset);
        fprintf(stderr, "value at %u: %u\n", test_id, value);
}

void test_store()
{
        uint32_t test_id = 0;
        uint32_t test_value = 10;
        Seg_store(test_value, test_id);
        //fprintf(stderr, "value at %u: %u\n", test_id, value);
}

void test_map()
{
        uint32_t length = 2;
        uint32_t Seg_id = 0;
        Seg_id = Seg_map(length);
        fprintf(stderr, "Seg_id: %u\n", Seg_id);
}

void test_unmap()
{
        uint32_t Seg_id = 0;
        Seg_unmap(Seg_id);
        //need to print out?
}

int main()
{
        test_map();
        test_store();
        test_load();
        test_unmap();
}
