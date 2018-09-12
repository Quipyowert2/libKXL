#include <stdio.h>
#include <memory.h>
#include "KXL.h"

// ������
KXL_Window *KXL_Root;
// �ǥ����ץ쥤̾
Uint8       KXL_DName[65] = "";

//==============================================================
//  �ǥ����ץ쥤̾����
//  ������ : �ǥ����ץ쥤̾
//==============================================================
void KXL_DisplayName(Uint8 *name)
{
  sprintf(KXL_DName, "%s", name);
}

//==============================================================
// �ե���ȥ��顼����
// ������ : ʸ����
//        : �֤ε���
//        : �Фε���
//        : �Ĥε���
//==============================================================
void KXL_Font(Uint8 *str, Uint8 r, Uint8 g, Uint8 b)
{
  Uint32 rgb;

  switch (KXL_Root->Depth) {
  case 15:
  case 16:
    rgb = ((r & 0x3f) << 12) | ((g & 0x3f) << 6) | (b & 0x3f);
    break;
  case 24:
    rgb = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    break;
  default:
    rgb = ((r & 0x07) << 4) | ((g & 0x07) << 2) | (b & 0x07);
    break;
  }
  if (str) {
    KXL_Root->FontGC = XCreateGC(KXL_Root->Display, KXL_Root->Win,
				 0, 0);
    KXL_Root->WinFont = XLoadQueryFont(KXL_Root->Display, str);
    if (KXL_Root->WinFont == (XFontStruct *)NULL) {
      fprintf(stderr, "KXL error message\nloading font error (%s)\n", str);
      exit(-1);
    }
    XSetFont(KXL_Root->Display, KXL_Root->FontGC,
	     KXL_Root->WinFont->fid);
    XSetGraphicsExposures(KXL_Root->Display, KXL_Root->FontGC, False);
  }
  XSetForeground(KXL_Root->Display, KXL_Root->FontGC, rgb);
}

//==============================================================
// �ե졼���ѿ�����
// ������ : �֤ε���
//        : �Фε���
//        : �Ĥε���
//==============================================================
void KXL_SetDrawColor(Uint8 r, Uint8 g, Uint8 b)
{
  Uint32 rgb;

  switch (KXL_Root->Depth) {
  case 15:
  case 16:
    rgb = ((r & 0x3f) << 12) | ((g & 0x3f) << 6) | (b & 0x3f);
    break;
  case 24:
    rgb = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    break;
  default:
    rgb = ((r & 0x07) << 4) | ((g & 0x07) << 2) | (b & 0x07);
    break;
  }
  XSetForeground(KXL_Root->Display, KXL_Root->Frame->Gc, rgb);
}

//==============================================================
// Ĺ��������
// ������ : ��
//        : ��
//        : ��
//        : �⤵
//        : �ɤ�Ĥ֤�(1���ɤ�Ĥ֤���0���ɤ�Ĥ֤��ʤ�)
//==============================================================
void KXL_DrawRectangle(Uint16 left, Uint16 top, Uint16 width, Uint16 height, Bool flag)
{
  if (flag == False)
    XDrawRectangle(KXL_Root->Display, KXL_Root->Frame->Buffer,
		   KXL_Root->Frame->Gc,
		   left, top, width, height);
  else
    XFillRectangle(KXL_Root->Display, KXL_Root->Frame->Buffer,
		   KXL_Root->Frame->Gc,
		   left, top, width, height);
}

//==============================================================
// ������
// ������ : ��
//        : ��
//        : ��
//        : ��
//==============================================================
void KXL_DrawLine(Uint16 left, Uint16 top, Uint16 right, Uint16 bottom)
{
  XDrawLine(KXL_Root->Display, KXL_Root->Frame->Buffer,
	    KXL_Root->Frame->Gc,
	    left, top, right, bottom);
}

