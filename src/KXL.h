#ifndef _KXLIB_H_
#define _KXLIB_H_

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

//================================================================
// Global structures and variables
//================================================================
// Event mask
#define KXL_EVENT_NO_MASK               0L
#define KXL_EVENT_KEY_PRESS_MASK        (1L<<0)
#define KXL_EVENT_KEY_RELEASE_MASK      (1L<<1)
#define KXL_EVENT_BUTTON_PRESS_MASK     (1L<<2)
#define KXL_EVENT_BUTTON_RELEASE_MASK   (1L<<3)
#define KXL_EVENT_BUTTON_MOTION_MASK    (1L<<13)
#define KXL_EVENT_EXPOSURE_MASK	        (1L<<15) 
// Event
#define KXL_EVENT_KEY_PRESS       2
#define KXL_EVENT_KEY_RELEASE     3
#define KXL_EVENT_BUTTON_PRESS    4
#define KXL_EVENT_BUTTON_RELEASE  5
#define KXL_EVENT_BUTTON_MOTION   6
#define KXL_EVENT_EXPOSE         12
// Key code
#define KXL_KEY_BackSpace       0xff08
#define KXL_KEY_Tab             0xff09
#define KXL_KEY_Return          0xff0d
#define KXL_KEY_Escape          0xff1b
#define KXL_KEY_Delete          0xffff
#define KXL_KEY_Home            0xff50
#define KXL_KEY_Left            0xff51
#define KXL_KEY_Up              0xff52
#define KXL_KEY_Right           0xff53
#define KXL_KEY_Down            0xff54
#define KXL_KEY_F1              0xffbe
#define KXL_KEY_F2              0xffbf
#define KXL_KEY_F3              0xffc0
#define KXL_KEY_F4              0xffc1
#define KXL_KEY_F5              0xffc2
#define KXL_KEY_F6              0xffc3
#define KXL_KEY_F7              0xffc4
#define KXL_KEY_F8              0xffc5
#define KXL_KEY_F9              0xffc6
#define KXL_KEY_F10             0xffc7
#define KXL_KEY_F11             0xffc8
#define KXL_KEY_F12             0xffc9
#define KXL_KEY_Shift_L	        0xffe1
#define KXL_KEY_Shift_R	        0xffe2
#define KXL_KEY_Control_L       0xffe3
#define KXL_KEY_Control_R       0xffe4
#define KXL_KEY_Space           0x020 // 
#define KXL_KEY_Exclam          0x021 // !
#define KXL_KEY_Quotedbl        0x022 // "
#define KXL_KEY_Numbersign      0x023 // #
#define KXL_KEY_Dollar          0x024 // $
#define KXL_KEY_Percent         0x025 // %
#define KXL_KEY_Ampersand       0x026 // &
#define KXL_KEY_Apostrophe      0x027 //
#define KXL_KEY_Quoteright      0x027 // 
#define KXL_KEY_Parenleft       0x028 // (
#define KXL_KEY_Parenright      0x029 // )
#define KXL_KEY_Asterisk        0x02a // *
#define KXL_KEY_Plus            0x02b // +
#define KXL_KEY_Comma           0x02c // ,
#define KXL_KEY_Minus           0x02d // -
#define KXL_KEY_Period          0x02e // .
#define KXL_KEY_Slash           0x02f // /
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
#define KXL_KEY_Colon           0x03a // :
#define KXL_KEY_Semicolon       0x03b // ;
#define KXL_KEY_Less            0x03c // <
#define KXL_KEY_Equal           0x03d // =
#define KXL_KEY_Greater         0x03e // >
#define KXL_KEY_Question        0x03f // ?
#define KXL_KEY_At              0x040 // @
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
#define KXL_KEY_Bracketleft     0x05b // "
#define KXL_KEY_Backslash       0x05c // '\'
#define KXL_KEY_Bracketright    0x05d // #
#define KXL_KEY_Asciicircum     0x05e
#define KXL_KEY_Underscore      0x05f // _
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
#define KXL_KEY_Asciitilde      0x07e // ~
// Type of variable
typedef signed char    Sint8;
typedef unsigned char  Uint8;
typedef signed short   Sint16;
typedef unsigned short Uint16;
typedef signed long    Sint32;
typedef unsigned long  Uint32;
// Image
typedef struct {
  Pixmap Buffer;         // Drawing image
  Pixmap Mask;           // Mask image
  GC     MaskGC;         // GC of mask image
  Uint16 Width;          // Drawing width
  Uint16 Height;         // Drawing height
} KXL_Image;
// Rectangle
typedef struct {
  Sint16 Left;           // Left
  Sint16 Top;            // Top
  Uint16 Width;          // Width
  Uint16 Height;         // Height
} KXL_Rect;
// Polygon
typedef XPoint KXL_Polygon;
// Sound control
typedef enum {
  KXL_SOUND_PLAY,        // Play sound
  KXL_SOUND_PLAY_LOOP,   // Loop play sound
  KXL_SOUND_STOP,        // Stop sound
  KXL_SOUND_STOP_ALL,    // Stop all sound
  KXL_SOUND_QUIT         // Quit the sound server
} KXL_Command;

