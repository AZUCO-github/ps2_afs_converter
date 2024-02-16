///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		040723
//			by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>
#include	"adx.h"

		unsigned long		file_size;
		unsigned long		file_name=0xffffffff;
extern	long				adx_bvol;
extern	unsigned long		adx_loop;
extern	unsigned long		adx_fout;
extern	unsigned long		adx_msf;			//make scale file
extern	unsigned long		adx_sc;				//scale cycle
extern	unsigned short		*adx_scl;
extern	unsigned long		scl_sso;			//scale save order
extern	char				dtsfn[MAX_PATH];	//decrypt table scale file

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);

extern	void	decode_afs			(	char *,
										unsigned long,
										HANDLE				);
extern	void	decode_aix			(	char *,
										unsigned long,
										unsigned char*		);
extern	void	decode_adx			(	char *,
										unsigned long,
										unsigned char*		);
extern	void	make_scale_file		(	char *,
										unsigned long,
										unsigned char*		);
extern	void	scale_save			(	COMPLEX_SCL *,
										HANDLE,
										unsigned long		);
extern	void	scale_load			(	COMPLEX_SCL *,
										HANDLE,
										unsigned long		);
extern	void	scale_bit_search	(	unsigned long,
										COMPLEX_SCL*,
										unsigned long,
										unsigned long,
										unsigned long,
										unsigned long,
										unsigned long		);
extern	void	vote				(	COMPLEX_VOTE *,
										COMPLEX_SCL *,
										unsigned long	);
extern	void	dbm					(	COMPLEX_SCL *,
										COMPLEX_VOTE *	);
extern	void	scale_text2bin(			unsigned short *,
										COMPLEX_SCL *,
										unsigned long,
										unsigned long	);

extern	void	ps2_rio_paradise(		COMPLEX_SCL *	);
extern	void	xb360_imas_l4u(			COMPLEX_SCL *	);



int	main(int argc,char *argv[])
{	unsigned char	*buff=NULL;
	COMPLEX_SCL		*scl=NULL;
	COMPLEX_SCL		*msc=NULL;
	COMPLEX_VOTE	*vot=NULL;
	HANDLE			hndl;
	HANDLE			hscl;
	unsigned long	tul;
	unsigned long	tul3;
	unsigned long	fnargc=0;
	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];

	unsigned long	scl_mod;			//scale analyze mode
	unsigned long	scl_sfl=8;			//scale search find length
	unsigned long	scl_ssb=0;			//scale search start bit
	unsigned long	scl_seb=15;			//scale search end bit
	unsigned long	scl_skp=0;			//scale search skip
	unsigned long	scl_spr=0;			//scale special routine
	sprintf(dtsfn,"default.scl");		//default name
					adx_bvol=0x1e49;	//ADX Base Volume

	printf("AFS Converter\n");
	printf("\t\tProg. By AZUCO\n\n");

	if(argc==1){
		printf("AFS <option ...> <file ...>\n");
		printf("<type>\t<option>\n");
		printf("AFS\t		解凍\n");
		printf("AFS\t/fn		file名の取扱い 0:ignore 1:old 2:new\n");
		printf("ADX\t/as		scale作成\n");
		printf("ADX\t		WAVE変換\n");
		printf("ADX\t/al<d>		loop回数\n");
		printf("ADX\t/af		フェードアウト指定\n");
		printf("ADX\t/bv<8X>		Base Volume\n");
		printf("SCL\t/sv		多数決\n");
		printf("SCL\t/sb		bit search\n");
		printf("SCL\t/sn		bit search (NOT)\n");
		printf("SCL\t/sk		bit search skip mode\n");
		printf("SCL\t/sl<8X>		search 報告一致長\n");
		printf("SCL\t/ss<1X>		search start bit\n");
		printf("SCL\t/se<1X>		search end bit\n");
		printf("SCL\t/so		bit > addr 順に記録\n");
		printf("   \t/ad<path>	scale file名\n");
		printf("   \t/sc<8X>		解凍用scale cycle\n");
//		printf("   \t/sp<8X>		多数決確度\n");
		printf("   \t/zr		PS2 リオパラダイス SCL\n");
		printf("   \t/zi		XBOX360 idol m@ster L4U SCL\n");
		exit(0);
	}

