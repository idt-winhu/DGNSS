
# ifndef NCGILIB_H
# define NCGILIB_H

#ifdef __cplusplus
extern "C" {
#endif

// 共用函式
                                                 // 載入傳入資料
int    cgi_load_data( void );
                                                 // 載入欄位記憶體釋放
void   cgi_release_data( void );
                                                 // 指向某欄 value 指標, 以能作業此欄
char  *cgiGFD( char *fld_name );
                                                 // html 方式回應
void   cgi_html_msg( char *message1, char *message2 );
                                                 // 某欄資料大小
long   cgiGFD_LEN( char *fld_name );

#ifdef __cplusplus
}
#endif

# endif
