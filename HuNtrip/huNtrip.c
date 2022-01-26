/*
   NMEA GPS server 呼叫 Library 
*/

#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#ifndef WIN32
#include <sys/socket.h>
#include <netdb.h>
#else
#include <io.h>
#include <winsock2.h>
#endif

#include "d:\apez\weblib\common\publ.h"
#include "huNtrip.h"

#include "D:\APEZ\WebLib\RtkLib\RtkLib.h"


#define MAX_READBUFSIZE 10000


///////////////////////////////////////////////////////////////////////////
// 以下為內部使用變數

static char     ntrip_err_message[10000] = "";                // 有誤時供回傳訊息
static rtcm_t   ntrip_m_rtcm;

//////////////////////////////////////////////////////////////////////////////////////
// 以下為內部共用函式

// 等待 socket 可 write
// (ps: 此與 husock 用 listen 不同)
static int blockUntilwriteable(int socket)
{
        int result = -1;
        static unsigned numFds;
        fd_set wd_set;

        do
        {
                FD_ZERO(&wd_set);
                if (socket < 0)
                    break;
                FD_SET((unsigned) socket, &wd_set);
                numFds = socket+1;
                
                result = select(numFds, NULL, &wd_set, NULL,0);// timeout);
                if (result == 0)
                {
                        break; // this is OK - timeout occurred
                }
                else if(result <= 0) 
                {
                        strcpy( ntrip_err_message, "socket 無法 write 錯誤!\n");
                        break;
                }

                if (!FD_ISSET(socket, &wd_set)) 
                {
                        strcpy( ntrip_err_message, "socket !FD_ISSET 錯誤!\n");
                        break;
                }

        } while(0);
        return result;
}

// 等待 socket 可 read
static int blockUntilReadable(int socket)
{
        fd_set rd_set;
        int result = -1;
        static unsigned numFds;

        do
        {
                FD_ZERO(&rd_set);
                if (socket < 0)
                    break;
                FD_SET((unsigned) socket, &rd_set);
                numFds = socket+1;
                result = select(numFds, &rd_set, NULL, NULL,0);
                if (result == 0) 
                {
                        break; // this is OK - timeout occurred
                } 
                else if (result <= 0) 
                {
                        break;
                }
    
                if (!FD_ISSET(socket, &rd_set)) 
                {
                        break;
                }       
        } while(0);

        return result;
}


//////////////////////////////////////////////////////////////////////////////////////
// 以下為外部呼叫函式定義

// 取得錯誤碼
char *Ntrip_GetError(void)
{
    return( ntrip_err_message );
}

// 初始化
int  Ntrip_Initial(void)
{
#ifdef  WIN32
  WORD wVersionRequested;      /* Needed for WSAStartup() */
  WSADATA wsaData;             /* Needed for WSAStartup() */
  int err;                     /* socket startup error code */
#endif

// windows 版 winsock 啟動
#ifdef WIN32
    /* Need to startup Winsocket stuff first... */
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        strcpy( ntrip_err_message, "WSAStartup() 錯誤!\n");
        return -1;
    }
#endif


// 其他啟動暫無

return(0);
}

// Connect Ntrip 主機準備與 Server 交談
// (回傳: 成功的 socket 供後續操作)
int  Ntrip_Connect( char *address, int destPortNum )
{
struct  hostent *hp;
struct  sockaddr_in server;
int     fSocketNum = -1;

    // 備妥 socket 準備連結
    fSocketNum = socket(AF_INET, SOCK_STREAM, 0);
    if (fSocketNum < 0)
    {
        strcpy( ntrip_err_message, "socket() 失敗!\n");
        return -1;
    }

    // 取下 server 主機
    hp = gethostbyname(address);
    if (hp == NULL )
    {
        strcpy( ntrip_err_message, "無此 Server，地址錯誤!\n");
        return -1;
    }

    // 連結此 server
    memset(&server,0,sizeof(struct sockaddr_in));
    memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short)destPortNum);
    if (connect(fSocketNum, (struct sockaddr*)&server, sizeof(struct sockaddr_in))!= 0)
    {
            strcpy( ntrip_err_message, "connect server 錯誤!\n");
            close(fSocketNum);
            return -1;
    }


return( fSocketNum );

}

// 斷線
void  Ntrip_Release( int fSocketNum )
{
        if(fSocketNum>=0)
        {
              close(fSocketNum);
        }

WSACleanup();

}


// 傳送命令
int  Ntrip_Send(int fSocketNum, char *cmd, int cmdSize)
{
int sendBytes=0;

        // 檢查 socket 直到 socket 可寫入, 以便傳送
        if(blockUntilwriteable(fSocketNum)<=0)
        {
            strcpy( ntrip_err_message, "socket 無法 write 錯誤!");
            return -1;
        }

        // 開始傳送
        if( (sendBytes = send(fSocketNum, cmd, cmdSize, 0)) != cmdSize )
        {
            strcpy( ntrip_err_message, "資料傳送未完全，可能斷線!!");
            return  sendBytes;
        }


    return  sendBytes;
}


