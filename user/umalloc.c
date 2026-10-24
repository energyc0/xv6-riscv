#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "kernel/spinlock.h"
#include "kernel/proc.h"
#include "user/user.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

/*
typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  if (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if (p == SBRK_ERROR)
    return 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}
*/

#define BLOCK_SIZE     (PGSIZE)
#define MIN_ALLOC_SIZE (16)
// 16, 32, 64, 128, 256, 512, 1024, 2048
#define BLOCK_TYPES_AMOUNT (8)

// Segregated Free List Header
// First variable contains objects size in the block
// The data block contains bitmap in the beginning
// The other data is for allocated objects
struct sfl_header {
  uint32 size;
  struct sfl_header *next;
};

// +1 for exceeding limit of max size
static struct sfl_header *blocks[BLOCK_TYPES_AMOUNT + 1];

static uint32
round_to_power_of_two(uint32 nbytes)
{
  if (nbytes < MIN_ALLOC_SIZE)
    return MIN_ALLOC_SIZE;
  // if the power of 2
  if ((nbytes & (nbytes - 1)) == nbytes)
    return nbytes;
  nbytes -= 1;
  nbytes |= nbytes >> 1;
  nbytes |= nbytes >> 2;
  nbytes |= nbytes >> 4;
  nbytes |= nbytes >> 8;
  nbytes |= nbytes >> 16;
  return nbytes + 1;
}
static uint32
find_block_type_idx(uint32 block_size)
{
  uint32 size = MIN_ALLOC_SIZE;
  for (uint i = 0; i < BLOCK_TYPES_AMOUNT; i++) {
    if (block_size <= size)
      return i;
    size <<= 1;
  }
  return BLOCK_TYPES_AMOUNT - 1;
}

// allocate sfl block for objects of the given size
// objs_size must be a power of two
static struct sfl_header *
alloc_block(uint32 objs_size)
{
  struct sfl_header *p = 0;
  if (objs_size >= BLOCK_SIZE) {
    const uint32 block_size =
      round_to_power_of_two(objs_size + sizeof(struct sfl_header));
    p = (void *)sbrk(block_size);
    printf("Current heap top: %p\n", (char *)p + block_size);
    if ((char *)p != SBRK_ERROR) {
      p->size = block_size - sizeof(struct sfl_header);
      p->next = 0;
    }
  } else {
    p = (void *)sbrk(BLOCK_SIZE);
    printf("Current heap top: %p\n", (char *)p + BLOCK_SIZE);
    if ((char *)p != SBRK_ERROR) {
      const uint32 block_size = objs_size + sizeof(struct sfl_header);
      const uint32 blocks_amount = BLOCK_SIZE / block_size;
      struct sfl_header *cur_p = p;
      for (uint32 i = 0; i < blocks_amount; i++) {
        cur_p->size = objs_size;
        if (i == blocks_amount - 1) {
          cur_p->next = 0;
        } else {
          struct sfl_header *next =
            (struct sfl_header *)((char *)cur_p + block_size);
          cur_p->next = next;
          cur_p = next;
        }
      }
    }
  }
  return p;
}

void
free(void *p)
{
  if (p == 0)
    return;
  struct sfl_header *header =
    (struct sfl_header *)((char *)p - sizeof(struct sfl_header));
  uint idx = find_block_type_idx(header->size);
  printf("Freeing block of size 0x%x\n", header->size);
  header->next = blocks[idx];
  blocks[idx] = header;
}

void *
malloc(uint32 nbytes)
{
  if (nbytes == 0)
    return 0;

  void *ret = 0;
  uint32 type_idx = find_block_type_idx(nbytes);
  if (type_idx < BLOCK_TYPES_AMOUNT) {
    if (blocks[type_idx] != 0) {
      struct sfl_header *base = blocks[type_idx];
      blocks[type_idx] = base->next;
      ret = (void *)((char *)base + sizeof(struct sfl_header));
    } else {
      uint32 obj_size = round_to_power_of_two(nbytes);
      struct sfl_header *base = (void *)alloc_block(obj_size);
      blocks[type_idx] = base->next;
      ret = (void *)((char *)base + sizeof(struct sfl_header));
    }
    // >2048 bytes
  } else {
    struct sfl_header *p = blocks[type_idx];
    if (p != 0 && p->size >= nbytes) {
      blocks[type_idx] = p->next;
      ret = (void *)((char *)p + sizeof(struct sfl_header));
    } else {
      for (; p->next != 0; p = p->next) {
        struct sfl_header *cur = p->next;
        if (cur->size >= nbytes) {
          p->next = cur->next;
          ret = (void *)((char *)cur + sizeof(struct sfl_header));
          break;
        };
      }
      if (ret == 0) {
        uint32 obj_size =
          round_to_power_of_two(sizeof(struct sfl_header) + nbytes);
        struct sfl_header *base = alloc_block(obj_size);
        base->next = blocks[type_idx];
        ret = (void *)((char *)base + sizeof(struct sfl_header));
      }
    }
  }
  printf("Allocated block at %p of size 0x%x for %u\n", ret,
         ((struct sfl_header *)((char *)ret - sizeof(struct sfl_header)))->size,
         nbytes);
  return ret;
}