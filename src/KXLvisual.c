#include <stdlib.h> // for exit
#include <string.h> // for strlen
#include <stdio.h>
#include "KXL.h"

//================================================================
// Internal variables and functions
//================================================================
KXL_Window *KXL_Root;
Uint8 KXL_DName[1024] = "";

//==============================================================
//  Set GC from pixmap
//  arguments：Pixmap
//           ：Pointer of GC
//==============================================================
void KXL_SetGC(Pixmap p, GC *gc)
{
  *gc = XCreateGC(KXL_Root->Display, p,  0, 0);
  XSetGraphicsExposures(KXL_Root->Display, *gc, False);
}

//==============================================================
//  Update to window
//  arguments : Frame left
//            : Frame top
//            : Frame width
//            : Frame height
//==============================================================
void KXL_Up_Date(Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h)
{
  XCopyArea(KXL_Root->Display,
            // from frame
            KXL_Root->Frame->Buffer,
            // to window
            KXL_Root->Win,
            // gc
            KXL_Root->Frame->Gc,
            // from frame rectangle
            src_l, src_t, src_w, src_h,
            // to window position
            0, 0);
  //XFlush(KXL_Root->Display);
  XSync(KXL_Root->Display, False);
}

//==============================================================
//  Clear to frame
//  arguments : Frame left
//            : Frame top
//            : Frame width
//            : Frame height
//==============================================================
void KXL_Clear_Frame(Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h)
{
  XFillRectangle(KXL_Root->Display,
                 KXL_Root->Frame->Buffer,
                 KXL_Root->Frame->Gc,
                 src_l, src_t, src_w, src_h);
}

//==============================================================
//  Copy from source image to new image
//  arguments    : Pointer of source image
//               : Source image left
//               : Source image top
//               : Source image width
//               : Source image height
//  Return value : Pointer of new image
//==============================================================
KXL_Image *KXL_Copy_Image(KXL_Image *src, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h)
{
  GC gc8, gc1;
  KXL_Image *dest;

  // GC作成
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // イメージにコピー
  dest         = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  dest->Width  = src_w;
  dest->Height = src_h;
  dest->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                               src_w, src_h, KXL_Root->Depth);
  XCopyArea(KXL_Root->Display,
            src->Buffer,
            dest->Buffer,
            gc8,
            src_l, src_t, src_w, src_h,
            0, 0);
  dest->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                             src_w, src_h, 1);
  XCopyArea(KXL_Root->Display,
            src->Mask,
            dest->Mask,
            gc1,
            src_l, src_t, src_w, src_h,
            0, 0);
  // クリップマスク作成
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // ローカル変数解放
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  return dest;
}

//==============================================================
//  Stretch copy from source image to new image
//  arguments    : Pointer of source image
//               : Source image left
//               : Source image top
//               : Source image width
//               : Source image height
//               : New image width
//               : New image height
//  Return value : Pointer of new image
//==============================================================
KXL_Image *KXL_Copy_StretchImage(KXL_Image *src, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Uint16 width, Uint16 height)
{
  GC gc8, gc1;
  KXL_Image *dest, *tmp;
  Uint32 ax = ((float)src_w / (float)width) * 1000;
  Uint32 ay = ((float)src_h / (float)height) * 1000;
  Uint32 i, p, pp;
  Uint16 ww = src->Width < width ? width : src->Width;
  Uint16 w2, h2;
  
  // GC作成
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // 横の拡縮用イメージ作成
  tmp = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  tmp->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                              ww, src_h,
                              KXL_Root->Depth);
  tmp->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                            ww, src_h,
                            1);
  w2 = width / 2;
  h2 = height / 2;
  // 横の拡縮
  for (i = 0, p = 0; i <= w2; i ++, p += ax) {
    pp = p / 1000;
    // From left to center
    // image
    XCopyArea(KXL_Root->Display,
              src->Buffer,
              tmp->Buffer,
              gc8,
              src_l + pp, src_t, 1, src_h,
              i, 0);
    // mask image
    XCopyArea(KXL_Root->Display,
              src->Mask,
              tmp->Mask,
              gc1,
              src_l + pp, src_t, 1, src_h,
              i, 0);
    // From right to center
    // image
    XCopyArea(KXL_Root->Display,
              src->Buffer,
              tmp->Buffer,
              gc8,
              src_l + src_w - 1 - pp, src_t, 1, src_h,
              width - 1 - i, 0);
    // mask image
    XCopyArea(KXL_Root->Display,
              src->Mask,
              tmp->Mask,
              gc1,
              src_l + src_w - 1 - pp, src_t, 1, src_h,
              width - 1 - i, 0);
  }
  // 拡縮後のイメージ作成
  dest         = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  dest->Width  = width;
  dest->Height = height;
  dest->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                               width, height,
                               KXL_Root->Depth);
  dest->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                             width, height,
                             1);
  // 縦の拡縮
  for (i = 0, p = 0; i <= h2; i ++, p += ay) {
    pp = p / 1000;
    // From up to center
    // image
    XCopyArea(KXL_Root->Display,
              tmp->Buffer,
              dest->Buffer,
              gc8,
              0, pp, width, 1,
              0, i);
    // mask image
    XCopyArea(KXL_Root->Display,
              tmp->Mask,
              dest->Mask,
              gc1,
              0, pp, width, 1,
              0, i);
    // From bottom to center
    // image
    XCopyArea(KXL_Root->Display,
              tmp->Buffer,
              dest->Buffer,
              gc8,
              0, src_h - 1 - pp, width, 1,
              0, height - 1 - i);
    // mask image
    XCopyArea(KXL_Root->Display,
              tmp->Mask,
              dest->Mask,
              gc1,
              0, src_h - 1 - pp, width, 1,
              0, height - 1 - i);
  }
  // Create clip mask GC
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // ローカル変数解放
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  XFreePixmap(KXL_Root->Display, tmp->Buffer);
  XFreePixmap(KXL_Root->Display, tmp->Mask);
  KXL_Free(tmp);
  return dest;
}

