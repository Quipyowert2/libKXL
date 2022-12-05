// sample.c
#include < KXL.h>

int main(void)
{
  Bool flag = False ;

  // Create a window by 100x100.
  // event receives only the bottom of a re-drawing event and key presss.
  // If return key is pushed, it will end.
  KXL_CreateWindow(100, 100, "sample"
                   KXL_EVENT_EXPOSURE_MASK |
                   KXL_EVENT_KEY_PRESS_MASK);

  while(flag != True ){
    // event loop
    while(KXL_CheckEvents()) {
      // which acquires a event
      switch(KXL_GetEvents()) {
      case KXL_EVENT_EXPOSE: // event of re-drawing
        // A frame is cleared
        KXL_ClearFrameImm(0, 0, 100, 100);
        // "sample" is drawn
        KXL_PutText(30, 50, "sample");
        / A frame is made to reflect in a window
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
  // A window is closed
  KXL_DeleteWindow();
  return 0;
}