//================================================================
// Internal structures
//================================================================
// Frame
typedef struct {
  Pixmap    Buffer;      // Buffer
  GC        Gc;          // GC
  Uint16    Width;       // Width
  Uint16    Height;      // Height
} KXL_Frame;
// Window
typedef struct {
  Display     *Display;  // Pointer of display structure
  Window       Win;      // window
  Uint16       Scr;      // Screen number
  XEvent       Event;    // Event of X window
  Uint16       Depth;    // Bits per pixel of X window
  Colormap     Cmap;     // Color map
  Uint16       Width;    // Width
  Uint16       Height;   // Height
  KXL_Frame   *Frame;    // Pointer of frame structure
  GC           FontGC;   // GC of font
  XFontStruct *WinFont;  // Pointer of font structrue
} KXL_Window;
// RGBE palette
typedef struct {
  Uint8 b, g, r, e;      // Blue, Green, Red, ?
} KXL_RGBE;
// Bitmap header
typedef struct {
  Uint8     magic[2];    // Bitmap magic
  Uint32    file_size;   // File size
  Uint16    reserved1;   // Reserved
  Uint16    reserved2;   // Reserved
  Uint32    offset;      // Offset of image
  Uint32    hed_size;    // Header size
  Uint32    width;       // Image width
  Uint32    height;      // Image height
  Uint16    plane;       // Plane
  Uint16    depth;       // Bits per pixel
  Uint32    lzd;         // Compression
  Uint32    image_size;  // Image size
  Uint32    x_pixels;    // Horizon resolution
  Uint32    y_pixels;    // Vertical resolution
  Uint32    pals;        // Palette
  Uint32    pals2;       // Palette
  KXL_RGBE *rgb;         // Palette data
  Uint32    w;           // Bytes per line
  Uint8    *data;        // Image data
} KXL_BitmapHeader;

//================================================================
// Internal function prototypes
//================================================================
void        KXL_Up_Date(Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h);
void        KXL_Clear_Frame(Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h);
KXL_Image  *KXL_Copy_Image(KXL_Image *src, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h);
KXL_Image  *KXL_Copy_StretchImage(KXL_Image *src, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Uint16 width, Uint16 height);
void        KXL_Put_Image(KXL_Image *img, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Sint16 x, Sint16 y);
void        KXL_Put_StretchImage(KXL_Image *img, Uint16 src_l, Uint16 src_t, Uint16 src_w, Uint16 src_h, Uint16 w, Uint16 h, Sint16 x, Sint16 y);
void        KXL_CreateBitmap8to16(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend);
void        KXL_CreateBitmap8to24(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend);
void        KXL_CreateBitmap8to1(Uint8 *from, XImage *to, Uint8 blend);
void        KXL_ReadBitmapHeader(const char *filename, KXL_BitmapHeader *hed);
Uint16      KXL_ReadU16(FILE *fp);
Uint32      KXL_ReadU32(FILE *fp);
#define     KXL_ReadS16 (Sint16)KXL_ReadU16
#define     KXL_ReadS32 (Sint32)KXL_ReadU16