//==============================================================
//  Put rectangle image
//  arguments    : Pointer of source image
//               : Source image left
//               : Source image top
//               : Source image width
//               : Source image height
//               : Put left position
//               : Put top position
//==============================================================
void KXL_Put_Image(KXL_Image *img, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Sint16 x, Sint16 y)
{
  KXL_Image *dest = KXL_Copy_Image(img, src_l, src_t, src_w, src_h);
  XSetClipOrigin(KXL_Root->Display, dest->MaskGC, x, y);
  XCopyArea(KXL_Root->Display,
            // from image
            dest->Buffer,
            // to frame
            KXL_Root->Frame->Buffer,
            // gc
            dest->MaskGC,
            // from image rectangle
            0, 0, src_w, src_h,
            // to frame position
            x, y);
  KXL_DeleteImage(dest);
}

//==============================================================
//  Put rectangle stretch image
//  arguments    : Pointer of source image
//               : Source image left
//               : Source image top
//               : Source image width
//               : Source image height
//               : Put image width
//               : Put image height
//               : Put left position
//               : Put top position
//==============================================================
void KXL_Put_StretchImage(KXL_Image *img, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Uint16 width, Uint16 height, Sint16 x, Sint16 y)
{
  KXL_Image *dest = KXL_Copy_StretchImage(img, src_l, src_t, src_w, src_h, width, height);
  XSetClipOrigin(KXL_Root->Display, dest->MaskGC, x, y);
  XCopyArea(KXL_Root->Display,
            // from image
            dest->Buffer,
            // to frame
            KXL_Root->Frame->Buffer,
            // gc
            dest->MaskGC,
            // from image rectangle
            0, 0, width, height,
            // to frame position
            x, y);
  KXL_DeleteImage(dest);
}

//================================================================
// Globall functions
//================================================================
//==============================================================
//  Set name of display
//  arguments : Name of display
//==============================================================
void KXL_DisplayName(Uint8 *name)
{
  sprintf(KXL_DName, "%s", name);
}

