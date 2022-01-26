/*
Ntrip GPS�ե��{��
*/

#ifndef _Ntrip_H
#define _Ntrip_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
// �H�U���~���ޥθ�Ƶ��c


///////////////////////////////////////////////////////////////////////////
// �H�U���~���ޥΨ禡

// ���o���~�X
char *Ntrip_GetError(void);

// ��l��
int  Ntrip_Initial(void);

// Connect Ntrip �D���ǳƻP Server ���
int  Ntrip_Connect( char *address, int destPortNum );

// �_�u
void  Ntrip_Release( int fSocketNum );

// �ǰe�R�O
int  Ntrip_Send( int fSocketNum, char *cmd, int cmdSize );

// �������
int  Ntrip_Read( int fSocketNum, char *readBuf, int readSize );

// �ǰe Ntrip GET �R�O�H���o Ntrip �A��
int  Ntrip_StartService( int fSocketNum, char *site_name, char *prog_name, char *user_name, char *password, char *response );

// �}�l�ѽX(���j)
int  Ntrip_descode( unsigned char *buff, int buffSize, char *ret_str );

#ifdef __cplusplus
}
#endif

#endif

