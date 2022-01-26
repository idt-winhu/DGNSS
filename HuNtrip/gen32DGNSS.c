//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2021/11/10 備考
//   1.原接國土測繪Ntrip主機，改接農試所
//   2.以固定送農試所Ntrip主機 32 站點座標
//   3.預計程式採無窮迴圈，依序取下農試所 32 站點最新衛星資料存檔
// --------------------------------------------------------------------------
//   4.此版與 Gen32DGNSS 目錄不同在以 ini 傳入，指定要收農試所何站台衛星資料   


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "D:\apez\weblib\common\publ.h"
#include "D:\APEZ\WebLib\RtkLib\RtkLib.h"
#include "D:\APEZ\WebLib\HuNtrip\huNtrip.h"
#include "D:\APEZ\WebLib\HuNtrip\huNtrip1.h"

//////////////////////////////////////////////////////////////////////
// 系統變數

static  HINSTANCE  APPINSTANCE     = NULL;     // 程式 INSTANCE
static  char       EXEPATH[300]    = "";

static  char       iniFileName     = "gen32DGNSS.ini";         // 預設站台 ini 名     


//////////////////////////////////////////////////////////////////////
// 主處理程序

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
// 以下為正式版本程式(上面是測試用)

// 定義 32 組資料
typedef struct DGNSS_prop
{
    char  id[100];
    char  username[100];
    char  password[100];
    char  GNGGA[300];
} DGNSS;

static DGNSS  dgnss32_info;
 
// 單一站台資料取得
static void  getDGNSStoFile(char *id, char *username, char *password, char *GNGGA)
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  buff1[10000];
char  temp[3000];
int   getBytes, len;
FILE *out;

// 啟動 socket
if( Ntrip_Initial() != 0 )
{
    return;
}

// 連結農試所 Ntrip 主機
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
    return;
}

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, "FKP", "testNTRIP", username, password, buff ) == -1 )
{
    Ntrip_Release(fSocket);
    return;
}

strcpy( temp, GNGGA );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    Ntrip_Release(fSocket);
    return;
}

// 等待 15 秒接收
Sleep(15000);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,10000);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    Ntrip_Release(fSocket);
    return;
}

// 存檔
strcpy(temp,"C:/Temp/_dgnss_");
strcat(temp,id);
strcat(temp,".data");
out = fopen( temp, "wb" );    
fwrite( buff, getBytes, 1, out );
fclose( out );

// 加寫 base64 
len = ToBase64_len(buff, buff1, getBytes);
strcpy(temp,"C:/Temp/_base64_");
strcat(temp,id);
strcat(temp,".data");
out = fopen( temp, "wb" );    
fwrite( buff1, len, 1, out );
fclose( out );

// 斷線
Ntrip_Release(fSocket);

}

// 農試所正式版
static void  main_go()
{
int   ii;
char  temp[1000];

    // 解析 ini 檔中參數
    strcpy( temp, EXEPATH );
    strcat( temp, iniFileName );
    GetPrivateProfileString( "SYS-INFO", "id",       "", dgnss32_info.id,        99, temp );
    GetPrivateProfileString( "SYS-INFO", "username", "", dgnss32_info.username,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "password", "", dgnss32_info.password,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "GNGGA",    "", dgnss32_info.GNGGA,    299, temp );

        getDGNSStoFile(dgnss32_info.id, dgnss32_info.username, dgnss32_info.password, dgnss32_info.GNGGA);
        // 休息 1 秒
        Sleep(1000);
}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // 保留 Instance
APPINSTANCE = hInstance;

// 取下傳入參數，傳入 ini 檔指示站台
if( lpszCmdLine )
    strcpy(iniFileName, lpszCmdLine );

GetExePath();

main_go();        

return( 0 );

}

