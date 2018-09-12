#include <stdio.h>
#include "KXL.h"

extern KXL_Window *KXL_Root;

//==============================================================
//  8bpp�Σ£ͣФ�16bpp������
//  ��������8bps�ǡ���
//        ��16bpsX���᡼��
//        ���ѥ�å�
//==============================================================
void KXL_CreateBitmap8to16(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend)
{
  Uint32 x, y, offset, no;

  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // ���ե��åȷ׻�
      offset = (y * to->bytes_per_line) + (x << 1);
      no = from[y * to->width + x];
      if (no == blend) { // ����ѥ�å��ֹ����ˤ���
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
      } else {
        // 000rrrrr, 000ggggg, 000bbbbb
        //            |
        // gg0bbbbb, rrrrrggg
        if (!(rgb[no].r | rgb[no].g | rgb[no].b)) { // �����ʹ���̵����
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
//  8bpp�Σ£ͣФ�24bpp������
//  ��������8bps�ǡ���
//        ��24bpsX���᡼��
//        ���ѥ�å�
//==============================================================
void KXL_CreateBitmap8to24(Uint8 *from, XImage *to, KXL_RGBE *rgb, Uint8 blend)
{
  Uint32 x, y, offset, no;

  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // ���ե��åȷ׻�
      offset = (y * to->bytes_per_line) + ((x * to->bits_per_pixel) >> 3);
      no = from[y * to->width + x];
      if (no == blend) { // ����ѥ�å��ֹ����ˤ���
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
        to->data[offset ++] = 0x00;
      } else {
        if (!(rgb[no].r | rgb[no].g | rgb[no].b)) { // �����ʹ���̵����
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
//  8bpp�Σ£ͣФ�1bpp������
//  ��������8bps�ǡ���
//        ��1bpsX���᡼��
//==============================================================
void KXL_CreateBitmap8to1(Uint8 *from, XImage *to, Uint8 blend)
{
  Uint16 x, y, offset, no;
  
  for (y = 0; y < to->height; y ++) {
    for (x = 0; x < to->width; x ++) {
      // ���ե��åȷ׻�
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
//  �ӥåȥޥåץإå������ɤ߹���
//  ���������ե�����̾
//        ���إå�����Υݥ���
//==============================================================
void KXL_ReadBitmapHeader(Uint8 *filename, KXL_BitmapHeader *hed)
{
  FILE *fp;
  Uint16 i, j;
  Uint8  data;

  // �ե�������ɤ߹������Ѥǳ���
  if ((fp = fopen(filename,"rb")) == 0) {
    fprintf(stderr, "KXL error message\n'%s' is open error\n", filename);
    exit(1);
  }
  // �إå��ɤ߹���
  fread(hed->magic, 1, 2, fp);
  if (hed->magic[0] != 'B' || hed->magic[1] != 'M') {
    fprintf(stderr, "KXL error message\n'%s' is not bitmap file\n", filename);
    exit(1);
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
  // 4 or 8bpp �ʳ��ϥ��ݡ��ȳ�
  if (hed->depth < 4 || hed->depth > 8) {
    fprintf(stderr, "KXL error message\n'%s' %dbps not support\n", 
            filename, hed->depth);
    exit(1);
  }
  hed->lzd        = KXL_ReadU32(fp);
  hed->image_size = KXL_ReadU32(fp);
  // ���᡼�����������ʤ���н�λ
  if (hed->image_size == 0) {
    fprintf(stderr, "KXL error message\n'%s not found image size\n",
            filename);
    exit(1);
  }
  hed->x_pixels   = KXL_ReadU32(fp);
  hed->y_pixels   = KXL_ReadU32(fp);
  hed->pals       = KXL_ReadU32(fp);
  hed->pals2      = KXL_ReadU32(fp);
  // ���ѥѥ�åȿ�����
  hed->pals = hed->pals ? hed->pals : (1 << hed->depth);
  // ���顼�ޥå׼���
  hed->rgb = (KXL_RGBE *)KXL_Malloc(sizeof(KXL_RGBE) * hed->pals);
  for (i = 0; i < hed->pals; i ++) {
    hed->rgb[i].b = fgetc(fp);
    hed->rgb[i].g = fgetc(fp);
    hed->rgb[i].r = fgetc(fp);
    hed->rgb[i].e = fgetc(fp);
    // X��16bpp�ʤ��������Ƥ���
    if (KXL_Root->Depth == 16) {
      hed->rgb[i].b /= 8;
      hed->rgb[i].g /= 8;
      hed->rgb[i].r /= 8;
    }
  }
  // ������4���ܿ�����������
  hed->w = ((hed->width + 3) / 4) * 4;
  // �ǡ����ΰ����
  if (hed->depth == 8)
    hed->data = (Uint8 *)KXL_Malloc(hed->image_size);
  else
    hed->data = (Uint8 *)KXL_Malloc(hed->image_size * 2);
  // �ǡ������������
  if (hed->depth == 8) {
    // 8bpp��bmp���ɤ߹��߳�Ǽ����
    for (i = 0; i < hed->height; i++) {
      // �ǽ��饤�󤫤��ɤ߹���
      fseek(fp, hed->offset + (hed->height - i - 1) * hed->w, 0); 
      fread(&(hed->data[i * hed->w]), hed->w, 1, fp);
    }
  } else {
    Uint32 w = (((hed->width / 2) + 3) / 4) * 4;
    // 4bpp��bmp���ɤ߹��߳�Ǽ����
    for (i = 0; i < hed->height; i++) {
      // �ǽ��饤�󤫤��ɤ߹���
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
