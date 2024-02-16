///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		AIX format
//			080316
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>
#include	"le.h"

		void	decode_aix	(	char *,
								unsigned long,
								unsigned char*		);
extern	unsigned long		file_size;
extern	unsigned long		file_name;
extern	HANDLE	NonOverWriteCreateFile(LPCSTR);



/*****����*****AIX �t�H�[�}�b�g************************************************
0x00		'AIXF'
0x04		AIXF�̃f�[�^��-8
0x20		AIX�̃f�[�^�̎n�܂�̃A�h���X
0x24		AIX�̃f�[�^�̒���
0x28		1�g���b�N�̃T���v����
0x2C		�T���v�����O���[�g
0x40		�g���b�N��

0x00		'AIXP'
0x04		����AIXP�܂ł̋���-8
0x08		�g���b�N�ԍ�
0x09		�S�g���b�N��
0x0A		AIXP�̃f�[�^�̒���
0x0C		�t���[���ʒu(0xffffffff=start/0xfffffffe=end)
0x10		�f�[�^�̎n�܂�
******************************************************************************/



void	decode_aix(	char			*name,
					unsigned long	size_,
					unsigned char	*src	)
{	unsigned long	TMX=*(src+0x40);	//Track MaX
	unsigned long	TNO;				//Track NO.
	unsigned long	TDA;				//Track Data Address
	unsigned long	TLN;				//Track data LeNgth
	unsigned long	p;
	char			nADX[MAX_PATH];
	HANDLE			hADX;
	unsigned long	TUL1;

	for(TNO=0;TNO<TMX;TNO++){
		p=ler4(src+0x20);
		sprintf(nADX,"%s_%02X.adx",name,TNO);
		hADX=NonOverWriteCreateFile((LPCSTR)nADX);
TRACK_CHECK:
		if(*(src+p+0x08)!=TNO){
			p+=ler4(src+p+0x04)+8;
			goto	TRACK_CHECK;
		}

		WriteFile(hADX,src+p+0x10,ler2(src+p+0x0a),&TUL1,NULL);

		if(ler4(src+p+0x0c)!=0xfffffffe){
			p+=ler4(src+p+0x04)+8;
			goto	TRACK_CHECK;
		}else{
			CloseHandle(hADX);
		}
	}
}
