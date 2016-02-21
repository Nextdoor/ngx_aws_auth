#include <time.h>
#include <ngx_times.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define AMZ_DATE_MAX_LEN 20

static ngx_str_t* ngx_aws_auth__compute_request_time(ngx_pool_t *pool, const time_t *timep) {
	ngx_str_t *const retval = ngx_palloc(pool, sizeof(ngx_str_t));
	retval->data = ngx_palloc(pool, AMZ_DATE_MAX_LEN);
	struct tm *tm_p = ngx_palloc(pool, sizeof(struct tm));
	gmtime_r(timep, tm_p);
	retval->len = strftime(retval->data, AMZ_DATE_MAX_LEN - 1, "%Y%m%dT%H%M%SZ", tm_p);
	return retval;
}

static void ngx_aws_auth__canonize_query_string(ngx_pool_t *pool,
	ngx_http_request_t *req) {
}


static ngx_str_t* ngx_aws_auth__host_from_bucket(ngx_pool_t *pool, ngx_str_t *s3_bucket) {
	static const char HOST_PATTERN[] = ".s3.amazonaws.com";
	ngx_str_t *host;

	host = ngx_palloc(pool, sizeof(ngx_str_t));
	host->len = s3_bucket->len + sizeof(HOST_PATTERN) + 1;
	host->data = ngx_palloc(pool, host->len);
	ngx_snprintf(host->data, host->len, "%v%s", s3_bucket, HOST_PATTERN);
	host->len = strnlen(host->data, host->len);

	return host;
}

static ngx_str_t* ngx_aws_auth__canonize_headers(ngx_pool_t *pool,
	ngx_http_request_t *req,
	ngx_str_t *s3_bucket, ngx_str_t *date, ngx_str_t *content_hash) {
	static const char HOST_PATTERN[] = ".s3.amazonaws.com";

	ngx_str_t *host = ngx_aws_auth__host_from_bucket(pool, s3_bucket);
}

static void ngx_aws_auth__make_canonical_request(ngx_pool_t *pool,
	ngx_http_request_t *req,
	ngx_str_t *s3_bucket_name) {
	// canonize query string
	// compute request body hash
	// canonize headers
}

static void ngx_aws_auth__string_to_sign(ngx_pool_t *pool,
	ngx_http_request_t *req) {
	// get canonical request
	// hash the request string
	// form the string to sign
}

static void ngx_aws_auth__sign(ngx_pool_t *pool, ngx_http_request_t *req) {
	// TODO:
	// get string to sign
	// generate signature
	// constuct auth header
	// attach auth header to outbound response
}
