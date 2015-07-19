#include <ngx_core.h>
#include <ngx_http.h>

static void ngx_aws_auth_x_amz_date_value();

static void ngx_aws_auth_auth_header_value(ngx_str_t *access_key, ngx_str_t *key_date,
	ngx_str_t *region, ngx_str_t *service);

static ngx_str_t * ngx_aws_auth_canonical_request(ngx_http_request_t *r);
static ngx_str_t * ngx_aws_auth_canonical_headers(ngx_http_request_t *r);
static ngx_str_t * ngx_aws_auth_canonical_query_string(ngx_http_request_t *r);
