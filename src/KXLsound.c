#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include "KXL.h"

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX_SOUNDS_PLAYING   16
#define SND_BLOCK_SIZE      4096

typedef struct {
  Bool         Active;
  KXL_Command  Action;
  Uint16       No;
  Uint8       *Pos;
  Uint32       Cnt;
  Uint32       Length;
} KXL_SoundControl;

struct {
  Uint16           ListCnt;
  Sint32           ID;
  Sint32           Pipe[2];
  Sint32           Device;
  Uint16           PlayCnt;
  KXL_SoundControl PlaySound[MAX_SOUNDS_PLAYING];
  Sint8            FBuff[SND_BLOCK_SIZE];
  Sint32           LBuff[SND_BLOCK_SIZE];
  Uint8            PBuff[SND_BLOCK_SIZE];
} KXL_SoundData;

typedef struct {
  Uint8  *Pos;
  Uint32  Length;
} KXL_WaveList;

KXL_WaveList **KXL_wavelist;

Bool KXL_SoundOk;

//==============================================================
//  サウンドサーバー
//==============================================================
void KXL_SoundServer(void)
{
  Uint16 i;
  KXL_SoundControl Command;
  fd_set sound_fdset;
  Sint32 sample_size;
  Sint32 max_sample_size;
  Sint32 fragment_size;
  Sint32 arg;
  Uint8 *sample_ptr;

  // command initialize
  close(KXL_SoundData.Pipe[1]);
  for (i = 0; i < MAX_SOUNDS_PLAYING; i ++)
    KXL_SoundData.PlaySound[i].Active = False;
  KXL_SoundData.PlayCnt = 0;
  
  FD_ZERO(&sound_fdset);
  FD_SET(KXL_SoundData.Pipe[0], &sound_fdset);
  
  // loop
  while (1) {
    FD_SET(KXL_SoundData.Pipe[0], &sound_fdset);
    select(KXL_SoundData.Pipe[0]+1, &sound_fdset, NULL, NULL, NULL);
    if (!FD_ISSET(KXL_SoundData.Pipe[0], &sound_fdset)) 
      continue;
    if (read(KXL_SoundData.Pipe[0], &Command,sizeof(Command)) != sizeof(Command)) 
      exit(-1);
    if (Command.Action == KXL_SOUND_STOP_ALL) { // all stop
      if (!KXL_SoundData.PlayCnt)
	continue;
      for (i = 0; i < MAX_SOUNDS_PLAYING; i ++)
        KXL_SoundData.PlaySound[i].Active = False;
      KXL_SoundData.PlayCnt = 0;
      continue;
    } else if (Command.Action == KXL_SOUND_STOP) { // no. stop
      if (!KXL_SoundData.PlayCnt)
	continue;
      for (i = 0; i < MAX_SOUNDS_PLAYING; i ++)
        if (KXL_SoundData.PlaySound[i].No == Command.No) {
          KXL_SoundData.PlaySound[i].Active = False;
          KXL_SoundData.PlayCnt --;
        }
      continue;
    }

    if (KXL_SoundData.PlayCnt || Command.Active == True) {
      struct timeval delay = {0, 0};
      if (!KXL_SoundData.PlayCnt) {
	arg = 0x00020009;
	if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SETFRAGMENT, &arg) >= 0) {
	  arg = AFMT_U8;
	  if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SETFMT, &arg) >= 0) {
	    arg = 0;
	    if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_STEREO, &arg) >= 0) {
	      arg = 8000;
	      if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SPEED, &arg) >= 0) {
		if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_GETBLKSIZE, &fragment_size) >= 0) {
		  max_sample_size = fragment_size;
		}
	      }
	    }
	  }
	}
      }
      if (Command.Active == True && KXL_SoundData.PlayCnt < MAX_SOUNDS_PLAYING) {
	for (i = 0; i < MAX_SOUNDS_PLAYING; i ++) {
	  if (KXL_SoundData.PlaySound[i].Active == False) {
	    KXL_SoundData.PlaySound[i] = Command;
	    KXL_SoundData.PlayCnt ++;
	    break;
	  }
	}
      }
      while (KXL_SoundData.PlayCnt &&
	     select(KXL_SoundData.Pipe[0]+1, &sound_fdset, NULL, NULL, &delay) < 1) {
	FD_SET(KXL_SoundData.Pipe[0], &sound_fdset);
	memset(KXL_SoundData.LBuff, 0, fragment_size * sizeof(Sint32));
	for (i = 0; i < MAX_SOUNDS_PLAYING; i ++) {
	  Uint16 j;
	  if (KXL_SoundData.PlaySound[i].Active == False)
	    continue;
	  sample_ptr = KXL_SoundData.PlaySound[i].Pos+KXL_SoundData.PlaySound[i].Cnt;
	  sample_size = MIN(max_sample_size, KXL_SoundData.PlaySound[i].Length - KXL_SoundData.PlaySound[i].Cnt);
	  KXL_SoundData.PlaySound[i].Cnt += sample_size;
	  memcpy(KXL_SoundData.FBuff, sample_ptr, sample_size);
	  if (KXL_SoundData.PlaySound[i].Action == KXL_SOUND_PLAY_LOOP &&
	      sample_size < max_sample_size) {
	    KXL_SoundData.PlaySound[i].Cnt = max_sample_size - sample_size;
	    memcpy(KXL_SoundData.FBuff + sample_size, KXL_SoundData.PlaySound[i].Pos, max_sample_size - sample_size);
	    sample_size = max_sample_size;
	  }
	  for (j = 0; j < sample_size; j ++)
	    KXL_SoundData.LBuff[j] += (Sint32)KXL_SoundData.FBuff[j];
	  if (KXL_SoundData.PlaySound[i].Cnt >= KXL_SoundData.PlaySound[i].Length) {
	    if (KXL_SoundData.PlaySound[i].Action == KXL_SOUND_PLAY_LOOP)
	      KXL_SoundData.PlaySound[i].Cnt = 0;
	    else {
	      KXL_SoundData.PlaySound[i].Active = False;
	      KXL_SoundData.PlayCnt --;
	    }
	  }
	}
	for (i = 0; i < fragment_size; i ++) {
	  if (KXL_SoundData.LBuff[i] < -255)
	    KXL_SoundData.PBuff[i] = 0;
	  else if (KXL_SoundData.LBuff[i] > 255)
	    KXL_SoundData.PBuff[i] = 255;
	  else 
	    KXL_SoundData.PBuff[i] = (KXL_SoundData.LBuff[i] >> 1) ^0x80;
	}
	write(KXL_SoundData.Device, KXL_SoundData.PBuff, fragment_size);
      }
    }
  }
}

