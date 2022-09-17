/*************************************************************************
  > File Name: stl_slab.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 12 Sep 2022 12:47:44 AM EDT
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "stl_slab.h"

uintptr_t stl_pagesize;
uintptr_t stl_pagesize_shift;
uintptr_t stl_cacheline_size;

static uintptr_t stl_slab_max_size;
static uintptr_t stl_slab_exact_size;
static uintptr_t stl_slab_exact_shift;

#define STL_SLAB_PAGE_MASK 3
#define STL_SLAB_PAGE 0
#define STL_SLAB_BIG 1
#define STL_SLAB_EXACT 2
#define STL_SLAB_SMALL 3

#define STL_SLAB_PAGE_FREE 0
#define STL_SLAB_PAGE_BUSY 0xffffffffffffffff
#define STL_SLAB_PAGE_START 0x8000000000000000

#define STL_SLAB_SHIFT_MASK 0x000000000000000f
#define STL_SLAB_MAP_MASK 0xffffffff00000000
#define STL_SLAB_MAP_SHIFT 32

#define STL_SLAB_BUSY 0xffffffffffffffff

#define stl_slab_slots(pool) (stl_slab_page *)((unsigned char *)(pool->addr))

#define stl_align(d, a) (((d) + (a - 1)) & ~(a - 1))
#define stl_align_ptr(p, a) \
    (unsigned char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

#define stl_slab_pageype(page) ((page)->prev & STL_SLAB_PAGE_MASK)

#define stl_slab_page_prev(page) \
    (stl_slab_page *)((page)->prev & ~STL_SLAB_PAGE_MASK)

#define stl_slab_page_addr(pool, page) \
    ((((page) - (pool)->pages) << stl_pagesize_shift) + (uintptr_t)(pool)->start)
inline static int stl_slab_sizes_init(stl_slab *pool, size_t pool_size)
{
    if (pool != NULL && pool_size > 0)
    {

        char *addr = (char *)malloc(sizeof(char) * pool_size);
        pool->end = addr + pool_size;
        pool->min_shift = 3;
        pool->addr = addr;

        stl_pagesize = getpagesize();

        for (size_t n = stl_pagesize; n >>= 1; stl_pagesize_shift++)
        { /* void */
        }

        uintptr_t n;

        stl_slab_max_size = stl_pagesize / 2;
        stl_slab_exact_size = stl_pagesize / (8 * sizeof(uintptr_t));
        for (n = stl_slab_exact_size; n >>= 1; stl_slab_exact_shift++)
        {
            /* void */
        }
        return 0;
    }
    return -1;
}

static stl_slab_page *stl_slab_alloc_pages(stl_slab *pool, uintptr_t pages)
{

    stl_slab_page *page, *p;

    for (page = pool->free.next; page != &pool->free; page = page->next)
    {

        if (page->slab >= pages)
        {

            if (page->slab > pages)
            {
                page[page->slab - 1].prev = (uintptr_t)&page[pages];

                page[pages].slab = page->slab - pages;
                page[pages].next = page->next;
                page[pages].prev = page->prev;

                p = (stl_slab_page *)page->prev;
                p->next = &page[pages];
                page->next->prev = (uintptr_t)&page[pages];
            }
            else
            {
                p = (stl_slab_page *)page->prev;
                p->next = page->next;
                page->next->prev = page->prev;
            }

            page->slab = pages | STL_SLAB_PAGE_START;
            page->next = NULL;
            page->prev = STL_SLAB_PAGE;

            pool->pfree -= pages;

            if (--pages == 0)
            {
                return page;
            }

            for (p = page + 1; pages; pages--)
            {
                p->slab = STL_SLAB_PAGE_BUSY;
                p->next = NULL;
                p->prev = STL_SLAB_PAGE;
                p++;
            }

            return page;
        }
    }

    return NULL;
}

