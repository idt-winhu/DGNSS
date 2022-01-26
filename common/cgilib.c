
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgilib.h"

// 傳入參數欄位宣告
CGIENTRY FAR *cgiin_stru;
HGLOBAL       glbCGIENTRY;
int           cgiin_N, cgiin_maxN;

// 取下 browser 傳入參數(即 name1=value1&name2=value2...)
//*******************************************************
static char *cgi_data( void *f, char *request )
{
char *input = NULL, ch;
char *temp = NULL;
int   length;
int   i=0;
int   size = 1024;

if( !strcmp( request, "GET" ) )
{
    input = getenv( "QUERY_STRING" );
    return input;
}
else if( !strcmp( request, "POST" ) )
{
    temp = getenv( "CONTENT_LENGTH" );
    if( temp == NULL )
        return input;
    length = atoi( temp );
    input = (char *)malloc( sizeof(char)*(size+1) );
    if( length == 0 )
    {
        input[0] = '\0';
        return input;
    }
    while( 1 )
    {
        input[i] = (char)fgetc( (FILE *)f);
        if( i == size )
        {
            input[ i+1 ] = '\0';
            size += 1024;
            input = (char *)realloc(input, sizeof(char)*(size+1));
        }
        --length;
        if( ( feof(stdin) || (!length) ) )
        {
            input[i+1] = '\0';
            return input;
        }
        ++i;
    }
}
else
{
    temp = getenv( "CONTENT_LENGTH" );
    if( temp == NULL )
        return input;
    length = atoi( temp );
    input = (char *)malloc( sizeof(char)*length + 1 );
    if( length == 0 )
    {
        input[0] = '\0';
        return input;
    }
    for( i=0; i<length; )
    {
        ch = (char)fgetc( (FILE *)f );
        if( ch == EOF ) break;
        input[i] = ch;
    }
    input[ i ] = ch;
    return input;
}

}

// 取傳入欄至分隔字元止(ps:通常應是 '&')
//*************************************
static int split( char *line, char stop, char *ret_data )
{
int   x=0;
                             // 取下資料, 最長 2000 字
for( x=0; ( line[x] && (line[x] != stop) && x<2000 ); x++ )
    ret_data[x] = line[x];
ret_data[x] = '\0';

return( x );

}

// 將 '+' 字元替換成空白
//***********************************
static void makespace( char *string )
{
int i;

for( i=0; string[i]; i++ )
    if( string[i] == '+' )
        string[i] = ' ';

}

