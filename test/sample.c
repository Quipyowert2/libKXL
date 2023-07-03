// sample.c
#include <KXL.h>

int main(int argc, char** argv)
{
  Bool flag = True;

  // Create a window by 100x100.
  // event receives only the bottom of a re-drawing event and key presss.
  // If return key is pushed, it will end.
  KXL_CreateWindow(100, 100, "sample",
                   KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);
  KXL_Image *theImage = KXL_LoadBitmap(argv[1], 0);

  while(flag != True ){
    // event loop
    while(KXL_CheckEvents()) {
      // which acquires a event
      switch(KXL_GetEvents()) {
      case KXL_EVENT_EXPOSE: // event of re-drawing
        // A frame is cleared
        KXL_ClearFrameImm(0, 0, 100, 100);
        // "sample" is drawn
        KXL_PutImage(theImage, 0, 0);
        // A frame is made to reflect in a window
        KXL_UpDateImm(0, 0, 100, 100);
        break;
      case KXL_EVENT_KEY_PRESS: // event of key press
        // If a return key was pushed, it will end
        if (KXL_GetKey() == KXL_KEY_Return)
          flag = True;
        break ;
      }
    }
  }
  KXL_DeleteImage(theImage);
  // A window is closed
  KXL_DeleteWindow();
  return 0;
}