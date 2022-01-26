
#include  <windows.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>
#include  <direct.h>

// 執行外部程式
void  WaitForExecute( char *run_prog, char *run_parm )
{

SHELLEXECUTEINFO ShExecInfo = {0};
ShExecInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
ShExecInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
ShExecInfo.hwnd         = NULL;
ShExecInfo.lpVerb       = NULL;
ShExecInfo.lpFile       = run_prog;
ShExecInfo.lpParameters = run_parm;
ShExecInfo.lpDirectory  = NULL;
ShExecInfo.nShow        = SW_HIDE;                         // 隱藏式執行
ShExecInfo.hInstApp     = NULL;
ShellExecuteEx(&ShExecInfo);
WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

}

// SUBSTR 部份字串, from 從 1 開始
char *SUBS( char *dest, char *sour, int from, int cn )
{
int   len = strlen(sour);

    *dest = '\0';
    if( from > len ) return( dest );
    from --;
    if( from < 0 ) from = 0;
    if( cn > (len-from) )
        cn = len - from;
    if( cn<=0 ) return( dest );
    memmove( dest,sour+from,cn );
    dest[cn]='\0';
    return( dest );
}

// 字串以某字元取代
char *REPLI( char *dest, char ch, int num )
{
int ii;

   for( ii=0;ii<num;ii++ )
      *(dest+ii) = ch;
   *(dest+ii) = '\0';
   return dest;
}

// 填空白字串
char *szSPACE( char *dest,int len )
{
    REPLI( dest+strlen(dest), ' ', len-strlen(dest) );
    return  dest;
}

// 切換目錄
BOOL  CHANGE_DIR( char *check_path )
{
char  temp[100];
int   len;

len = strlen(check_path);
if( len == 0 ) return( TRUE );
strcpy( temp, check_path );
if( temp[len-1] == '\\' ) temp[len-1] = '\0';
if( chdir( temp ) == 0 )  return( TRUE );
else return( FALSE );

}

// 建立目錄
// (ps: 此含子層自動檢查建立)
void  MAKE_DIR( char *path )
{

// 由左到右陸續移轉建立
int    ii,jj;
char   now_path[100];

for(ii=0,jj=0;ii<strlen(path);ii++)
{
    now_path[ii] = path[ii];
    if( path[ii]=='\\' || path[ii]=='/' )
    {
        jj++;
        if( jj>1 )
        {
            now_path[ii+1] = '\0';
            if( !CHANGE_DIR(now_path) )
                mkdir(now_path);
        }
    }
}
if( path[ii-1]!='\\' && path[ii-1]!='/' )
    if( !CHANGE_DIR(path) )
        mkdir(path);
}

// 補滿字串長度
char *MAP_LEN( char *dest,int len )
{

int  len1 = strlen( dest );
if( len1 < len )
    REPLI( dest+strlen(dest), ' ', len-strlen(dest) );
dest[len] = '\0';
return( dest );

}

// 字串結尾空白去除
char *TRIM( char *sour )
{
int  len = strlen( sour );

    if( len == 0 ) return( sour );
    while( --len >= 0 )
    {
        if( *(sour+len) == '\0' || *(sour+len) == ' ' )
            *(sour+len) = '\0';
        else
            break;
    }
    return( sour );
}

// 字串前頭空白去除
char  *LTRIM( char *sour )
{
char  temp[100];
int   ii, len;

strcpy( temp, sour );
len = strlen( temp );
for( ii=0; ii<len; ii++ )
   if( temp[ii] != ' ' )
      break;
strcpy( sour, temp+ii );

return( sour );

}

// 字串轉 long
long VAL_L( char *sstr )
{
char buf[128] ;
int  n = strlen( sstr );

   if ( n >= sizeof(buf) )  n= sizeof(buf) -1 ;
   memcpy(buf, sstr, (size_t) n) ;
   buf[n] = '\000' ;
   return (atol(buf) ) ;
}