int stl_slab_init(stl_slab *pool, size_t pool_size)
{
    if (stl_slab_sizes_init(pool, pool_size) != 0)
    {
        return -1;
    }
    unsigned char *p;
    size_t size;
    intptr_t m;
    uintptr_t i, n, pages;
    stl_slab_page *slots, *page;

    pool->min_size = (size_t)1 << pool->min_shift;

    slots = stl_slab_slots(pool);

    p = (unsigned char *)slots;
    size = pool->end - (unsigned char *)pool->addr;

    memset(p, 0xA5, size);

    n = stl_pagesize_shift - pool->min_shift;
    for (i = 0; i < n; i++)
    {
        //  fprintf(stdout,"i=%d\n",i);
        /* only "next" is used in list head */
        slots[i].slab = 0;
        slots[i].next = &slots[i];
        slots[i].prev = 0;
    }

    p += n * sizeof(stl_slab_page);

    pool->stats = (stl_slab_stat *)p;
    memset(pool->stats, 0, n * sizeof(stl_slab_stat));

    p += n * sizeof(stl_slab_stat);

    size -= n * (sizeof(stl_slab_page) + sizeof(stl_slab_stat));

    pages = (uintptr_t)(size / (stl_pagesize + sizeof(stl_slab_page)));

    pool->pages = (stl_slab_page *)p;
    memset(pool->pages, 0, pages * sizeof(stl_slab_page));

    page = pool->pages;

    /* only "next" is used in list head */
    pool->free.slab = 0;
    pool->free.next = page;
    pool->free.prev = 0;

    page->slab = pages;
    page->next = &pool->free;
    page->prev = (uintptr_t)&pool->free;

    pool->start = stl_align_ptr(p + pages * sizeof(stl_slab_page), stl_pagesize);

    m = pages - (pool->end - pool->start) / stl_pagesize;
    if (m > 0)
    {
        pages -= m;
        page->slab = pages;
    }

    pool->last = pool->pages + pages;
    pool->pfree = pages;
    return 0;
}

void *stl_slab_alloc(stl_slab *pool, size_t size)
{
    size_t s;
    uintptr_t p, m, mask, *bitmap;
    uintptr_t i, n, slot, shift, map;
    stl_slab_page *page, *prev, *slots;

    if (size > stl_slab_max_size)
    {

        page = stl_slab_alloc_pages(pool, (size >> stl_pagesize_shift) + ((size % stl_pagesize) ? 1 : 0));
        if (page)
        {
            p = stl_slab_page_addr(pool, page);
        }
        else
        {
            p = 0;
        }

        goto done;
    }

    if (size > pool->min_size)
    {
        shift = 1;
        for (s = size - 1; s >>= 1; shift++)
        { /* void */
        }
        slot = shift - pool->min_shift;
    }
    else
    {
        shift = pool->min_shift;
        slot = 0;
    }

    pool->stats[slot].reqs++;

    slots = stl_slab_slots(pool);
    page = slots[slot].next;

    if (page->next != page)
    {

        if (shift < stl_slab_exact_shift)
        {

            bitmap = (uintptr_t *)stl_slab_page_addr(pool, page);

            map = (stl_pagesize >> shift) / (8 * sizeof(uintptr_t));

            for (n = 0; n < map; n++)
            {

                if (bitmap[n] != STL_SLAB_BUSY)
                {

                    for (m = 1, i = 0; m; m <<= 1, i++)
                    {
                        if (bitmap[n] & m)
                        {
                            continue;
                        }

                        bitmap[n] |= m;

                        i = (n * 8 * sizeof(uintptr_t) + i) << shift;

                        p = (uintptr_t)bitmap + i;

                        pool->stats[slot].used++;

                        if (bitmap[n] == STL_SLAB_BUSY)
                        {
                            for (n = n + 1; n < map; n++)
                            {
                                if (bitmap[n] != STL_SLAB_BUSY)
                                {
                                    goto done;
                                }
                            }

                            prev = stl_slab_page_prev(page);
                            prev->next = page->next;
                            page->next->prev = page->prev;

                            page->next = NULL;
                            page->prev = STL_SLAB_SMALL;
                        }

                        goto done;
                    }
                }
            }
        }
        else if (shift == stl_slab_exact_shift)
        {

            for (m = 1, i = 0; m; m <<= 1, i++)
            {
                if (page->slab & m)
                {
                    continue;
                }

                page->slab |= m;

                if (page->slab == STL_SLAB_BUSY)
                {
                    prev = stl_slab_page_prev(page);
                    prev->next = page->next;
                    page->next->prev = page->prev;

                    page->next = NULL;
                    page->prev = STL_SLAB_EXACT;
                }

                p = stl_slab_page_addr(pool, page) + (i << shift);

                pool->stats[slot].used++;

                goto done;
            }
        }
        else
        { /* shift > stl_slab_exact_shift */

            mask = ((uintptr_t)1 << (stl_pagesize >> shift)) - 1;
            mask <<= STL_SLAB_MAP_SHIFT;

            for (m = (uintptr_t)1 << STL_SLAB_MAP_SHIFT, i = 0;
                 m & mask;
                 m <<= 1, i++)
            {
                if (page->slab & m)
                {
                    continue;
                }

                page->slab |= m;

                if ((page->slab & STL_SLAB_MAP_MASK) == mask)
                {
                    prev = stl_slab_page_prev(page);
                    prev->next = page->next;
                    page->next->prev = page->prev;

                    page->next = NULL;
                    page->prev = STL_SLAB_BIG;
                }

                p = stl_slab_page_addr(pool, page) + (i << shift);

                pool->stats[slot].used++;

                goto done;
            }
        }
    }

    page = stl_slab_alloc_pages(pool, 1);

    if (page)
    {
        if (shift < stl_slab_exact_shift)
        {
            bitmap = (uintptr_t *)stl_slab_page_addr(pool, page);

            n = (stl_pagesize >> shift) / ((1 << shift) * 8);

            if (n == 0)
            {
                n = 1;
            }

            /* "n" elements for bitmap, plus one requested */

            for (i = 0; i < (n + 1) / (8 * sizeof(uintptr_t)); i++)
            {
                bitmap[i] = STL_SLAB_BUSY;
            }

            m = ((uintptr_t)1 << ((n + 1) % (8 * sizeof(uintptr_t)))) - 1;
            bitmap[i] = m;

            map = (stl_pagesize >> shift) / (8 * sizeof(uintptr_t));

            for (i = i + 1; i < map; i++)
            {
                bitmap[i] = 0;
            }

            page->slab = shift;
            page->next = &slots[slot];
            page->prev = (uintptr_t)&slots[slot] | STL_SLAB_SMALL;

            slots[slot].next = page;

            pool->stats[slot].total += (stl_pagesize >> shift) - n;

            p = stl_slab_page_addr(pool, page) + (n << shift);

            pool->stats[slot].used++;

            goto done;
        }
        else if (shift == stl_slab_exact_shift)
        {

            page->slab = 1;
            page->next = &slots[slot];
            page->prev = (uintptr_t)&slots[slot] | STL_SLAB_EXACT;

            slots[slot].next = page;

            pool->stats[slot].total += 8 * sizeof(uintptr_t);

            p = stl_slab_page_addr(pool, page);

            pool->stats[slot].used++;

            goto done;
        }
        else
        { /* shift > stl_slab_exact_shift */

            page->slab = ((uintptr_t)1 << STL_SLAB_MAP_SHIFT) | shift;
            page->next = &slots[slot];
            page->prev = (uintptr_t)&slots[slot] | STL_SLAB_BIG;

            slots[slot].next = page;

            pool->stats[slot].total += stl_pagesize >> shift;

            p = stl_slab_page_addr(pool, page);

            pool->stats[slot].used++;

            goto done;
        }
    }

    p = 0;

    pool->stats[slot].fails++;
done:
    return (void *)p;
}

