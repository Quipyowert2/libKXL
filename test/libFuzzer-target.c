#include <KXL.h>
#include <stdlib.h>
#include <stdint.h>

static FILE* temp = NULL;
static char* tempname = NULL;

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (!temp) {
        char Template[] = "/tmp/XXXXXX.bmp";
        tempname = mktemp(Template);
        temp = fopen(tempname, "w+");
    }

    /* First byte is the blend value */
    uint8_t blend = Data[0];

    fwrite(Data+1, Size-1, 1, temp);
    KXL_Image* img = KXL_LoadBitmap(tempname, blend);
    KXL_DeleteImage(img);
    return 0;
}