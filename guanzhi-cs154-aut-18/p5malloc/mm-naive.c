/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  Blocks are never coalesced or reused.  The size of
 * a block is found at the first aligned word before the block (we need
 * it for realloc).
 *
 * This code is correct and blazingly fast, but very bad usage-wise since
 * it never frees anything.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define SIZE_PTR(p)  ((size_t*)(((char*)(p)) - SIZE_T_SIZE))

#define WSIZE       8      /* word size (bytes) */  
#define DSIZE       16      /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<12)  /* initial heap size (bytes) */
#define OVERHEAD    (2 * WSIZE)     /* overhead of header and footer (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
/* NB: this code calls a 32-bit quantity a word */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))  

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~(DSIZE-1))
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)  
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define NEXT(bp)  (*(char**)(bp + WSIZE))
#define PREV(bp)  (*(char**)bp)
/* $end mallocmacros */

/* Global variables */
static char *heap_listp = 0;  /* pointer to first block */  
static char *free_listp = NULL;  /* explicit free list pointer, to the first free block */

/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);
static void push(void *bp);
static void pop(void *bp);

/*
 * mm_init - Called when a new trace starts.
 */
int mm_init(void)
{
	heap_listp = mem_sbrk(4*DSIZE);
	if(heap_listp == NULL)
	{
		return -1; 
	}
	PUT(heap_listp, 0);                        /* alignment padding */
 	PUT(heap_listp+WSIZE, PACK(OVERHEAD, 1));  /* prologue header */ 
 	PUT(heap_listp+DSIZE, PACK(OVERHEAD, 1));  /* prologue footer */ 
 	PUT(heap_listp+WSIZE+DSIZE, PACK(0, 1));   /* epilogue header */
 	heap_listp += DSIZE;
	free_listp = heap_listp;
  	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
  	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
	{
    	return -1;
  	}
	return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
  	int newsize = MAX(ALIGN(size) + DSIZE; DSIZE*4);
	if (size <= 0)
	{
		return NULL; 
	}
	//Search free list for a fit 
	void *p; 
	if ((p = find_fit(newsize)) != NULL)
	{
		place(p, newsize); 
		return p; 
	}
	//If no fit found then extend memory and place the block 
	size_t ext_size = MAX(newsize, CHUNKSIZE); 
	p = extend_heap(ext_size/WSIZE); 
	if (p != NULL)
	{
		place(p, newsize); 
		return p; 
	}
	return NULL; 
  }
}

/*
 * free - We don't know how to free a block.  So we ignore this call.
 *      Computers have big memories; surely it won't be a problem.
 */
void mm_free(void *ptr)
{
}

/*
 * realloc - Change the size of the block by mallocing a new block,
 *      copying its data, and freeing the old block.  I'm too lazy
 *      to do better.
 */
void *mm_realloc(void *oldptr, size_t size)
{
  size_t oldsize;
  void *newptr;

  /* If size == 0 then this is just free, and we return NULL. */
  if(size == 0) {
    mm_free(oldptr);
    return 0;
  }

  /* If oldptr is NULL, then this is just malloc. */
  if(oldptr == NULL) {
    return mm_malloc(size);
  }

  newptr = mm_malloc(size);

  /* If realloc() fails the original block is left untouched  */
  if(!newptr) {
    return 0;
  }

  /* Copy the old data. */
  oldsize = *SIZE_PTR(oldptr);
  if(size < oldsize) oldsize = size;
  memcpy(newptr, oldptr, oldsize);

  /* Free the old block. */
  mm_free(oldptr);

  return newptr;
}

/*
 * calloc - Allocate the block and set it to zero.
 */
void *mm_calloc (size_t nmemb, size_t size)
{
  size_t bytes = nmemb * size;
  void *newptr;

  newptr = mm_malloc(bytes);
  memset(newptr, 0, bytes);

  return newptr;
}

/*
 * mm_checkheap - There are no bugs in my code, so I don't need to check,
 */
void mm_checkheap(int verbose)
{
}
