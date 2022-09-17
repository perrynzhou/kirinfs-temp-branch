/*************************************************************************
    > File Name: stl_sha1.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 12 Sep 2022 03:54:41 AM EDT
 ************************************************************************/

#include <stdio.h>
#include "stl_sha1.h"
static const unsigned char *stl_sha1_body(stl_sha1 *ctx, const unsigned char *data,
                                          size_t size);

int stl_sha1_init(stl_sha1 *ctx)
{
  if (ctx)
  {
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;
    ctx->e = 0xc3d2e1f0;

    ctx->bytes = 0;
    return 0;
  }
  return -1;
}

stl_sha1 *stl_sha1_alloc()
{
  stl_sha1 *sa1 = calloc(1, sizeof(stl_sha1));
  if (stl_sha1_init(sa1) != 0)
  {
    free(sa1);
    return NULL;
  }
  return sa1;
}

void stl_sha1_update(stl_sha1 *ctx, const void *data, size_t size)
{
  size_t used, free;

  used = (size_t)(ctx->bytes & 0x3f);
  ctx->bytes += size;

  if (used)
  {
    free = 64 - used;

    if (size < free)
    {
      ngx_memcpy(&ctx->buffer[used], data, size);
      return;
    }

    ngx_memcpy(&ctx->buffer[used], data, free);
    data = (unsigned char *)data + free;
    size -= free;
    (void)stl_sha1_body(ctx, ctx->buffer, 64);
  }

  if (size >= 64)
  {
    data = stl_sha1_body(ctx, data, size & ~(size_t)0x3f);
    size &= 0x3f;
  }

  ngx_memcpy(ctx->buffer, data, size);
}

void stl_sha1_final(unsigned char result[20], stl_sha1 *ctx)
{
  size_t used, free;

  used = (size_t)(ctx->bytes & 0x3f);

  ctx->buffer[used++] = 0x80;

  free = 64 - used;

  if (free < 8)
  {
    ngx_memzero(&ctx->buffer[used], free);
    (void)stl_sha1_body(ctx, ctx->buffer, 64);
    used = 0;
    free = 64;
  }

  ngx_memzero(&ctx->buffer[used], free - 8);

  ctx->bytes <<= 3;
  ctx->buffer[56] = (unsigned char)(ctx->bytes >> 56);
  ctx->buffer[57] = (unsigned char)(ctx->bytes >> 48);
  ctx->buffer[58] = (unsigned char)(ctx->bytes >> 40);
  ctx->buffer[59] = (unsigned char)(ctx->bytes >> 32);
  ctx->buffer[60] = (unsigned char)(ctx->bytes >> 24);
  ctx->buffer[61] = (unsigned char)(ctx->bytes >> 16);
  ctx->buffer[62] = (unsigned char)(ctx->bytes >> 8);
  ctx->buffer[63] = (unsigned char)ctx->bytes;

  (void)stl_sha1_body(ctx, ctx->buffer, 64);

  result[0] = (unsigned char)(ctx->a >> 24);
  result[1] = (unsigned char)(ctx->a >> 16);
  result[2] = (unsigned char)(ctx->a >> 8);
  result[3] = (unsigned char)ctx->a;
  result[4] = (unsigned char)(ctx->b >> 24);
  result[5] = (unsigned char)(ctx->b >> 16);
  result[6] = (unsigned char)(ctx->b >> 8);
  result[7] = (unsigned char)ctx->b;
  result[8] = (unsigned char)(ctx->c >> 24);
  result[9] = (unsigned char)(ctx->c >> 16);
  result[10] = (unsigned char)(ctx->c >> 8);
  result[11] = (unsigned char)ctx->c;
  result[12] = (unsigned char)(ctx->d >> 24);
  result[13] = (unsigned char)(ctx->d >> 16);
  result[14] = (unsigned char)(ctx->d >> 8);
  result[15] = (unsigned char)ctx->d;
  result[16] = (unsigned char)(ctx->e >> 24);
  result[17] = (unsigned char)(ctx->e >> 16);
  result[18] = (unsigned char)(ctx->e >> 8);
  result[19] = (unsigned char)ctx->e;

  ngx_memzero(ctx, sizeof(*ctx));
}

