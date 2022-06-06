
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "D:\DGNSS\common\publ.h"


#include "D:\DGNSS\RtkLib\RtkLib.h"
#include "D:\DGNSS\HuNtrip\huNtrip.h"
#include "D:\DGNSS\HuNtrip\huNtrip1.h"

//////////////////////////////////////////////////////////////////////
// 系統變數

static  HINSTANCE  APPINSTANCE     = NULL;     // 程式 INSTANCE
static  char       EXEPATH[300]    = "";

//////////////////////////////////////////////////////////////////////
// 傳入參數
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
// 主處理程序

// 海大測試用(顯示訊息)
static  void  test_Ntrip( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "error", MB_OK);
    return;
}

// 連結 Ntrip 主機
if( (fSocket = Ntrip_Connect( "140.121.130.145", 2101 )) == -1 )
//if( (fSocket = Ntrip_Connect( "210.241.63.193", 81 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
//if( Ntrip_StartService( fSocket, "DGNSS", "testNTRIP", "IDT01", "idt26585858", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "get error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

//MessageBox(NULL, buff, "Ntrip Start", MB_OK);
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(1000);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// 存檔供除錯
out = fopen( "getData.txt", "wb" );    
fwrite( buff, getBytes, 1, out );
fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, temp, MB_OK);

// 開始解碼
if( Ntrip_descode( (unsigned char *)buff, getBytes, temp ) <= 0 )
    MessageBox(NULL, "收到資料但無法解譯", "解譯錯誤", MB_OK);

MessageBox(NULL, temp, "OK", MB_OK);

// 斷線
Ntrip_Release(fSocket);

}

// 國土測試用(顯示訊息)
static  void  test_Ntrip_1( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

MessageBox(NULL, "socket", "1", MB_OK);

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "error", MB_OK);
    return;
}

MessageBox(NULL, "connect", "2", MB_OK);

// 連結 Ntrip 主機
//if( (fSocket = Ntrip_Connect( "140.121.130.145", 2101 )) == -1 )
if( (fSocket = Ntrip_Connect( "210.241.63.193", 81 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

MessageBox(NULL, "start service", "3", MB_OK);

// 啟動 Ntrip 服務 
//if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
if( Ntrip_StartService( fSocket, "DGNSS", "testNTRIP", "IDT01", "idt26585858", buff ) == -1 )
{
    //MessageBox(NULL, Ntrip_GetError(), "get error", MB_OK);
    //Ntrip_Release(fSocket);
    //return;
	MessageBox(NULL, buff, "1", MB_OK);
}

MessageBox(NULL, "send", "4", MB_OK);

MessageBox(NULL, buff, "連線啟動 Ntrip", MB_OK);
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(2000);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// 存檔供除錯
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, getBytes, 1, out );
//fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, "衛星資訊回傳", MB_OK);

// 開始解碼
if( Ntrip1_deocder(buff, getBytes, temp) <= 0 )
    MessageBox(NULL, "收到資料但無法解譯", "error", MB_OK);
else
    MessageBox(NULL, temp, "回傳解碼", MB_OK);

MessageBox(NULL, "End", "OK", MB_OK);

// 斷線
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


// 海大正式版(不顯示，存檔)
static void  main_go()
{
int    ret = 0;
int    fSocket;
char   buff[10000];
char   temp[3000], temp1[300];
int    getBytes;
FILE  *out;

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
    return;

// 連結 Ntrip 主機
if( (fSocket = Ntrip_Connect( sysServerIp, sysServerPort )) == -1 )
    return;

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, sysSiteName, sysProgName, sysUserID, sysPassword, buff ) == -1 )
{
    Ntrip_Release(fSocket);
    return;
}

// 傳送座標給主機校正
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

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    Ntrip_Release(fSocket);
    return;
}

// 開始解碼
Ntrip_descode( (unsigned char *)buff, getBytes, temp ); 

// 回傳資料存檔
strcpy( temp1, EXEPATH );
strcat( temp1, sysOutFile );
out = fopen( temp1, "wb" );    
fwrite( temp, strlen(temp), 1, out );
fclose( out );

// 斷線
Ntrip_Release(fSocket);


}

// 回傳存檔
static void write_back(char *temp)
{
char temp1[1000];
FILE  *out;

strcpy( temp1, EXEPATH );
strcat( temp1, sysOutFile );
out = fopen( temp1, "wb" );    
fwrite( temp, strlen(temp), 1, out );
fclose( out );
}

// 國土正式版(不顯示，存檔)
static void  main_go1()
{
int    ret = 0;
int    fSocket;
char   buff[10000];
char   temp[3000], temp1[300];
int    getBytes;
char   sum, *q;

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
{
	write_back("{'err_msg':'socket error'}");
    return;
}

// 連結 Ntrip 主機
if( (fSocket = Ntrip_Connect( sysServerIp, sysServerPort )) == -1 )
{
	write_back("{'err_msg':'can not connect ntrip'}");
    return;
}

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, sysSiteName, sysProgName, sysUserID, sysPassword, buff ) == -1 )
{
char temp[3000];
    strcpy(temp,"{'err_msg':'");
	strcat(temp,buff);
	strcat(temp,"'}");
	write_back(temp);
    Ntrip_Release(fSocket);
    return;
}

// 傳送座標給主機校正
strcpy( temp, "$GPGGA,004806," );
STR_D( sysLAT*100, temp1, 9, 4 );
strcat( temp, temp1 );
strcat( temp, ",N," );
STR_D( sysLNG*100, temp1, 9, 3 );
strcat( temp, temp1 );
strcat( temp, ",E,4,10,1,200,M,1,M,3,0");
// 加檢查碼
for (q=(char *)temp+1,sum=0;*q;q++) sum^=*q; /* check-sum */
sprintf(temp+strlen(temp),"*%02X",sum);
strcat( temp, "\r\n\r\n" );
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
	write_back("{'err_msg':'can not send command to ntrip'}");
    Ntrip_Release(fSocket);
    return;
}

Sleep(5000);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
	write_back("{'err_msg':'ntrip no response any data'}");
    Ntrip_Release(fSocket);
    return;
}

// 開始解碼
Ntrip1_deocder(buff, getBytes, temp);

// 回傳資料存檔
write_back(temp);

// 斷線
Ntrip_Release(fSocket);


}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // 保留 Instance
APPINSTANCE = hInstance;

// 取下傳入參數
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

// 測試用(顯示訊息)
//test_Ntrip();             // 此解海大 Ntrip
//test_Ntrip_1();           // 此解國土

// 正式版(不顯示，存檔)
//main_go();                // 海大正式版
main_go1();               // 國土正式版

return( 0 );

}

