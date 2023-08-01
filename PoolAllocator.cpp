#include "PoolAllocator.h"
#include <stdlib.h>

static uint8_t mem_pool[PA_MEM_POOL_SIZE];

typedef struct Chunk {
  struct Chunk * next_chunk;
} Chunk_t;

typedef struct {
  Chunk_t * first_free_chunk;
  Chunk_t * last_free_chunk;
  size_t pool_first_address;
  size_t pool_last_address;
  size_t chunk_number;
  size_t allocated_chunk;
  pa_return_status_t init_status;
} PoolAllocator_t;

PoolAllocator_t pool_allocator;

pa_return_status_t pa_init_allocator(void) {
  pool_allocator.chunk_number = PA_MEM_POOL_SIZE / PA_MEM_CHUNK_SIZE;
  Chunk_t * chunk = NULL;
  void * address = NULL;
  address = &mem_pool[0];
  pool_allocator.first_free_chunk = (Chunk_t *)address;
  pool_allocator.pool_first_address = (size_t)address;
  for (size_t i = 0; i < pool_allocator.chunk_number; i++) {
    address = &mem_pool[i * PA_MEM_CHUNK_SIZE];
    chunk = (Chunk_t *)address;
    if (i == pool_allocator.chunk_number - 1) {
      chunk->next_chunk = NULL;
      pool_allocator.last_free_chunk = chunk;
      pool_allocator.pool_last_address = (size_t)address;
      break;
    }
    address = &mem_pool[(i + 1) * PA_MEM_CHUNK_SIZE];
    chunk->next_chunk = (Chunk_t *)address;
  }
  pool_allocator.allocated_chunk = 0;
  pool_allocator.init_status = PA_INIT_SUCCEEDED;
}

void * pa_allocate(void) {
  if (pool_allocator.init_status != PA_INIT_SUCCEEDED) {
    return NULL;
  }
  if (pool_allocator.allocated_chunk == pool_allocator.chunk_number) {
    return NULL;
  }
  void * address = pool_allocator.first_free_chunk;
  if (pool_allocator.first_free_chunk->next_chunk != NULL) {
    pool_allocator.first_free_chunk = pool_allocator.first_free_chunk->next_chunk;
  }
  ++pool_allocator.allocated_chunk;
  return address;
}

pa_return_status_t pa_deallocate(void * ptr) {
  if (pool_allocator.init_status != PA_INIT_SUCCEEDED) {
    return PA_INIT_FAILED;
  }
  Chunk_t * chunk = NULL;
  size_t address = 0;
  size_t index = 0;
  address = (size_t)ptr;
  if (address < pool_allocator.pool_first_address && address > pool_allocator.pool_last_address) {
    return PA_INCORRECT_MEM;
  }
  index = address - pool_allocator.pool_first_address;
  if (index % PA_MEM_CHUNK_SIZE != 0) {
    return PA_INCORRECT_MEM;
  }
  chunk = (Chunk_t *)ptr;
  chunk->next_chunk = NULL;
  pool_allocator.last_free_chunk->next_chunk = chunk;
  pool_allocator.last_free_chunk = chunk;
  if (pool_allocator.allocated_chunk == pool_allocator.chunk_number) {
    pool_allocator.first_free_chunk = pool_allocator.last_free_chunk;
  }
  --pool_allocator.allocated_chunk;
  return PA_MEM_DEALLOCATED;
}
