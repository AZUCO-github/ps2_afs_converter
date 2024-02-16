///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		AFS format
//			041204
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

extern	bool	is_pss		(unsigned char *);
extern	bool	is_pac		(unsigned char *);
extern	bool	is_icp		(unsigned char *);
extern	bool	is_adx		(unsigned char *,unsigned long);
extern	bool	is_tim2		(unsigned char *);
extern	bool	is_gim		(unsigned char *);
extern	bool	is_fpk		(unsigned char *);
extern	bool	is_bin		(unsigned char *);
extern	void	decode_adx	(unsigned char *,char *);
extern	void	decode_bin	(unsigned char *,char *,unsigned long);
		void	decode_afs	(	char *,
								unsigned long,
								HANDLE				);
extern	unsigned long		file_size;
extern	unsigned long		file_name;
extern	HANDLE	NonOverWriteCreateFile(LPCSTR);



/*****メモ*****AFS フォーマット************************************************
0x00		'AFS',0
0x04		AFS内のアイテム数
0x08		最初のアイテム（FIAと表記する）
0x??+0x00	アイテムのアドレス
0x??+0x04	アイテムの長さ

FIA-0x08	アイテムテーブルのアドレス
FIA-0x04	アイテムテーブルの長さ

アイテムテーブル内アイテム管理情報
長さ0x30固定
0x00		アイテム名(通常NULL終端だが0x20を超えるものは違う)
0x20		よくわからんｗ
アイテムのアドレスは含まれないと思われ
******************************************************************************/



void	decode_afs(	char 			*fname_,
					unsigned long	file_size,
					HANDLE hndl					)
{	unsigned char	*IAT;		//Item Address Table
	unsigned char	*IIT;		//Item Infomation Table
	unsigned long	IMX;		//Item MaX
	unsigned long	INO;		//Item NO.
	unsigned long	IDA;		//Item Data Address
	unsigned long	ILN;		//Item LeNgth
	char			INB[0x100];	//Item Name Buffer

	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];
	HANDLE			hIT;
	unsigned char	*bIT=NULL;

	unsigned long	tul;



	ReadFile(hndl,INB,0x08,&tul,NULL);	//AFS + Item MaX
	IMX=*(unsigned long*)(INB+4);

	IAT=new unsigned char[(IMX+1)*8];
	ReadFile(hndl,IAT,(IMX+1)*8,&tul,NULL);

	if(file_name==0xffffffff){					//無指定
		tul=*(unsigned long*)(IAT)-0x08;
		SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
		ReadFile(hndl,INB,0x04,&tul,NULL);		//一番最初のアイテムの前
		tul=*(unsigned long*)(INB);
		if(tul==0)	file_name=2;				//old ?
		else		file_name=1;
	}

	switch(file_name){
		case	0:	//ファイル名テーブルを無視する
			IIT=NULL;
			break;
		case	1:	//Type 1 (old ver)
			tul=*(unsigned long*)(IAT)-0x08;
			SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
			ReadFile(hndl,INB,0x04,&tul,NULL);	//一番最初のアイテムの前

			tul=*(unsigned long*)(INB);
			SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
			IIT=new unsigned char[IMX*0x30];
			ReadFile(hndl,IIT,IMX*0x30,&tul,NULL);
			break;
		case	2:	//Type 2 (new ver)
			tul=*(unsigned long*)(IAT+0x08*IMX);
			SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
			IIT=new unsigned char[IMX*0x30];
			ReadFile(hndl,IIT,IMX*0x30,&tul,NULL);
			break;
	}



	for(INO=0;INO<IMX;INO++){
		IDA=*(unsigned long*)(IAT+(INO<<3));
		ILN=*(unsigned long*)(IAT+(INO<<3)+4);

		switch(file_name){
			case	0:	//ファイル名テーブルを無視する
				sprintf(INB,"%s_%04X.BIN",fname_,INO);	break;
			case	1:	//Type 1 (old ver)
			case	2:	//Type 2 (new ver)
				memcpy(INB,IIT+0x30*INO,0x20);
				INB[0x21]=0x00;
				INB[0x22]=0x00;
				break;
		}

		SetFilePointer(hndl,IDA,NULL,FILE_BEGIN);
		bIT=NULL;
		bIT=new unsigned char[ILN];
		ReadFile (hndl,bIT,ILN,&tul,NULL);

		_splitpath(INB,drive,dir,fname,ext);
		if(is_pss (bIT)		&&(strcmp(_strlwr(ext),".pss")!=0))
													sprintf(INB,"%s.pss",INB);
		if(is_adx (bIT,ILN)	&&(strcmp(_strlwr(ext),".adx")!=0))
													sprintf(INB,"%s.adx",INB);
		if(is_tim2(bIT)		&&(strcmp(_strlwr(ext),".tm2")!=0))
													sprintf(INB,"%s.tm2",INB);
		if(is_tim2(bIT+2)	&&(strcmp(_strlwr(ext),".tm2")!=0))
													sprintf(INB,"%s.tm2",INB);
		if(is_pac (bIT)		&&(strcmp(_strlwr(ext),".pac")!=0))
													sprintf(INB,"%s.pac",INB);
		if(is_gim (bIT)		&&(strcmp(_strlwr(ext),".gim")!=0))
													sprintf(INB,"%s.gim",INB);

		printf("No.%04X %08X %s\n",INO,ILN,INB);
		hIT=NonOverWriteCreateFile((LPCSTR)INB);
		if(hIT==(HANDLE)-1){
			printf("\tError\tcan't create bin file\n");
			exit(-1);	}

		WriteFile(hIT ,bIT,ILN,&tul,NULL);

		CloseHandle(hIT);
		if(bIT!=NULL)	delete[]	bIT;
	}
}
