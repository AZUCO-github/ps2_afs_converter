///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		ADX format
//			0407022
//				by	AZUCO
//
//		ADXコンバート部分についてはBEROさんのソースを改変
//			adv2wav
//			(c)2001 BERO
//			http://www.geocities.co.jp/Playtown/2004/
//			bero@geocities.co.jp
//			adx info from: http://ku-www.ss.titech.ac.jp/~yatsushi/adx.html
//
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>
#include	"le.h"
#include	"adx.h"

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);

		long				adx_bvol;
		unsigned long		adx_loop;
		unsigned long		adx_fout;
		unsigned long		adx_cunt;
		unsigned long		adx_msf;			//make scale file
		unsigned long		adx_sc=0x8000;		//scale cycle
		char				dtsfn[MAX_PATH];	//decrypt table scale file

		bool	is_adx(unsigned char *,unsigned long);
		void	decode_adx		(char *,unsigned long,unsigned char *);
		void	make_scale_file	(char *,unsigned long,unsigned char *);

		unsigned short		*adx_scl;



void	convert(short *out,unsigned char *in,PREV *prev)
{
//	int		scale=ler2(in)^ler2((unsigned char*)&adx_scl[adx_cunt%adx_sc]);
	int		scale=ler2(in)^adx_scl[adx_cunt%adx_sc];
	int		i,d,s0;
	int		s1=prev->s1;
	int		s2=prev->s2;

//printf("%04X\n",adx_scl[adx_cunt%adx_sc]);
adx_cunt++;

	in+=2;
	for(i=0;i<16;i++) {
		d=in[i]>>4;
			if(d&0x08)	d-=0x10;
			s0=(d*scale*adx_bvol +ADXK0*s1 -ADXK1*s2)>>14;
			if(s0> 0x7fff)	s0= 0x7fff;
			if(s0<-0x7fff)	s0=-0x7fff;
			*out++=s0;	s2=s1;	s1=s0;

		d=in[i]&0x0f;
			if(d&0x08)	d-=0x10;
			s0=(d*scale*adx_bvol +ADXK0*s1 -ADXK1*s2)>>14;
			if(s0> 0x7fff)	s0= 0x7fff;
			if(s0<-0x7fff)	s0=-0x7fff;
			*out++=s0;	s2=s1;	s1=s0;
	}
	prev->s1=s1;
	prev->s2=s2;
}