//==============================================================
//  Create window
//  arguments : width
//            : height
//            : title string
//            : Event of X
//==============================================================
void KXL_CreateWindow(Uint16 w, Uint16 h, Uint8 *title, Uint32 event)
{
  XSizeHints sh;

  // ウィンドウ用の領域を確保する
  KXL_Root = (KXL_Window *)KXL_Malloc(sizeof(KXL_Window));
  KXL_Root->Display = NULL;
  KXL_Root->Frame = NULL;
  KXL_Root->WinFont = NULL;
  KXL_Root->FontGC = NULL;
  // Connect to X server
  if (!(KXL_Root->Display = XOpenDisplay(KXL_DName))) {
    fprintf(stderr, "KXL error message\nCannot open display\n");
    exit(1);
  }
  // Get screen no.
  KXL_Root->Scr  = DefaultScreen(KXL_Root->Display);
  // Get color map
  KXL_Root->Cmap = DefaultColormap(KXL_Root->Display, KXL_Root->Scr);
  // get bpp(bits per pixel)
  KXL_Root->Depth = DefaultDepth(KXL_Root->Display, KXL_Root->Scr);
  // Support by 16, 24, 32
  if (KXL_Root->Depth < 16) {
    fprintf(stderr,
            "KXL error message\n"
            "%bpp:%dbpp color not support.\n"
            "Please 16 or 24 or 32bpp color",
            KXL_Root->Depth, KXL_Root->Depth);
    exit(1);
  }
  // Create window
  KXL_Root->Win = XCreateSimpleWindow(KXL_Root->Display,
                                      RootWindow(KXL_Root->Display, 0),
                                      0, 0,
                                      w, h, 
                                      0, 
                                      WhitePixel(KXL_Root->Display, KXL_Root->Scr),
                                      BlackPixel(KXL_Root->Display, KXL_Root->Scr)
                                      );
  KXL_Root->Width  = w;
  KXL_Root->Height = h;
  // Set color map
  XSetWindowColormap(KXL_Root->Display, KXL_Root->Win, KXL_Root->Cmap);
  // Set event
  XSelectInput(KXL_Root->Display, KXL_Root->Win, event);
  // Set title bar
  XStoreName(KXL_Root->Display, KXL_Root->Win, title);
  // Set window hint
  sh.flags      = PMaxSize | PMinSize;
  sh.min_width  = w;
  sh.min_height = h;
  sh.max_width  = w;
  sh.max_height = h;
  XSetWMNormalHints(KXL_Root->Display, KXL_Root->Win, &sh);
  // Load font
  KXL_Font("-adobe-courier-bold-r-normal--14-*-*-*-*-*-iso8859-1",
           0xff, 0xff, 0xff);
  // Mapping window
  XMapWindow(KXL_Root->Display, KXL_Root->Win);
  XFlush(KXL_Root->Display);
  // Create frame
  KXL_Root->Frame = 0;
  KXL_ReSizeFrame(w, h);
  KXL_Clear_Frame(0, 0, w, h);
  // Auto repeat off
  XAutoRepeatOff(KXL_Root->Display);
}

//==============================================================
// Delete window
//==============================================================
void KXL_DeleteWindow(void)
{
  // Delete frame
  XFreePixmap(KXL_Root->Display, KXL_Root->Frame->Buffer);
  XFreeGC(KXL_Root->Display, KXL_Root->Frame->Gc);
  KXL_Free(KXL_Root->Frame);
  // Auto repeat on
  XAutoRepeatOn(KXL_Root->Display);
  // Delete font
  XFreeFont(KXL_Root->Display, KXL_Root->WinFont);
  XFreeGC(KXL_Root->Display, KXL_Root->FontGC);
  // Delete window
  XDestroyWindow(KXL_Root->Display, KXL_Root->Win);
  // Cutting X server
  XCloseDisplay(KXL_Root->Display);
  KXL_Free(KXL_Root);
}

//==============================================================
//  Resize frame
//  arguments : new frame width
//            : new frame height
//==============================================================
void KXL_ReSizeFrame(Uint16 w, Uint16 h)
{
  if (KXL_Root->Frame) {
    // 既存のフレーム削除
    XFreePixmap(KXL_Root->Display, KXL_Root->Frame->Buffer);
    XFreeGC(KXL_Root->Display, KXL_Root->Frame->Gc);
    KXL_Free(KXL_Root->Frame);
  }
  KXL_Root->Frame         = (KXL_Frame *)KXL_Malloc(sizeof(KXL_Frame));
  KXL_Root->Frame->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                                          w, h, KXL_Root->Depth);
  KXL_Root->Frame->Gc     = XCreateGC(KXL_Root->Display,
                                      KXL_Root->Frame->Buffer,
                                      0, 0);
  KXL_Root->Frame->Width  = w;
  KXL_Root->Frame->Height = h;
}