//==============================================================
//  サウンド再生
//  引き数：サウンド番号
//        ：コマンド
//==============================================================
void KXL_PlaySound(Uint16 no, KXL_Command action)
{
  KXL_SoundControl SendCommand;

  if (KXL_SoundOk == False)
    return;
  SendCommand.Active = True;
  SendCommand.Action = action;
  SendCommand.No     = no;
  SendCommand.Pos    = KXL_wavelist[no]->Pos;
  SendCommand.Cnt    = 0;
  SendCommand.Length = KXL_wavelist[no]->Length;
  write(KXL_SoundData.Pipe[1], &SendCommand, sizeof(SendCommand));
}

//==============================================================
//  サウンドファイル読み込み
//  引き数：ディレクトリ
//        ：ファイル名
//==============================================================
KXL_WaveList *KXL_LoadSound(Uint8 *path, Uint8 *fname)
{
  KXL_WaveList *new;
  Uint8 filename[256];
  Uint32 length;
  FILE *file;
  Uint32 i;
    
  new = (KXL_WaveList *)KXL_Malloc(sizeof(KXL_WaveList));
  sprintf(filename,"%s/%s.wav", path, fname);
  if ((file = fopen(filename,"r")) == NULL) {
    fprintf(stderr, "KXL error message\nKXL_LoadSound : '%s/%s.wav' open error\n",
	    path, fname);
    return 0;
  }
  for (i = 0; i < 40; i ++)
    fgetc(file);
  new->Length = KXL_ReadU32(file);

  new->Pos = (Uint8 *)KXL_Malloc(new->Length);

  fread(new->Pos, new->Length, 1, file);
  fclose(file);
  for (i = 0; i < new->Length; i ++)
    new->Pos[i] = new->Pos[i] ^0x80;
  return new;
}

//==============================================================
//  サウンドファイル読み込み
//  引き数：ディレクトリ
//        ：ファイル名リスト
//==============================================================
void KXL_LoadSoundData(Uint8 *path, Uint8 **fname)
{
  Uint16 i, max = 0;
  
  while (fname[max][0])
    max ++;
  KXL_wavelist = (KXL_WaveList **)KXL_Malloc(sizeof(KXL_WaveList *) * max);
  for (i = 0; i < max; i ++)
    KXL_wavelist[i] = KXL_LoadSound(path, fname[i]);
  KXL_SoundData.ListCnt = max;
}

//==============================================================
//  サウンドサーバー初期化
//  引き数：ディレクトリ
//        ：ファイル名リスト
//==============================================================
void KXL_InitSound(Uint8 *path, Uint8 **fname)
{
  KXL_SoundOk = False;
  KXL_LoadSoundData(path, fname);
  if (pipe(KXL_SoundData.Pipe) < 0) {
    fprintf(stderr, "KXL error message\npipe error\n");
    return;
  }
  if ((KXL_SoundData.ID = fork()) < 0) {
    fprintf(stderr, "KXL error message\nfork error\n");
    return;
  }
  KXL_SoundOk = True;
  if (!KXL_SoundData.ID) {
    if ((KXL_SoundData.Device = open("/dev/dsp", O_WRONLY)) != -1) {
      KXL_SoundServer();
      exit(-1);
    } else {
      KXL_SoundOk = False;
    }
  } else {
    close(KXL_SoundData.Pipe[0]);
  }
  if (KXL_SoundOk == False)
    fprintf(stderr, "KXL error message\nKXL_InitSoundServer : no sounds\n");
}

//==============================================================
//  サウンドサーバー終了
//==============================================================
void KXL_EndSound(void)
{
  if (KXL_SoundOk == False)
    return;
  KXL_PlaySound(0, KXL_SOUND_STOP_ALL);
  close(KXL_SoundData.Device);
  while (KXL_SoundData.ListCnt)
    KXL_Free(KXL_wavelist[-- KXL_SoundData.ListCnt]);
  KXL_Free(KXL_wavelist);
  kill(KXL_SoundData.ID, SIGTERM);
}
