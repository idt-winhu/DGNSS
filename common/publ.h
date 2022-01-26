# ifndef PUBL_H
# define PUBL_H

#ifdef __cplusplus
extern "C" {
#endif

// �@�Ψ禡
                                                 // ���ݰ���ܧ���
void  WaitForExecute( char *run_prog, char *run_parm );
                                                 // SUBSTR �����r��, from �q 1 �}�l
char *SUBS( char *dest,char *sour,int from,int cn );
                                                 // �r��H�Y�r����N
char *REPLI( char *dest, char ch, int num );
                                                 // ��ťզr��
char *szSPACE( char *dest,int len );
                                                 // �����ؿ�
BOOL  CHANGE_DIR( char *check_path );
                                                 // �إߥؿ�
void  MAKE_DIR( char *path );
                                                 // �ɺ��r�����
char *MAP_LEN( char *dest,int len );
                                                 // �r�굲���ťեh��
char *TRIM( char *sour );
                                                 // �r��e�Y�ťեh��
char *LTRIM( char *sour );
                                                 // �r���� long
long  VAL_L( char *sstr );
                                                 // �r���� double
double VAL_D(char *sstr);
                                                 // long �ƭ���r��
char *STR_L( long l_val, char *ptr, int num);
                                                 // long ��r��, ���e�ݸ� 0
char *STR_L1( long l_val, char *ptr, int num);
                                                 // double �Ʀr��r��
char *STR_D( double doub_val, char *buffer, int len, int dec);
                                                 // �ثe���
char *szDATE( char *ret_str );
                                                 // �Ǧ^�ثe�ɶ��r��
char *szTIME( char *ret_str );
                                                 // �ɮ׬O�_�s�b
BOOL  fFileExist( char *filename );
                                                 // �ɮקR��
int   DELE_FILE(char *filename);
                                                 // �ɮ׽ƻs
int   COPY_FILE(char *fromfile,char *tofile);
                                                 // �j�M�r�ꤤ�Y�r����m
int   cAT( char s_c, char *sour );
                                                 // �ഫ CCYYMMDD ��������@�榡
char  *FORMAT_DATE( char *dbf_date, char *picture );
                                                 // �N�ƭȤ�����r CCYYMMDD
char  *DTOC( char *buffer, double datenum);
                                                 // �N����r����ƭȤ��
double CTOD(char *datestr);
                                                 // 0.01 ����¦���L�q delay
void   MYDELAY( int tsecs );
                                                 // �e�� '0' �禡
void   FULL_0( char *dest );
                                                 // �զX�ɮץ��W, �ä@�֧P�_�ɮ׬O�_�s�b
BOOL   FILE_EXIST( char *dest, char *pathname, char *addhead, char *filename, char *addend );
                                                 // �s�y��� '\' �����|�W�禡
void   PATH_MAKE( char *sour );
                                                 // ���o email �M�Τ���榡
void   RFC_DATE(char * pszBuffer);
                                                 // TXT ��Ū��
char  *READ_TXT( char *filename, char *retbuff );
                                                 // TXT �ɼg�J
void   WRITE_TXT( char *savebuff, char *filename );
                                                 // ���o�ثe����ɶ��r��
char  *getDateTimeStr( char *ret_str );

char *AsciiToUtf8(char *ascii);
char *Utf8ToAscii(char *utf8);

char *getDateTimeStr_SQL( char *ret_str );
                                                 // LOG �g�J
void   myLOG( char *logname, char *szFmt, ... );

char  *myURI( char *sour );

void   myURI_decode(char *dst, const char *src);

char *ToBase64(char *text, char *result);       // ��r�ꬰ base64

int ToBase64_len(char *text, char *result, int len);  // �� base64

#ifdef __cplusplus
}
#endif

# endif