// 接收資料
int  Ntrip_Read( int fSocketNum, char *readBuf, int readSize )
{
int bytesRead = -1;

    if(blockUntilReadable(fSocketNum)<=0)
    {
        return -1;
    }

    bytesRead = recv( fSocketNum, (unsigned char*)readBuf, readSize, 0 );

return  bytesRead;
}


// 傳送 Ntrip GET 命令以取得 Ntrip 服務
// (ps: response 為 GET 命令回傳資料)
int  Ntrip_StartService( int fSocketNum, char *site_name, char *prog_name, char *user_name, char *password, char *response )
{
int      bytesRead = 0;

char     cmdFmt[] = "GET /%s HTTP/1.0\r\nUser-Agent: NTRIP %s\r\nAccept: */*\r\nConnection: close\r\nAuthorization: Basic %s\r\n\r\n";
//char     cmdFmt[] = "GET /%s HTTP/1.0\r\nUser-Agent: NTRIP %s\r\nNtrip-Version: Ntrip/2.0\r\nConnection: close\r\nAuthorization: Basic %s\r\n\r\n";
//char     cmdFmt[] = "GET /%s HTTP/1.0\r\nUser-Agent: NTRIP %s\r\nAccept: */*\r\nNtrip-Version: Ntrip/2.3\r\nConnection: close\r\nAuthorization: Basic %s\r\n\r\n";

int      cmdSize = 0;
char     cmd[1024];
char     temp[300], temp1[300];
unsigned responseCode;

        if( fSocketNum < 0 )
        {
            strcpy( ntrip_err_message, "尚未 connect ");
            return -1;
        }

        // 準備組合命令列
        strcpy(temp, user_name);
        strcat(temp, ":");
        strcat(temp, password);
        ToBase64(temp, temp1);

        // 組合
        sprintf( cmd, cmdFmt, site_name, prog_name, temp1 );
        cmdSize = strlen(cmd);

        // 傳送命令
        if( Ntrip_Send(fSocketNum, cmd, cmdSize) != cmdSize )
        {
            return -1;
        }

        Sleep(1000);

        // 接收回傳資料到 readBuf
        bytesRead = Ntrip_Read( fSocketNum, response, MAX_READBUFSIZE );
        if (bytesRead <= 0)
        {
            strcpy( ntrip_err_message, "收不到 Ntrip Server 回應");
            return -1;
        }

        // 開始解析回傳資料, 判斷是否有誤

        // 首行應有 200 字樣 (ICY 200 或 HTTP 200)
        if (sscanf(response, "%*s%u", &responseCode) != 1)
        {
            strcpy( ntrip_err_message, "Ntrip GET 回應首行有誤");
            return  -1;
        }
        if (responseCode != 200)
        {
            strcpy( ntrip_err_message, "Ntrip GET 回應首行有誤, 非 xxx200xxx");
            return  -1;
        }

return bytesRead;

}

// 開始解碼(此解海大 type19)
int  Ntrip_descode( unsigned char *buff, int buffSize, char *ret_str )
{
int      ii,status;
FILE     *out, *in;
char     sstr[1000], temp1[1000];

// 存檔供除錯
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, buffSize, 1, out );
//fclose( out );

// 初始解碼結構
init_rtcm(&ntrip_m_rtcm);

// 將收到的字逐字送入解碼
for( ii=0; ii<buffSize; ii++ )
    status = input_rtcm2(&ntrip_m_rtcm, (unsigned char)buff[ii]);

// 改用檔案解碼
//in = fopen( "getData.txt", "rb" );    
//status = input_rtcm2f(&ntrip_m_rtcm, in);
//fclose( in );

// 組成 json 回傳
strcpy( ret_str, "{\"rtcm2pr\":[");

// 顯示收到的資料(此時由 obs 取資料)
if( ntrip_m_rtcm.obsflag == 1 )
{
    status = ntrip_m_rtcm.obs.n;
    for( ii=0; ii<ntrip_m_rtcm.obs.n; ii++ )
    {
        strcpy( sstr, "\n{\"prn\":\"" );
        STR_L( (int)ntrip_m_rtcm.obs.data[ii].sat, temp1, 2);
        strcat( sstr, temp1 );
        strcat( sstr, "\",\n\"satsys\":\"G\",\n\"pr\":\"" );
        STR_D( ntrip_m_rtcm.obs.data[ii].P[0], temp1, 12, 3 );
        strcat( sstr, temp1 );
        strcat( sstr, "\",\n\"snr\":\"45\"}" );
        if( ii != 0 )
           strcat( ret_str, "," );
        strcat( ret_str, sstr );
    }   
}
else
{
    status = -1;
}
strcat( ret_str, "]}");

// 釋放解碼器
free_rtcm(&ntrip_m_rtcm);

// 回傳接收衛星數
return( status );
}

