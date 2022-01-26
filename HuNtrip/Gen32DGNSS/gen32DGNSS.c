//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2021/11/10 �Ʀ�
//   1.�챵��g��øNtrip�D���A�ﱵ�A�թ�
//   2.�H�T�w�e�A�թ�Ntrip�D�� 32 ���I�y��
//   3.�w�p�{���ĵL�a�j��A�̧Ǩ��U�A�թ� 32 ���I�̷s�ìP��Ʀs�� 


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

static  char       savePathName[300]     = "C:/Temp/";         // �w�]�s�ɸ��|    

//////////////////////////////////////////////////////////////////////
// �D�B�z�{��

// ���j���ե�(��ܰT��)
static  void  test_Ntrip( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

// �Ұ�
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "error", MB_OK);
    return;
}

// �s�� Ntrip �D��
if( (fSocket = Ntrip_Connect( "140.121.130.145", 2101 )) == -1 )
//if( (fSocket = Ntrip_Connect( "210.241.63.193", 81 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

// �Ұ� Ntrip �A�� 
if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
//if( Ntrip_StartService( fSocket, "DGNSS", "testNTRIP", "IDT01", "idt26585858", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "get error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

//MessageBox(NULL, buff, "Ntrip Start", MB_OK);
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );

// �ǰe�y�е��D���ե�
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(1000);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// �s�ɨѰ���
out = fopen( "getData.txt", "wb" );    
fwrite( buff, getBytes, 1, out );
fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, temp, MB_OK);

// �}�l�ѽX
if( Ntrip_descode( (unsigned char *)buff, getBytes, temp ) <= 0 )
    MessageBox(NULL, "�����Ʀ��L�k��Ķ", "��Ķ���~", MB_OK);

MessageBox(NULL, temp, "OK", MB_OK);

// �_�u
Ntrip_Release(fSocket);

}

// ��g���ե�(��ܰT��)
static  void  test_Ntrip_1( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

// �Ұ�
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "error", MB_OK);
    return;
}

// �s�� Ntrip �D��
//if( (fSocket = Ntrip_Connect( "140.121.130.145", 2101 )) == -1 )
if( (fSocket = Ntrip_Connect( "210.241.63.193", 81 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

// �Ұ� Ntrip �A�� 
//if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
if( Ntrip_StartService( fSocket, "DGNSS", "testNTRIP", "IDT01", "idt26585858", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "get error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, buff, "�s�u�Ұ� Ntrip", MB_OK);
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );

// �ǰe�y�е��D���ե�
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(5000);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// �s�ɨѰ���
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, getBytes, 1, out );
//fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, "�ìP��T�^��", MB_OK);

// �}�l�ѽX
if( Ntrip1_deocder(buff, getBytes, temp) <= 0 )
    MessageBox(NULL, "�����Ʀ��L�k��Ķ", "error", MB_OK);
else
    MessageBox(NULL, temp, "�^�ǸѽX", MB_OK);

MessageBox(NULL, "End", "OK", MB_OK);

// �_�u
Ntrip_Release(fSocket);

}

// ���չA�թ� Ntrip �D��
static  void  test_Ntrip_2( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

MessageBox(NULL, "socket", "�Ұ�", MB_OK);

// �Ұ�
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "socket error", MB_OK);
    return;
}

MessageBox(NULL, "Ntrip �D��", "�s�u", MB_OK);

// �s���A�թ� Ntrip �D��
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

MessageBox(NULL, "�Ұ� Ntrip �A��", "�Ұ�", MB_OK);

// �Ұ� Ntrip �A�� 
//if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
if( Ntrip_StartService( fSocket, "FKP", "testNTRIP", "taridemo101", "rbe3g4", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "init ntrip service error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, buff, "Ntrip �Ұʦ^��", MB_OK);


MessageBox(NULL, "�ǰe���x���O", "�R�O", MB_OK);
strcpy( temp, "$GNGGA,080000.00,2512.708288,N,12132.987958,E,1,12,0,0,M,0,M,,*5F\r\n\r\n" );

// �ǰe�y�е��D���ե�
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(5000);

MessageBox(NULL, "���U�^�Ǹ��", "����", MB_OK);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, "��s�� getData.txt", "�s��", MB_OK);

// �s��
out = fopen( "getData.txt", "wb" );    
fwrite( buff, getBytes, 1, out );
fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, "�ìP��T�^��", MB_OK);

// �}�l�ѽX
//if( Ntrip1_deocder(buff, getBytes, temp) <= 0 )
//    MessageBox(NULL, "�����Ʀ��L�k��Ķ", "error", MB_OK);
//else
//    MessageBox(NULL, temp, "�^�ǸѽX", MB_OK);


// �_�u
Ntrip_Release(fSocket);

MessageBox(NULL, "OK", "����", MB_OK);

}

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
} DGNSS;

