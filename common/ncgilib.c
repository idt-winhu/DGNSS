
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 由 stdin 傳入參數欄位結構
typedef struct
{
   char       name[31];                     // 欄名長度可到 30
   char      *value;                        // 資料採指標彈性
   long       v_len;                        // value 資料長度(ps:用於主程式無法用 strlen())
}  CGIENTRY;
static  CGIENTRY     *cgiin_stru;
static  int           cgiin_N = 0;

// 取下 browser 傳入參數(即 name1=value1&name2=value2... 字串)
static char *cgi_data( void *f, char *request, long *ret_len )
{
char  *input = NULL, ch;
char  *temp = NULL;
long   length;
long   i=0;

// 仍有可能以 GET 方式送短參數
if( !strcmp( request, "GET" ) )
{
    // 直接讀取系統變數 "QUERY_STRING"
    input = getenv( "QUERY_STRING" );
    if( input!=NULL )
       *ret_len = strlen(input);
    else
       *ret_len = 0;
    return input;
}
// 否則即是 POST 方式即由 stdin 送入大量資料, 資料長度由系統變數 "CONTENT_LENGTH" 通知
else
{
    temp = getenv( "CONTENT_LENGTH" );
    if( temp == NULL )
    {
        *ret_len = 0;
        input = NULL;
        return input;
    }
    length = atol( temp );
    input = (char *)GlobalAlloc( GMEM_FIXED, sizeof(char)*length+1 );
    if( length == 0 )
    {
        *ret_len = 0;
        input[0] = '\0';
        return input;
    }
    for( i=0; i<length; i++ )
    {
        ch = (char)fgetc( (FILE *)f );
        if( ch == EOF )
            break;
        input[i] = ch;
    }
    input[ i ] = '\0';

    *ret_len = length;
    return input;
}

}

// 取傳入資料至分隔字元止(ps:通常應是 '&')
static long  split( char *line, char stop, char *ret_data )
{
long   x=0;

for( x=0; (*(line+x) != '\0') && (*(line+x) != stop); x++ )
    *(ret_data+x) = *(line+x);
*(ret_data+x) = '\0';

return( x );

}

// 將傳入 16 進位數值轉正確(ps: '%' 帶頭)
static void convert( char *string )
{
long  x, y;
char  digit;

for( x=0,y=0; string[y]; ++x,++y )
{
    string[x] = string[y];
    if( string[y] == '%' )
    {
        if( '0'<=string[y+1] && string[y+1] <='9' )
            digit = string[y+1] - '0';
        else if( 'a'<=string[y+1] && string[y+1] <='f' )
            digit = string[y+1] - 'a'+10;
        else if( 'A'<=string[y+1] && string[y+1] <='F' )
            digit = string[y+1] - 'A'+10;
        digit *= 16;
        if( '0'<=string[y+2] && string[y+2] <='9' )
            digit = digit+string[y+2] - '0';
        else if( 'a'<=string[y+2] && string[y+2] <='f' )
            digit = digit+string[y+2] - 'a'+10;
        else if( 'A'<=string[y+2] && string[y+2] <='F' )
            digit = digit+string[y+2] - 'A'+10;
        string[x] = digit;
        y += 2;
    }
}
string[x] = '\0';

}

// 判斷欄位總數
static  int  calc_fields( char *input, char split_c )
{
long  ii;
int   jj;

ii = 0;
jj = 0;
while( *(input+ii) != '\0' )
{
    if( *(input+ii) == split_c )
        jj++;
    ii++;
}

return( jj );

}

// 計算及配置此單一欄位所需記憶體
static long  coun_fd_len( char *line, char stop )
{
long   x=0;

// 計算此欄大小
for( x=0; (*(line+x) != '\0') && (*(line+x) != stop); x++ );

return( x );

}

//**************************************************************************************************************
// 取 Browser 傳入參數主程式
//
int cgi_load_data( void )
{
char         *method = NULL;
char         *input = NULL;
long          length = 0;
long          count, vlen;
int           nn;
char         *one_fd_buf = NULL;
char          name[50];
CGIENTRY     *p;

                             // 讀取系統變數 REQUEST_METHOD, 以得知參數傳入方式
method = getenv( "REQUEST_METHOD" );
if( method == NULL )
    return( -1 );                              // 直接點取 exe 執行
                             // 取下資料
input = cgi_data( (void *)stdin, method, &length );
if( length == 0 )
    return( 0 );                              // 無參數
                             // 判斷欄位總數
cgiin_N = calc_fields( input, '=' );
                             // 配置欄位儲存記憶體, 準備將資料轉入 name value 欄位結構
cgiin_stru = (CGIENTRY *)malloc( sizeof(CGIENTRY)*(cgiin_N+1) );
                             // 依序將各欄轉入
nn = 0;
for( count=0; input[count] != '\0' && count<length; count++ )
{
                             // 計算及配置此欄記憶體
    vlen = coun_fd_len( input+count, '&' );
    one_fd_buf = (char *)GlobalAlloc( GMEM_FIXED, sizeof(char)*vlen+1 );
                             // 取下此欄欄名
    count = count + split( input+count, '=', name );
    count++;
                             // 取下此欄內容
    count = count + split( input+count, '&', one_fd_buf );
    convert( one_fd_buf );                       // '%' 16進位數值欄轉換
                             // 轉存至欄位結構中
    p = cgiin_stru + nn;
    strcpy(p->name, name);
    p->value = one_fd_buf;
    p->v_len = vlen-1;

    nn ++;
}
                             // cgi_data 讀取暫存記憶體釋放
GlobalFree( input );

return( cgiin_N );

}

// 欄位結構記憶體釋放
//***************************
void  cgi_release_data( void )
{
int        ii;
CGIENTRY  *p;

   // 先釋放各欄之 value 配置
   for( ii=0; ii < cgiin_N; ii++ )
   {
       p = cgiin_stru + ii;
       if( p->value != NULL )
           GlobalFree( p->value );
       p->value = NULL;
   }
   free( cgiin_stru );
}

// 指向某欄 value 指標, 以能作業此欄
//***********************************
char  *cgiGFD( char *fld_name )
{
int           ii;
CGIENTRY     *p;
                             // 先搜尋此欄
for(ii=0;ii<cgiin_N;ii++)
{
    p = cgiin_stru + ii;
    if( !strcmp(p->name,fld_name) )
        break;
}
if( ii != cgiin_N )
    return( cgiin_stru[ii].value );
                             // 無此欄傳回 NULL
return( NULL );

}

// 通用回傳網頁訊息
//***********************************
void  cgi_html_msg( char *message1, char *message2 )
{

printf( "Content-type:text/html\n");
printf( "\n");
printf( "\n");
printf( "<HTML>\n");
printf( "<BODY>\n");
printf( message1 );
printf( message2 );
printf( "</BODY>\n");
printf( "</HTML>\n");

}

// 傳回 value 資料長度
// (ps: 因如傳檔需做 base64 轉換, strlen() 僅 int 大小不足)
//*********************************************************
long cgiGFD_LEN( char *fld_name )
{
int           ii;
CGIENTRY     *p;
                             // 先搜尋此欄
for(ii=0;ii<cgiin_N;ii++)
{
    p = cgiin_stru + ii;
    if( !strcmp(p->name,fld_name) )
        break;
}
                             // 無此欄傳回 0
if( ii == cgiin_N )
    return( 0 );
                             // 找到此欄, 計算大小

return( cgiin_stru[ii].v_len );

}