static void stl_slab_free_pages(stl_slab *pool, stl_slab_page *page, uintptr_t pages)
{
    stl_slab_page *prev, *join;

    pool->pfree += pages;

    page->slab = pages--;

    if (pages)
    {
        memset(&page[1], 0, pages * sizeof(stl_slab_page));
    }

    if (page->next)
    {
        prev = stl_slab_page_prev(page);
        prev->next = page->next;
        page->next->prev = page->prev;
    }

    join = page + page->slab;

    if (join < pool->last)
    {

        if (stl_slab_pageype(join) == STL_SLAB_PAGE)
        {

            if (join->next != NULL)
            {
                pages += join->slab;
                page->slab += join->slab;

                prev = stl_slab_page_prev(join);
                prev->next = join->next;
                join->next->prev = join->prev;

                join->slab = STL_SLAB_PAGE_FREE;
                join->next = NULL;
                join->prev = STL_SLAB_PAGE;
            }
        }
    }

    if (page > pool->pages)
    {
        join = page - 1;

        if (stl_slab_pageype(join) == STL_SLAB_PAGE)
        {

            if (join->slab == STL_SLAB_PAGE_FREE)
            {
                join = stl_slab_page_prev(join);
            }

            if (join->next != NULL)
            {
                pages += join->slab;
                join->slab += page->slab;

                prev = stl_slab_page_prev(join);
                prev->next = join->next;
                join->next->prev = join->prev;

                page->slab = STL_SLAB_PAGE_FREE;
                page->next = NULL;
                page->prev = STL_SLAB_PAGE;

                page = join;
            }
        }
    }

    if (pages)
    {
        page[pages].prev = (uintptr_t)page;
    }

    page->prev = (uintptr_t)&pool->free;
    page->next = pool->free.next;

    page->next->prev = (uintptr_t)page;

    pool->free.next = page;
}

