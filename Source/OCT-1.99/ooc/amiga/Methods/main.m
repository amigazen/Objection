/*
 * main.m -- Test harness for Amiga-specific classes (MsgPort, EList, ENode, IdList).
 * Reports test name, expected outcome, actual outcome, and pass/fail.
 * Does not exit on first failure; runs every section and prints summary.
 * No interactive input; suitable for automated runs.
 *
 * Link with BASELIB (foundation) and OCSUBS (substrate); see smakefile.
 */

#include <octhead.h>
#include <apps/MsgPort.h>
#include <objc/EList.h>
#include <objc/ENode.h>
#include <apps/IdList.h>
@includes

#include <stdio.h>
#include <string.h>

static int tests_run;
static int tests_passed;

static void
ok(int pass, const char *test_name, const char *expected, const char *actual)
{
	tests_run++;
	if (pass)
		tests_passed++;
	printf("  %s: %s\n    expected: %s\n    actual:   %s\n    result:   %s\n\n",
		test_name, pass ? "PASS" : "FAIL", expected, actual, pass ? "PASS" : "FAIL");
}

static void
ok_int(int pass, const char *test_name, const char *expected, int actual_val)
{
	char buf[64];
	sprintf(buf, "%d", actual_val);
	ok(pass, test_name, expected, buf);
}

static void
ok_str(int pass, const char *test_name, const char *expected, const char *actual_val)
{
	ok(pass, test_name, expected, actual_val != (const char *)0 ? actual_val : "(null)");
}

/* --- MsgPort --- */
static void
run_msgport_tests(void)
{
	id port;
	const char *name;

	printf("--- MsgPort: new, str:, str, free ---\n");
	port = [MsgPort new];
	ok(port != nil, "MsgPort new", "non-nil", port == nil ? "nil" : "non-nil");
	if (port == nil)
		return;
	[port str: "amiga_test_port"];
	name = (const char *)[port str];
	ok_str(name != (const char *)0 && strcmp(name, "amiga_test_port") == 0,
		"MsgPort str: / str", "amiga_test_port", name != (const char *)0 ? name : "(null)");
	[port free];
}

/* --- EList and ENode --- */
static void
run_elist_enode_tests(void)
{
	id list;
	id n1, n2, n3;
	id first_elt, last_elt;
	id elt;
	int n;

	printf("--- EList / ENode: new, addHead:, addTail:, count, first, last, next, previous, remHead, remTail, free ---\n");
	list = [EList new];
	ok(list != nil, "EList new", "non-nil", list == nil ? "nil" : "non-nil");
	if (list == nil)
		return;
	n = [list count];
	ok_int(n == 0, "EList count empty", "0", n);

	n1 = [ENode new];
	n2 = [ENode new];
	n3 = [ENode new];
	ok(n1 != nil && n2 != nil && n3 != nil, "ENode new", "non-nil", (n1 != nil && n2 != nil && n3 != nil) ? "non-nil" : "nil");
	[list addHead: n2];
	[list addHead: n1];
	[list addTail: n3];
	n = [list count];
	ok_int(n == 3, "EList count after addHead/addTail", "3", n);

	first_elt = [list first];
	ok(first_elt != nil, "EList first", "non-nil", first_elt == nil ? "nil" : "non-nil");
	last_elt = [list last];
	ok(last_elt != nil, "EList last", "non-nil", last_elt == nil ? "nil" : "non-nil");
	elt = [first_elt next];
	ok(elt != nil, "ENode next", "non-nil", elt == nil ? "nil" : "non-nil");
	elt = [n3 previous];
	ok(elt != nil, "ENode previous", "non-nil", elt == nil ? "nil" : "non-nil");

	elt = [list remHead];
	ok(elt != nil, "EList remHead", "non-nil", elt == nil ? "nil" : "non-nil");
	[elt free];
	n = [list count];
	ok_int(n == 2, "EList count after remHead", "2", n);

	elt = [list remTail];
	ok(elt != nil, "EList remTail", "non-nil", elt == nil ? "nil" : "non-nil");
	[elt free];
	n = [list count];
	ok_int(n == 1, "EList count after remTail", "1", n);

	[list free];
}

/* --- IdList (List with toFirst) --- */
static void
run_idlist_tests(void)
{
	id lst;
	id obj;
	id first;
	int n;

	printf("--- IdList: new, addObject:, count, toFirst, first ---\n");
	lst = [IdList new];
	ok(lst != nil, "IdList new", "non-nil", lst == nil ? "nil" : "non-nil");
	if (lst == nil)
		return;
	obj = [Object new];
	[lst addObject: obj];
	n = [lst count];
	ok_int(n == 1, "IdList count after addObject:", "1", n);
	first = [lst toFirst];
	ok(first == obj, "IdList toFirst", "same as added", first == obj ? "same" : "different");
	[lst removeObject: obj];
	n = [lst count];
	ok_int(n == 0, "IdList count after removeObject:", "0", n);
	[lst free];
	[obj free];
}

static void
finish(void)
{
	printf("========================================\n");
	printf("Summary: %d passed, %d failed, %d total\n",
		tests_passed, tests_run - tests_passed, tests_run);
	printf("========================================\n");
}

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	setvbuf(stdout, (char *)0, _IONBF, 0);
	setvbuf(stderr, (char *)0, _IONBF, 0);
	tests_run = 0;
	tests_passed = 0;

	printf("========================================\n");
	printf("Amiga Methods test harness\n");
	printf("========================================\n\n");

	run_msgport_tests();
	run_elist_enode_tests();
	run_idlist_tests();

	finish();
	return tests_passed == tests_run ? 0 : 1;
}