//==============================================================
// �ݥꥴ������
// ������ : KXLPolygon�Υݥ���
//        : �ݥꥴ���
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
// ������
// ������ : �غ�ɸ
//        : �ٺ�ɸ
//==============================================================
void KXL_DrawPoint(Uint16 x, Uint16 y)
{
  XDrawPoint(KXL_Root->Display, KXL_Root->Frame->Buffer,
	     KXL_Root->Frame->Gc,
	     x, y);
}

//==============================================================
//  �ԥå����ޥåפ���ǣä����
//  ���������ԥå����ޥå�
//        ���ǣäΥݥ���
//==============================================================
void KXL_SetGC(Pixmap p, GC *gc)
{
  *gc = XCreateGC(KXL_Root->Display, p,  0, 0);
  XSetGraphicsExposures(KXL_Root->Display, *gc, False);
}

//==============================================================
//  ̾��   : ������ɥ�����
//  ������ : ������ɤ���
//         : ������ɥ��ι⤵
//         : �����ȥ�С���ɽ������ʸ����
//         : X���٥��
//==============================================================
void KXL_CreateWindow(Uint16 w, Uint16 h, Uint8 *title, Uint32 event)
{
  XSizeHints sh;
  KXL_Rect rect;

  // ������ɥ��Ѥ��ΰ����ݤ���
  KXL_Root=(KXL_Window *)KXL_Malloc(sizeof(KXL_Window));
  // X�����С�����³����
  if (!(KXL_Root->Display = XOpenDisplay(KXL_DName))) {
    fprintf(stderr, "KXL error message\nCannot open display\n");
    exit(1);
  }
  // �����꡼����������
  KXL_Root->Scr  = DefaultScreen(KXL_Root->Display);
  // ���顼�ޥåפ��������
  KXL_Root->Cmap = DefaultColormap(KXL_Root->Display, KXL_Root->Scr);
  // �����٤��������
  KXL_Root->Depth = DefaultDepth(KXL_Root->Display, KXL_Root->Scr);
  if (KXL_Root->Depth < 16) {
    fprintf(stderr, "KXL error message\n%depth:%d not support\n", KXL_Root->Depth);
    exit(1);
  }
  // ������ɥ����������
  KXL_Root->Win = XCreateSimpleWindow(KXL_Root->Display,
				      RootWindow(KXL_Root->Display, 0),
                                      0, 0,
                                      w, h, 
                                      0, 
                                      WhitePixel(KXL_Root->Display, KXL_Root->Scr),
				      BlackPixel(KXL_Root->Display, KXL_Root->Scr)
				      );
  // ���顼�ޥåפ����ꤹ��
  XSetWindowColormap(KXL_Root->Display, KXL_Root->Win, KXL_Root->Cmap);
  // ���٥�Ȥ����ꤹ��
  XSelectInput(KXL_Root->Display, KXL_Root->Win, event);
  // �����ȥ�С��˥����ȥ�����ꤹ��
  XStoreName(KXL_Root->Display, KXL_Root->Win, title);
  // ������ɥ��Υҥ�Ȥ����ꤹ��
  sh.flags      = PMaxSize | PMinSize;
  sh.min_width  = w;
  sh.min_height = h;
  sh.max_width  = w;
  sh.max_height = h;
  XSetWMNormalHints(KXL_Root->Display, KXL_Root->Win, &sh);
  // X�����С��˥ե���Ȥ���ɤ���
  // �ե�����Ѥ�GC�����ꤹ��
  KXL_Font("-adobe-courier-bold-r-normal--14-*-*-*-*-*-iso8859-1",
	   0xff, 0xff, 0xff);
  KXL_Root->Width  = w;
  KXL_Root->Height = h;
  XMapWindow(KXL_Root->Display, KXL_Root->Win);
  XFlush(KXL_Root->Display);
  // �ե졼�����
  KXL_Root->Frame = 0;
  KXL_ReSizeFrame(w, h);
  rect.Left   = 0;
  rect.Top    = 0;
  rect.Width  = w;
  rect.Height = h;
  KXL_ClearFrame(rect);
  //�����ȥ�ԡ��ȥ���
  XAutoRepeatOff(KXL_Root->Display);
}

