//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2021/11/10 備考
//   1.原接國土測繪Ntrip主機，改接農試所
//   2.以固定送農試所Ntrip主機 32 站點座標
//   3.預計程式採無窮迴圈，依序取下農試所 32 站點最新衛星資料存檔 


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

static  char       savePathName[300]     = "C:/Temp/";         // 預設存檔路徑    

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

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "error", MB_OK);
    return;
}

// 連結 Ntrip 主機
//if( (fSocket = Ntrip_Connect( "140.121.130.145", 2101 )) == -1 )
if( (fSocket = Ntrip_Connect( "210.241.63.193", 81 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

// 啟動 Ntrip 服務 
//if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
if( Ntrip_StartService( fSocket, "DGNSS", "testNTRIP", "IDT01", "idt26585858", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "get error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, buff, "連線啟動 Ntrip", MB_OK);
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(5000);

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

// 測試農試所 Ntrip 主機
static  void  test_Ntrip_2( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[3000];
int   getBytes;
FILE *out;

MessageBox(NULL, "socket", "啟動", MB_OK);

// 啟動
ret = Ntrip_Initial();
if( ret != 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "socket error", MB_OK);
    return;
}

MessageBox(NULL, "Ntrip 主機", "連線", MB_OK);

// 連結農試所 Ntrip 主機
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "connect error", MB_OK);
    return;
}

MessageBox(NULL, "啟動 Ntrip 服務", "啟動", MB_OK);

// 啟動 Ntrip 服務 
//if( Ntrip_StartService( fSocket, "test", "testNTRIP", "admin", "password", buff ) == -1 )
if( Ntrip_StartService( fSocket, "FKP", "testNTRIP", "taridemo101", "rbe3g4", buff ) == -1 )
{
    MessageBox(NULL, Ntrip_GetError(), "init ntrip service error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, buff, "Ntrip 啟動回傳", MB_OK);


MessageBox(NULL, "傳送站台指令", "命令", MB_OK);
strcpy( temp, "$GNGGA,080000.00,2512.708288,N,12132.987958,E,1,12,0,0,M,0,M,,*5F\r\n\r\n" );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "send error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

Sleep(5000);

MessageBox(NULL, "取下回傳資料", "接收", MB_OK);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,4096);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
    MessageBox(NULL, Ntrip_GetError(), "recv error", MB_OK);
    Ntrip_Release(fSocket);
    return;
}

MessageBox(NULL, "轉存至 getData.txt", "存檔", MB_OK);

// 存檔
out = fopen( "getData.txt", "wb" );    
fwrite( buff, getBytes, 1, out );
fclose( out );

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, "衛星資訊回傳", MB_OK);

// 開始解碼
//if( Ntrip1_deocder(buff, getBytes, temp) <= 0 )
//    MessageBox(NULL, "收到資料但無法解譯", "error", MB_OK);
//else
//    MessageBox(NULL, temp, "回傳解碼", MB_OK);


// 斷線
Ntrip_Release(fSocket);

MessageBox(NULL, "OK", "完成", MB_OK);

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
// 以下為正式版本程式(上面是測試用)

// 定義 32 組資料
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
//MessageBox(NULL, "socket error", "message",MB_OK);
    return;
}

// 連結農試所 Ntrip 主機
if( (fSocket = Ntrip_Connect( "59.125.10.251", 2101 )) == -1 )
{
//MessageBox(NULL, "can not connect ntrip", "message",MB_OK);
    return;
}

// 啟動 Ntrip 服務 
if( Ntrip_StartService( fSocket, "FKP", "", username, password, buff ) == -1 )
{
//MessageBox(NULL, "can not start ntrip service", "message",MB_OK);
    Ntrip_Release(fSocket);
    return;
}

strcpy( temp, GNGGA );

// 傳送座標給主機校正
if( Ntrip_Send( fSocket, temp, strlen(temp) ) <= 0 )
{
    Ntrip_Release(fSocket);
//MessageBox(NULL, "send ntrip error", "message",MB_OK);
    return;
}

// 等待 15 秒接收
Sleep(15000);

// 讀取 Ntrip 回傳(此時為一封包)
memset(buff,0,10000);
if( ( getBytes = Ntrip_Read( fSocket, buff, 4096 )) <= 0 )
{
//MessageBox(NULL, "ntrip no response", "message",MB_OK);
    Ntrip_Release(fSocket);
    return;
}

// 可先斷線
Ntrip_Release(fSocket);


// 存檔
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

// 加寫 base64 
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

// 農試所正式版
static void  main_go()
{
int   ii;

    // 依序取 32 站點衛星資料(每個5秒)，轉存 C:/Temp/dgnss_01~32.dat
    for(ii=0;ii<siteSize;ii++)
    {
        getDGNSStoFile(dgnss32_info[ii].id,dgnss32_info[ii].username,dgnss32_info[ii].password,dgnss32_info[ii].GNGGA);
        // 休息 1 秒
        Sleep(1000);
    }
}

int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{
int   ii, jj=0;
char  temp[1000];
                             // 保留 Instance
APPINSTANCE = hInstance;

// 取下傳入參數
if( lpszCmdLine!=NULL && strlen(lpszCmdLine)>0 )
{
    strcpy(savePathName, lpszCmdLine );
}

GetExePath();

// 測試用(顯示訊息)
//test_Ntrip();             // 此解海大 Ntrip
//test_Ntrip_1();           // 此解國土
//test_Ntrip_2();             // 測農試所 Ntrip 主機

main_go();                  // 農試所正式版

return( 0 );

}

