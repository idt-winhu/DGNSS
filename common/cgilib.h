
// HTML INPUT GET 結構
typedef struct
{
    char name[21];                     // 欄名長度可到 20
    char value[2001];                  // 資料可達 2000
}  CGIENTRY;

extern CGIENTRY FAR *cgiin_stru;
extern HGLOBAL       glbCGIENTRY;
extern int           cgiin_N, cgiin_maxN;

// 共用函式
                                                 // 載入傳入參數(ps:自動配置記憶體)
BOOL  cgi_load_data( int alloc_N );
                                                 // 取 Browser 傳入參數主程式(ps:外部記憶體)
BOOL  cgi_load_data_to( CGIENTRY *html_fild, int fild_N );
                                                 // 欄位結構記憶體釋放
void  cgi_release_data( void );
                                                 // 指向某欄 value 指標, 以能作業此欄
char  FAR *cgiGFD( char *fld_name );
                                                 // 通用回傳網頁訊息
void  cgi_html_msg( char *message1, char *message2 );
                                                 // 條列資料時之顏色互換
void  cgi_color_switch( BOOL *sw, char *color );

