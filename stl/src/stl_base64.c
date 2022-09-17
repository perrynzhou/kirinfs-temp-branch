/*************************************************************************
  > File Name: stl_base64.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 一  9/ 5 18:03:19 2022
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>

static const char _base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int8_t b64lookup[128] =  {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
};

int stl_base64_encode(const char *src, int len, char *dst)
{
  char *p;
  const char *s,
      *end = src + len;
  int pos = 2;
  uint32_t buf = 0;

  s = src;
  p = dst;

  while (s < end)
  {
    buf |= (unsigned char)*s << (pos << 3);
    pos--;
    s++;

    /* write it out */
    if (pos < 0)
    {
      *p++ = _base64[(buf >> 18) & 0x3f];
      *p++ = _base64[(buf >> 12) & 0x3f];
      *p++ = _base64[(buf >> 6) & 0x3f];
      *p++ = _base64[buf & 0x3f];

      pos = 2;
      buf = 0;
    }
  }
  if (pos != 2)
  {
    *p++ = _base64[(buf >> 18) & 0x3f];
    *p++ = _base64[(buf >> 12) & 0x3f];
    *p++ = (pos == 0) ? _base64[(buf >> 6) & 0x3f] : '=';
    *p++ = '=';
  }

  return p - dst;
}

int stl_base64_decode(const char *src, int len, char *dst)
{
  const char *srcend = src + len,
             *s = src;
  char *p = dst;
  char c;
  int b = 0;
  uint32_t buf = 0;
  int pos = 0, end = 0;

  while (s < srcend)
  {
    c = *s++;

    /* Leave if a whitespace is found */
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
      return -1;

    if (c == '=')
    {
      /* end sequence */
      if (!end)
      {
        if (pos == 2)
        {
          end = 1;
        }
        else if (pos == 3)
        {
          end = 2;
        }
        else
        {
          /*
           * Unexpected "=" character found while decoding base64
           * sequence.
           */
          return -1;
        }
      }
      b = 0;
    }
    else
    {
      b = -1;
      if (c > 0 && c < 127)
      {
        b = b64lookup[(unsigned char)c];
      }
      if (b < 0)
      {
        /* invalid symbol found */
        return -1;
      }
    }
    /* add it to buffer */
    buf = (buf << 6) + b;
    pos++;
    if (pos == 4)
    {
      *p++ = (buf >> 16) & 255;
      if (end == 0 || end > 1)
      {
        *p++ = (buf >> 8) & 255;
      }
      if (end == 0 || end > 2)
      {
        *p++ = buf & 255;
      }
      buf = 0;
      pos = 0;
    }
  }

  if (pos != 0)
  {
    return -1;
  }

  return p - dst;
}

int stl_base64_enc_len(int srclen)
{
  return (srclen + 2) * 4 / 3;
}

int stl_base64_dec_len(int srclen)
{
  return (srclen * 3) >> 2;
}