//==============================================================
// ������ɥ����
//==============================================================
void KXL_DeleteWindow(void)
{
  XFreePixmap(KXL_Root->Display, KXL_Root->Frame->Buffer);
  XFreeGC(KXL_Root->Display, KXL_Root->Frame->Gc);
  KXL_Free(KXL_Root->Frame);
  XAutoRepeatOn(KXL_Root->Display);
  XFreeGC(KXL_Root->Display, KXL_Root->FontGC);
  XDestroyWindow(KXL_Root->Display, KXL_Root->Win);
  XCloseDisplay(KXL_Root->Display);
  KXL_Free(KXL_Root);
}

//==============================================================
//  �ե졼�ॵ�����ѹ�
//  ������ : �ե졼�����
//         : �ե졼��ι⤵
//==============================================================
void KXL_ReSizeFrame(Uint16 w, Uint16 h)
{
  if (KXL_Root->Frame) {
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
//  ���åץǡ���
//  ������ : ���
//==============================================================
void KXL_UpDate(KXL_Rect r)
{
  XCopyArea(KXL_Root->Display,
	    KXL_Root->Frame->Buffer,
	    KXL_Root->Win, 
	    KXL_Root->Frame->Gc,
            r.Left, r.Top, r.Width, r.Height,
	    0, 0);
  //XFlush(KXL_Root->Display);
  XSync(KXL_Root->Display, False);
}

//==============================================================
//  ���᡼�����ԡ�
//  ������ : �����᡼���Υݥ���
//         : �����᡼���ζ��
//  ����� : ���ԡ��������᡼���Υݥ���
//==============================================================
KXL_Image *KXL_CopyImage(KXL_Image *src, KXL_Rect r)
{
  GC gc8, gc1;
  KXL_Image *dest;

  // GC����
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // ���᡼���˥��ԡ�
  dest         = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  dest->Width  = r.Width;
  dest->Height = r.Height;
  dest->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                               r.Width, r.Height, KXL_Root->Depth);
  XCopyArea(KXL_Root->Display,
	    src->Buffer,
	    dest->Buffer,
	    gc8, 
            r.Left, r.Top, r.Width, r.Height,
	    0, 0);
  dest->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                             r.Width, r.Height, 1);
  XCopyArea(KXL_Root->Display,
	    src->Mask,
	    dest->Mask,
	    gc1,
            r.Left, r.Top, r.Width, r.Height, 0, 0);
  // ����åץޥ�������
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // �������ѿ�����
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  return dest;
}