// 字串轉 double
double VAL_D(char *sstr)
{
char buffer[50] ;
int  len,string_len = strlen(sstr) ;

   len =  (string_len >= 50 )  ?  49 : string_len ;
   memcpy( buffer, sstr, (size_t) len ) ;
   buffer[len] = '\000' ;

   return( strtod( buffer, (char **) 0)   ) ;
}

// long 數值轉字串
char *STR_L( long l_val, char *ptr, int num)
{
    int   n, num_pos ;
    long  i_long ;

    i_long =  (l_val>0) ? l_val : -l_val ;
    num_pos =  n =  (num > 0) ? num : -num ;
    while (n-- > 0)
    {
       ptr[n] = (char) ('0'+ i_long%10) ;
       i_long = i_long/10 ;
    }
    if ( i_long > 0 )
    {
      memset( ptr, (int) '*', (size_t) num_pos ) ;
      ptr[num_pos] = '\0';
      return ptr ;
    }
    num--;
    for (n=0; n<num; n++)
       if (ptr[n]=='0')
          ptr[n]= ' ';
       else
          break ;
    if (l_val < 0)
    {
       if ( ptr[0] != ' ' )
       {
          memset( ptr, (int) '*', (size_t) num_pos ) ;
          ptr[num_pos] = '\0';
          return ptr ;
       }
       for (n=num; n>=0; n--)
          if (ptr[n]==' ')
          {
             ptr[n]= '-' ;
             break ;
          }
    }
    ptr[num_pos] = '\0';
    return(ptr) ;
}

// long 轉字串, 但前需補 0
char *STR_L1( long l_val, char *ptr, int num)
{
    int   n, num_pos ;
    long  i_long ;

    i_long =  (l_val>0) ? l_val : -l_val ;
    num_pos =  n =  (num > 0) ? num : -num ;
    while (n-- > 0)
    {
       ptr[n] = (char) ('0'+ i_long%10) ;
       i_long = i_long/10 ;
    }
    if ( i_long > 0 )
    {
      memset( ptr, (int) '*', (size_t) num_pos ) ;
      ptr[num_pos] = '\0';
      return ptr ;
    }
    ptr[num_pos] = '\0';
    return(ptr) ;
}

// double 數字轉字串
char *STR_D( double doub_val, char *buffer, int len, int dec)
{
int   dec_val, sign_val ;
int   pre_len, post_len, sign_pos ; /* pre and post decimal point lengths */
char *result, *out_string ;

   if ( len < 0 )    len = -len  ;
   if ( len > 128 )  len =  128 ;
   memset( buffer, (int) '0', (size_t) len) ;
   out_string =  buffer ;
   if (dec > 0)
   {
      post_len =  dec ;
      if (post_len > 15)     post_len =  15 ;
      if (post_len > len-1)  post_len =  len-1 ;
      pre_len  =  len -post_len -1 ;
      out_string[ pre_len] = '.' ;
   }
   else
   {
      pre_len  =  len ;
      post_len = 0 ;
   }
   result =  fcvt( doub_val, post_len, &dec_val, &sign_val) ;
   if (dec_val > 0)
      sign_pos =   pre_len-dec_val -1 ;
   else
   {
      sign_pos =   pre_len - 2 ;
      if ( pre_len == 1) sign_pos = 0 ;
   }
   if ( dec_val > pre_len ||  pre_len<0  ||  sign_pos< 0 && sign_val)
   {
      /* overflow */
      memset( out_string, (int) '*', (size_t) len) ;
      buffer[len] =  '\000' ;
      return( buffer) ;
   }
   if (dec_val > 0)
   {
      memset( out_string, (int) ' ', (size_t) pre_len- dec_val) ;
      memmove( out_string+ pre_len- dec_val, result, (size_t) dec_val) ;
   }
   else
   {
      if (pre_len> 0)  memset( out_string, (int) ' ', (size_t) (pre_len-1)) ;
   }
   if ( sign_val)  out_string[sign_pos] = '-' ;
   out_string += pre_len+1 ;
   if (dec_val >= 0)
   {
      result+= dec_val ;
   }
   else
   {
      out_string    -= dec_val ;
      post_len += dec_val ;
   }
   if ( post_len > (int) strlen(result) )
      post_len =  (int) strlen( result) ;
   if (post_len > 0)   memmove( out_string, result, (size_t) post_len) ;

   buffer[len] =  '\000' ;
   return( buffer ) ;
}

