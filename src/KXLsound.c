#include <stdlib.h> // for exit
#include <string.h> // for memset, memcpy
#include <unistd.h> // for pipe, close
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/soundcard.h>
#include <linux/limits.h> // for PATH_MAX
#ifdef USE_PULSEAUDIO
#include <pulse/simple.h>
#include <pulse/error.h>
#endif
#include "KXL.h"

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX_SOUNDS_PLAYING   16
#define SND_BLOCK_SIZE      4096

typedef struct {
  Bool         Active; // active
  KXL_Command  Action; // command
  Uint16       No;     // number
  Uint8       *Pos;    // position
  Uint32       Cnt;    // count
  Uint32       Length; // length
} KXL_SoundControl;

struct {
  Uint16           ListCnt;
  Sint32           ID;
#ifdef USE_PULSEAUDIO
  int              Pipe[2];
  pa_simple       *Device;
#else
  Sint32           Pipe[2];
  Sint32           Device;
#endif
  Uint16           PlayCnt;
  KXL_SoundControl PlaySound[MAX_SOUNDS_PLAYING];
  Sint8            FBuff[SND_BLOCK_SIZE];
  Sint32           LBuff[SND_BLOCK_SIZE];
  Uint8            PBuff[SND_BLOCK_SIZE];
} KXL_SoundData;

typedef struct {
  Uint8  *Data;
  Uint32  Length;
} KXL_WaveList;

KXL_WaveList *KXL_wavelist;

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
  Sint32 max_sample_size = 4096;
  Sint32 fragment_size = 4096;
  Sint32 arg;
  Uint8 *sample_ptr;

  // command initialize
  for (i = 0; i < MAX_SOUNDS_PLAYING; i ++)
    KXL_SoundData.PlaySound[i].Active = False;
  KXL_SoundData.PlayCnt = 0;

  // receive data clear
  FD_ZERO(&sound_fdset);
  FD_SET(KXL_SoundData.Pipe[0], &sound_fdset);
  
  // loop
  while (1) {
    FD_SET(KXL_SoundData.Pipe[0], &sound_fdset);
    select(KXL_SoundData.Pipe[0] + 1, &sound_fdset, NULL, NULL, NULL);
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
    } else if (Command.Action == KXL_SOUND_QUIT) {
      for (i = 0; i < MAX_SOUNDS_PLAYING; i ++)
        KXL_SoundData.PlaySound[i].Active = False;
      KXL_SoundData.PlayCnt = 0;
      break;
    }
    
    if (KXL_SoundData.PlayCnt || Command.Active == True) {
      struct timeval delay = {0, 0};
#ifndef USE_PULSEAUDIO
      if (!KXL_SoundData.PlayCnt) {
        // set fragment
        arg = 0x00020009;
        if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SETFRAGMENT, &arg) >= 0) {
          // set format
          arg = AFMT_U8;
          if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SETFMT, &arg) >= 0) {
            // set monaural
            arg = 0;
            if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_STEREO, &arg) >= 0) {
              // set sampling rate
              arg = 8000;
              if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_SPEED, &arg) >= 0) {
                // get block size
                if (ioctl(KXL_SoundData.Device, SNDCTL_DSP_GETBLKSIZE, &fragment_size) >= 0) {
                  max_sample_size = fragment_size;
                }
              }
            }
          }
        }
      }
#endif
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
#ifdef USE_PULSEAUDIO
        int error = 0;
        pa_simple_write(KXL_SoundData.Device, KXL_SoundData.PBuff, fragment_size, &error);
        if (error)
          fprintf(stderr, "KXL error message\nfailed to write sound data: %s\n", pa_strerror(error));
#else
        write(KXL_SoundData.Device, KXL_SoundData.PBuff, fragment_size);
#endif
      }
    }
  }
#ifdef USE_PULSEAUDIO
  if (KXL_SoundData.Device) {
    int error;
    pa_simple_drain(KXL_SoundData.Device, &error);
    if (error)
      fprintf(stderr, "KXL error message\nfailed to drain sound data: %s\n", pa_strerror(error));
    pa_simple_free(KXL_SoundData.Device);
  }
#endif
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
  memset(&SendCommand, 0, sizeof(SendCommand));
  SendCommand.Active = True;
  SendCommand.Action = action;
  SendCommand.No     = no;
  SendCommand.Cnt    = 0;
  if (action == KXL_SOUND_STOP_ALL) {
    SendCommand.Pos = 0;
    SendCommand.Length = 0;
  } else {
    SendCommand.Pos    = KXL_wavelist[no].Data;
    SendCommand.Length = KXL_wavelist[no].Length;
  }
  write(KXL_SoundData.Pipe[1], &SendCommand, sizeof(SendCommand));
}

