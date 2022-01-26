# ifndef PUBL_H
# define PUBL_H

#ifdef __cplusplus
extern "C" {
#endif

// 共用函式
                                                 // 等待執行至完成
void  WaitForExecute( char *run_prog, char *run_parm );
                                                 // SUBSTR 部份字串, from 從 1 開始
char *SUBS( char *dest,char *sour,int from,int cn );
                                                 // 字串以某字原取代
char *REPLI( char *dest, char ch, int num );
                                                 // 填空白字串
char *szSPACE( char *dest,int len );
                                                 // 切換目錄
BOOL  CHANGE_DIR( char *check_path );
                                                 // 建立目錄
void  MAKE_DIR( char *path );
                                                 // 補滿字串長度
char *MAP_LEN( char *dest,int len );
                                                 // 字串結尾空白去除
char *TRIM( char *sour );
                                                 // 字串前頭空白去除
char *LTRIM( char *sour );
                                                 // 字串轉 long
long  VAL_L( char *sstr );
                                                 // 字串轉 double
double VAL_D(char *sstr);
                                                 // long 數值轉字串
char *STR_L( long l_val, char *ptr, int num);
                                                 // long 轉字串, 但前需補 0
char *STR_L1( long l_val, char *ptr, int num);
                                                 // double 數字轉字串
char *STR_D( double doub_val, char *buffer, int len, int dec);
                                                 // 目前日期
char *szDATE( char *ret_str );
                                                 // 傳回目前時間字串
char *szTIME( char *ret_str );
                                                 // 檔案是否存在
BOOL  fFileExist( char *filename );
                                                 // 檔案刪除
int   DELE_FILE(char *filename);
                                                 // 檔案複製
int   COPY_FILE(char *fromfile,char *tofile);
                                                 // 搜尋字串中某字元位置
int   cAT( char s_c, char *sour );
                                                 // 轉換 CCYYMMDD 日期為任一格式
char  *FORMAT_DATE( char *dbf_date, char *picture );
                                                 // 將數值日期轉文字 CCYYMMDD
char  *DTOC( char *buffer, double datenum);
                                                 // 將日期字串轉數值日期
double CTOD(char *datestr);
                                                 // 0.01 秒為基礎之微量 delay
void   MYDELAY( int tsecs );
                                                 // 前補 '0' 函式
void   FULL_0( char *dest );
                                                 // 組合檔案全名, 並一併判斷檔案是否存在
BOOL   FILE_EXIST( char *dest, char *pathname, char *addhead, char *filename, char *addend );
                                                 // 製造後補 '\' 的路徑名函式
void   PATH_MAKE( char *sour );
                                                 // 取得 email 專用日期格式
void   RFC_DATE(char * pszBuffer);
                                                 // TXT 檔讀取
char  *READ_TXT( char *filename, char *retbuff );
                                                 // TXT 檔寫入
void   WRITE_TXT( char *savebuff, char *filename );
                                                 // 取得目前日期時間字串
char  *getDateTimeStr( char *ret_str );

char *AsciiToUtf8(char *ascii);
char *Utf8ToAscii(char *utf8);

char *getDateTimeStr_SQL( char *ret_str );
                                                 // LOG 寫入
void   myLOG( char *logname, char *szFmt, ... );

char  *myURI( char *sour );

void   myURI_decode(char *dst, const char *src);

char *ToBase64(char *text, char *result);       // 轉字串為 base64

int ToBase64_len(char *text, char *result, int len);  // 轉 base64

#ifdef __cplusplus
}
#endif

# endif
