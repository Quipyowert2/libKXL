#include <stdio.h>
#include <memory.h>
#include "KXL.h"

// 管理用
KXL_Window *KXL_Root;
// ディスプレイ名
Uint8       KXL_DName[65] = "";

//==============================================================
//  ディスプレイ名設定
//  引き数 : ディスプレイ名
//==============================================================
void KXL_DisplayName(Uint8 *name)
{
  sprintf(KXL_DName, "%s", name);
}

//==============================================================
// フォントカラー設定
// 引き数 : 文字列
//        : 赤の輝度
//        : 緑の輝度
//        : 青の輝度
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
// フレーム用色指定
// 引き数 : 赤の輝度
//        : 緑の輝度
//        : 青の輝度
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
// 長方形描画
// 引き数 : 左
//        : 上
//        : 幅
//        : 高さ
//        : 塗りつぶし(1ｰ塗りつぶし、0ｰ塗りつぶさない)
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
// 線描画
// 引き数 : 左
//        : 上
//        : 右
//        : 下
//==============================================================
void KXL_DrawLine(Uint16 left, Uint16 top, Uint16 right, Uint16 bottom)
{
  XDrawLine(KXL_Root->Display, KXL_Root->Frame->Buffer,
	    KXL_Root->Frame->Gc,
	    left, top, right, bottom);
}

//==============================================================
// ポリゴン描画
// 引き数 : KXLPolygonのポインタ
//        : ポリゴン数
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
// 点描画
// 引き数 : Ｘ座標
//        : Ｙ座標
//==============================================================
void KXL_DrawPoint(Uint16 x, Uint16 y)
{
  XDrawPoint(KXL_Root->Display, KXL_Root->Frame->Buffer,
	     KXL_Root->Frame->Gc,
	     x, y);
}

//==============================================================
//  ピックスマップからＧＣを作成
//  引き数：ピックスマップ
//        ：ＧＣのポインタ
//==============================================================
void KXL_SetGC(Pixmap p, GC *gc)
{
  *gc = XCreateGC(KXL_Root->Display, p,  0, 0);
  XSetGraphicsExposures(KXL_Root->Display, *gc, False);
}

//==============================================================
//  名称   : ウィンドウ作成
//  引き数 : ウィンドの幅
//         : ウィンドウの高さ
//         : タイトルバーに表示する文字列
//         : Xイベント
//==============================================================
void KXL_CreateWindow(Uint16 w, Uint16 h, Uint8 *title, Uint32 event)
{
  XSizeHints sh;
  KXL_Rect rect;

  // ウィンドウ用の領域を確保する
  KXL_Root=(KXL_Window *)KXL_Malloc(sizeof(KXL_Window));
  // Xサーバーに接続する
  if (!(KXL_Root->Display = XOpenDisplay(KXL_DName))) {
    fprintf(stderr, "KXL error message\nCannot open display\n");
    exit(1);
  }
  // スクリーンを取得する
  KXL_Root->Scr  = DefaultScreen(KXL_Root->Display);
  // カラーマップを取得する
  KXL_Root->Cmap = DefaultColormap(KXL_Root->Display, KXL_Root->Scr);
  // 色深度を取得する
  KXL_Root->Depth = DefaultDepth(KXL_Root->Display, KXL_Root->Scr);
  if (KXL_Root->Depth < 16) {
    fprintf(stderr, "KXL error message\n%depth:%d not support\n", KXL_Root->Depth);
    exit(1);
  }
  // ウィンドウを作成する
  KXL_Root->Win = XCreateSimpleWindow(KXL_Root->Display,
				      RootWindow(KXL_Root->Display, 0),
                                      0, 0,
                                      w, h, 
                                      0, 
                                      WhitePixel(KXL_Root->Display, KXL_Root->Scr),
				      BlackPixel(KXL_Root->Display, KXL_Root->Scr)
				      );
  // カラーマップを設定する
  XSetWindowColormap(KXL_Root->Display, KXL_Root->Win, KXL_Root->Cmap);
  // イベントを設定する
  XSelectInput(KXL_Root->Display, KXL_Root->Win, event);
  // タイトルバーにタイトルを設定する
  XStoreName(KXL_Root->Display, KXL_Root->Win, title);
  // ウィンドウのヒントを設定する
  sh.flags      = PMaxSize | PMinSize;
  sh.min_width  = w;
  sh.min_height = h;
  sh.max_width  = w;
  sh.max_height = h;
  XSetWMNormalHints(KXL_Root->Display, KXL_Root->Win, &sh);
  // Xサーバーにフォントをロードする
  // フォント用のGCを設定する
  KXL_Font("-adobe-courier-bold-r-normal--14-*-*-*-*-*-iso8859-1",
	   0xff, 0xff, 0xff);
  KXL_Root->Width  = w;
  KXL_Root->Height = h;
  XMapWindow(KXL_Root->Display, KXL_Root->Win);
  XFlush(KXL_Root->Display);
  // フレーム作成
  KXL_Root->Frame = 0;
  KXL_ReSizeFrame(w, h);
  rect.Left   = 0;
  rect.Top    = 0;
  rect.Width  = w;
  rect.Height = h;
  KXL_ClearFrame(rect);
  //オートリピートオフ
  XAutoRepeatOff(KXL_Root->Display);
}