void stl_slab_free(stl_slab *pool, void *p)
{
    size_t size;
    uintptr_t slab, m, *bitmap;
    uintptr_t i, n, type, slot, shift, map;
    stl_slab_page *slots, *page;

    if ((unsigned char *)p < pool->start || (unsigned char *)p > pool->end)
    {
        goto fail;
    }

    n = ((unsigned char *)p - pool->start) >> stl_pagesize_shift;
    page = &pool->pages[n];
    slab = page->slab;
    type = stl_slab_pageype(page);

    switch (type)
    {

    case STL_SLAB_SMALL:

        shift = slab & STL_SLAB_SHIFT_MASK;
        size = (size_t)1 << shift;

        if ((uintptr_t)p & (size - 1))
        {
            goto wrong_chunk;
        }

        n = ((uintptr_t)p & (stl_pagesize - 1)) >> shift;
        m = (uintptr_t)1 << (n % (8 * sizeof(uintptr_t)));
        n /= 8 * sizeof(uintptr_t);
        bitmap = (uintptr_t *)((uintptr_t)p & ~((uintptr_t)stl_pagesize - 1));

        if (bitmap[n] & m)
        {
            slot = shift - pool->min_shift;

            if (page->next == NULL)
            {
                slots = stl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t)&slots[slot] | STL_SLAB_SMALL;
                page->next->prev = (uintptr_t)page | STL_SLAB_SMALL;
            }

            bitmap[n] &= ~m;

            n = (stl_pagesize >> shift) / ((1 << shift) * 8);

            if (n == 0)
            {
                n = 1;
            }

            i = n / (8 * sizeof(uintptr_t));
            m = ((uintptr_t)1 << (n % (8 * sizeof(uintptr_t)))) - 1;

            if (bitmap[i] & ~m)
            {
                goto done;
            }

            map = (stl_pagesize >> shift) / (8 * sizeof(uintptr_t));

            for (i = i + 1; i < map; i++)
            {
                if (bitmap[i])
                {
                    goto done;
                }
            }

            stl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= (stl_pagesize >> shift) - n;

            goto done;
        }

        goto chunk_already_free;

    case STL_SLAB_EXACT:

        m = (uintptr_t)1 << (((uintptr_t)p & (stl_pagesize - 1)) >> stl_slab_exact_shift);
        size = stl_slab_exact_size;

        if ((uintptr_t)p & (size - 1))
        {
            goto wrong_chunk;
        }

        if (slab & m)
        {
            slot = stl_slab_exact_shift - pool->min_shift;

            if (slab == STL_SLAB_BUSY)
            {
                slots = stl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t)&slots[slot] | STL_SLAB_EXACT;
                page->next->prev = (uintptr_t)page | STL_SLAB_EXACT;
            }

            page->slab &= ~m;

            if (page->slab)
            {
                goto done;
            }

            stl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= 8 * sizeof(uintptr_t);

            goto done;
        }

        goto chunk_already_free;

    case STL_SLAB_BIG:

        shift = slab & STL_SLAB_SHIFT_MASK;
        size = (size_t)1 << shift;

        if ((uintptr_t)p & (size - 1))
        {
            goto wrong_chunk;
        }

        m = (uintptr_t)1 << ((((uintptr_t)p & (stl_pagesize - 1)) >> shift) + STL_SLAB_MAP_SHIFT);

        if (slab & m)
        {
            slot = shift - pool->min_shift;

            if (page->next == NULL)
            {
                slots = stl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t)&slots[slot] | STL_SLAB_BIG;
                page->next->prev = (uintptr_t)page | STL_SLAB_BIG;
            }

            page->slab &= ~m;

            if (page->slab & STL_SLAB_MAP_MASK)
            {
                goto done;
            }

            stl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= stl_pagesize >> shift;

            goto done;
        }

        goto chunk_already_free;

    case STL_SLAB_PAGE:

        if ((uintptr_t)p & (stl_pagesize - 1))
        {
            goto wrong_chunk;
        }

        if (!(slab & STL_SLAB_PAGE_START))
        {

            goto fail;
        }

        if (slab == STL_SLAB_PAGE_BUSY)
        {

            goto fail;
        }

        size = slab & ~STL_SLAB_PAGE_START;

        stl_slab_free_pages(pool, page, size);
        memset(p, 0, size << stl_pagesize_shift);

        return;
    }

    return;

done:

    pool->stats[slot].used--;

    memset(p, 0, size);

    return;

wrong_chunk:
    goto fail;

chunk_already_free:

fail:

    return;
}
