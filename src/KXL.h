#ifndef _KXLIB_H_
#define _KXLIB_H_

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// イベントマスク
#define KXL_EVENT_NO_MASK		0L
#define KXL_EVENT_KEY_PRESS_MASK	(1L<<0)  
#define KXL_EVENT_KEY_RELEASE_MASK	(1L<<1)  
#define KXL_EVENT_BUTTON_PRESS_MASK	(1L<<2)  
#define KXL_EVENT_BUTTON_RELEASE_MASK	(1L<<3)  
#define KXL_EVENT_BUTTON_MOTION_MASK    (1L<<13)
#define KXL_EVENT_EXPOSURE_MASK		(1L<<15) 

// イベント
#define KXL_EVENT_KEY_PRESS	  2
#define KXL_EVENT_KEY_RELEASE	  3
#define KXL_EVENT_BUTTON_PRESS	  4
#define KXL_EVENT_BUTTON_RELEASE  5
#define KXL_EVENT_BUTTON_MOTION   6
#define KXL_EVENT_EXPOSE	 12

// キーコード
#define KXL_KEY_BackSpace	0xFF08
#define KXL_KEY_Tab		0xFF09
#define KXL_KEY_Return		0xFF0D
#define KXL_KEY_Escape		0xFF1B
#define KXL_KEY_Delete		0xFFFF
#define KXL_KEY_Home		0xFF50
#define KXL_KEY_Left		0xFF51
#define KXL_KEY_Up		0xFF52
#define KXL_KEY_Right		0xFF53
#define KXL_KEY_Down		0xFF54
#define KXL_KEY_F1		0xFFBE
#define KXL_KEY_F2		0xFFBF
#define KXL_KEY_F3		0xFFC0
#define KXL_KEY_F4		0xFFC1
#define KXL_KEY_F5		0xFFC2
#define KXL_KEY_F6		0xFFC3
#define KXL_KEY_F7		0xFFC4
#define KXL_KEY_F8		0xFFC5
#define KXL_KEY_F9		0xFFC6
#define KXL_KEY_F10		0xFFC7
#define KXL_KEY_F11		0xFFC8
#define KXL_KEY_F12		0xFFC9
#define KXL_KEY_Shift_L		0xFFE1
#define KXL_KEY_Shift_R		0xFFE2
#define KXL_KEY_Control_L	0xFFE3
#define KXL_KEY_Control_R	0xFFE4
#define KXL_KEY_Space           0x020
#define KXL_KEY_Exclam          0x021
#define KXL_KEY_Quotedbl        0x022
#define KXL_KEY_Numbersign      0x023
#define KXL_KEY_Dollar          0x024
#define KXL_KEY_Percent         0x025
#define KXL_KEY_Ampersand       0x026
#define KXL_KEY_Apostrophe      0x027
#define KXL_KEY_Quoteright      0x027
#define KXL_KEY_Parenleft       0x028
#define KXL_KEY_Parenright      0x029
#define KXL_KEY_Asterisk        0x02a
#define KXL_KEY_Plus            0x02b
#define KXL_KEY_Comma           0x02c
#define KXL_KEY_Minus           0x02d
#define KXL_KEY_Period          0x02e
#define KXL_KEY_Slash           0x02f
#define KXL_KEY_0               0x030
#define KXL_KEY_1               0x031
#define KXL_KEY_2               0x032
#define KXL_KEY_3               0x033
#define KXL_KEY_4               0x034
#define KXL_KEY_5               0x035
#define KXL_KEY_6               0x036
#define KXL_KEY_7               0x037
#define KXL_KEY_8               0x038
#define KXL_KEY_9               0x039
#define KXL_KEY_Colon           0x03a
#define KXL_KEY_Semicolon       0x03b
#define KXL_KEY_Less            0x03c
#define KXL_KEY_Equal           0x03d
#define KXL_KEY_Greater         0x03e
#define KXL_KEY_Question        0x03f
#define KXL_KEY_At              0x040
#define KXL_KEY_A               0x041
#define KXL_KEY_B               0x042
#define KXL_KEY_C               0x043
#define KXL_KEY_D               0x044
#define KXL_KEY_E               0x045
#define KXL_KEY_F               0x046
#define KXL_KEY_G               0x047
#define KXL_KEY_H               0x048
#define KXL_KEY_I               0x049
#define KXL_KEY_J               0x04a
#define KXL_KEY_K               0x04b
#define KXL_KEY_L               0x04c
#define KXL_KEY_M               0x04d
#define KXL_KEY_N               0x04e
#define KXL_KEY_O               0x04f
#define KXL_KEY_P               0x050
#define KXL_KEY_Q               0x051
#define KXL_KEY_R               0x052
#define KXL_KEY_S               0x053
#define KXL_KEY_T               0x054
#define KXL_KEY_U               0x055
#define KXL_KEY_V               0x056
#define KXL_KEY_W               0x057
#define KXL_KEY_X               0x058
#define KXL_KEY_Y               0x059
#define KXL_KEY_Z               0x05a
#define KXL_KEY_Bracketleft     0x05b
#define KXL_KEY_Backslash       0x05c
#define KXL_KEY_Bracketright    0x05d
#define KXL_KEY_Asciicircum     0x05e
#define KXL_KEY_Underscore      0x05f
#define KXL_KEY_Grave           0x060
#define KXL_KEY_Quoteleft       0x060
#define KXL_KEY_a               0x061
#define KXL_KEY_b               0x062
#define KXL_KEY_c               0x063
#define KXL_KEY_d               0x064
#define KXL_KEY_e               0x065
#define KXL_KEY_f               0x066
#define KXL_KEY_g               0x067
#define KXL_KEY_h               0x068
#define KXL_KEY_i               0x069
#define KXL_KEY_j               0x06a
#define KXL_KEY_k               0x06b
#define KXL_KEY_l               0x06c
#define KXL_KEY_m               0x06d
#define KXL_KEY_n               0x06e
#define KXL_KEY_o               0x06f
#define KXL_KEY_p               0x070
#define KXL_KEY_q               0x071
#define KXL_KEY_r               0x072
#define KXL_KEY_s               0x073
#define KXL_KEY_t               0x074
#define KXL_KEY_u               0x075
#define KXL_KEY_v               0x076
#define KXL_KEY_w               0x077
#define KXL_KEY_x               0x078
#define KXL_KEY_y               0x079
#define KXL_KEY_z               0x07a
#define KXL_KEY_Braceleft       0x07b
#define KXL_KEY_Bar             0x07c
#define KXL_KEY_Braceright      0x07d
#define KXL_KEY_Asciitilde      0x07e

