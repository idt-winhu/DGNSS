/*
Ntrip GPS校正程式
*/

#ifndef _Ntrip_H
#define _Ntrip_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
// 以下為外部引用資料結構


///////////////////////////////////////////////////////////////////////////
// 以下為外部引用函式

// 取得錯誤碼
char *Ntrip_GetError(void);

// 初始化
int  Ntrip_Initial(void);

// Connect Ntrip 主機準備與 Server 交談
int  Ntrip_Connect( char *address, int destPortNum );

// 斷線
void  Ntrip_Release( int fSocketNum );

// 傳送命令
int  Ntrip_Send( int fSocketNum, char *cmd, int cmdSize );

// 接收資料
int  Ntrip_Read( int fSocketNum, char *readBuf, int readSize );

// 傳送 Ntrip GET 命令以取得 Ntrip 服務
int  Ntrip_StartService( int fSocketNum, char *site_name, char *prog_name, char *user_name, char *password, char *response );

// 開始解碼(海大)
int  Ntrip_descode( unsigned char *buff, int buffSize, char *ret_str );

#ifdef __cplusplus
}
#endif

#endif

