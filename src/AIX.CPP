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



/*****メモ*****AIX フォーマット************************************************
0x00		'AIXF'
0x04		AIXFのデータ長-8
0x20		AIXのデータの始まりのアドレス
0x24		AIXのデータの長さ
0x28		1トラックのサンプル数
0x2C		サンプリングレート
0x40		トラック数

0x00		'AIXP'
0x04		次のAIXPまでの距離-8
0x08		トラック番号
0x09		全トラック数
0x0A		AIXPのデータの長さ
0x0C		フレーム位置(0xffffffff=start/0xfffffffe=end)
0x10		データの始まり
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