// 変数サイズタイプ
typedef char           Sint8;
typedef unsigned char  Uint8;
typedef short          Sint16;
typedef unsigned short Uint16;
typedef long           Sint32;
typedef unsigned long  Uint32;

// フレーム構造体
typedef struct {
  Pixmap    Buffer;      // ピックスマップ
  GC        Gc;          // GC
  Uint16    Width;       // 幅
  Uint16    Height;      // 高さ
} KXL_Frame;

// ウィンドウ構造体
typedef struct {
  Display     *Display;  // ディスプレイ
  Window       Win;      // ウィンドウ
  Uint16       Scr;      // スクリーン
  XEvent       Event;    // イベント
  Uint16       Depth;    // 色深度
  GC           FontGC;   // フォントGC
  Colormap     Cmap;     // カラーマップ
  XFontStruct *WinFont;  // フォント
  Uint16       Width;    // 幅
  Uint16       Height;   // 高さ
  KXL_Frame   *Frame;    // フレームバッファ
} KXL_Window;

// イメージ構造体
typedef struct {
  Pixmap Buffer;         // イメージ
  Pixmap Mask;           // マスクイメージ
  GC     MaskGC;         // マスクGC
  Uint16 Width;          // 幅
  Uint16 Height;         // 高さ
} KXL_Image;

// 矩形構造体
typedef struct {
  Sint16 Left;           // 左
  Sint16 Top;            // 上
  Uint16 Width;          // 幅
  Uint16 Height;         // 高さ
} KXL_Rect;

// サウンド制御コマンド
typedef enum {
  KXL_SOUND_PLAY,        // 再生
  KXL_SOUND_PLAY_LOOP,   // 繰り返し再生
  KXL_SOUND_STOP,        // 停止
  KXL_SOUND_STOP_ALL     // 全停止
} KXL_Command;

// ポリゴン
typedef XPoint KXL_Polygon;