// 將傳入數值參數(ps: '%' 帶頭 16 進位)
//**************************************
static void convert( char *string )
{
int x,y;
char digit;

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

// 取 Browser 傳入參數主程式
// ps:此採外部記憶體
//***************************
BOOL  cgi_load_data_to( CGIENTRY *html_fild, int fild_N )
{
char   one_fd_buf[2048];
char   *method = NULL;
char   *input = NULL;
int    count, len;
CGIENTRY  form;
int    now_fd;
int    ii;
                             // 取得原始傳入字串並判斷是否有資料
method = getenv( "REQUEST_METHOD" );
if( method == NULL )
    return( FALSE );
input = cgi_data( (void *)stdin, method );
if( input == NULL )
    return( FALSE );
len = strlen( input );
if( len == 0 )
    return( FALSE );
for( count=0; input[count] != '\0' && count<len ; count++ )
{
                             // 取下此欄資料
    count = count + split( input+count, '&', one_fd_buf );
    makespace( one_fd_buf );                     // '+' 用空白取代
    convert( one_fd_buf );                       // '%' 數值欄轉換
    split( one_fd_buf, '=', form.name );         // 取下欄名
                             // 取下 value
    strcpy( form.value, one_fd_buf+strlen(form.name)+1 );
                             // 轉存至暫存區, 找同欄名轉存
    for( now_fd = 0; now_fd < fild_N; now_fd++ )
       if( !strcmp( html_fild[now_fd].name, form.name ) )
       {
                             // 去除前空白
          for(ii=0; form.value[ii] == ' ';ii++);
              strcpy( html_fild[now_fd].value, form.value+ii );
          break;
       }
}
                             // cgi_data 讀取暫存記憶體釋放
free( input );
                             // cgiin_stru 及 cgiin_N 需指向此, 以便後續作業如 cgiGFD() 可用
cgiin_stru = html_fild;
cgiin_N = fild_N;

return( TRUE );

}

// 取 Browser 傳入參數主程式
// ps:此採自行呼叫記憶體
//***************************
BOOL cgi_load_data( int alloc_N )
{
char          one_fd_buf[2048];
char         *method = NULL;
char         *input = NULL;
int           count, len;
CGIENTRY      form;
int           ii;
CGIENTRY FAR *p;
                             // 先將傳入參數整個讀入至 input
method = getenv( "REQUEST_METHOD" );
if( method == NULL )
    return( FALSE );
input = cgi_data( (void *)stdin, method );
if( input == NULL )
    return( FALSE );
len = strlen( input );
if( len == 0 )
    return( FALSE );
                             // 配置欄位儲存記憶體, 準備將資料轉入 name value 欄位結構
glbCGIENTRY = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(CGIENTRY)*(alloc_N+1) );
cgiin_stru = (CGIENTRY FAR *)GlobalLock( glbCGIENTRY );
cgiin_maxN = alloc_N;
                             // 依序將各欄轉入
cgiin_N = 0;
for( count=0; input[count] != '\0' && count<len ; count++ )
{
                             // 取下此欄資料
    count = count + split( input+count, '&', one_fd_buf );
    makespace( one_fd_buf );                     // '+' 用空白取代
    convert( one_fd_buf );                       // '%' 數值欄轉換
    split( one_fd_buf, '=', form.name );         // 取下欄名
                             // 取下 value
    strcpy( form.value, one_fd_buf+strlen(form.name)+1 );
                             // 轉存至欄位結構中
    p = cgiin_stru + cgiin_N;
    strcpy(p->name, form.name);
                             // value 需順便去除前空白
    for(ii=0; form.value[ii] == ' ';ii++);
         strcpy( p->value, form.value+ii );
    cgiin_N ++;
}
                             // cgi_data 讀取暫存記憶體釋放
free( input );
return( TRUE );

}

// 欄位結構記憶體釋放
//***************************
void cgi_release_data( void )
{

if( glbCGIENTRY != NULL )
{
    GlobalUnlock( glbCGIENTRY );
    GlobalFree( glbCGIENTRY );
}

}

// 指向某欄 value 指標, 以能作業此欄
//***********************************
char FAR *cgiGFD( char *fld_name )
{
int           ii;
CGIENTRY FAR *p;
                             // 先搜尋此欄
for(ii=0;ii<cgiin_N;ii++)
{
    p = cgiin_stru + ii;
    if( !strcmp(p->name,fld_name) )
        break;
}
                             // 無此欄自動新增, 使程式不因網頁更動而失敗
if( ii == cgiin_N )
{
    cgiin_N ++;
    p = cgiin_stru + ii;
    strcpy( p->name, fld_name );
    p->value[0] = '\0';
}

return( cgiin_stru[ii].value );

}

// 通用回傳網頁訊息
void  cgi_html_msg( char *message1, char *message2 )
{

printf( "Content-type:text/html\n");
printf( "\n");
printf( "\n");
printf( "<HTML>\n");
printf( "<HEAD><TITLE>Message</TITLE></HEAD>\n");
printf( "<BODY>\n");
printf( "<H3><BR>");
printf( message1 );
printf( "<BR></H3>\n");
printf( "<H3><BR>");
printf( message2 );
printf( "<BR></H3>\n");
printf( "</BODY>\n");
printf( "</HTML>\n");

}

// 條列資料時之顏色互換
void  cgi_color_switch( BOOL *sw, char *color )
{

if( *sw )
{
    strcpy( color, "BGCOLOR=#CCCCCC" );
    *sw = FALSE;
}
else
{
    strcpy( color, "BGCOLOR=#FFFFFF" );
    *sw = TRUE;
}

}

