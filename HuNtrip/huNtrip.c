/*
   NMEA GPS server �I�s Library 
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
// �H�U�������ϥ��ܼ�

static char     ntrip_err_message[10000] = "";                // ���~�ɨѦ^�ǰT��
static rtcm_t   ntrip_m_rtcm;

//////////////////////////////////////////////////////////////////////////////////////
// �H�U�������@�Ψ禡

// ���� socket �i write
// (ps: ���P husock �� listen ���P)
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
                        strcpy( ntrip_err_message, "socket �L�k write ���~!\n");
                        break;
                }

                if (!FD_ISSET(socket, &wd_set)) 
                {
                        strcpy( ntrip_err_message, "socket !FD_ISSET ���~!\n");
                        break;
                }

        } while(0);
        return result;
}

// ���� socket �i read
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
// �H�U���~���I�s�禡�w�q

// ���o���~�X
char *Ntrip_GetError(void)
{
    return( ntrip_err_message );
}

// ��l��
int  Ntrip_Initial(void)
{
#ifdef  WIN32
  WORD wVersionRequested;      /* Needed for WSAStartup() */
  WSADATA wsaData;             /* Needed for WSAStartup() */
  int err;                     /* socket startup error code */
#endif

// windows �� winsock �Ұ�
#ifdef WIN32
    /* Need to startup Winsocket stuff first... */
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        strcpy( ntrip_err_message, "WSAStartup() ���~!\n");
        return -1;
    }
#endif


// ��L�ҰʼȵL

return(0);
}

// Connect Ntrip �D���ǳƻP Server ���
// (�^��: ���\�� socket �ѫ���ާ@)
int  Ntrip_Connect( char *address, int destPortNum )
{
struct  hostent *hp;
struct  sockaddr_in server;
int     fSocketNum = -1;

    // �Ƨ� socket �ǳƳs��
    fSocketNum = socket(AF_INET, SOCK_STREAM, 0);
    if (fSocketNum < 0)
    {
        strcpy( ntrip_err_message, "socket() ����!\n");
        return -1;
    }

    // ���U server �D��
    hp = gethostbyname(address);
    if (hp == NULL )
    {
        strcpy( ntrip_err_message, "�L�� Server�A�a�}���~!\n");
        return -1;
    }

    // �s���� server
    memset(&server,0,sizeof(struct sockaddr_in));
    memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short)destPortNum);
    if (connect(fSocketNum, (struct sockaddr*)&server, sizeof(struct sockaddr_in))!= 0)
    {
            strcpy( ntrip_err_message, "connect server ���~!\n");
            close(fSocketNum);
            return -1;
    }


return( fSocketNum );

}

// �_�u
void  Ntrip_Release( int fSocketNum )
{
        if(fSocketNum>=0)
        {
              close(fSocketNum);
        }

WSACleanup();

}


// �ǰe�R�O
int  Ntrip_Send(int fSocketNum, char *cmd, int cmdSize)
{
int sendBytes=0;

        // �ˬd socket ���� socket �i�g�J, �H�K�ǰe
        if(blockUntilwriteable(fSocketNum)<=0)
        {
            strcpy( ntrip_err_message, "socket �L�k write ���~!");
            return -1;
        }

        // �}�l�ǰe
        if( (sendBytes = send(fSocketNum, cmd, cmdSize, 0)) != cmdSize )
        {
            strcpy( ntrip_err_message, "��ƶǰe�������A�i���_�u!!");
            return  sendBytes;
        }


    return  sendBytes;
}


// �������
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


// �ǰe Ntrip GET �R�O�H���o Ntrip �A��
// (ps: response �� GET �R�O�^�Ǹ��)
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
            strcpy( ntrip_err_message, "�|�� connect ");
            return -1;
        }

        // �ǳƲզX�R�O�C
        strcpy(temp, user_name);
        strcat(temp, ":");
        strcat(temp, password);
        ToBase64(temp, temp1);

        // �զX
        sprintf( cmd, cmdFmt, site_name, prog_name, temp1 );
        cmdSize = strlen(cmd);

        // �ǰe�R�O
        if( Ntrip_Send(fSocketNum, cmd, cmdSize) != cmdSize )
        {
            return -1;
        }

        Sleep(1000);

        // �����^�Ǹ�ƨ� readBuf
        bytesRead = Ntrip_Read( fSocketNum, response, MAX_READBUFSIZE );
        if (bytesRead <= 0)
        {
            strcpy( ntrip_err_message, "������ Ntrip Server �^��");
            return -1;
        }

        // �}�l�ѪR�^�Ǹ��, �P�_�O�_���~

        // �������� 200 �r�� (ICY 200 �� HTTP 200)
        if (sscanf(response, "%*s%u", &responseCode) != 1)
        {
            strcpy( ntrip_err_message, "Ntrip GET �^�����榳�~");
            return  -1;
        }
        if (responseCode != 200)
        {
            strcpy( ntrip_err_message, "Ntrip GET �^�����榳�~, �D xxx200xxx");
            return  -1;
        }

return bytesRead;

}

// �}�l�ѽX(���Ѯ��j type19)
int  Ntrip_descode( unsigned char *buff, int buffSize, char *ret_str )
{
int      ii,status;
FILE     *out, *in;
char     sstr[1000], temp1[1000];

// �s�ɨѰ���
//out = fopen( "getData.txt", "wb" );    
//fwrite( buff, buffSize, 1, out );
//fclose( out );

// ��l�ѽX���c
init_rtcm(&ntrip_m_rtcm);

// �N���쪺�r�v�r�e�J�ѽX
for( ii=0; ii<buffSize; ii++ )
    status = input_rtcm2(&ntrip_m_rtcm, (unsigned char)buff[ii]);

// ����ɮ׸ѽX
//in = fopen( "getData.txt", "rb" );    
//status = input_rtcm2f(&ntrip_m_rtcm, in);
//fclose( in );

// �զ� json �^��
strcpy( ret_str, "{\"rtcm2pr\":[");

// ��ܦ��쪺���(���ɥ� obs �����)
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

// ����ѽX��
free_rtcm(&ntrip_m_rtcm);

// �^�Ǳ����ìP��
return( status );
}

