
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SHMTX_H_INCLUDED_
#define _NGX_SHMTX_H_INCLUDED_


#include <ngx_core.h>

#define ngx_shmtx_lock(x)   { /*void*/ }
#define ngx_shmtx_unlock(x) { /*void*/ }

typedef struct {
    ngx_uint_t   lock;
} ngx_shmtx_sh_t;


typedef struct {
    ngx_uint_t     spin;
} ngx_shmtx_t;


#endif /* _NGX_SHMTX_H_INCLUDED_ */
