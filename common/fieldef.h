
# ifndef FIELDEF_H
# define FIELDEF_H

#ifdef __cplusplus
extern "C" {
#endif

// DBF/TEXT/SQL/CSV �q�θ�Ʈw���ޱ����c
typedef struct
{
//   char    Ename[11];
   char    Ename[21];               // ��� 11 �Ω� DBF, �� 21 �Ω��L��Ʈw, DBF �h�ۦ�p�ߩR�W���W�L
   char    type;
   int     width;
   int     decimals;
   char    Cname[17];
   int     Cname_len;

//   char    value[256];       // DBF ���̦h�u�� 255
//   char    value[1024];      // �ĳ̤j�ƥH�@�־A�Ω� csvface �Ψ�L�i���j���, DBF �ϥήɦA��^
   char    value[3000];      // �Ω� LINE �ϸꫬ�A����(SQL text ���A�̤j 512byte, �G���Ω� csv, SQL�ݧ�� binary ���)

                             // �H�U�T�楻�� PDM �W�ϥ�, ���������Τ@�@��, ���B��t�J
   int     ctrl_mode;        // ��X�J�Ҧ�( 0/��ܤο�J, 1/�����, 2/����)
   int     modi_mode;        // �۩w��Ҧ�( 0/�i�R��, 1/�i�襤��Ϊ���, 2/����)
   int     fld_sec;          // ���K����( 0/���q, 1/�K, 2/���K, 3/�����K... )
}  FIELDEF;

                                       // ���]�w
void  DBFIELD_SET( FIELDEF *p, char *Ename, char Etype, int Elen, int Edec,
                   char *Cname, int Clen, int CtrlMode, int ModiMode, int FldSec  );
                                       // �]�w�n�ާ@��쵲�c
void  DBFIELD_USE( FIELDEF *FieldStru, int FieldColN );
                                       // ����W��X����m
int   DBFIELD_WHERE( char *FieldName );
                                       // �M���w�İ�
void  DBFIELD_CLEAR( void );
                                       // �m�J�Y�� value ��
void  DBFIELD_REPL( char *FieldName, char *save_value );
                                       // ���X�Y�� value ��
char *DBFIELD_GFD( char *FieldName, char *ret_value );

char *DBFIELD_GFV( char *FieldName );


#ifdef __cplusplus
}
#endif

# endif

