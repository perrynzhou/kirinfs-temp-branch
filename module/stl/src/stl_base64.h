/*************************************************************************
    > File Name: stl_base64.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: 一  9/ 5 18:03:13 2022
 ************************************************************************/

#ifndef _STL_BASE64_H
#define _STL_BASE64_H
int	stl_base64_encode(const char *src, int len, char *dst);
int	stl_base64_decode(const char *src, int len, char *dst);
int	stl_base64_enc_len(int srclen);
int	stl_base64_dec_len(int srclen);
#endif
