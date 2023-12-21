#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "../tool/file.h"
#include "../sounddef.h"
#include "../v2mconv.h"

extern int patchesused[128];


int main(int argc, const char **argv)
{
	if (argc<2)
	{
		printf("\nCONV2M (C) kb^fr 2003\n\nUsage: conv2m <infile> [outfile]\n\n");
		return 1;
	}

	const char *infile=argv[1];
	char *outfile;

	sdInit();

	if (argc<3)
	{
		outfile=new char[strlen(infile)+5];
		const char *pos=strrchr(infile,'.');
		if (pos)
		{
			strncpy(outfile,infile,pos-infile);
			strcpy(outfile+(pos-infile),"_new");
			strcat(outfile,pos);
		}
		else
		{
			strcpy(outfile,infile);
			strcat(outfile,"_new");
		}		
	}
	else
	{
    const char *of=argv[2];
		outfile=new char[strlen(of)+1];
		strcpy(outfile,of);
	}

	fileS in;
	in.open(infile);
	fileM inmemf;
	inmemf.open(in);
	sU32 inlen=inmemf.size();
	sU8 *inptr=(sU8*)inmemf.detach();
	in.close();


	unsigned char *outptr;
	unsigned char *outptr2;
	int outlen;

	ConvertV2M(inptr,inlen,&outptr,&outlen);

  printf("exporting patches...\n");
	const unsigned char *pdata[128];
  sU32 np=GetV2MPatchData(inptr,inlen,&outptr2,pdata);

	for (sU32 i=0; i<np; i++) if (patchesused[i])
	{
		//sInt p=(v2curpatch+i)%128;
		//memcpy(soundmem+128*4+v2soundsize*p,v2mpatches[i],v2soundsize);

    char buf[4096] = { 0 };
		sprintf(buf,"%s.%03d.v2p",infile,i);

    char* p = strrchr(buf, '\\');
    if (!p)
    {
      p = strrchr(buf, '/');
    }
    if (!p)
    {
      p = buf;
    }
    if (*p == '\\' || *p == '/')
    {
      p++;
    }
    if (strlen(p) >= 32)
    {
      p[31] = 0;
    }

    printf("writing %s...\n", buf);
    fileS out;
    out.open(buf,fileS::wr|fileS::cr);

    out.puts("v2p1");
	  out.write(p,32);

    out.putsU32(v2version);
	  out.write(pdata[i],v2soundsize);

    out.close();
	}
	
  
	printf("writing %s...\n",outfile);
	fileS out;
	out.open(outfile,fileS::wr|fileS::cr);
	out.write(outptr,outlen);
	out.close();

	delete[] outptr;
	delete[] outptr2;
	delete[] outfile;
  sdClose();
}
