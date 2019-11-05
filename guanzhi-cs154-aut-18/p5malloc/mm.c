#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size_t)(size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define SIZE_PTR(p)  ((size_t*)(((char*)(p)) - SIZE_T_SIZE))

#define WORDSIZE    8      
#define DOUBSIZE    16     
#define CHUNKSIZE   (1<<12) 
#define OVERHEAD    16    

#define MAX(x, y) ((x) > (y)? (x) : (y))  

#define PACK(size, alloc)  ((size) | (alloc))

#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))  

#define GET_SIZE(p)  (GET(p) & ~(0xF))
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDR(p)       ((char *)(p) - WORDSIZE)  
#define FTR(p)       ((char *)(p) + GET_SIZE(HDR(p)) - DOUBSIZE)

#define NEXT_BLK(p)  ((char *)(p) + GET_SIZE(((char *)(p) - WORDSIZE)))
#define PREV_BLK(p)  ((char *)(p) - GET_SIZE(((char *)(p) - DOUBSIZE)))

#define NEXT(p)  (*(char**)(p + WORDSIZE))
#define PREV(p)  (*(char**)p)
/* $end mallocmacros */

/* Global variables */
static char *heap_listp = 0;  
static char *free_listp = 0;  

/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *p, size_t nsize);
static void *find_fit(size_t nsize);
static void *coalesce(void *p);
static void push(void *p);
static void pop(void *p);

static void *extend_heap(size_t words)
{
	char *p; 
	size_t size;
	void *ret_p;  
	if (words%2 == 0)
	{
		size = words * WORDSIZE; 
	}
	else {size = (words+1)*WORDSIZE; }
	if (size < CHUNKSIZE)
	{	size = CHUNKSIZE;}
	p = mem_sbrk(size); 
	if ((long) p < 0)
	{
		return NULL; 
	}
	PUT(HDR(p), PACK(size, 0));
	PUT(FTR(p), PACK(size, 0));
	PUT(HDR(NEXT_BLK(p)), PACK(0,1));
	ret_p = coalesce(p);
	return ret_p;
}

static void place(void *p, size_t nsize)
{
	size_t csize = GET_SIZE(HDR(p));
	size_t rest = csize - nsize; 
	if (rest >= 32)
	{
		PUT(HDR(p), PACK(nsize, 1));
		PUT(FTR(p), PACK(nsize, 1));
		pop(p);
		p = NEXT_BLK(p); 
		PUT(HDR(p), PACK(rest, 0));
        PUT(FTR(p), PACK(rest, 0));
		push(p);
	} 	
	else
	{
		PUT(HDR(p), PACK(csize, 1)); 
		PUT(FTR(p), PACK(csize, 1)); 
		pop(p); 
	}
}

static void *find_fit(size_t nsize)
{
	for (void *p = free_listp; GET_ALLOC(HDR(p)) == 0 && GET_SIZE(HDR(p)) > 0; p = NEXT(p))
	{
		if (GET_SIZE(HDR(p)) >= nsize)
		{
			return p; 
		}
	}
	return NULL; 	
}

static void *coalesce(void *p)
{
	size_t next = GET_ALLOC(HDR(NEXT_BLK(p))); 
	size_t prev = GET_ALLOC(FTR(PREV_BLK(p))) || PREV_BLK(p) == p; 
	size_t size = GET_SIZE(HDR(p)); 

	if (next && prev)
	{
		push(p); 
		return p; 
	}
	else if (next && !prev)
	{
		size += GET_SIZE(HDR(PREV_BLK(p))); 
		p = PREV_BLK(p); 
		pop(p); 
		PUT(HDR(p), PACK(size, 0)); 
		PUT(FTR(p), PACK(size, 0)); 
	}
	else if(!next && prev)
	{
		size += GET_SIZE(HDR(NEXT_BLK(p))); 
		pop(NEXT_BLK(p)); 
		PUT(HDR(p), PACK(size,0)); 
		PUT(FTR(p), PACK(size,0)); 
	}
	else
	{
		size += GET_SIZE(HDR(PREV_BLK(p))) + GET_SIZE(HDR(NEXT_BLK(p))); 
		pop(PREV_BLK(p)); 
		pop(NEXT_BLK(p)); 	
		p = PREV_BLK(p); 
		PUT(HDR(p), PACK(size,0)); 
		PUT(FTR(p), PACK(size,0)); 
	}
	push(p); 
	return p; 
}

static void push(void *p)
{
	NEXT(p) = free_listp; 
	PREV(free_listp) = p; 
	PREV(p) = NULL; 
	free_listp = p; 
}

static void pop(void *p)
{
	if (PREV(p))
	{
		NEXT(PREV(p)) = NEXT(p); 
	}
	else
	{
		free_listp = NEXT(p); 
	}
	PREV(NEXT(p)) = PREV(p); 
}


/*
 * mm_init - Called when a new trace starts.
 */
int mm_init(void)
{
	heap_listp = mem_sbrk(64);
	if(heap_listp == NULL)
	{
		return -1; 
	}
	PUT(heap_listp, 0);                        
 	PUT(heap_listp+WORDSIZE, PACK(OVERHEAD, 1)); 
  	PUT(heap_listp+DOUBSIZE, PACK(OVERHEAD, 1));  
 	PUT(heap_listp+WORDSIZE+DOUBSIZE, PACK(0, 1)); 
	free_listp = heap_listp + DOUBSIZE;
  	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
  	if (extend_heap(CHUNKSIZE/WORDSIZE) == NULL)
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
	if (heap_listp == 0)
	{
		mm_init(); 
	}
	size_t newsize; 
	if (size <= 0)
	{
		return NULL; 
	}
	if (size <= DOUBSIZE)
	{
		newsize = DOUBSIZE + OVERHEAD; 
	}
	else
	{
		newsize = DOUBSIZE * ((size + OVERHEAD + DOUBSIZE - 1)/DOUBSIZE); 
	}
	//Search free list for a fit 
	char *p = find_fit(newsize); 
	if (p != NULL)
	{
		place(p, newsize); 
		return p; 
	}
	//If no fit found then extend memory and place the block 
	size_t ext_size = MAX(newsize, CHUNKSIZE); 
	p = extend_heap(ext_size/WORDSIZE); 
	if (p != NULL)
	{
		place(p, newsize); 
		return p; 
	}
	else
	{
		return NULL; 
	}
}

/*
 * free - We don't know how to free a block.  So we ignore this call.
 *      Computers have big memories; surely it won't be a problem.
 */
void mm_free(void *ptr)
{
	if(ptr == NULL) { return;}
	size_t s = GET_SIZE(HDR(ptr)); 
	if (heap_listp == 0)
	{ mm_init();}
	PUT(HDR(ptr), PACK(s,0)); 
	PUT(FTR(ptr), PACK(s, 0)); 
	coalesce(ptr); 
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