// 目前日期
char  *szDATE( char *ret_str )
{
long      clock;
struct tm *get_time;
char      temp[100];

time(&clock);
get_time = gmtime(&clock);
STR_L1( get_time->tm_year+1900, temp, 4 );
strcpy( ret_str, temp );
STR_L1( get_time->tm_mon+1, temp, 2 );
strcat( ret_str, temp );
STR_L1( get_time->tm_mday, temp, 2 );
strcat( ret_str, temp );

return( ret_str );

}

// 傳回目前時間字串
char *szTIME( char *ret_str )
{
long        time_val ;
struct  tm  *tm_ptr ;
char        *r_ptr;

    time ( (time_t *) &time_val) ;
    tm_ptr =  localtime( (time_t *) &time_val) ;
    r_ptr = ret_str;
    STR_L( (long) tm_ptr->tm_hour, r_ptr, -2) ;
    r_ptr+= 2 ;
    *r_ptr++ = ':' ;
    STR_L( (long) tm_ptr->tm_min, r_ptr, -2) ;
    r_ptr+= 2;
    *r_ptr++ = ':' ;
    STR_L( (long) tm_ptr->tm_sec, r_ptr, -2) ;
    r_ptr+= 2 ;
    *r_ptr = '\0';

    return( ret_str );
}

// 檔案是否存在
BOOL  fFileExist( char *filename )
{
HANDLE           hFind;
WIN32_FIND_DATA  FindData;

hFind = FindFirstFile( filename, &FindData );
if( hFind == INVALID_HANDLE_VALUE )
    return( FALSE );
FindClose( hFind );

return( TRUE );

}

// 檔案刪除
int  DELE_FILE(char *filename)
{
   if( fFileExist( filename ) )
       if( remove(filename)== 0 ) return(1);
   return(0);
}

// 檔案複製
int COPY_FILE(char *fromfile,char *tofile)
{
   FILE *in,*out;
   int c;

//   if( !fFileExist(fromfile) )
//       return(0);
   if( !strcmp( fromfile, tofile ) )
       return( 0 );
   if( (in=fopen(fromfile,"rb")) == NULL ) return(0);
   if( (out=fopen(tofile,"wb")) == NULL ) return(0);
   while( (c=fgetc(in)) !=EOF ) fputc(c,out);
   fclose(in);
   fclose(out);
   return(1);
}

// 搜尋字串中某字元位置
int  cAT( char s_c, char *sour )
{
int ii,len;

len = strlen( sour );
for( ii = 0; ii< len; ii++ )
    if( sour[ii] == s_c )
        return( ii );
return( -1 );

}

// 轉換 CCYYMMDD 日期為任一格式
char  *FORMAT_DATE( char *dbf_date, char *picture )
{
char   buffer[11] ;
int    pos ;
char   chr ;
char   *chr_pos ;
char   *from = dbf_date;
char   *t_from = "CCYYMMDD" ;

    memset( buffer, (int) ' ', (size_t) sizeof(buffer) ) ;
    strcpy( buffer, picture ) ;
    while ( (chr=*t_from++)  != 0)
    {
       if (  (chr_pos= strchr(picture,chr)) ==0)
       {
          from++;
          continue ;
       }
       pos = (int) (chr_pos - picture) ;
       buffer[pos++] = *from++ ;
       while (chr== *t_from)
       {
          if (chr== picture[pos])
             buffer[pos++] = *from ;
          t_from++ ;
          from++ ;
       }
    }
    strcpy( dbf_date,buffer );

    return( dbf_date );
}

// 將數值日期轉文字 CCYYMMDD
char *DTOC( char *buffer, double datenum)
{
   STR_D( datenum, buffer, 8, 0);
   FORMAT_DATE( buffer, "CCYYMMDD" );
   return( buffer );
}