void	decode_adx(char *name,unsigned long size_,unsigned char *src)
{
	char			tfn[0x100];
	unsigned long	tul1=0;
	unsigned long	tul2=0;
	unsigned long	tul3=0;
	short			tss1=0;
	unsigned long	p=(ler4(src)&0x7fffffff)+4;
	unsigned long	q=0;
	unsigned long	adx_lsel=0;

	unsigned long		sample_max			=ler4(src+0x0c);
	unsigned long		sample_loop_start;
	unsigned long		sample_loop_end;
	unsigned long		sample_loop;
	unsigned long		sample_coda;
	unsigned char		*wav;
	short				buf[32*4];
	int					size=sample_max;
	PREV				prev[2];
	WAVHDR				wavhdr;
	HANDLE				hwav;
	HANDLE				hscl;

adx_cunt=0;

/*****メモ*********************************************************************
	ループがある場合は、セクタ先頭にパッディングしないのでpが特定値になる
******************************************************************************/
	switch(*(src+0x12)){										//adx ver?
		case	0x03:	sample_loop_start	=	ler4(src+0x1c);
						sample_loop_end		=	ler4(src+0x24);
						break;
		default:		printf("unknown adx ver\n");
		case	0x04:	sample_loop_start	=	ler4(src+0x28);
						sample_loop_end		=	ler4(src+0x30);
						break;
	}
						sample_loop			=	sample_loop_end + 1 -
												sample_loop_start;
						sample_coda			=	sample_max -
												sample_loop_end;


//	if(adx_loop==0)	adx_loop=1;

		sprintf(wavhdr.hriff,	"RIFF");
		sprintf(wavhdr.hwave,	"WAVEfmt ");
		sprintf(wavhdr.hdata,	"data");
				wavhdr.hdrsize		=0x10;
				wavhdr.format		=1;		//PCM
				wavhdr.bits			=16;

				wavhdr.channel		=*(src+0x07);
				wavhdr.freq			=ler4(src+0x08);
				wavhdr.blocksize	=wavhdr.channel<<1;
				wavhdr.byte_per_sec =wavhdr.freq*wavhdr.blocksize;
	if(adx_loop==0)
				wavhdr.datasize		=wavhdr.blocksize*
										sample_max;
	else
		if(adx_fout==1)
				wavhdr.datasize		=wavhdr.blocksize*(
										sample_loop_start +
										sample_loop * adx_loop +
										(0x8000<<FOUT) );
		else	wavhdr.datasize		=wavhdr.blocksize*(
										sample_loop_start +
										sample_loop * adx_loop +
										sample_coda );
				wavhdr.totalsize	=wavhdr.datasize+sizeof(wavhdr)-8;

	wav=new unsigned char[(sample_max+0x20)*wavhdr.channel*2];



	sprintf(tfn,"%s.wav",name);
	hwav=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hwav==(HANDLE)-1){
		printf("\tError\tcan't create wav file\n");
		exit(-1);									}
	WriteFile(hwav,&wavhdr,sizeof(WAVHDR),&tul1,NULL);

	prev[0].s1 = 0;
	prev[0].s2 = 0;
	prev[1].s1 = 0;
	prev[1].s2 = 0;

	switch(wavhdr.channel){
		case	1:
			while(size) {
				convert((short *)(wav+q),		src+p,		prev);
				p+=18;	q+=64;
				if(size>32)	size-=32;
					else	size=0;
			}
			break;
		case	2:
			while(size) {
				convert(buf,		src+p,		prev);
				convert(buf+0x20,	src+p+18,	prev+1);
				for(tul1=0;tul1<32;tul1++){
					*(short *)(wav+q+tul1*4)	=buf[tul1];
					*(short *)(wav+q+tul1*4+2)	=buf[tul1+0x20];	}
				p+=36;	q+=128;
				if(size>32)	size-=32;
					else	size=0;
			}
			break;
	}

	if(adx_loop==0){
			tul1=WriteFile(hwav,wav,sample_max*2*wavhdr.channel,&p,NULL);
	}else{
			WriteFile(hwav,wav,sample_loop_start*2*wavhdr.channel,&p,NULL);
		for(tul1=0;tul1<adx_loop;tul1++)
			WriteFile(hwav,wav+p,sample_loop*2*wavhdr.channel,&q,NULL);

		if(adx_fout==1){
			q=0;
			switch(wavhdr.channel){
				case	1:
					for(tul1=(0x8000<<FOUT);tul1!=0;tul1--){
						*(short *)(wav+p+q  )	=
						*(short *)(wav+p+q  ) * (tul1>>FOUT) / 0x8000;
						q+=2;												}
					WriteFile(hwav,wav+p,(0x8000<<FOUT)*2*wavhdr.channel,
																	&q,NULL);
					break;
				case	2:
					for(tul1=(0x8000<<FOUT);tul1!=0;tul1--){
						*(short *)(wav+p+q  )	=
						*(short *)(wav+p+q  ) * (tul1>>FOUT) / 0x8000;
						*(short *)(wav+p+q+2)	=
						*(short *)(wav+p+q+2) * (tul1>>FOUT) / 0x8000;
						q+=4;												}
					WriteFile(hwav,wav+p,(0x8000<<FOUT)*2*wavhdr.channel,
																	&q,NULL);
					break;
			}
		}else{
			WriteFile(hwav,wav+p+q,sample_coda*2*wavhdr.channel,&q,NULL);
		}
	}
	CloseHandle(hwav);
	delete[]	wav;
}



void	make_scale_file(char *name,unsigned long size_,unsigned char *src)
{
	char			tfn[0x100];
	unsigned long	tx,ty,tz;
	unsigned long	tul1=0;
	unsigned long	p=(ler4(src)&0x7fffffff)+4;
	HANDLE			hscl;
	int				size=ler4(src+0x0c);

	sprintf(tfn,"%s.scl",name);
	hscl=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hscl==(HANDLE)-1){
		printf("\tError\tcan't create scl file\n");
		exit(-1);									}

	tx=0;

	switch(*(src+0x07)){
		case	1:
			while(size) {
				tz=ler2(src+p   );
				for(ty=0;ty<16;ty++){
					sprintf(tfn,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X\n",
						tz,tx,ty,(tz>>(15-ty))&1,0,1,0,0);
					WriteFile(hscl,tfn,strlen(tfn),&tul1,NULL);				}

				p+=18;	tx++;
				if(size>32)	size-=32;
					else	size=0;
			}
			break;
		case	2:
			while(size) {
				tz=ler2(src+p   );
				for(ty=0;ty<16;ty++){
					sprintf(tfn,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X\n",
						tz,tx  ,ty,(tz>>(15-ty))&1,0,1,0,0);
					WriteFile(hscl,tfn,strlen(tfn),&tul1,NULL);				}

				tz=ler2(src+p+18);
				for(ty=0;ty<16;ty++){
					sprintf(tfn,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X\n",
						tz,tx+1,ty,(tz>>(15-ty))&1,0,1,0,0);
					WriteFile(hscl,tfn,strlen(tfn),&tul1,NULL);				}

				p+=36;	tx+=2;
				if(size>32)	size-=32;
					else	size=0;
			}
			break;
	}

	CloseHandle(hscl);
}



bool	is_adx(unsigned char *src,unsigned long size)
{	unsigned long	p=(ler4(src)&0x7fffffff)-2;

	if(size<=p)				return(FALSE);

	if(	(src[p+0]=='(')&&
		(src[p+1]=='c')&&
		(src[p+2]==')')&&
		(src[p+3]=='C')&&
		(src[p+4]=='R')&&
		(src[p+5]=='I')	)	return(TRUE);
							return(FALSE);
}
