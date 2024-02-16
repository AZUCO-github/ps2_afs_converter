#include	<windows.h>
#include	<stdio.h>

bool	is_gim(unsigned char *);
bool	is_pac(unsigned char *);
bool	is_pss(unsigned char *);
bool	is_tim2(unsigned char *);



bool	is_gim(unsigned char *src)
{	if(	(src[0]=='G')&&
		(src[1]=='I')&&
		(src[2]=='M')	)	return(TRUE);
							return(FALSE);
}

bool	is_pac(unsigned char *src)
{	if(	(src[0]=='P')&&
		(src[1]=='A')&&
		(src[2]=='C')	)	return(TRUE);
							return(FALSE);
}

bool	is_pss(unsigned char *src)
{	if(	(src[0]==0x00)&&
		(src[1]==0x00)&&
		(src[2]==0x01)&&
		(src[3]==0xbd)	)	return(TRUE);

	if(	(src[0]==0x00)&&
		(src[1]==0x00)&&
		(src[2]==0x01)&&
		(src[3]==0xba)	)	return(TRUE);

	if(	(src[0]==0x00)&&
		(src[1]==0x00)&&
		(src[2]==0x01)&&
		(src[3]==0xb9)	)	return(TRUE);

							return(FALSE);	}

bool	is_tim2(unsigned char *src)
{	if(	(src[0]=='T')&&
		(src[1]=='I')&&
		(src[2]=='M')&&
		(src[3]=='2')	)	return(TRUE);
							return(FALSE);	}

