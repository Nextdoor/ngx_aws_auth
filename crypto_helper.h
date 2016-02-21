#ifndef __NGX_AWS_AUTH__CRYPTO_HELPER__
#define __NGX_AWS_AUTH__CRYPTO_HELPER__


#include <ngx_core.h>
#include <ngx_palloc.h>


ngx_str_t* hash_sha256(ngx_pool_t *pool, ngx_str_t *blob);
ngx_str_t* sign_sha256(ngx_pool_t *pool, ngx_str_t *blob, ngx_str_t *signing_key);

#endif