//==============================================================
//  ���Υ��᡼������̾��������᡼���򥳥ԡ�
//  ������ : �����᡼���Υݥ���
//         : ��������
//         : �������⤵
//  ����� : ���������᡼���Υݥ���
//==============================================================
KXL_Image *KXL_StrechImage(KXL_Image *src, Uint16 width, Uint16 height)
{
  GC gc8, gc1;
  KXL_Image *dest, *tmp;
  float aw = (float)src->Width / width;
  float ah = (float)src->Height / height;
  float w,h;
  Sint16 x,y;
  Uint16 ww = src->Width < width ? width : src->Width;

  // GC����
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // ���᡼���˥��ԡ�
  tmp          = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  tmp->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                              ww, src->Height,
			      KXL_Root->Depth);
  tmp->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
			    ww, src->Height,
			    1);
  // ���γȽ�
  for (x = 0, w = 0.0; x < width; x ++, w += aw) {
    XCopyArea(KXL_Root->Display,
	      src->Buffer,
	      tmp->Buffer,
	      gc8, 
              (Sint16)w, 0, 1, src->Height,
	      x, 0);
    XCopyArea(KXL_Root->Display,
	      src->Mask,
	      tmp->Mask,
	      gc1,
              (Sint16)w, 0, 1, src->Height,
	      x, 0);
  }
  // �Ƚ̸�Υ��᡼������
  dest         = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  dest->Width  = width;
  dest->Height = height;
  dest->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                               width, height,
			       KXL_Root->Depth);
  dest->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                             width, height,
			     1);
  // �ĤγȽ�
  for (y = 0, h = 0.0; y < height; y ++, h += ah) {
    XCopyArea(KXL_Root->Display,
	      tmp->Buffer,
	      dest->Buffer,
	      gc8, 
              0, (Sint16)h, width, 1,
	      0, y);
    XCopyArea(KXL_Root->Display,
	      tmp->Mask,
	      dest->Mask,
	      gc1,
              0, (Sint16)h, width, 1,
	      0, y);
  }
  XFreePixmap(KXL_Root->Display, tmp->Buffer);
  XFreePixmap(KXL_Root->Display, tmp->Mask);
  KXL_Free(tmp);
  // ����åץޥ�������
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // �������ѿ�����
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  return dest;
}

//==============================================================
//  ���᡼������
//  ������ : ���᡼���Υݥ���
//==============================================================
void KXL_DeleteImage(KXL_Image *img)
{
  XFreePixmap(KXL_Root->Display, img->Buffer);
  XFreePixmap(KXL_Root->Display, img->Mask);
  XFreeGC(KXL_Root->Display, img->MaskGC);
  KXL_Free(img);
}

//==============================================================
//  ������Σ£ͣХե�������ɤ߹���
//  ������ : �ե�����̾
//  ����� �����᡼���Υݥ���
//==============================================================
KXL_Image *KXL_LoadBitmap(Uint8 *filename, Uint8 blend)
{
  // �إå������ѹ�¤��
  KXL_BitmapHeader hed;
  Uint32 i, j, k, l, no;
  KXL_Image *new;
  XImage *img;
  GC gc8, gc1;
  Visual *v = DefaultVisual(KXL_Root->Display, KXL_Root->Scr);

  KXL_ReadBitmapHeader(filename, &hed);
  new = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  // ���᡼������������
  new->Width = hed.w;
  new->Height = hed.height;

  // 8bps�Υӥåȥޥåפ�24 or 16bps������
  img = XCreateImage(KXL_Root->Display,
		     v,
		     KXL_Root->Depth,
		     ZPixmap, 0, 0,
		     new->Width, new->Height,
		     BitmapPad(KXL_Root->Display), 0);
  img->data = (Uint8 *)KXL_Malloc(img->bytes_per_line * new->Height);
  if (KXL_Root->Depth == 16)
    KXL_CreateBitmap8to16(hed.data, img, hed.rgb, blend);
  else if (KXL_Root->Depth == 24)
    KXL_CreateBitmap8to24(hed.data, img, hed.rgb, blend);
  // ���᡼����ԥå����ޥåפ˥��ԡ�����
  new->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
			      new->Width, new->Height,
			      KXL_Root->Depth);
  KXL_SetGC(new->Buffer, &gc8);
  XPutImage(KXL_Root->Display, new->Buffer,
	    gc8,
	    img,
	    0, 0,
	    0, 0, new->Width, new->Height);
  XDestroyImage(img);

  // 8bps�Υӥåȥޥåפ�1bps������
  img = XCreateImage(KXL_Root->Display,
		     v,
		     1,
		     XYPixmap, 0, 0,
		     new->Width, new->Height,
		     BitmapPad(KXL_Root->Display), 0);
  img->data = (Uint8 *)KXL_Malloc(img->bytes_per_line * new->Height);
  KXL_CreateBitmap8to1(hed.data, img, blend);
  // �ޥ������᡼����ԥå����ޥåפ˥��ԡ�����
  new->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
			    new->Width, new->Height,
			    1);
  KXL_SetGC(new->Mask, &gc1);
  XPutImage(KXL_Root->Display, new->Mask,
	    gc1,
	    img,
	    0, 0,
	    0, 0, new->Width, new->Height);
  XDestroyImage(img);

  // ����åץޥ�������
  new->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, new->MaskGC, new->Mask);

  // ����ѥ�����������
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  KXL_Free(hed.rgb);
  KXL_Free(hed.data);
  return new;
}

