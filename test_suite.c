#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "aws_functions.c"

ngx_pool_t *pool;

static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void host_header_ctor(void **state) {
	ngx_str_t bucket;
	ngx_str_t* host;

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
	ngx_str_t* date;

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

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(x_amz_date),
        cmocka_unit_test(host_header_ctor),
    };

	pool = ngx_create_pool(1000000, NULL);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
