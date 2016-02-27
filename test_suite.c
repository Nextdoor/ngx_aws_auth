#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "aws_functions.h"

ngx_pool_t *pool;

static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void host_header_ctor(void **state) {
	ngx_str_t bucket;
	const ngx_str_t* host;

    (void) state; /* unused */

	bucket.data = "test-es-three";
	bucket.len = strlen(bucket.data);
	host = ngx_aws_auth__host_from_bucket(pool, &bucket);
	assert_string_equal("test-es-three.s3.amazonaws.com", host->data);

	bucket.data = "complex.sub.domain.test";
	bucket.len = strlen(bucket.data);
	host = ngx_aws_auth__host_from_bucket(pool, &bucket);
	assert_string_equal("complex.sub.domain.test.s3.amazonaws.com", host->data);
}

static void x_amz_date(void **state) {
	time_t t;
	const ngx_str_t* date;

    (void) state; /* unused */
	
	t = 1;
	date = ngx_aws_auth__compute_request_time(pool, &t);
	assert_int_equal(date->len, 16);
	assert_string_equal("19700101T000001Z", date->data);

	t = 1456036272;
	date = ngx_aws_auth__compute_request_time(pool, &t);
	assert_int_equal(date->len, 16);
	assert_string_equal("20160221T063112Z", date->data);
}


static void hmac_sha256(void **state) {
    ngx_str_t key;
    ngx_str_t text;
    ngx_str_t* hash;
    (void) state; /* unused */

    key.data = "abc"; key.len=3;
    text.data = "asdf"; text.len=4;
    hash = ngx_aws_auth__sign_sha256_hex(pool, &text, &key);
	assert_int_equal(64, hash->len);
	assert_string_equal("07e434c45d15994e620bf8e43da6f652d331989be1783cdfcc989ddb0a2358e2", hash->data);

    key.data = "\011\001\057asf"; key.len=6;
    text.data = "lorem ipsum"; text.len=11;
    hash = ngx_aws_auth__sign_sha256_hex(pool, &text, &key);
	assert_int_equal(64, hash->len);
	assert_string_equal("827ce31c45e77292af25fef980c3e7afde23abcde622ecd8e82e1be6dd94fad3", hash->data);
}


static void sha256(void **state) {
    ngx_str_t text;
    ngx_str_t* hash;
    (void) state; /* unused */

    text.data = "asdf"; text.len=4;
    hash = ngx_aws_auth__hash_sha256(pool, &text);
	assert_int_equal(64, hash->len);
	assert_string_equal("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b", hash->data);

    text.len=0;
    hash = ngx_aws_auth__hash_sha256(pool, &text);
	assert_int_equal(64, hash->len);
	assert_string_equal("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", hash->data);
}


int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(x_amz_date),
        cmocka_unit_test(host_header_ctor),
        cmocka_unit_test(hmac_sha256),
        cmocka_unit_test(sha256),
    };

	pool = ngx_create_pool(1000000, NULL);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
