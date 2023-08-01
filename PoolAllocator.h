#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <stdint.h>

#define PA_MEM_POOL_SIZE  500
#define PA_MEM_CHUNK_SIZE 20      //Minimum chunk size 4

typedef enum {
  PA_MEM_DEALLOCATED,
  PA_INCORRECT_MEM,
  PA_INIT_SUCCEEDED,
  PA_INIT_FAILED,
} pa_return_status_t;

pa_return_status_t pa_init_allocator(void);

void * pa_allocate(void);

pa_return_status_t pa_deallocate(void * ptr);

#endif
