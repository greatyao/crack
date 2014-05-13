#ifndef PRIVATE_H
#define PRIVATE_H

#ifdef  __cplusplus
extern "C" {
#endif

unsigned char *SHA1(const unsigned char *d, size_t n, unsigned char *md);
int lws_b64_encode_string(const char *in, int in_len, char *out, int out_size);

#ifdef  __cplusplus
}
#endif

#endif