// ビットマップヘッダ情報構造体
typedef struct {
  Uint8 b, g, r, e;
} KXL_RGBE;
typedef struct {
  Uint8     magic[2];    // マジック
  Uint32    file_size;   // ファイルサイズ
  Uint16    reserved1;   // 予約
  Uint16    reserved2;   // 予約
  Uint32    offset;      // イメージへのオフセット
  Uint32    hed_size;    // 情報ヘッダサイズ
  Uint32    width;       // 幅
  Uint32    height;      // 高さ
  Uint16    plane;       // プレーン数
  Uint16    depth;       // 1ピクセルあたりのビット数
  Uint32    lzd;         // 圧縮形式
  Uint32    image_size;  // イメージサイズ
  Uint32    x_pixels;    // 水平解像度
  Uint32    y_pixels;    // 垂直解像度
  Uint32    pals;        // パレット数
  Uint32    pals2;       // 重要なパレット数
  KXL_RGBE *rgb;         // パレットデータ
  Uint32    w;           // 補正後の幅
  Uint8    *data;        // ドットデータ
} KXL_BitmapHeader;

// ビジュアル関数
void        KXL_DisplayName(Uint8 *name);
void        KXL_CreateWindow(Uint16 w, Uint16 h, Uint8 *title, Uint32 event);
void        KXL_DeleteWindow(void);
void        KXL_Font(Uint8 *str, Uint8 r, Uint8 g, Uint8 b);
void        KXL_UpDate(KXL_Rect r);
void        KXL_ReSizeFrame(Uint16 w, Uint16 h);
void        KXL_ClearFrame(KXL_Rect r);
KXL_Image  *KXL_LoadBitmap(Uint8 *filename, Uint8 blend);
KXL_Image  *KXL_CopyImage(KXL_Image *src, KXL_Rect r);
KXL_Image  *KXL_StrechImage(KXL_Image *src, Uint16 width, Uint16 height);
void        KXL_DeleteImage(KXL_Image *img);
void        KXL_PutImage(KXL_Image *img, Sint16 x, Sint16 y);
void        KXL_PutRectImage(KXL_Image *img, KXL_Rect r, Sint16 x, Sint16 y);
void        KXL_PutText(Sint16 x, Sint16 y, Uint8 *str);
Uint16      KXL_TextWidth(Uint8 *str);
Uint32      KXL_GetEvents(void);
Bool        KXL_CheckEvents(void);
Uint16      KXL_GetKey(void);
Uint16      KXL_GetButton(Uint16 *x, Uint16 *y);
void        KXL_SetDrawColor(Uint8 r, Uint8 g, Uint8 b);
void        KXL_DrawRectangle(Uint16 left, Uint16 top, Uint16 width, Uint16 height, Bool flag);
void        KXL_DrawLine(Uint16 left, Uint16 top, Uint16 right, Uint16 bottom);
void        KXL_DrawPolygon(KXL_Polygon *data, Uint16 max, Bool next, Bool flag);
void        KXL_DrawPoint(Uint16 x, Uint16 y);

// イメージ関数
void        KXL_CreateBitmap8to16(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend);
void        KXL_CreateBitmap8to24(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend);
void        KXL_CreateBitmap8to1(Uint8 *from, XImage *to, Uint8 blend);
void        KXL_ReadBitmapHeader(Uint8 *filename, KXL_BitmapHeader *hed);

// サウンド関数
void        KXL_InitSound(Uint8 *path, Uint8 **fname);
void        KXL_EndSound(void);
void        KXL_PlaySound(Uint16 no, KXL_Command action);

// その他の関数
#define     KXL_DirectionAdd(dest,cnt,add) \
              cnt += add;\
              dest = cnt >> 8
#define     KXL_ReadS16(fp) (Sint16)KXL_Read16U((fp))
#define     KXL_ReadS32(fp) (Sint32)KXL_Read16U((fp))

void       *KXL_Malloc(Uint32 size);
void       *KXL_Realloc(void *src, Uint32 size);
void        KXL_Free(void *src);
void        KXL_Timer(Uint16 time);
void        KXL_ResetTimer(void);
Bool        KXL_GetTimer(void);
Uint16      KXL_GetDirection(KXL_Rect src, KXL_Rect target);
void        KXL_GetDirectionAdd(Sint16, Sint16 *x, Sint16 *y);
Bool        KXL_RectIntersect(KXL_Rect m, KXL_Rect y);
Uint16      KXL_ReadU16(FILE *fp);
Uint32      KXL_ReadU32(FILE *fp);

#endif