/*
 * Helper functions.
 */

#define ROTATE(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

#define F1(b, c, d) (((b) & (c)) | ((~(b)) & (d)))
#define F2(b, c, d) ((b) ^ (c) ^ (d))
#define F3(b, c, d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))

#define STEP(f, a, b, c, d, e, w, t)                          \
  temp = ROTATE(5, (a)) + f((b), (c), (d)) + (e) + (w) + (t); \
  (e) = (d);                                                  \
  (d) = (c);                                                  \
  (c) = ROTATE(30, (b));                                      \
  (b) = (a);                                                  \
  (a) = temp;

/*
 * GET() reads 4 input bytes in big-endian byte order and returns
 * them as uint32_t.
 */

#define GET(n)                      \
  ((uint32_t)p[n * 4 + 3] |         \
   ((uint32_t)p[n * 4 + 2] << 8) |  \
   ((uint32_t)p[n * 4 + 1] << 16) | \
   ((uint32_t)p[n * 4] << 24))

/*
 * This processes one or more 64-byte data blocks, but does not update
 * the bit counters.  There are no alignment requirements.
 */

static const unsigned char *
stl_sha1_body(stl_sha1 *ctx, const unsigned char *data, size_t size)
{
  uint32_t a, b, c, d, e, temp;
  uint32_t saved_a, saved_b, saved_c, saved_d, saved_e;
  uint32_t words[80];
  uintptr_t i;
  const unsigned char *p;

  p = data;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;
  e = ctx->e;

  do
  {
    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;
    saved_e = e;

    /* Load data block into the words array */

    for (i = 0; i < 16; i++)
    {
      words[i] = GET(i);
    }

    for (i = 16; i < 80; i++)
    {
      words[i] = ROTATE(1, words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16]);
    }

    /* Transformations */

    STEP(F1, a, b, c, d, e, words[0], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[1], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[2], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[3], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[4], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[5], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[6], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[7], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[8], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[9], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[10], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[11], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[12], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[13], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[14], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[15], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[16], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[17], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[18], 0x5a827999);
    STEP(F1, a, b, c, d, e, words[19], 0x5a827999);

    STEP(F2, a, b, c, d, e, words[20], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[21], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[22], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[23], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[24], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[25], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[26], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[27], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[28], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[29], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[30], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[31], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[32], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[33], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[34], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[35], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[36], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[37], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[38], 0x6ed9eba1);
    STEP(F2, a, b, c, d, e, words[39], 0x6ed9eba1);

    STEP(F3, a, b, c, d, e, words[40], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[41], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[42], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[43], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[44], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[45], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[46], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[47], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[48], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[49], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[50], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[51], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[52], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[53], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[54], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[55], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[56], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[57], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[58], 0x8f1bbcdc);
    STEP(F3, a, b, c, d, e, words[59], 0x8f1bbcdc);

    STEP(F2, a, b, c, d, e, words[60], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[61], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[62], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[63], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[64], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[65], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[66], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[67], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[68], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[69], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[70], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[71], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[72], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[73], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[74], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[75], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[76], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[77], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[78], 0xca62c1d6);
    STEP(F2, a, b, c, d, e, words[79], 0xca62c1d6);

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;
    e += saved_e;

    p += 64;

  } while (size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;
  ctx->e = e;

  return p;
}
int  stl_sha1_deinit(stl_sha1 *ctx)
{
  if(ctx) {
     memset(ctx,0,sizeof(stl_sha1));
     return 0;
  }
  return -1;
}
void  stl_sha1_destroyt(stl_sha1 *ctx) {
  if(ctx) {
     free(ctx);
     ctx = NULL;
  }
}