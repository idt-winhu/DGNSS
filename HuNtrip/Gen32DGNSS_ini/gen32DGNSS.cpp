//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2021/11/10 �Ʀ�
//   1.�챵��g��øNtrip�D���A�ﱵ�A�թ�
//   2.�H�T�w�e�A�թ�Ntrip�D�� 32 ���I�y��
//   3.�w�p�{���ĵL�a�j��A�̧Ǩ��U�A�թ� 32 ���I�̷s�ìP��Ʀs��
// --------------------------------------------------------------------------
//   4.�����P Gen32DGNSS �ؿ����P�b�H ini �ǤJ�A���w�n���A�թҦ󯸥x�ìP���   


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "D:\apez\weblib\common\publ.h"
#include "D:\APEZ\WebLib\RtkLib\RtkLib.h"
#include "D:\APEZ\WebLib\HuNtrip\huNtrip.h"
#include "D:\APEZ\WebLib\HuNtrip\huNtrip1.h"

//////////////////////////////////////////////////////////////////////
// �t���ܼ�

static  HINSTANCE  APPINSTANCE     = NULL;     // �{�� INSTANCE
static  char       EXEPATH[300]    = "";

static  char       iniFileName[100]     = "gen32DGNSS.ini";         // �w�]���x ini �W     


//////////////////////////////////////////////////////////////////////
// �D�B�z�{��

static  void  GetExePath( void )
{
char       temp[100];
int        ii;

GetModuleFileName( NULL, temp, 100 );
for( ii=strlen(temp);ii>=0;ii-- )
    if( temp[ii] == '\\' )
        break;
temp[ii+1] = '\0';
strcpy( EXEPATH, temp );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �H�U�����������{��(�W���O���ե�)

// �w�q 32 �ո��
typedef struct DGNSS_prop
{
    char  id[100];
    char  username[100];
    char  password[100];
    char  GNGGA[300];
	char  savepath[300];
} DGNSS;

static DGNSS  dgnss32_info;
 
// ��@���x��ƨ��o
static BOOL  getDGNSStoFile(char *id, char *username, char *password, char *GNGGA, char *savepath)
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  buff1[10000];
char  buff2[10000];
char  temp[3000];
int   getBytes, len;
FILE *out;

// �Ұ� socket
if( Ntrip_Initial() != 0 )
{
//MessageBox(NULL, "socket error", "message",MB_OK);
    return FALSE;
}

// �s���A�թ� Ntrip �D��
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
//MessageBox(NULL, "can not connect ntrip", "message",MB_OK);
    return FALSE;
}

// �Ұ� Ntrip �A�� 
if( Ntrip_StartService( fSocket, "FKP", "", username, password, buff ) == -1 )
{
//MessageBox(NULL, "can not start ntrip service", "message",MB_OK);
    Ntrip_Release(fSocket);
    return FALSE;
}

strcpy( temp, GNGGA );
strcat( temp, "\r\n\r\n");

// �ǰe�y�е��D���ե�
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    Ntrip_Release(fSocket);
//MessageBox(NULL, "send ntrip error", "message",MB_OK);
    return FALSE;
}

// ���� 10 ����
Sleep(15000);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,10000);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
//MessageBox(NULL, "ntrip no response", "message",MB_OK);
    Ntrip_Release(fSocket);
    return FALSE;
}

// �i���_�u
Ntrip_Release(fSocket);

// �s��
strcpy(temp, savepath);
strcat(temp,"_dgnss_");
strcat(temp,id);
strcat(temp,".data");
DELE_FILE(temp);
//MessageBox(NULL, "write binary file", "message",MB_OK);
//Sleep(1000);
if( (out = fopen( temp, "wb" )) != NULL )
{    
	//MessageBox(NULL, "write binary file", "message",MB_OK);
    fwrite( buff, getBytes, 1, out );
    fclose( out );
}

// �[�g base64 
len = ToBase64_len(buff, buff1, getBytes);
//MessageBox(NULL, "convert base64", "message",MB_OK);
strcpy(temp,savepath);
strcat(temp,"_base64_");
strcat(temp,id);
strcat(temp,".data");
DELE_FILE(temp);
//MessageBox(NULL, "write base64 file", "message",MB_OK);
//Sleep(1000);
if( (out = fopen( temp, "w" )) != NULL )
{
	//MessageBox(NULL, "convert base64", "message",MB_OK);
    fwrite( buff1, len, 1, out );
    fclose( out );
}

return TRUE;
}

// �A�թҥ�����
static void  main_go()
{
int   ii;
char  temp[1000];

while(1)
{
    // �ѪR ini �ɤ��Ѽ�
    strcpy( temp, EXEPATH );
    strcat( temp, iniFileName );

    GetPrivateProfileString( "SYS-INFO", "id",       "", dgnss32_info.id,        99, temp );
    GetPrivateProfileString( "SYS-INFO", "username", "", dgnss32_info.username,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "password", "", dgnss32_info.password,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "GNGGA",    "", dgnss32_info.GNGGA,    299, temp );
    GetPrivateProfileString( "SYS-INFO", "savepath", "", dgnss32_info.savepath, 299, temp );

	try
    {
        if( getDGNSStoFile(dgnss32_info.id, dgnss32_info.username, dgnss32_info.password, dgnss32_info.GNGGA, dgnss32_info.savepath) == FALSE )
			Sleep(30000);
		else        // �� 1 ��
            Sleep(1000);
	}
    catch(...)
    {
        Sleep(30000);
    }
}

}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // �O�d Instance
APPINSTANCE = hInstance;


// ���U�ǤJ�ѼơA�ǤJ ini �ɫ��ܯ��x
if( lpszCmdLine!=NULL && strlen(lpszCmdLine)>0 )
{
    strcpy(iniFileName, lpszCmdLine );
}

GetExePath();

main_go();        

return( 0 );

}

