#include "CUnit/Basic.h"
#include "mx.h"

static void test_lookup_success()
{
	struct MXRecord rec;

	CU_ASSERT_EQUAL(resolve_mx("local.iu7.bmstu.ru", &rec), 0);
	CU_ASSERT_EQUAL(rec.priority, 10);
	CU_ASSERT_STRING_EQUAL(rec.name, "local.iu7.bmstu.ru");
}

static void test_lookup_nonex()
{
	struct MXRecord rec;
	CU_ASSERT_NOT_EQUAL(resolve_mx("localhost", &rec), 0);
	CU_ASSERT_NOT_EQUAL(resolve_mx("notexisting.iu7.bmstu.ru", &rec), 0);
}

static void test_lookup_nomx()
{
	struct MXRecord rec;
	CU_ASSERT_NOT_EQUAL(resolve_mx("example.com", &rec), 0);
}

static void test_pick_mx()
{
	struct MXRecord records[] = {
		{30, "mx1.example.com"},
		{10, "mx2.example.com"},
		{1, "mx3.example.com"},
		{20, "mx4.example.com"},
		{1, "mx5.example.com"},
		{10, "mx6.example.com"},
		{20, "mx7.example.com"},
		{1, "mx8.example.com"},
	};

	int i, is_undet = 0;
	struct MXRecord *r1, *r2;

	r1 = pick_mx(records, 8);
	CU_ASSERT_EQUAL(r1->priority, 1);

	for (i=0; i<10; i++) {
		r2 = pick_mx(records, 8);
		if (r1 != r2) {
			is_undet = 1;
			break;
		}
	}

	CU_ASSERT_TRUE(is_undet);
}

static int init_suite(void)
{
	// nothing to do
	return 0;
}

static int clean_suite(void)
{
	// nothing to do
	return 0;
}

static CU_TestInfo tests[] = {
	{"Lookup for a local.iu7.bmstu.ru MX record", test_lookup_success},
	{"Lookup for a non-existent domain", test_lookup_nonex},
	{"Lookup for a domain without MX record", test_lookup_nomx},
	{"Test that pick_mx selects the record with highest priority", test_pick_mx},
	CU_TEST_INFO_NULL
};

CU_SuiteInfo test_mx_suite[] = {
	{
		"MX Lookup tests",
		init_suite,
		clean_suite,
		tests
	},
	CU_SUITE_INFO_NULL
};