// 將日期字串轉數值日期
double CTOD(char *datestr)
{
char *ptr;

   ptr=FORMAT_DATE( datestr,"CCYYMMDD" );  // 先轉為 CCYYMMDD 字串
   return( VAL_D(ptr) );
}

// 0.01 秒為基礎之微量 delay
void  MYDELAY( int tsecs )
{
clock_t  clock1, clock2;
double   nn;

clock1 = clock();
while( 1 )
{
   clock2 = clock();
   nn = (clock2-clock1)/CLK_TCK*100;
   if( nn > tsecs ) break;
}

}

// 前補 '0' 函式
void  FULL_0( char *dest )
{
int  ii,len = strlen(dest);

    for(ii=0; ii<len; ii++)
    if( dest[ii] == ' ' )
        dest[ii] = '0';
    else
        break;
}

// 組合檔案全名, 並一併判斷檔案是否存在
BOOL  FILE_EXIST( char *dest, char *pathname, char *addhead, char *filename, char *addend )
{
    strcpy( dest, pathname );
    TRIM( dest );
    strcat( dest, addhead );
    TRIM( dest );
    strcat( dest, filename );
    TRIM( dest );
    strcat( dest, addend );
    if( !fFileExist( dest ) )
        return( FALSE );

    return( TRUE );
}

// 製造後補 '\' 的路徑名函式
void  PATH_MAKE( char *sour )
{

TRIM( sour );
if( strlen( sour ) != 0 )
    if( sour[ strlen( sour ) -1 ] != '\\' )
        strcat( sour, "\\" );

}

// 取得 email 專用日期格式
void RFC_DATE(char * pszBuffer)
{
//struct tm * ptm;
//time_t curtime;
//const char * szDays[] = { "Sun", "Mon", "Tue", "Wed",
//        "Thu", "Fri", "Sat" };
//const char * szMonths[] = { "Jan", "Feb", "Mar", "Apr", "May",
//        "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
//                             // Set the _tzname, _daylight, and _timezone variables.
//tzset();
//                             // Get the current time.
//time(&curtime);
//ptm = localtime(&curtime);
//                             // Fill out the buffer.
//wsprintf(pszBuffer, "%s, %02d %s %02d %02d:%02d:%02d %s",
//        (LPSTR)szDays[ptm->tm_wday],
//        ptm->tm_mday,
//        (LPSTR)szMonths[ptm->tm_mon],
//        ptm->tm_year,
//        ptm->tm_hour,
//        ptm->tm_min,
//        ptm->tm_sec,
//        (LPSTR)_tzname[_daylight]);
}

// TXT 檔讀取
char *READ_TXT( char *filename, char *retbuff )
{
FILE     *mo;
int      nn;
int      c;

retbuff[0] = '\0';
if( !fFileExist( filename ) )
    return( retbuff );
mo = fopen( filename, "r" );
nn = 0;
while( 1 )
{
    if( (c=fgetc(mo)) == EOF )
        break;
    retbuff[nn] = c;
    nn++;
}
retbuff[nn] = '\0';
fclose(mo);

return( retbuff );

}

// TXT 檔寫入
void  WRITE_TXT( char *savebuff, char *filename )
{
HFILE    out;
FILE     *mo;
int      len, nn;
                             // 無此檔自動產生空檔
if( !fFileExist( filename ) )
{
    strlwr( filename );
    out = _lcreat( filename, 0 );
    _lclose( out );
}
mo = fopen( filename, "w" );
                             // 為去除 chr(10) 故採逐字存入
len = strlen(savebuff);
nn = 0;
while( nn<len )
{
    if( savebuff[nn] != 10 )
        fputc( savebuff[nn], mo );
    nn++;
}
fclose( mo );

}

