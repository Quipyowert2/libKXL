#include <stdio.h>
#include <stdlib.h> // for exit
#include <sys/stat.h>
#include "KXL.h"

extern KXL_Window *KXL_Root;

//==============================================================
//  8bppのＢＭＰを16bpp化する
//  引き数：8bpsデータ
//        ：16bpsXイメージ
//        ：パレット
//==============================================================
void KXL_CreateBitmap8to16(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend)
{
  Uint32 x, y, offset, no;

  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // オフセット計算
      offset = (y * to->bytes_per_line) + (x << 1);
      no = from[y * to->width + x];
      if (no == blend) { // 指定パレット番号を黒にする
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
      } else {
        // 000rrrrr, 000ggggg, 000bbbbb
        //            |
        // gg0bbbbb, rrrrrggg
        if (!(rgb[no].r | rgb[no].g | rgb[no].b)) { // 完全な黒を無くす
          to->data[offset++] = 0x41;
          to->data[offset++] = 0x08;
        } else {
          to->data[offset++] = rgb[no].b | (rgb[no].g << 6);
          to->data[offset++] = (rgb[no].r << 3) | (rgb[no].g >> 2);
        }
      }
    }
  }
}

//==============================================================
//  8bppのＢＭＰを24bpp化する
//  引き数：8bpsデータ
//        ：24bpsXイメージ
//        ：パレット
//==============================================================
void KXL_CreateBitmap8to24(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend)
{
  Uint32 x, y, offset, no;

  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // オフセット計算
      offset = (y * to->bytes_per_line) + ((x * to->bits_per_pixel) >> 3);
      no = from[y * to->width + x];
      if (no == blend) { // 指定パレット番号を黒にする
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
      } else {
        if (!(rgb[no].r | rgb[no].g | rgb[no].b)) { // 完全な黒を無くす
          to->data[offset ++] = 0x01;
          to->data[offset ++] = 0x01;
          to->data[offset ++] = 0x01;
        } else {
          to->data[offset ++] = rgb[no].b;
          to->data[offset ++] = rgb[no].g;
          to->data[offset ++] = rgb[no].r;
        }
      }
    }
  }
}

//==============================================================
//  8bppのＢＭＰを1bpp化する
//  引き数：8bpsデータ
//        ：1bpsXイメージ
//==============================================================
void KXL_CreateBitmap8to1(Uint8 *from, XImage *to, Uint8 blend)
{
  Uint16 x, y, offset, no;
  
  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // オフセット計算
      offset = (y * to->bytes_per_line) + (x >> 3);
      no = from[y * to->width + x];
      if (no != blend)
        to->data[offset] |= 1 << (x & 7);
      else
        to->data[offset] &= ~(1 << (x & 7));
    }
  }
}

