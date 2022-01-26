
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "D:\apez\weblib\common\publ.h"

#include "D:\APEZ\WebLib\HuNtrip\huNtrip.h"

#include "D:\APEZ\WebLib\RtkLib\RtkLib.h"

static  char      EXEPATH[100];

static  void  test_Ntrip( void )
{
int   ret = 0;
int   fSocket;
char  buff[10000];
char  temp[1000], temp1[1000];
int   getBytes;

rtcm_t   m_rtcm;
int      ii,status,jj;
FILE     *out, *in;

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

// 傳送座標給主機校正
strcpy( temp, "$GPGGA,004806,2504.7298,N,12133.918,E,4,10,1,200,M,1,M,3,0*56\r\n\r\n" );
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

STR_L( getBytes, temp, 5 );
MessageBox(NULL, buff, temp, MB_OK);

// 存檔供除錯
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, getBytes, 1, out );
//fclose( out );

// 開始解碼
init_rtcm(&m_rtcm);

//// 將收到的字逐字送入解碼
for( ii=0; ii<getBytes; ii++ )
    status = input_rtcm2(&m_rtcm, (unsigned char)buff[ii]);

// 改用檔案解碼
//in = fopen( "getData.txt", "rb" );    
//status = input_rtcm2f(&m_rtcm, in);
//fclose( in );

// 顯示收到的資料(此時由 obs 取資料)
if( m_rtcm.obsflag == 1 )
{
    STR_L(m_rtcm.obs.n, temp, 3);
    MessageBox(NULL, temp, "收到衛星數", MB_OK);
    for( ii=0; ii<m_rtcm.obs.n; ii++ )
    {
        strcpy( temp1, "衛星編號:" );
        STR_L( (int)m_rtcm.obs.data[ii].sat, temp1+strlen(temp1), 3);
        strcpy( temp, "" );
        for (jj=0;jj<NFREQ;jj++) 
        {
           if( jj!=0 )
              strcat( temp, "," );
           STR_D( m_rtcm.obs.data[ii].P[jj], temp+strlen(temp), 12, 3 );
        }
        MessageBox(NULL, temp, temp1, MB_OK);
    }   
}
else
    MessageBox(NULL, "收到資料但無法解譯", "解譯錯誤", MB_OK);

// 釋放解碼器
free_rtcm(&m_rtcm);

MessageBox(NULL, "OK", "message", MB_OK);


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


int  PASCAL  WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine,int nCmdShow )
{

GetExePath();

test_Ntrip();

return( 0 );

}