// 取得目前日期時間字串, 供唯一
char *getDateTimeStr( char *ret_str )
{
SYSTEMTIME      t_val;
char            temp[100];

//    GetSystemTime( &t_val );
    GetLocalTime( &t_val );
    *ret_str = '\0';
    STR_L1( t_val.wYear, temp, 4 );
    strcat( ret_str, temp );
    STR_L1( t_val.wMonth, temp, 2 );
    strcat( ret_str, temp );
    STR_L1( t_val.wDay, temp, 2 );
    strcat( ret_str, temp );
    STR_L1( t_val.wHour, temp, 2 );
    strcat( ret_str, temp );
    STR_L1( t_val.wMinute, temp, 2 );
    strcat( ret_str, temp );
    STR_L1( t_val.wSecond, temp, 2 );
    strcat( ret_str, temp );

    return( ret_str );
}


char *AsciiToUtf8(char *ascii)
{
int asciiLen;
int unicodeLen; wchar_t *unicode;
int utf8Len;    char *utf8;
// Ascii to Unicode
asciiLen = strlen(ascii);
unicodeLen = MultiByteToWideChar(CP_ACP, (DWORD)NULL, ascii, asciiLen, NULL, 0);
// allocate one more space for '/0'
unicode = malloc((unicodeLen + 2) * sizeof(wchar_t));
MultiByteToWideChar(CP_ACP, (DWORD)NULL, ascii, asciiLen, unicode, unicodeLen);
unicode[unicodeLen] = '\0';
// Unicode to UTF-8
utf8Len = WideCharToMultiByte(CP_UTF8, (DWORD)NULL, unicode, unicodeLen, NULL, 0, NULL, NULL);
// allocate one more space for '/0'
utf8 = malloc(utf8Len + 2);
WideCharToMultiByte(CP_UTF8, (DWORD)NULL, unicode, unicodeLen, utf8, utf8Len, NULL, NULL);
utf8[utf8Len] = '\0';
free(unicode);
return utf8;
}

char *Utf8ToAscii(char *utf8)
{
int utf8Len;
wchar_t *unicode;
int unicodeLen;
char *ascii;
int asciiLen;

// UTF-8 to Unicode
utf8Len = strlen(utf8);
unicodeLen = MultiByteToWideChar(CP_UTF8, (DWORD)NULL, utf8, utf8Len, NULL, 0);
// allocate one more space for '/0'
unicode = malloc((unicodeLen + 2) * sizeof(wchar_t));
MultiByteToWideChar(CP_UTF8, (DWORD)NULL, utf8, utf8Len, unicode, unicodeLen);
unicode[unicodeLen] = '\0';
// Unicode to Ascii
asciiLen = WideCharToMultiByte(CP_ACP, (WORD)NULL, unicode, unicodeLen, NULL, 0, NULL, NULL);
// allocate one more space for '/0'
ascii = malloc(asciiLen + 2);
WideCharToMultiByte(CP_ACP, (WORD)NULL, unicode, unicodeLen, ascii, asciiLen, NULL, NULL);
ascii[asciiLen] = '\0';
free(unicode);
return ascii;

}


// 取得目前日期時間字串, 採 sql 可存入格式
char *getDateTimeStr_SQL( char *ret_str )
{
SYSTEMTIME      t_val;
char            temp[100];

//    GetSystemTime( &t_val );
    GetLocalTime( &t_val );
    *ret_str = '\0';
    STR_L1( t_val.wYear, temp, 4 );
    strcat( ret_str, temp );
    strcat( ret_str, "-" );
    STR_L1( t_val.wMonth, temp, 2 );
    strcat( ret_str, temp );
    strcat( ret_str, "-" );
    STR_L1( t_val.wDay, temp, 2 );
    strcat( ret_str, temp );
    strcat( ret_str, " " );
    STR_L1( t_val.wHour, temp, 2 );
    strcat( ret_str, temp );
    strcat( ret_str, ":" );
    STR_L1( t_val.wMinute, temp, 2 );
    strcat( ret_str, temp );
    strcat( ret_str, ":" );
    STR_L1( t_val.wSecond, temp, 2 );
    strcat( ret_str, temp );
    strcat( ret_str, ".000" );

    return( ret_str );
}