//================================================================
// Global function prototypes
//================================================================
//================================================================
// Visual function prototypes
//================================================================
// Window functions
void        KXL_DisplayName(const char *name);
void        KXL_CreateWindow(Uint16 w, Uint16 h, const char *title, Uint32 event);
void        KXL_DeleteWindow(void);
// Frame functions
void        KXL_ReSizeFrame(Uint16 w, Uint16 h);
#define     KXL_UpDateRect(rr) KXL_Up_Date(rr.Left, rr.Top, rr.Width, rr.Height)
#define     KXL_UpDateImm KXL_Up_Date
#define     KXL_UpDate KXL_UpDateRect // Downward compatibility
#define     KXL_ClearFrameRect(rr) KXL_Clear_Frame(rr.Left, rr.Top, rr.Width, rr.Height)
#define     KXL_ClearFrameImm KXL_Clear_Frame
#define     KXL_ClearFrame KXL_ClearFrameRect // Downward compatibility
// Text functions
void        KXL_Font(const char *str, Uint8 r, Uint8 g, Uint8 b);
void        KXL_PutText(Sint16 x, Sint16 y, const char *str);
Uint16      KXL_TextWidth(const char *str);
// Drawing functions
void        KXL_SetDrawColor(Uint8 r, Uint8 g, Uint8 b);
void        KXL_DrawPoint(Sint16 left, Sint16 top);
void        KXL_DrawLine(Sint16 left, Sint16 top, Sint16 right, Sint16 bottom);
void        KXL_DrawRectangle(Sint16 left, Sint16 top, Uint16 width, Uint16 height, Bool flag);
void        KXL_DrawPolygon(KXL_Polygon *data, Uint16 max, Bool next, Bool flag);
// Image functions
KXL_Image  *KXL_LoadBitmap(const char *filename, Uint8 blend);
#define     KXL_CopyImageRect(src, rr) KXL_Copy_Image(src, rr.Left, rr.Top, rr.Width, rr.Height)
#define     KXL_CopyImageImm KXL_Copy_Image
#define     KXL_CopyImage KXL_CopyImageRect // Downward compatibility
#define     KXL_CopyStretchImageRect(src, rr, w, h) KXL_Copy_StretchImage(src, rr.Left, rr.Top, rr.Width, rr.Height, w, h)
#define     KXL_CopyStretchImageImm KXL_Copy_StretchImage
#define     KXL_CopyStretchImage(src, w, h) KXL_Copy_StretchImage(src, 0, 0, src->Width, src->Height, w, h)
#define     KXL_StrechImage KXL_CopyStretchImage // Downward compatibility
void        KXL_PutImage(KXL_Image *img, Sint16 x, Sint16 y);
#define     KXL_PutImageRect(src, rr, x, y) KXL_Put_Image(src, rr.Left, rr.Top, rr.Width, rr.Height, x, y)
#define     KXL_PutImageImm KXL_Put_Image
#define     KXL_PutRectImage KXL_PutImageRect // Downward compatibility
#define     KXL_PutStretchImage(src, w, h, x, y) KXL_Put_StretchImage(src, 0, 0, src->Width, src->Height, w, h, x, y)
#define     KXL_PutStretchImageRect(src, rr, w, h, x, y) KXL_Put_StretchImage(src, rr.Left, rr.Top, rr.Width, rr.Height, w, h, x, y)
#define     KXL_PutStretchImageImm KXL_Put_StretchImage
void        KXL_DeleteImage(KXL_Image *img);
// Event functions
Bool        KXL_CheckEvents(void);
Uint32      KXL_GetEvents(void);
Uint16      KXL_GetKey(void);
Uint16      KXL_GetButton(Uint16 *x, Uint16 *y);
//================================================================
// Sound function prototypes
//================================================================
void        KXL_InitSound(const char *path, char **fname);
void        KXL_EndSound(void);
void        KXL_PlaySound(Uint16 no, KXL_Command action);
//================================================================
// Etc. function prototypes
//================================================================
// Timer functions
void        KXL_Timer(Uint16 time);
void        KXL_ResetTimer(void);
Bool        KXL_GetTimer(void);
// Memory functions
void       *KXL_Malloc(Uint32 size);
void       *KXL_Realloc(void *src, Uint32 size);
void        KXL_Free(void *src);
// Game functions
Uint16      KXL_GetDirection(KXL_Rect src, KXL_Rect target);
void        KXL_GetDirectionAdd(Sint16 , Sint16 *x, Sint16 *y);
#define     KXL_DirectionAdd(dest, cnt, add) \
  cnt += add;\
  dest = cnt >> 8
#define     KXL_RectIntersect(m, y)\
  ((m.Left < y.Left + y.Width  - 1 &&\
    m.Top  < y.Top  + y.Height - 1 &&\
    y.Left < m.Left + m.Width  - 1 &&\
    y.Top  < m.Top  + m.Height - 1)\
    ? True : False)


#endif
