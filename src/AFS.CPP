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



/*****����*****AFS �t�H�[�}�b�g************************************************
0x00		'AFS',0
0x04		AFS���̃A�C�e����
0x08		�ŏ��̃A�C�e���iFIA�ƕ\�L����j
0x??+0x00	�A�C�e���̃A�h���X
0x??+0x04	�A�C�e���̒���

FIA-0x08	�A�C�e���e�[�u���̃A�h���X
FIA-0x04	�A�C�e���e�[�u���̒���

�A�C�e���e�[�u�����A�C�e���Ǘ����
����0x30�Œ�
0x00		�A�C�e����(�ʏ�NULL�I�[����0x20�𒴂�����͈̂Ⴄ)
0x20		�悭�킩���
�A�C�e���̃A�h���X�͊܂܂�Ȃ��Ǝv���
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

	if(file_name==0xffffffff){					//���w��
		tul=*(unsigned long*)(IAT)-0x08;
		SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
		ReadFile(hndl,INB,0x04,&tul,NULL);		//��ԍŏ��̃A�C�e���̑O
		tul=*(unsigned long*)(INB);
		if(tul==0)	file_name=2;				//old ?
		else		file_name=1;
	}

	switch(file_name){
		case	0:	//�t�@�C�����e�[�u���𖳎�����
			IIT=NULL;
			break;
		case	1:	//Type 1 (old ver)
			tul=*(unsigned long*)(IAT)-0x08;
			SetFilePointer(hndl,tul,NULL,FILE_BEGIN);
			ReadFile(hndl,INB,0x04,&tul,NULL);	//��ԍŏ��̃A�C�e���̑O

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
			case	0:	//�t�@�C�����e�[�u���𖳎�����
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