// LOG 寫入
void  myLOG( char *logname, char *szFmt, ... )
{
va_list   marker;
char      szCmdStr[5000];
HFILE    out;
FILE     *mo;

                             // 組合訊息
va_start(marker, szFmt);
wvsprintf(szCmdStr, szFmt, marker);
va_end(marker);
                             // 無此檔自動產生空檔
if( !fFileExist( logname ) )
{
    out = _lcreat( logname, 0 );
    _lclose( out );
}
mo = fopen( logname, "a" );

fputs( szCmdStr, mo );

fclose( mo );

}

static  char  transfer_URL[10000];

char *myURI( char *sour )
{
char  hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
char  targ[10000];
int   sour_len, ii, jj;
int   tt=0;
int   num_char;
char  temp[2];

sour_len = strlen(sour);
jj = 0;
for( ii=0; ii<sour_len; ii++ )
{
    num_char = (int)sour[ii];
    if (num_char < 0)
        num_char = 256 + num_char;
    if( num_char < 128 )
    {
        targ[jj] = sour[ii];
        jj++;
    }
    else
    {
        targ[jj] = '%';jj++;
        while (num_char >= 16)
        {
            tt = num_char % 16;
            num_char = (int)floor(num_char / 16);
            temp[0] = hexVals[tt];
        }
        temp[1] = hexVals[num_char];
        targ[jj] = temp[1]; jj++;
        targ[jj] = temp[0]; jj++;
    }
}
targ[jj] = '\0';

strcpy( transfer_URL, targ );

return( transfer_URL );

}

void myURI_decode(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

// 轉字串為 base64
char *ToBase64(char *text, char *result)
{
    char digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int  i = 0;
    int  j = 0;
    unsigned char cur, prev, byteNum;

    while(i < strlen(text) )
    {
        cur = text[i];
        byteNum = i % 3;

        switch(byteNum)
        {
            case 0:                                     // first byte
                result[j] = digits[((unsigned char)(cur >> 2)) & 077];
                j++;
                break;

            case 1:                                     // second byte
                result[j] = digits[((unsigned char)((prev & 3) << 4 | (cur >> 4))) & 077];
                j++;
                break;

            case 2:                                     // third byte
                result[j] = digits[((unsigned char)((prev & 0x0f) << 2 | (cur >> 6))) & 077];
                j++;
                result[j] = digits[((unsigned char)(cur & 0x3f)) & 077];
                j++;
                break;
        }

        prev = cur;
        i++;
    }

    if (byteNum == 0)
    {
        result[j] = digits[((unsigned char)((prev & 3) << 4)) & 077];
        j++;
        result[j] = '=';
        j++;
        result[j] = '=';
        j++;
    }
    else if (byteNum == 1)
    {
        result[j] = digits[((unsigned char)((prev & 0x0f) << 2)) & 077];
        j++;
        result[j] = '=';
        j++;
    }

    result[j] = '\0';

    return result;
}

// 轉 base64
int ToBase64_len(char *text, char *result, int len)
{
    char digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int  i = 0;
    int  j = 0;
    unsigned char cur, prev, byteNum;

    while(i < len )
    {
        cur = text[i];
        byteNum = i % 3;

        switch(byteNum)
        {
            case 0:                                     // first byte
                result[j] = digits[((unsigned char)(cur >> 2)) & 077];
                j++;
                break;

            case 1:                                     // second byte
                result[j] = digits[((unsigned char)((prev & 3) << 4 | (cur >> 4))) & 077];
                j++;
                break;

            case 2:                                     // third byte
                result[j] = digits[((unsigned char)((prev & 0x0f) << 2 | (cur >> 6))) & 077];
                j++;
                result[j] = digits[((unsigned char)(cur & 0x3f)) & 077];
                j++;
                break;
        }

        prev = cur;
        i++;
    }

    if (byteNum == 0)
    {
        result[j] = digits[((unsigned char)((prev & 3) << 4)) & 077];
        j++;
        result[j] = '=';
        j++;
        result[j] = '=';
        j++;
    }
    else if (byteNum == 1)
    {
        result[j] = digits[((unsigned char)((prev & 0x0f) << 2)) & 077];
        j++;
        result[j] = '=';
        j++;
    }

    result[j] = '\0';

    return j;
}