static DGNSS  dgnss32_info[] = {
    {"01","taridemo101","rbe3g4","$GNGGA,080000.00,2512.708288,N,12132.987958,E,1,12,0,0,M,0,M,,*5F\r\n\r\n"},
    {"02","taridemo102","j88hxl","$GNGGA,080000.00,2503.712428,N,12123.362518,E,1,12,0,0,M,0,M,,*51\r\n\r\n"},
    //{"03","taridemo103","69mj37","$GNGGA,080000.00,2503.712428,N,12142.964578,E,1,12,0,0,M,0,M,,*53\r\n\r\n"},
    {"04","taridemo104","7nfh5a","$GNGGA,080000.00,2450.705574,N,12108.395758,E,1,12,0,0,M,0,M,,*5F\r\n\r\n"},
    {"05","taridemo105","qbja4r","$GNGGA,080000.00,2450.705574,N,12123.332518,E,1,12,0,0,M,0,M,,*5D\r\n\r\n"},
    {"06","taridemo106","7db4n1","$GNGGA,080000.00,2450.705574,N,12140.400178,E,1,12,0,0,M,0,M,,*5C\r\n\r\n"},
    {"07","taridemo107","e7rdsi","$GNGGA,080000.00,2434.866174,N,12055.887558,E,1,12,0,0,M,0,M,,*50\r\n\r\n"},
    {"08","taridemo108","x17l7l","$GNGGA,080000.00,2434.866174,N,12111.307078,E,1,12,0,0,M,0,M,,*55\r\n\r\n"},
    {"09","taridemo109","fyt5if","$GNGGA,080000.00,2434.866174,N,12125.580958,E,1,12,0,0,M,0,M,,*50\r\n\r\n"},
    {"10","taridemo110","qg2xx2","$GNGGA,080000.00,2434.866174,N,12138.415378,E,1,12,0,0,M,0,M,,*59\r\n\r\n"},
    {"11","taridemo111","vzxy9f","$GNGGA,080000.00,2412.612774,N,12038.465898,E,1,12,0,0,M,0,M,,*55\r\n\r\n"},
    {"12","taridemo112","t9rn3e","$GNGGA,080000.00,2412.612774,N,12056.797518,E,1,12,0,0,M,0,M,,*56\r\n\r\n"},
    {"13","taridemo113","mhcwnr","$GNGGA,080000.00,2412.612774,N,12114.562498,E,1,12,0,0,M,0,M,,*50\r\n\r\n"},
    {"14","taridemo114","gghrpb","$GNGGA,080000.00,2412.612774,N,12127.731118,E,1,12,0,0,M,0,M,,*59\r\n\r\n"},
    {"15","taridemo115","qpdza9","$GNGGA,080000.00,2353.230074,N,12028.826718,E,1,12,0,0,M,0,M,,*59\r\n\r\n"},
    {"16","taridemo116","sikwbc","$GNGGA,080000.00,2353.230074,N,12045.508518,E,1,12,0,0,M,0,M,,*51\r\n\r\n"},
    {"17","taridemo117","4kxs1z","$GNGGA,080000.00,2353.230074,N,12106.664338,E,1,12,0,0,M,0,M,,*5A\r\n\r\n"},
    {"18","taridemo118","1c72x7","$GNGGA,080000.00,2353.230074,N,12122.817298,E,1,12,0,0,M,0,M,,*5D\r\n\r\n"},
    {"19","taridemo119","yslqu1","$GNGGA,080000.00,2329.294034,N,12020.039118,E,1,12,0,0,M,0,M,,*56\r\n\r\n"},
    {"20","taridemo120","pmvy2r","$GNGGA,080000.00,2329.294034,N,12041.815818,E,1,12,0,0,M,0,M,,*5E\r\n\r\n"},
    {"21","taridemo121","14nnx4","$GNGGA,080000.00,2329.294034,N,12101.813158,E,1,12,0,0,M,0,M,,*50\r\n\r\n"},
    {"22","taridemo122","l27yd2","$GNGGA,080000.00,2329.294034,N,12118.091818,E,1,12,0,0,M,0,M,,*57\r\n\r\n"},
    {"23","taridemo123","23lvdd","$GNGGA,080000.00,2306.090954,N,12017.394978,E,1,12,0,0,M,0,M,,*5C\r\n\r\n"},
    {"24","taridemo124","d3hlbv","$GNGGA,080000.00,2306.090954,N,12037.519158,E,1,12,0,0,M,0,M,,*57\r\n\r\n"},
    {"25","taridemo125","bxmd2k","$GNGGA,080000.00,2306.090954,N,12057.488238,E,1,12,0,0,M,0,M,,*5D\r\n\r\n"},
    {"26","taridemo126","wpdq4l","$GNGGA,080000.00,2306.090954,N,12114.917758,E,1,12,0,0,M,0,M,,*53\r\n\r\n"},
    {"27","taridemo127","b81pq1","$GNGGA,080000.00,2245.258294,N,12024.398838,E,1,12,0,0,M,0,M,,*52\r\n\r\n"},
    {"28","taridemo128","m3fhzw","$GNGGA,080000.00,2245.258294,N,12041.561178,E,1,12,0,0,M,0,M,,*5C\r\n\r\n"},
    {"29","taridemo129","99k47r","$GNGGA,080000.00,2245.258294,N,12056.642358,E,1,12,0,0,M,0,M,,*58\r\n\r\n"},
    {"30","taridemo130","vmakuz","$GNGGA,080000.00,2229.095674,N,12035.483298,E,1,12,0,0,M,0,M,,*5C\r\n\r\n"},
    {"31","taridemo131","4a3pnh","$GNGGA,080000.00,2229.095674,N,12048.742458,E,1,12,0,0,M,0,M,,*52\r\n\r\n"},
    {"32","taridemo132","znkd1e","$GNGGA,080000.00,2212.133620,N,12047.118678,E,1,12,0,0,M,0,M,,*50\r\n\r\n"},
};

