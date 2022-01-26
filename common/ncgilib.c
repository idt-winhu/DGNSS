
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// �� stdin �ǤJ�Ѽ���쵲�c
typedef struct
{
   char       name[31];                     // ��W���ץi�� 30
   char      *value;                        // ��Ʊī��мu��
   long       v_len;                        // value ��ƪ���(ps:�Ω�D�{���L�k�� strlen())
}  CGIENTRY;
static  CGIENTRY     *cgiin_stru;
static  int           cgiin_N = 0;

// ���U browser �ǤJ�Ѽ�(�Y name1=value1&name2=value2... �r��)
static char *cgi_data( void *f, char *request, long *ret_len )
{
char  *input = NULL, ch;
char  *temp = NULL;
long   length;
long   i=0;

// �����i��H GET �覡�e�u�Ѽ�
if( !strcmp( request, "GET" ) )
{
    // ����Ū���t���ܼ� "QUERY_STRING"
    input = getenv( "QUERY_STRING" );
    if( input!=NULL )
       *ret_len = strlen(input);
    else
       *ret_len = 0;
    return input;
}
// �_�h�Y�O POST �覡�Y�� stdin �e�J�j�q���, ��ƪ��ץѨt���ܼ� "CONTENT_LENGTH" �q��
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

// ���ǤJ��Ʀܤ��j�r����(ps:�q�`���O '&')
static long  split( char *line, char stop, char *ret_data )
{
long   x=0;

for( x=0; (*(line+x) != '\0') && (*(line+x) != stop); x++ )
    *(ret_data+x) = *(line+x);
*(ret_data+x) = '\0';

return( x );

}

// �N�ǤJ 16 �i��ƭ��ॿ�T(ps: '%' �a�Y)
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

// �P�_����`��
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

// �p��ΰt�m����@���һݰO����
static long  coun_fd_len( char *line, char stop )
{
long   x=0;

// �p�⦹��j�p
for( x=0; (*(line+x) != '\0') && (*(line+x) != stop); x++ );

return( x );

}

//**************************************************************************************************************
// �� Browser �ǤJ�ѼƥD�{��
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

                             // Ū���t���ܼ� REQUEST_METHOD, �H�o���ѼƶǤJ�覡
method = getenv( "REQUEST_METHOD" );
if( method == NULL )
    return( -1 );                              // �����I�� exe ����
                             // ���U���
input = cgi_data( (void *)stdin, method, &length );
if( length == 0 )
    return( 0 );                              // �L�Ѽ�
                             // �P�_����`��
cgiin_N = calc_fields( input, '=' );
                             // �t�m����x�s�O����, �ǳƱN�����J name value ��쵲�c
cgiin_stru = (CGIENTRY *)malloc( sizeof(CGIENTRY)*(cgiin_N+1) );
                             // �̧ǱN�U����J
nn = 0;
for( count=0; input[count] != '\0' && count<length; count++ )
{
                             // �p��ΰt�m����O����
    vlen = coun_fd_len( input+count, '&' );
    one_fd_buf = (char *)GlobalAlloc( GMEM_FIXED, sizeof(char)*vlen+1 );
                             // ���U������W
    count = count + split( input+count, '=', name );
    count++;
                             // ���U���椺�e
    count = count + split( input+count, '&', one_fd_buf );
    convert( one_fd_buf );                       // '%' 16�i��ƭ����ഫ
                             // ��s����쵲�c��
    p = cgiin_stru + nn;
    strcpy(p->name, name);
    p->value = one_fd_buf;
    p->v_len = vlen-1;

    nn ++;
}
                             // cgi_data Ū���Ȧs�O��������
GlobalFree( input );

return( cgiin_N );

}

// ��쵲�c�O��������
//***************************
void  cgi_release_data( void )
{
int        ii;
CGIENTRY  *p;

   // ������U�椧 value �t�m
   for( ii=0; ii < cgiin_N; ii++ )
   {
       p = cgiin_stru + ii;
       if( p->value != NULL )
           GlobalFree( p->value );
       p->value = NULL;
   }
   free( cgiin_stru );
}

// ���V�Y�� value ����, �H��@�~����
//***********************************
char  *cgiGFD( char *fld_name )
{
int           ii;
CGIENTRY     *p;
                             // ���j�M����
for(ii=0;ii<cgiin_N;ii++)
{
    p = cgiin_stru + ii;
    if( !strcmp(p->name,fld_name) )
        break;
}
if( ii != cgiin_N )
    return( cgiin_stru[ii].value );
                             // �L����Ǧ^ NULL
return( NULL );

}

// �q�Φ^�Ǻ����T��
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

// �Ǧ^ value ��ƪ���
// (ps: �]�p���ɻݰ� base64 �ഫ, strlen() �� int �j�p����)
//*********************************************************
long cgiGFD_LEN( char *fld_name )
{
int           ii;
CGIENTRY     *p;
                             // ���j�M����
for(ii=0;ii<cgiin_N;ii++)
{
    p = cgiin_stru + ii;
    if( !strcmp(p->name,fld_name) )
        break;
}
                             // �L����Ǧ^ 0
if( ii == cgiin_N )
    return( 0 );
                             // ��즹��, �p��j�p

return( cgiin_stru[ii].v_len );

}
