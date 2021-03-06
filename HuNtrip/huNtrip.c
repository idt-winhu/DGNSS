/*
   NMEA GPS server ㊣ Library 
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

#include "d:\DGNSS\common\publ.h"
#include "huNtrip.h"

#include "D:\DGNSS\RtkLib\RtkLib.h"


#define MAX_READBUFSIZE 10000


///////////////////////////////////////////////////////////////////////////
// ず场ㄏノ跑计

static char     ntrip_err_message[10000] = "";                // Τ粇ㄑ肚癟
static rtcm_t   ntrip_m_rtcm;

//////////////////////////////////////////////////////////////////////////////////////
// ず场ノㄧΑ

// 单 socket  write
// (ps: 籔 husock ノ listen ぃ)
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
                        strcpy( ntrip_err_message, "socket 礚猭 write 岿粇!\n");
                        break;
                }

                if (!FD_ISSET(socket, &wd_set)) 
                {
                        strcpy( ntrip_err_message, "socket !FD_ISSET 岿粇!\n");
                        break;
                }

        } while(0);
        return result;
}

// 单 socket  read
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
// 场㊣ㄧΑ﹚竡

// 眔岿粇絏
char *Ntrip_GetError(void)
{
    return( ntrip_err_message );
}

// ﹍て
int  Ntrip_Initial(void)
{
#ifdef  WIN32
  WORD wVersionRequested;      /* Needed for WSAStartup() */
  WSADATA wsaData;             /* Needed for WSAStartup() */
  int err;                     /* socket startup error code */
#endif

// windows  winsock 币笆
#ifdef WIN32
    /* Need to startup Winsocket stuff first... */
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        strcpy( ntrip_err_message, "WSAStartup() 岿粇!\n");
        return -1;
    }
#endif


// ㄤ币笆既礚

return(0);
}

// Connect Ntrip 诀非称籔 Server ユ酵
// (肚: Θ socket ㄑ尿巨)
int  Ntrip_Connect( char *address, int destPortNum )
{
struct  hostent *hp;
struct  sockaddr_in server;
int     fSocketNum = -1;

    // 称Т socket 非称硈挡
    fSocketNum = socket(AF_INET, SOCK_STREAM, 0);
    if (fSocketNum < 0)
    {
        strcpy( ntrip_err_message, "socket() ア毖!\n");
        return -1;
    }

    //  server 诀
    hp = gethostbyname(address);
    if (hp == NULL )
    {
        strcpy( ntrip_err_message, "礚 Server岿粇!\n");
        return -1;
    }

    // 硈挡 server
    memset(&server,0,sizeof(struct sockaddr_in));
    memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short)destPortNum);
    if (connect(fSocketNum, (struct sockaddr*)&server, sizeof(struct sockaddr_in))!= 0)
    {
            strcpy( ntrip_err_message, "connect server 岿粇!\n");
            close(fSocketNum);
            return -1;
    }


return( fSocketNum );

}

// 耞絬
void  Ntrip_Release( int fSocketNum )
{
        if(fSocketNum>=0)
        {
              close(fSocketNum);
        }

WSACleanup();

}


// 肚癳㏑
int  Ntrip_Send(int fSocketNum, char *cmd, int cmdSize)
{
int sendBytes=0;

        // 浪琩 socket  socket 糶, 獽肚癳
        if(blockUntilwriteable(fSocketNum)<=0)
        {
            strcpy( ntrip_err_message, "socket 礚猭 write 岿粇!");
            return -1;
        }

        // 秨﹍肚癳
        if( (sendBytes = send(fSocketNum, cmd, cmdSize, 0)) != cmdSize )
        {
            strcpy( ntrip_err_message, "戈肚癳ゼЧ耞絬!!");
            return  sendBytes;
        }


    return  sendBytes;
}


// 钡Μ戈
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


// 肚癳 Ntrip GET ㏑眔 Ntrip 狝叭
// (ps: response  GET ㏑肚戈)
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
            strcpy( ntrip_err_message, "﹟ゼ connect ");
            return -1;
        }

        // 非称舱㏑
        strcpy(temp, user_name);
        strcat(temp, ":");
        strcat(temp, password);
        ToBase64(temp, temp1);

        // 舱
        sprintf( cmd, cmdFmt, site_name, prog_name, temp1 );
        cmdSize = strlen(cmd);

        // 肚癳㏑
        if( Ntrip_Send(fSocketNum, cmd, cmdSize) != cmdSize )
        {
            return -1;
        }

        Sleep(1000);

        // 钡Μ肚戈 readBuf
        bytesRead = Ntrip_Read( fSocketNum, response, MAX_READBUFSIZE );
        if (bytesRead <= 0)
        {
            strcpy( ntrip_err_message, "Μぃ Ntrip Server 莱");
            return -1;
        }

        // 秨﹍秆猂肚戈, 耞琌Τ粇

        // ︽莱Τ 200 妓 (ICY 200 ┪ HTTP 200)
        if (sscanf(response, "%*s%u", &responseCode) != 1)
        {
            strcpy( ntrip_err_message, "Ntrip GET 莱︽Τ粇");
            return  -1;
        }
        if (responseCode != 200)
        {
            strcpy( ntrip_err_message, "Ntrip GET 莱︽Τ粇, 獶 xxx200xxx");
            return  -1;
        }

return bytesRead;

}

// 秨﹍秆絏(秆 type19)
int  Ntrip_descode( unsigned char *buff, int buffSize, char *ret_str )
{
int      ii,status;
FILE     *out, *in;
char     sstr[1000], temp1[1000];

// 郎ㄑ埃岿
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, buffSize, 1, out );
//fclose( out );

// ﹍秆絏挡篶
init_rtcm(&ntrip_m_rtcm);

// 盢Μ硋癳秆絏
for( ii=0; ii<buffSize; ii++ )
    status = input_rtcm2(&ntrip_m_rtcm, (unsigned char)buff[ii]);

// эノ郎秆絏
//in = fopen( "getData.txt", "rb" );    
//status = input_rtcm2f(&ntrip_m_rtcm, in);
//fclose( in );

// 舱Θ json 肚
strcpy( ret_str, "{\"rtcm2pr\":[");

// 陪ボΜ戈(パ obs 戈)
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

// 睦秆絏竟
free_rtcm(&ntrip_m_rtcm);

// 肚钡Μ矫琍计
return( status );
}

