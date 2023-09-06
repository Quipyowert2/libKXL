#ifdef __cplusplus
extern "C" {
#endif
#include <KXL.h>
#ifdef __cplusplus
}
#endif
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

static FILE* temp = NULL;
static char* tempname = NULL;


void fuzzCleanup(void);

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    /* If this is the first call of this function, create temp file. */
    if (!temp) {
        char Template[] = "test_bmp_XXXXXX";
        tempname = mktemp(Template);
        if (!tempname)
            perror("tempname is NULL");
        temp = fopen(tempname, "w+");
        if (!temp) {
            perror("temp is NULL");
            printf("tempname=%s\n", tempname);
        }
        KXL_CreateWindow(800,600,"fuzz test",KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);
        atexit(fuzzCleanup);
    }

    assert(temp);

    if (Size == 0)
        return 0;

    /* First byte is the blend value */
    uint8_t blend = Data[0];

    /* Write to the file */
    truncate(tempname, Size);
    rewind(temp);
    fwrite(Data+1, Size-1, 1, temp);
    fflush(temp);

    /* Call function under test */
    KXL_Image* img = KXL_LoadBitmap(tempname, blend);

    /* Cleanup so we don't leak memory */
    KXL_DeleteImage(img);
    return 0;
}
void fuzzCleanup(void) {
    KXL_DeleteWindow();
    fclose(temp);
    unlink(tempname);
}