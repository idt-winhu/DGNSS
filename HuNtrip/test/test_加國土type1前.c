
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "D:\apez\weblib\common\publ.h"
#include "D:\APEZ\WebLib\HuNtrip\huNtrip.h"


//////////////////////////////////////////////////////////////////////
// �t���ܼ�

static  HINSTANCE  APPINSTANCE     = NULL;     // �{�� INSTANCE
static  char       EXEPATH[300]    = "";

//////////////////////////////////////////////////////////////////////
// �ǤJ�Ѽ�
static  char    sysServerIp[100] = "140.121.130.145";
static  int     sysServerPort    = 2101;
static  char    sysSiteName[100] = "test";
static  char    sysProgName[100] = "testNTRIP";
static  char    sysUserID[100]   = "admin";
static  char    sysPassword[100] = "password";
static  double  sysLAT           = 25.047298;
static  double  sysLNG           = 121.33918;
static  char    sysOutFile[100]  = "response.txt";


//////////////////////////////////////////////////////////////////////
// �D�B�z�{��

// ���ե�(��ܰT��)
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

//MessageBox(NULL, buff, "Ntrip Start", MB_OK);

// �ǰe�y�е��D���ե�
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );
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


// ������(����ܡA�s��)
static void  main_go()
{
int    ret = 0;
int    fSocket;
char   buff[10000];
char   temp[3000], temp1[300];
int    getBytes;
FILE  *out;

// �Ұ�
ret = Ntrip_Initial();
if( ret != 0 )
    return;

// �s�� Ntrip �D��
if( (fSocket = Ntrip_Connect( sysServerIp, sysServerPort )) == -1 )
    return;

// �Ұ� Ntrip �A�� 
if( Ntrip_StartService( fSocket, sysSiteName, sysProgName, sysUserID, sysPassword, buff ) == -1 )
{
    Ntrip_Release(fSocket);
    return;
}

// �ǰe�y�е��D���ե�
strcpy( temp, "$GPGGA,004806," );
STR_D( sysLAT*100, temp1, 9, 4 );
strcat( temp, temp1 );
strcat( temp, ",N," );
STR_D( sysLNG*100, temp1, 9, 3 );
strcat( temp, temp1 );
strcat( temp, ",E,4,10,1,200,M,1,M,3,0*\r\n\r\n" );
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    Ntrip_Release(fSocket);
    return;
}

Sleep(1000);

// Ū�� Ntrip �^��(���ɬ��@�ʥ])
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    Ntrip_Release(fSocket);
    return;
}

// �}�l�ѽX
Ntrip_descode( (unsigned char *)buff, getBytes, temp ); 

// �^�Ǹ�Ʀs��
strcpy( temp1, EXEPATH );
strcat( temp1, sysOutFile );
out = fopen( temp1, "wb" );    
fwrite( temp, strlen(temp), 1, out );
fclose( out );

// �_�u
Ntrip_Release(fSocket);


}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // �O�d Instance
APPINSTANCE = hInstance;

// ���U�ǤJ�Ѽ�
if( (ii=cAT(',',lpszCmdLine)) != -1 )
{
    strncpy( sysServerIp, lpszCmdLine, ii );
    sysServerIp[ii] = '\0';

    jj = jj+ii+1;
    if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
    {
        strncpy( temp, lpszCmdLine+jj, ii );
        temp[ii] = '\0';
        sysServerPort = VAL_L(temp);

        jj = jj+ii+1;
        if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
        {
            strncpy( sysSiteName, lpszCmdLine+jj, ii );
            sysSiteName[ii] = '\0';

            jj = jj+ii+1;
            if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
            {
                strncpy( sysProgName, lpszCmdLine+jj, ii );
                sysProgName[ii] = '\0';

                jj = jj+ii+1;
                if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
                {
                    strncpy( sysUserID, lpszCmdLine+jj, ii );
                    sysUserID[ii] = '\0';

                    jj = jj+ii+1;
                    if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
                    {
                        strncpy( sysPassword, lpszCmdLine+jj, ii );
                        sysPassword[ii] = '\0';

                        jj = jj+ii+1;
                        if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
                        {
                            strncpy( temp, lpszCmdLine+jj, ii );
                            temp[ii] = '\0';
                            sysLAT = VAL_D(temp);

                            jj = jj+ii+1;
                            if( (ii=cAT(',', lpszCmdLine+jj)) != -1 )
                            {
                                strncpy( temp, lpszCmdLine+jj, ii );
                                temp[ii] = '\0';
                                sysLNG = VAL_D(temp);

                                jj = jj+ii+1;
                                strcpy(sysOutFile, lpszCmdLine+jj);
                            }
                        }
                    }
                }
            }
        }
    }
}


GetExePath();

// ���ե�(��ܰT��)
test_Ntrip();

// ������(����ܡA�s��)
//main_go();

return( 0 );

}

