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
#include <time.h>

#include "D:\DGNSS\common\publ.h"
#include "D:\DGNSS\RtkLib\RtkLib.h"
#include "D:\DGNSS\HuNtrip\huNtrip.h"
#include "D:\DGNSS\HuNtrip\huNtrip1.h"

//////////////////////////////////////////////////////////////////////
// 系統變數

static  HINSTANCE  APPINSTANCE     = NULL;     // 程式 INSTANCE
static  char       EXEPATH[300]    = "";

static  char       iniFileName[100]     = "gen32DGNSS.ini";         // 預設站台 ini 名     


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
	char  savepath[300];
} DGNSS;

static DGNSS  dgnss32_info;

//產生有系統時間的GNGGA
static char* gen_GNGGA(char *GNGGA_ini)
{
	char nmeaStr[80]="";
	char timeStr[11]="";

	// get system time
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	int hour = local->tm_hour - 8;	// UTC+0
	if(hour < 0){ hour += 24; }
	sprintf(timeStr, "%02d%02d%02d.00", hour, local->tm_min, local->tm_sec);


	strcat(nmeaStr, "$GNGGA,");
	strcat(nmeaStr, timeStr);
	strcat(nmeaStr, GNGGA_ini);

	// calculate checksum
	int checksum = 0;
	for(int i=1;i<strlen(nmeaStr);i++){ checksum ^= nmeaStr[i]; }

	// 將checksum轉為大寫16進位
	char checksumStr[4] = {'/0'};
	sprintf(checksumStr, "*%2X", checksum);
	strcat(nmeaStr, checksumStr);

	return nmeaStr;
}
 
// 單一站台資料取得
static BOOL  getDGNSStoFile(int fSocket, char *id, char *GNGGA, char *savepath)
{
int   ret = 0;
//int   fSocket;
char  buff[10000];
char  buff1[10000];
char  buff2[10000];
char  temp[3000];
int   getBytes, len;
FILE *out;
/*
// 啟動 socket
if( Ntrip_Initial() != 0 )
{
//MessageBox(NULL, "socket error", "message",MB_OK);
    return FALSE;
}

// 連結農試所 Ntrip 主機
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
//MessageBox(NULL, "can not connect ntrip", "message",MB_OK);
    return FALSE;
}

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, "FKP", "", username, password, buff ) == -1 )
{
//MessageBox(NULL, "can not start ntrip service", "message",MB_OK);
    Ntrip_Release(fSocket);
    return FALSE;
}*/

strcpy( temp, gen_GNGGA(GNGGA) );
strcat( temp, "\r\n\r\n");

//MessageBox(NULL, temp, "message",MB_OK);
// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
//    Ntrip_Release(fSocket);
//MessageBox(NULL, "send ntrip error", "message",MB_OK);
    return FALSE;
}

//MessageBox(NULL, "start receive", "message",MB_OK);
// 等待 10 秒接收
Sleep(1000);
//MessageBox(NULL, "wait receive", "message",MB_OK);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,10000);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
//MessageBox(NULL, "ntrip no response", "message",MB_OK);
//    Ntrip_Release(fSocket);
    return FALSE;
}

// 可先斷線
//Ntrip_Release(fSocket);

// 存檔
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

// 加寫 base64 
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

// Ntrip 啟動流程
static int Ntrip_Start(char *username, char *password)
{
	int fSocket = -1;
	char  buff[10000];

	// 啟動 socket
	if( Ntrip_Initial() != 0 )
	{
	//MessageBox(NULL, "socket error", "message",MB_OK);
	    //return FALSE;
	}

	// 連結農試所 Ntrip 主機
	if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
	{
	//MessageBox(NULL, "can not connect ntrip", "message",MB_OK);
	    //return FALSE;
	}

	// 啟動 Ntrip 服務 

	if( Ntrip_StartService( fSocket, "FKP", "", username, password, buff ) == -1 )
	{
	//MessageBox(NULL, "can not start ntrip service", "message",MB_OK);
		Ntrip_Release(fSocket);
		fSocket = -1;
	    //return FALSE;
	}
	return fSocket;
}
// 農試所正式版
static void  main_go()
{
int   ii;
char  temp[1000];
int   fSocket;

// 解析 ini 檔中參數
strcpy( temp, EXEPATH );
strcat( temp, iniFileName );

GetPrivateProfileString( "SYS-INFO", "id",       "", dgnss32_info.id,        99, temp );
GetPrivateProfileString( "SYS-INFO", "username", "", dgnss32_info.username,  99, temp );
GetPrivateProfileString( "SYS-INFO", "password", "", dgnss32_info.password,  99, temp );
GetPrivateProfileString( "SYS-INFO", "GNGGA",    "", dgnss32_info.GNGGA,    299, temp );
GetPrivateProfileString( "SYS-INFO", "savepath", "", dgnss32_info.savepath, 299, temp );

//MessageBox(NULL, temp, dgnss32_info.username,MB_OK);

// 啟動 Ntrip
fSocket = Ntrip_Start(dgnss32_info.username, dgnss32_info.password);

while(1)
{
	/*
    // 解析 ini 檔中參數
    strcpy( temp, EXEPATH );
    strcat( temp, iniFileName );

    GetPrivateProfileString( "SYS-INFO", "id",       "", dgnss32_info.id,        99, temp );
    GetPrivateProfileString( "SYS-INFO", "username", "", dgnss32_info.username,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "password", "", dgnss32_info.password,  99, temp );
    GetPrivateProfileString( "SYS-INFO", "GNGGA",    "", dgnss32_info.GNGGA,    299, temp );
    GetPrivateProfileString( "SYS-INFO", "savepath", "", dgnss32_info.savepath, 299, temp );
	*/
	try
    {
        if( getDGNSStoFile(fSocket, dgnss32_info.id, dgnss32_info.GNGGA, dgnss32_info.savepath) == FALSE )
		{
			Sleep(30000);
			Ntrip_Release(fSocket);
			fSocket = Ntrip_Start(dgnss32_info.username, dgnss32_info.password);
		}
		else        // 休息 1 秒
        {
			Sleep(1000);
		}
	}
    catch(...)
    {
        Sleep(30000);
		Ntrip_Release(fSocket);
		fSocket = Ntrip_Start(dgnss32_info.username, dgnss32_info.password);
    }
}

}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // 保留 Instance
APPINSTANCE = hInstance;


// 取下傳入參數，傳入 ini 檔指示站台
if( lpszCmdLine!=NULL && strlen(lpszCmdLine)>0 )
{
    strcpy(iniFileName, lpszCmdLine );
}

GetExePath();

main_go();        

return( 0 );

}