//==============================================================
//  サウンドファイル読み込み
//  引き数：ディレクトリ
//        ：ファイル名
//==============================================================
KXL_WaveList KXL_LoadSound(const char *path, const char *fname)
{
  KXL_WaveList new;
  char filename[PATH_MAX];
  Uint32 length;
  FILE *file;
  Uint32 i;
  Uint8 dummy[40];
  
  snprintf(filename, sizeof(filename), "%s/%s.wav", path, fname);
  if ((file = fopen(filename,"r")) == NULL) {
    fprintf(stderr, "KXL error message\nKXL_LoadSound : '%s/%s.wav' open error\n",
            path, fname);
    new.Data = 0;
    return new;
  }
  fread(dummy, sizeof(Uint8), 40, file);
  new.Length = KXL_ReadU32(file);
  new.Data = (Uint8 *)KXL_Malloc(new.Length);
  fread(new.Data, sizeof(Uint8), new.Length, file);
  fclose(file);
  for (i = 0; i < new.Length; i ++) new.Data[i] ^= 0x80;
  return new;
}

//==============================================================
//  サウンドファイル読み込み
//  引き数：ディレクトリ
//        ：ファイル名リスト
//==============================================================
void KXL_LoadSoundData(const char *path, char **fname)
{
  Uint16 i, max = 0;
  
  while (fname[max][0]) max ++;
  KXL_wavelist = (KXL_WaveList *)KXL_Malloc(sizeof(KXL_WaveList ) * max);
  for (i = 0; i < max; i ++)
    KXL_wavelist[i] = KXL_LoadSound(path, fname[i]);
  KXL_SoundData.ListCnt = max;
}

//==============================================================
//  サウンドサーバー初期化
//  引き数：ディレクトリ
//        ：ファイル名リスト
//==============================================================
void KXL_InitSound(const char *path, char **fname)
{
  Uint16 i = 0;
  KXL_SoundOk = False;

  KXL_LoadSoundData(path, fname);
  for (i = 0;i < KXL_SoundData.ListCnt;i++)
    if (KXL_wavelist[i].Data == NULL)
      return;
#ifndef USE_PULSEAUDIO
  // device check
  if ((KXL_SoundData.Device = open("/dev/dsp", O_WRONLY)) == -1) {
    fprintf(stderr, "KXL error message\nnot found sound card\n");
    return;
  }
#endif
  // create pipe
  if (pipe(KXL_SoundData.Pipe) < 0) {
    fprintf(stderr, "KXL error message\npipe error\n");
    return;
  }
  // create child process
  if ((KXL_SoundData.ID = fork()) < 0) {
    fprintf(stderr, "KXL error message\nfork error\n");
    return;
  }

  if (!KXL_SoundData.ID) { // child
    close(KXL_SoundData.Pipe[1]);
#ifdef USE_PULSEAUDIO
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_U8,
    ss.channels = 1;
    ss.rate = 8000;
    int error = 0;
    // device check
    KXL_SoundData.Device = pa_simple_new(NULL,
                                         "Geki2",
                                         PA_STREAM_PLAYBACK,
                                         NULL,
                                         "Music",
                                         &ss,
                                         NULL,
                                         NULL,
                                         &error
                                         );
    if (!KXL_SoundData.Device || error) {
      fprintf(stderr, "KXL error message\nnot found sound card\n");
      return;
    }
#endif
    KXL_SoundServer();
    exit(-1);
  } else { // parents
    close(KXL_SoundData.Pipe[0]);
    KXL_SoundOk = True;
  }
}

//==============================================================
//  サウンドサーバー終了
//==============================================================
void KXL_EndSound(void)
{
#ifndef USE_PULSEAUDIO
  if (KXL_SoundData.Device != -1)
    close(KXL_SoundData.Device);
#endif
  if (KXL_SoundOk == True) {
    KXL_PlaySound(0, KXL_SOUND_QUIT);
    int pid, status;
    pid = waitpid(KXL_SoundData.ID, &status, 0);
    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status))
        fprintf(stderr, "KXL error message\nsound server exited with status %d", WEXITSTATUS(status));
    }
    else
      fprintf(stderr, "KXL error message\nsound server terminated abnormally");
  }
  while (KXL_SoundData.ListCnt)
    KXL_Free(KXL_wavelist[-- KXL_SoundData.ListCnt].Data);
  KXL_Free(KXL_wavelist);
}

