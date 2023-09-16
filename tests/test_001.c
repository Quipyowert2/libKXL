#include <check.h>
#include <KXL.h>

static KXL_Window *KXL_Root;

START_TEST (test_CreateWindow)
{
    KXL_CreateWindow(100, 100, "CreateWindow test",
                   KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);

    ck_assert( KXL_Root->Display != NULL );
}
END_TEST

int main(void)
{
    return 0;
}