///////	option	///////////////////////////////////////////////////////////////
	for(tul=1;tul<argc;tul++){
		if(argv[tul][0]=='/'){
			if(	argv[tul][1]=='a' &&
				argv[tul][2]=='l'	)
				sscanf(&argv[tul][3],"%d",&adx_loop);

			if(	argv[tul][1]=='a' &&
				argv[tul][2]=='f'	)
				adx_fout=1;

			if(	argv[tul][1]=='a' &&
				argv[tul][2]=='s'	)
				adx_msf=1;

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='v'	)
				scl_mod=4;

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='n'	)
				scl_mod=2;

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='b'	)
				scl_mod=1;

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='k'	)
				scl_skp=1;

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='o'	)
				scl_sso=1;

			if(	argv[tul][1]=='a' &&
				argv[tul][2]=='d'	)
				sscanf(&argv[tul][3],"%s",dtsfn);

			if(	argv[tul][1]=='a' &&
				argv[tul][2]=='c'	)
				sscanf(&argv[tul][3],"%x",&adx_sc);

			if(	argv[tul][1]=='b' &&
				argv[tul][2]=='v'	)
				sscanf(&argv[tul][3],"%x",&adx_bvol);

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='l'	)
				sscanf(&argv[tul][3],"%x",&scl_sfl);

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='s'	)
				sscanf(&argv[tul][3],"%x",&scl_ssb);

			if(	argv[tul][1]=='s' &&
				argv[tul][2]=='e'	){
				sscanf(&argv[tul][3],"%x",&scl_seb);
				if(scl_seb>15)	scl_seb=15;				}

			if(	argv[tul][1]=='f' &&					//0	ignore file name
				argv[tul][2]=='n'	)					//1	type 1 (old ver)
				sscanf(&argv[tul][3],"%d",&file_name);	//2	type 2 (new ver)

			if(	argv[tul][1]=='z' &&
				argv[tul][2]=='r'	)
				scl_spr=1;				//PS2 リオパラダイス Special Routine

			if(	argv[tul][1]=='z' &&
				argv[tul][2]=='i'	)
				scl_spr=2;				//XBOX360 im@sL4U Special Routine
		}else{
			if(fnargc==0)	fnargc=tul;
		}
	}