//==============================================================
// set font name and color
// arguments : Null or Name of font
//           : red
//           : green
//           : blue
//==============================================================
void KXL_Font(Uint8 *str, Uint8 r, Uint8 g, Uint8 b)
{
  Uint32 rgb;

  if (KXL_Root->Depth == 16)
    rgb = ((r / 8) << 12) | ((g / 8) << 6) | (b / 8);
  else // 24 or 32
    rgb = (r << 16) | (g << 8) | b;
  if (str) {
    if (KXL_Root->WinFont) {
      XFreeFont(KXL_Root->Display, KXL_Root->WinFont);
    }
    if (!KXL_Root->FontGC) {
      KXL_Root->FontGC = XCreateGC(KXL_Root->Display, KXL_Root->Win,
                                   0, 0);
    }
    KXL_Root->WinFont = XLoadQueryFont(KXL_Root->Display, str);
    if (KXL_Root->WinFont == (XFontStruct *)NULL) {
      fprintf(stderr,
              "KXL error message\n"
              "loading font error (%s)\n",
              str);
      exit(-1);
    }
    XSetFont(KXL_Root->Display, KXL_Root->FontGC,
             KXL_Root->WinFont->fid);
    XSetGraphicsExposures(KXL_Root->Display, KXL_Root->FontGC, False);
  }
  XSetForeground(KXL_Root->Display, KXL_Root->FontGC, rgb);
}

//==============================================================
//  Put text string
//  arguments : Put left position
//            : Put top position
//            : Put text string
//==============================================================
void KXL_PutText(Sint16 x, Sint16 y, Uint8 *str)
{
  XDrawString(KXL_Root->Display,
              KXL_Root->Frame->Buffer, 
              KXL_Root->FontGC,
              x, y, str, strlen(str));
}

//==============================================================
//  Get text width
//  arguments    : Text string
//  Return value : Text width
//==============================================================
Uint16 KXL_TextWidth(Uint8 *str)
{
  return XTextWidth(KXL_Root->WinFont, str, strlen(str));
}

//==============================================================
// Set drawing color
// arguments : red
//           : green
//           : blue
//==============================================================
void KXL_SetDrawColor(Uint8 r, Uint8 g, Uint8 b)
{
  Uint32 rgb;

  if (KXL_Root->Depth == 16)
    rgb = ((r / 8) << 12) | ((g / 8) << 6) | (b / 8);
  else // 24 or 32
    rgb = (r << 16) | (g << 8) | b;
  XSetForeground(KXL_Root->Display, KXL_Root->Frame->Gc, rgb);
}

//==============================================================
// Drawing point
// arguments : left
//           : top
//==============================================================
void KXL_DrawPoint(Sint16 left, Sint16 top)
{
  XDrawPoint(KXL_Root->Display, KXL_Root->Frame->Buffer,
             KXL_Root->Frame->Gc,
             left, top);
}

//==============================================================
// Drawing line
// arguments : left
//           : top
//           : right
//           : bottom
//==============================================================
void KXL_DrawLine(Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
  XDrawLine(KXL_Root->Display, KXL_Root->Frame->Buffer,
            KXL_Root->Frame->Gc,
            left, top, right, bottom);
}

//==============================================================
// Drawing rectangle
// arguments : left
//           : top
//           : width
//           : height
//           : True = fill、False = not fill
//==============================================================
void KXL_DrawRectangle(Sint16 left, Sint16 top, Uint16 width, Uint16 height, Bool flag)
{
  if (flag == False) // not fill
    XDrawRectangle(KXL_Root->Display, KXL_Root->Frame->Buffer,
                   KXL_Root->Frame->Gc,
                   left, top, width, height);
  else // fill
    XFillRectangle(KXL_Root->Display, KXL_Root->Frame->Buffer,
                   KXL_Root->Frame->Gc,
                   left, top, width, height);
}

//==============================================================
// Drawing polygon
// arguments : Pointer of KXLPolygon
//           : Max-number
//           : True = Absolute position, False = Relative position
//           : True = fill, False = not fill
//==============================================================
void KXL_DrawPolygon(KXL_Polygon *data, Uint16 max, Bool next, Bool flag)
{

  if (flag == False)
    XDrawLines(KXL_Root->Display, KXL_Root->Frame->Buffer,
               KXL_Root->Frame->Gc,
               data, max,
               next == True ? CoordModeOrigin : CoordModePrevious);
  else
    XFillPolygon(KXL_Root->Display, KXL_Root->Frame->Buffer,
                 KXL_Root->Frame->Gc,
                 data, max,
                 Convex,
                 next == True ? CoordModeOrigin : CoordModePrevious);
}

