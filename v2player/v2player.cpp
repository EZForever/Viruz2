#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../tool/file.h"
#include "../libv2.h"
#include "../sounddef.h"
#include "../v2mconv.h"
#include "../v2mplayer.h"

static V2MPlayer player;

int main(int argc, char** argv)
{
  puts("Farbrausch V2 Music Player v0.dontcare");
  puts("Code and Synthesizer (W) 2000-2008 kb/Farbrausch\n");

  char* inputpath;
  if (argc < 2)
  {
    // infer file name from self base name
    auto inputpathsize = strlen(argv[0]) + 4 + 1; // worst case scenario size
    inputpath = (char*)malloc(inputpathsize);
    if (!inputpath)
    {
      return 1;
    }
    strcpy_s(inputpath, inputpathsize, argv[0]);

    auto p = strrchr(inputpath, '.');
    if (p)
    {
      *p = 0;
    }
    strcat_s(inputpath, inputpathsize, ".v2m");
  }
  else
  {
    inputpath = _strdup(argv[1]);
  }

  fileS input;
  if(!input.open(inputpath, fileS::rd | fileS::ex))
  {
    fprintf(stderr, "error: could not open %s\n\n", inputpath);
    free(inputpath);
    return 1;
  }

  auto inputsize = input.size();
  auto inputbuf = new unsigned char[inputsize];
  if (input.read(inputbuf, inputsize) != inputsize)
  {
    fprintf(stderr, "error: could not read %s\n\n", inputpath);
    delete[] inputbuf;
    input.close();
    free(inputpath);
    return 1;
  }
  input.close();

  unsigned char* outputbuf;
  int outputsize;
  sdInit();
  ConvertV2M(inputbuf, inputsize, &outputbuf, &outputsize);
  sdClose();
  delete[] inputbuf;
  if (!outputbuf)
  {
    fprintf(stderr, "error: bad v2m format in %s\n\n", inputpath);
    free(inputpath);
    return 1;
  }

  player.Init();
  player.Open(outputbuf);
  dsInit(player.RenderProxy, &player, GetConsoleWindow());

  printf("now playing: %s\n", inputpath);
  puts("press ESC to quit\n");
  player.Play();
  while (player.IsPlaying() && GetAsyncKeyState(VK_ESCAPE) >= 0)
  {
    Sleep(10);
  }

  delete[] outputbuf;
  free(inputpath);
  return 0;
}