//strcpy(argv[fnargc],"E:\\src\\afs\\src\\ADX\scl\\bb_02.scl");
//adx_sc=0x8000;
//scl_mod=4;

	hndl=CreateFile(argv[fnargc],
					GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hndl==(HANDLE)-1){
		printf("\tError\tsource file not found\n");
		exit(-1);	}

	file_size=GetFileSize(hndl,NULL);

	_splitpath(argv[fnargc],drive,dir,fname,ext);

	if(	strcmp(_strlwr(ext),".afs")==0){
		decode_afs (fname,file_size,hndl);
		goto esc;
	}

	if(	strcmp(_strlwr(ext),".aix")==0){
		buff=new unsigned char[file_size];
		ReadFile(hndl,buff,file_size,&tul,NULL);
		decode_aix(fname,file_size,buff);
		goto esc;
	}

	if(	strcmp(_strlwr(ext),".adx")==0){
		buff=new unsigned char[file_size];
		ReadFile(hndl,buff,file_size,&tul,NULL);

		if(adx_msf==1){
				make_scale_file	(fname,file_size,buff);
		}else{
				hscl=CreateFile(dtsfn,GENERIC_READ,0,0,OPEN_EXISTING,
													FILE_ATTRIBUTE_NORMAL,0);
				if(hscl!=(HANDLE)-1){
					tul3=GetFileSize(hscl,NULL)/COMPLEX_SCL_SIZE;
					scl=new COMPLEX_SCL[tul3+16];
					scale_load(scl,hscl,tul3);
					CloseHandle(hscl);
if(scl_spr==1)	ps2_rio_paradise(scl);
if(scl_spr==2)	xb360_imas_l4u(scl);
					if(adx_sc==0)			adx_sc=tul3/16;		}

				if(adx_sc==0)			adx_sc=0x100;
				adx_scl=new unsigned short[adx_sc];
				memset(adx_scl,0,2*adx_sc);

				if(scl!=NULL){
					scale_text2bin(adx_scl,scl,adx_sc,tul3);
					delete[]	scl;	scl=NULL;
				}

				decode_adx		(fname,file_size,buff);
				if(adx_scl!=NULL){	delete[]	adx_scl;	adx_scl=NULL;	}
		}
		goto esc;
	}

	if(	strcmp(_strlwr(ext),".scl")==0){
		if(scl_mod==1){
			tul3=file_size/COMPLEX_SCL_SIZE;
			scl=new COMPLEX_SCL[tul3+16];
			scale_load(scl,hndl,tul3);
			scale_bit_search(tul3,scl,scl_sfl,scl_ssb,scl_seb,0,scl_skp);

			buff=new unsigned char[MAX_PATH];
			sprintf((char *)buff,"%s.scl",fname);
			hscl=NonOverWriteCreateFile((LPCSTR)buff);
			if(hscl==(HANDLE)-1){
				printf("\tError\tcan't create wav file\n");
				exit(-1);									}
			scale_save(scl,hscl,tul3);
			goto esc;									}

		if(scl_mod==2){
			tul3=file_size/COMPLEX_SCL_SIZE;
			scl=new COMPLEX_SCL[tul3+16];
			scale_load(scl,hndl,tul3);
			scale_bit_search(tul3,scl,scl_sfl,scl_ssb,scl_seb,1,scl_skp);

			buff=new unsigned char[MAX_PATH];
			sprintf((char *)buff,"%s.scl",fname);
			hscl=NonOverWriteCreateFile((LPCSTR)buff);
			if(hscl==(HANDLE)-1){
				printf("\tError\tcan't create wav file\n");
				exit(-1);									}
			scale_save(scl,hscl,tul3);
			goto esc;									}

		if(scl_mod==4){
			vot=new COMPLEX_VOTE[adx_sc*16];
			msc=new COMPLEX_SCL[adx_sc*16];
			memset(vot,0,sizeof(COMPLEX_VOTE)*adx_sc*16);
			memset(msc,0,sizeof(COMPLEX_SCL )*adx_sc*16);
			CloseHandle(hndl);

			for(tul=1;tul<argc;tul++){
				if(argv[tul][0]!='/'){
					_splitpath(argv[tul],drive,dir,fname,ext);
					if(	strcmp(_strlwr(ext),".scl")==0){
						hscl=CreateFile(argv[tul],GENERIC_READ,0,0,
										OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
printf("%08X %s\n",tul,argv[tul]);
						if(hscl==(HANDLE)-1)	goto next_vote;
						tul3=GetFileSize(hscl,NULL)/COMPLEX_SCL_SIZE;
						scl=new COMPLEX_SCL[tul3+16];
						scale_load(scl,hscl,tul3);
						CloseHandle(hscl);
						vote(vot,scl,tul3);
						if(scl!=NULL){	delete[]	scl;	scl=NULL;	}
next_vote:				NULL;
					}
				}
			}
			dbm(msc,vot);
			if(vot!=NULL){	delete[]	vot;	vot=NULL;	}

			buff=new unsigned char[MAX_PATH];
			hscl=NonOverWriteCreateFile((LPCSTR)dtsfn);
			if(hscl==(HANDLE)-1){
				printf("\tError\tcan't create SCL file\n");
				exit(-1);									}
			scale_save(msc,hscl,adx_sc*16);
			CloseHandle(hscl);
			if(msc!=NULL){	delete[]	msc;	msc=NULL;	}
		}

		goto esc;
	}
esc:
	CloseHandle(hndl);
	if(scl!=NULL){	delete[]	scl;	scl=NULL;	}
	if(buff!=NULL){	delete[]	buff;	buff=NULL;	}
}