//==============================================================
//  ビットマップヘッダ情報読み込み
//  引き数：ファイル名
//        ：ヘッダ情報のポインタ
//  If the image data is NULL, that means the function has failed.
//==============================================================
void KXL_ReadBitmapHeader(const char *filename, KXL_BitmapHeader *hed)
{
  FILE *fp;
  Uint16 i, j;
  Uint8  data;

  hed->data = NULL;
  struct stat st;
  Bool stat_ok = 0;

  // ファイルを読み込み専用で開く
  if ((fp = fopen(filename,"rb")) == 0) {
    fprintf(stderr, "KXL error message\n'%s' is open error\n", filename);
    return;
  }
  int fd = fileno(fp);
  if (fd != -1)
    stat_ok = !fstat(fd, &st);
  // ヘッダ読み込み
  fread(hed->magic, 1, 2, fp);
  if (hed->magic[0] != 'B' || hed->magic[1] != 'M') {
    fprintf(stderr, "KXL error message\n'%s' is not bitmap file\n", filename);
    fclose(fp);
    return;
  }
  hed->file_size  = KXL_ReadU32(fp);
  hed->reserved1  = KXL_ReadU16(fp);
  hed->reserved2  = KXL_ReadU16(fp);
  hed->offset     = KXL_ReadU32(fp);
  hed->hed_size   = KXL_ReadU32(fp);
  hed->width      = KXL_ReadU32(fp);
  hed->height     = KXL_ReadU32(fp);
  hed->plane      = KXL_ReadU16(fp);
  hed->depth      = KXL_ReadU16(fp);
  // 4 or 8bpp 以外はサポート外
  if (hed->depth < 4 || hed->depth > 8) {
    fprintf(stderr, "KXL error message\n'%s' %dbps not support\n", 
            filename, hed->depth);
    fclose(fp);
    return;
  }
  hed->lzd        = KXL_ReadU32(fp);
  hed->image_size = KXL_ReadU32(fp);
  // イメージサイズがなければ終了
  if (hed->image_size == 0) {
    fprintf(stderr, "KXL error message\n'%s not found image size\n",
            filename);
    fclose(fp);
    return;
  }
  hed->x_pixels   = KXL_ReadU32(fp);
  hed->y_pixels   = KXL_ReadU32(fp);
  hed->pals       = KXL_ReadU32(fp);
  hed->pals2      = KXL_ReadU32(fp);
  // 使用パレット数設定
  hed->pals = hed->pals ? hed->pals : (1 << hed->depth);
  // Check if there is space for the palette.
  if (stat_ok) {
#if defined(_LP64) || defined(__LP64__)
    off_t pos = ftello(fp);
    if ((pos + 4 * (off_t)hed->pals) > st.st_size) {
#else
    long pos = ftell(fp);
    if ((pos + 4 * (long)hed->pals) > st.st_size) {
#endif
      fprintf(stderr, "KXL error message\n'%s' not found palette or truncated", filename);
      fclose(fp);
      return;
    }
  }
  // カラーマップ取得
  hed->rgb = (KXL_RGBE *)KXL_Malloc(sizeof(KXL_RGBE) * hed->pals);
  for (i = 0; i < hed->pals; i ++) {
    hed->rgb[i].b = fgetc(fp);
    hed->rgb[i].g = fgetc(fp);
    hed->rgb[i].r = fgetc(fp);
    hed->rgb[i].e = fgetc(fp);
    // Xが16bppなら補正しておく
    if (KXL_Root->Depth == 16) {
      hed->rgb[i].b /= 8;
      hed->rgb[i].g /= 8;
      hed->rgb[i].r /= 8;
    }
  }
  // 横幅を4の倍数で補正する
  hed->w = ((hed->width + 3) / 4) * 4;
  // Check if there is space for the palette.
  if (stat_ok) {
#if defined(_LP64) || defined(__LP64__)
    off_t pos = ftello(fp);
    if ((pos + (off_t)hed->image_size) > st.st_size) {
#else
    long pos = ftell(fp);
    if ((pos + (long)hed->image_size) > st.st_size) {
#endif
      fprintf(stderr, "KXL error message\n'%s' not found image data or truncated", filename);
      free(hed->rgb);
      fclose(fp);
      return;
    }
  }
  // データ領域確保
  if (hed->depth == 8)
    hed->data = (Uint8 *)KXL_Malloc(hed->image_size);
  else
    hed->data = (Uint8 *)KXL_Malloc(hed->image_size * 2);
  // データを取得する
  if (hed->depth == 8) {
    // 8bppのbmpを読み込み格納する
    for (i = 0; i < hed->height; i++) {
      // 最終ラインから読み込む
      fseek(fp, hed->offset + (hed->height - i - 1) * hed->w, 0); 
      fread(&(hed->data[i * hed->w]), hed->w, 1, fp);
    }
  } else {
    Uint32 w = (((hed->width / 2) + 3) / 4) * 4;
    // 4bppのbmpを読み込み格納する
    for (i = 0; i < hed->height; i++) {
      // 最終ラインから読み込む
      fseek(fp, hed->offset + (hed->height - i - 1) * w, 0); 
      for (j = 0; j < w; j ++) {
        data = fgetc(fp);
        hed->data[i * hed->w + j * 2 + 0] = data >> 4;
        hed->data[i * hed->w + j * 2 + 1] = data & 0x0f;
      }
    }
  }
  hed->depth = 8;
  fclose(fp);
}
