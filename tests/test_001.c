#include <stdlib.h>
#include <check.h>
#include <KXL.h>

extern KXL_Window *KXL_Root;

START_TEST (test_CreateWindow)
{
    KXL_CreateWindow(100, 100, "CreateWindow test",
                   KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);

    ck_assert( KXL_Root->Display != NULL );
    KXL_DeleteWindow();
}
END_TEST

START_TEST(test_LoadBitmap)
{
    KXL_CreateWindow(100, 100, "CreateWindow test",
                   KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);

    KXL_Image *bmp = KXL_LoadBitmap("../../geki2/bmp/boss1.bmp", 0);
    ck_assert(bmp != NULL);
    KXL_DeleteImage(bmp);
    KXL_DeleteWindow();
}
END_TEST

Suite * KXL_suite(void)
{
    Suite *s;
    TCase *KXL_core;

    s = suite_create("KXL tests");
    KXL_core = tcase_create("Core tests");

    tcase_add_test(KXL_core, test_CreateWindow);
    tcase_add_test(KXL_core, test_LoadBitmap);

    suite_add_tcase(s, KXL_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = KXL_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}