//==============================================================
// ウィンドウ削除
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
//  フレームサイズ変更
//  引き数 : フレームの幅
//         : フレームの高さ
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
//  アップデート
//  引き数 : 矩形
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
//  イメージコピー
//  引き数 : 元イメージのポインタ
//         : 元イメージの矩形
//  戻り値 : コピーしたイメージのポインタ
//==============================================================
KXL_Image *KXL_CopyImage(KXL_Image *src, KXL_Rect r)
{
  GC gc8, gc1;
  KXL_Image *dest;

  // GC作成
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // イメージにコピー
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
  // クリップマスク作成
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // ローカル変数解放
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  return dest;
}

//==============================================================
//  元のイメージから縮小したイメージをコピー
//  引き数 : 元イメージのポインタ
//         : 新しい幅
//         : 新しい高さ
//  戻り値 : 新しいイメージのポインタ
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

  // GC作成
  KXL_SetGC(src->Buffer, &gc8);
  KXL_SetGC(src->Mask, &gc1);
  // イメージにコピー
  tmp          = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  tmp->Buffer = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
                              ww, src->Height,
			      KXL_Root->Depth);
  tmp->Mask = XCreatePixmap(KXL_Root->Display, KXL_Root->Win,
			    ww, src->Height,
			    1);
  // 横の拡縮
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
  // クリップマスク作成
  dest->MaskGC = XCreateGC(KXL_Root->Display, KXL_Root->Frame->Buffer, 0, 0);
  XSetClipMask(KXL_Root->Display, dest->MaskGC, dest->Mask);
  // ローカル変数解放
  XFreeGC(KXL_Root->Display, gc8);
  XFreeGC(KXL_Root->Display, gc1);
  return dest;
}

//==============================================================
//  イメージ解放
//  引き数 : イメージのポインタ
//==============================================================
void KXL_DeleteImage(KXL_Image *img)
{
  XFreePixmap(KXL_Root->Display, img->Buffer);
  XFreePixmap(KXL_Root->Display, img->Mask);
  XFreeGC(KXL_Root->Display, img->MaskGC);
  KXL_Free(img);
}

//==============================================================
//  ８ｂｐｓのＢＭＰファイルの読み込み
//  引き数 : ファイル名
//  戻り値 ：イメージのポインタ
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

  KXL_ReadBitmapHeader(filename, &hed);
  new = (KXL_Image *)KXL_Malloc(sizeof(KXL_Image));
  // イメージサイズ設定
  new->Width = hed.w;
  new->Height = hed.height;

  // 8bpsのビットマップを24 or 16bps化する
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
  // イメージをピックスマップにコピーする
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
  XPutImage(KXL_Root->Display, new->Mask,
	    gc1,
	    img,
	    0, 0,
	    0, 0, new->Width, new->Height);
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
//  イメージ描画
//  引き数 : イメージのポインタ
//         : 水平描画位置
//         : 垂直描画位置
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
//  イメージ描画
//  引き数 : イメージのポインタ
//         : イメージの矩形
//         : 水平描画位置
//         : 垂直描画位置
//==============================================================
void KXL_PutRectImage(KXL_Image *img, KXL_Rect r, Sint16 x, Sint16 y)
{
  KXL_Image *dest = KXL_CopyImage(img, r);
  KXL_PutImage(dest, x, y);
  KXL_DeleteImage(dest);
}

//==============================================================
//  テキスト描画
//  引き数 : 水平描画位置
//         : 垂直描画位置
//         : 文字列のポインタ
//==============================================================
void KXL_PutText(Sint16 x, Sint16 y, Uint8 *str)
{
  XDrawString(KXL_Root->Display,
	      KXL_Root->Frame->Buffer, 
	      KXL_Root->FontGC,
	      x, y, str, strlen(str));
}

//==============================================================
//  テキストの幅取得
//  引き数 : 文字列のポインタ
//  戻り値 : 描画幅
//==============================================================
Uint16 KXL_TextWidth(Uint8 *str)
{
  return XTextWidth(KXL_Root->WinFont, str, strlen(str));
}

//==============================================================
//  フレームクリア
//  引き数 : 矩形
//==============================================================
void KXL_ClearFrame(KXL_Rect r)
{
  XFillRectangle(KXL_Root->Display,
		 KXL_Root->Frame->Buffer,
		 KXL_Root->Frame->Gc,
                 r.Left, r.Top, r.Width, r.Height);
}

//==============================================================
//  ウィンドウイベントタイプ取得
//  戻り値：イベントタイプ
//==============================================================
Uint32 KXL_GetEvents(void)
{
  return KXL_Root->Event.type;
}

//==============================================================
//  ウィンドウイベント取得
//  戻り値：イベント発生
//==============================================================
Bool KXL_CheckEvents(void)
{
  return XCheckMaskEvent(KXL_Root->Display, ~0, &(KXL_Root->Event));
}

//==============================================================
//  キー取得
//  戻り値：キーコード
//==============================================================
Uint16 KXL_GetKey(void)
{
  return XLookupKeysym(&(KXL_Root->Event.xkey), 0);
}

//==============================================================
//  マウスボタン取得
//  引き数 : Ｘ座標のポインタ
//         : Ｙ座標のポインタ
//  戻り値 : ボタン番号
//==============================================================
Uint16 KXL_GetButton(Uint16 *x, Uint16 *y)
{
  *x = KXL_Root->Event.xbutton.x;
  *y = KXL_Root->Event.xbutton.y;
  return KXL_Root->Event.type == KXL_EVENT_BUTTON_MOTION ? 0 : KXL_Root->Event.xbutton.button;
}
