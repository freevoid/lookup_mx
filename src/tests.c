#include "CUnit/Basic.h"
#include "log.h"

#define register_suite(suite_symbol) {\
	extern CU_SuiteInfo suite_symbol[];\
    if (CU_register_suites(suite_symbol) != CUE_SUCCESS) {\
		goto clean;\
	};\
}

int main()
{
	int retval = 0;
	CU_pSuite suite = NULL;

	log_configure(LOG_DEBUG, stderr);

	if (CUE_SUCCESS != CU_initialize_registry())
		goto exit;

	// Generating test suite
	register_suite(test_mx_suite);

	// Run all tests using the CUnit Basic interface
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	retval = CU_get_number_of_tests_failed();
clean:
	CU_cleanup_registry();
exit:
	return retval || CU_get_error();
}
