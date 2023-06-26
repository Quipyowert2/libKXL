#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <signal.h>
#include <sys/time.h>
#include "KXL.h"

Bool KXL_TimerFlag;

// 360度データ
static Sint16 sin360[] = {
    0,
    4,    8,   13,   17,   22,   26,   31,   35,   40,   44,
   48,   53,   57,   61,   66,   70,   74,   79,   83,   87,
   91,   95,  100,  104,  108,  112,  116,  120,  124,  127,
  131,  135,  139,  143,  146,  150,  154,  157,  161,  164,
  167,  171,  174,  177,  181,  184,  187,  190,  193,  196,
  198,  201,  204,  207,  209,  212,  214,  217,  219,  221,
  223,  226,  228,  230,  232,  233,  235,  237,  238,  240,
  242,  243,  244,  246,  247,  248,  249,  250,  251,  252,
  252,  253,  254,  254,  255,  255,  255,  255,  255,  256,
  255,  255,  255,  255,  255,  254,  254,  253,  252,  252,
  251,  250,  249,  248,  247,  246,  244,  243,  242,  240,
  238,  237,  235,  233,  232,  230,  228,  226,  223,  221,
  219,  217,  214,  212,  209,  207,  204,  201,  198,  196,
  193,  190,  187,  184,  181,  177,  174,  171,  167,  164,
  161,  157,  154,  150,  146,  143,  139,  135,  131,  127,
  124,  120,  116,  112,  108,  104,  100,   95,   91,   87,
   83,   79,   74,   70,   66,   61,   57,   53,   48,   44,
   40,   35,   31,   26,   22,   17,   13,    8,    4,    0,
   -4,   -8,  -13,  -17,  -22,  -26,  -31,  -35,  -40,  -44,
  -48,  -53,  -57,  -61,  -66,  -70,  -74,  -79,  -83,  -87,
  -91,  -95, -100, -104, -108, -112, -116, -120, -124, -127,
 -131, -135, -139, -143, -146, -150, -154, -157, -161, -164,
 -167, -171, -174, -177, -181, -184, -187, -190, -193, -196,
 -198, -201, -204, -207, -209, -212, -214, -217, -219, -221,
 -223, -226, -228, -230, -232, -233, -235, -237, -238, -240,
 -242, -243, -244, -246, -247, -248, -249, -250, -251, -252,
 -252, -253, -254, -254, -255, -255, -255, -255, -255, -256,
 -255, -255, -255, -255, -255, -254, -254, -253, -252, -252,
 -251, -250, -249, -248, -247, -246, -244, -243, -242, -240,
 -238, -237, -235, -233, -232, -230, -228, -226, -223, -221,
 -219, -217, -214, -212, -209, -207, -204, -201, -198, -196,
 -193, -190, -187, -184, -181, -177, -174, -171, -167, -164,
 -161, -157, -154, -150, -146, -143, -139, -135, -131, -128,
 -124, -120, -116, -112, -108, -104, -100,  -95,  -91,  -87,
  -83,  -79,  -74,  -70,  -66,  -61,  -57,  -53,  -48,  -44,
  -40,  -35,  -31,  -26,  -22,  -17,  -13,   -8,   -4,    0,
};

//==============================================================
//  タイマーフラグ取得
//==============================================================
Bool KXL_GetTimer(void)
{
  return KXL_TimerFlag;
}

//==============================================================
//  タイマーフラグリセット
//==============================================================
void KXL_ResetTimer(void)
{
  KXL_TimerFlag = False;
}

//==============================================================
//  タイマーコールバック
//==============================================================
void KXL_TimerCallBack(int dummy)
{
  KXL_TimerFlag = True;
}

//==============================================================
//  タイマー設定
//  引き数：フレーム数
//==============================================================
void KXL_Timer(Uint16 time)
{
  struct itimerval val={
    {0, 1000000 / time},
    {0, 1000000 / time},
  };
  signal(SIGALRM, KXL_TimerCallBack);
  setitimer(ITIMER_REAL, &val, NULL);
  KXL_TimerFlag = False;
}

//==============================================================
//  メモリ確保
//  引き数：メモリサイズ
//  戻り値：確保したメモリのポインタ
//==============================================================
void *KXL_Malloc(Uint32 size)
{
  void *new;

  new = malloc(size);
  if (new == NULL) {
    fprintf(stderr, "KXL error message\nOut Of memory!!\n");
    exit(1);
  }
  return new;
}

//==============================================================
//  メモリ再確保
//  引き数：メモリのポインタ
//        ：メモリサイズ
//  戻り値：再確保したメモリのポインタ
//==============================================================
void *KXL_Realloc(void *src, Uint32 size)
{
  void *new;

  new = realloc(src, size);
  if (new == NULL) {
    fprintf(stderr, "KXL error message\nOut Of memory!!\n");
    free(src);
    exit(1);
  }
  return new;
}

//==============================================================
//  メモリ解放
//  引き数：メモリのポインタ
//==============================================================
void KXL_Free(void *src)
{
  free(src);
}

//==============================================================
//  方角取得
//  引き数：自分の矩形
//        ：相手の矩形
//==============================================================
Uint16 KXL_GetDirection(KXL_Rect src, KXL_Rect target) {
  Uint16 k, x, y;
  Uint16 mx, my, yx, yy;

  mx = src.Left + (src.Width >> 1);
  my = src.Top + (src.Height >> 1);
  yx = target.Left + (target.Width >> 1);
  yy = target.Top + (target.Height >> 1);

  x = abs(yx - mx);
  y = abs(yy - my);
  if (yx == mx)
    k = (yy > my) ? 0 : 180;
  else if (yy == my)
    k = (yx > mx) ? 90 : 270;
  else if (yx > mx)
    if (yy > my)
      k = 90 * x / (x + y);
    else
      k = 180 - (90 * x / (x + y));
  else
    if (yy > my)
      k = 360 - (90 * x / (x + y));
    else
      k = (90 * x / (x + y)) + 180;
  return k;
}

//==============================================================
//  角度による加算値設定
//  引き数：角度
//        ：水平加算値のポインタ
//        ：垂直加算値のポインタ
//==============================================================
void KXL_GetDirectionAdd(Sint16 dir, Sint16 *x, Sint16 *y) {
  Sint16 dir2 = dir + 90;

  while (dir < 0) dir += 360;
  while (dir > 360) dir -= 360;
  *x = sin360[dir];

  while (dir2 < 0) dir2 += 360;
  while (dir2 > 360) dir2 -= 360;
  *y = sin360[dir2];
}

//==============================================================
//  １６リトルビットエンディアン読み込み
//  引き数：ファイルポインタ
//  戻り値：１６ビット値
//==============================================================
Uint16 KXL_ReadU16(FILE *fp)
{
  Uint8 c[2];

  fread(c, 1, 2, fp);
  return (Uint16)(c[0] +  c[1] * 0x100);
}

//==============================================================
//  ３２ビットリトルエンディアン読み込み
//  引き数：ファイルポインタ
//  戻り値：３２ビット値
//==============================================================
Uint32 KXL_ReadU32(FILE *fp)
{
  Uint8 c[4];

  fread(c, 1, 4, fp);
  return (Uint32)(c[0] + c[1] * 0x100L + c[2] * 0x10000L + c[3] * 0x1000000L);
}