static int siteSize = sizeof(dgnss32_info) / sizeof(DGNSS);
 
// ��@���x��ƨ��o
static void  getDGNSStoFile(char *id, char *username, char *password, char *GNGGA)
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  buff1[10000];
char  temp[3000];
int   getBytes, len;
FILE *out;

// �Ұ� socket
if( Ntrip_Initial() != 0 )
{
//MessageBox(NULL, "socket error", "message",MB_OK);
    return;
}

// �s���A�թ� Ntrip �D��
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
//MessageBox(NULL, "can not connect ntrip", "message",MB_OK);
    return;
}

// �Ұ� Ntrip �A�� 
if( Ntrip_StartService( fSocket, "FKP", "", username, password, buff ) == -1 )
{
//MessageBox(NULL, "can not start ntrip service", "message",MB_OK);
    Ntrip_Release(fSocket);
    return;
}

strcpy( temp, GNGGA );

// �ǰe�y�е��D���ե�
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    Ntrip_Release(fSocket);
//MessageBox(NULL, "send ntrip error", "message",MB_OK);
    return;
}

// ���� 15 ����
Sleep(15000);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,10000);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
//MessageBox(NULL, "ntrip no response", "message",MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// �i���_�u
Ntrip_Release(fSocket);


// �s��
strcpy(temp, savePathName);
strcat(temp,"dgnss_");
strcat(temp,id);
strcat(temp,".data");
DELE_FILE(temp);
if( (out = fopen( temp, "wb" )) != NULL )
{    
//MessageBox(NULL, "write binary file", "message",MB_OK);
    fwrite( buff, getBytes, 1, out );
    fclose( out );
}

// �[�g base64 
len = ToBase64_len(buff, buff1, getBytes);
strcpy(temp, savePathName);
strcat(temp,"base64_");
strcat(temp,id);
strcat(temp,".data");
DELE_FILE(temp);
if( (out = fopen( temp, "w" )) != NULL )
{
//MessageBox(NULL, "write binary file", "message",MB_OK);
    fwrite( buff1, len, 1, out );
    fclose( out );
}

}

// �A�թҥ�����
static void  main_go()
{
int   ii;

    // �̧Ǩ� 32 ���I�ìP���(�C��5��)�A��s C:/Temp/dgnss_01~32.dat
    for(ii=0;ii<siteSize;ii++)
    {
        getDGNSStoFile(dgnss32_info[ii].id,dgnss32_info[ii].username,dgnss32_info[ii].password,dgnss32_info[ii].GNGGA);
        // �� 1 ��
        Sleep(1000);
    }
}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // �O�d Instance
APPINSTANCE = hInstance;

// ���U�ǤJ�Ѽ�
if( lpszCmdLine!=NULL && strlen(lpszCmdLine)>0 )
{
    strcpy(savePathName, lpszCmdLine );
}

GetExePath();

// ���ե�(��ܰT��)
//test_Ntrip();             // ���Ѯ��j Ntrip
//test_Ntrip_1();           // ���Ѱ�g
//test_Ntrip_2();             // ���A�թ� Ntrip �D��

main_go();                  // �A�թҥ�����

return( 0 );

}