//==============================================================
//  Load bitmap file of 8bits per pixel
//  arguments    : File name
//  Return value : Pointer of new image
//==============================================================
KXL_Image *KXL_LoadBitmap(Uint8 *filename, Uint8 blend)
{
  // ヘッダ情報用構造体
  KXL_BitmapHeader hed;
  Uint32 i, j, k, l, no;
  KXL_Image *new;
  XImage *img;
  GC gc8, gc1;
  Visual *v = DefaultVisual(KXL_Root->Display, KXL_Root->Scr);

  // ビットマップヘッダ読込み
  KXL_ReadBitmapHeader(filename, &hed);
  // イメージサイズ設定
  new = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  new->Width = hed.w;
  new->Height = hed.height;

  // 8bpsのビットマップを24 or 16bpp化する
  img = XCreateImage(KXL_Root->Display,
                     v,
                     KXL_Root->Depth,
                     ZPixmap, 0, 0,
                     new->Width, new->Height,
                     BitmapPad(KXL_Root->Display), 0);
  img->data = (Uint8 *)KXL_Malloc(img->bytes_per_line * new->Height);
  if (KXL_Root->Depth == 16)
    KXL_CreateBitmap8to16(hed.data, img, hed.rgb, blend);
  else // 24 or 32
    KXL_CreateBitmap8to24(hed.data, img, hed.rgb, blend);
  // イメージをピックスマップにコピーする
  new->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                              new->Width, new->Height,
                              KXL_Root->Depth);
  KXL_SetGC(new->Buffer, &gc8);
  XPutImage(KXL_Root->Display,
            new->Buffer,                    // to
            gc8,
            img,                            // from
            0, 0,                           // to
            0, 0, new->Width, new->Height); // from
  XDestroyImage(img);

  // 8bpsのビットマップを1bps化する
  img = XCreateImage(KXL_Root->Display,
                     v,
                     1,
                     XYPixmap, 0, 0,
                     new->Width, new->Height,
                     BitmapPad(KXL_Root->Display), 0);
  img->data = (Uint8 *)KXL_Malloc(img->bytes_per_line * new->Height);
  KXL_CreateBitmap8to1(hed.data, img, blend);
  // マスクイメージをピックスマップにコピーする
  new->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                            new->Width, new->Height,
                            1);
  KXL_SetGC(new->Mask, &gc1);
  XPutImage(KXL_Root->Display,
            new->Mask,                      // to
            gc1,
            img,                            // from
            0, 0,                           // to
            0, 0, new->Width, new->Height); // from
  XDestroyImage(img);

  // クリップマスク作成
  new->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, new->MaskGC, new->Mask);

  // 作業用メモリを解放する
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  KXL_Free(hed.rgb);
  KXL_Free(hed.data);
  return new;
}

//==============================================================
//  Put image
//  Arguments : Pointer of source image
//            : Put left position
//            : Put top position
//==============================================================
void KXL_PutImage(KXL_Image *img, Sint16 x, Sint16 y)
{
  XSetClipOrigin(KXL_Root->Display, img->MaskGC, x, y);
  XCopyArea(KXL_Root->Display,
            // from image
            img->Buffer,
            // to frame
            KXL_Root->Frame->Buffer,
            // gc
            img->MaskGC,
            // from image rectangle
            0, 0, img->Width, img->Height,
            // to frame position
            x, y);
}

//==============================================================
//  Delete image
//  Arguments : Pointer of source image
//==============================================================
void KXL_DeleteImage(KXL_Image *img)
{
  XFreePixmap(KXL_Root->Display, img->Buffer);
  XFreePixmap(KXL_Root->Display, img->Mask);
  XFreeGC(KXL_Root->Display, img->MaskGC);
  KXL_Free(img);
}

//==============================================================
//  Check event of window
//  Return value : True - Outbreak, False - Nothing
//==============================================================
Bool KXL_CheckEvents(void)
{
  return XCheckMaskEvent(KXL_Root->Display, ~0, &(KXL_Root->Event));
}

//==============================================================
//  Get event number of window
//  Return value : Event number
//==============================================================
Uint32 KXL_GetEvents(void)
{
  return KXL_Root->Event.type;
}

//==============================================================
//  Get key code
//  Return value : Key code
//==============================================================
Uint16 KXL_GetKey(void)
{
  return XLookupKeysym(&(KXL_Root->Event.xkey), 0);
}

//==============================================================
//  Get mouse button information
//  Arguments    : Left position
//               : Top position
//  Return value : Button number
//==============================================================
Uint16 KXL_GetButton(Uint16 *x, Uint16 *y)
{
  *x = KXL_Root->Event.xbutton.x;
  *y = KXL_Root->Event.xbutton.y;
  return KXL_Root->Event.type == KXL_EVENT_BUTTON_MOTION ? 0 : KXL_Root->Event.xbutton.button;
}
