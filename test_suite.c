#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "aws_functions.c"

ngx_pool_t *pool;

static void null_test_success(void **state) {
    (void) state; /* unused */
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
    };

	pool = ngx_create_pool(1000000, NULL);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