//==============================================================
//  ���᡼������
//  ������ : ���᡼���Υݥ���
//         : ��ʿ�������
//         : ��ľ�������
//==============================================================
void KXL_PutImage(KXL_Image *img, Sint16 x, Sint16 y)
{
  XSetClipOrigin(KXL_Root->Display, img->MaskGC, x, y);
  XCopyArea(KXL_Root->Display,
	    img->Buffer,
	    KXL_Root->Frame->Buffer,
	    img->MaskGC,
            0, 0,
	    img->Width, img->Height, x, y);
}

//==============================================================
//  ���᡼������
//  ������ : ���᡼���Υݥ���
//         : ���᡼���ζ��
//         : ��ʿ�������
//         : ��ľ�������
//==============================================================
void KXL_PutRectImage(KXL_Image *img, KXL_Rect r, Sint16 x, Sint16 y)
{
  KXL_Image *dest = KXL_CopyImage(img, r);
  KXL_PutImage(dest, x, y);
  KXL_DeleteImage(dest);
}

//==============================================================
//  �ƥ���������
//  ������ : ��ʿ�������
//         : ��ľ�������
//         : ʸ����Υݥ���
//==============================================================
void KXL_PutText(Sint16 x, Sint16 y, Uint8 *str)
{
  XDrawString(KXL_Root->Display,
	      KXL_Root->Frame->Buffer, 
	      KXL_Root->FontGC,
	      x, y, str, strlen(str));
}

//==============================================================
//  �ƥ����Ȥ�������
//  ������ : ʸ����Υݥ���
//  ����� : ������
//==============================================================
Uint16 KXL_TextWidth(Uint8 *str)
{
  return XTextWidth(KXL_Root->WinFont, str, strlen(str));
}

//==============================================================
//  �ե졼�९�ꥢ
//  ������ : ���
//==============================================================
void KXL_ClearFrame(KXL_Rect r)
{
  XFillRectangle(KXL_Root->Display,
		 KXL_Root->Frame->Buffer,
		 KXL_Root->Frame->Gc,
                 r.Left, r.Top, r.Width, r.Height);
}

//==============================================================
//  ������ɥ����٥�ȥ����׼���
//  ����͡����٥�ȥ�����
//==============================================================
Uint32 KXL_GetEvents(void)
{
  return KXL_Root->Event.type;
}

//==============================================================
//  ������ɥ����٥�ȼ���
//  ����͡����٥��ȯ��
//==============================================================
Bool KXL_CheckEvents(void)
{
  return XCheckMaskEvent(KXL_Root->Display, ~0, &(KXL_Root->Event));
}

//==============================================================
//  ��������
//  ����͡�����������
//==============================================================
Uint16 KXL_GetKey(void)
{
  return XLookupKeysym(&(KXL_Root->Event.xkey), 0);
}

//==============================================================
//  �ޥ����ܥ������
//  ������ : �غ�ɸ�Υݥ���
//         : �ٺ�ɸ�Υݥ���
//  ����� : �ܥ����ֹ�
//==============================================================
Uint16 KXL_GetButton(Uint16 *x, Uint16 *y)
{
  *x = KXL_Root->Event.xbutton.x;
  *y = KXL_Root->Event.xbutton.y;
  return KXL_Root->Event.type == KXL_EVENT_BUTTON_MOTION ? 0 : KXL_Root->Event.xbutton.button;
}
