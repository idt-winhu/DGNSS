
// HTML INPUT GET ���c
typedef struct
{
    char name[21];                     // ��W���ץi�� 20
    char value[2001];                  // ��ƥi�F 2000
}  CGIENTRY;

extern CGIENTRY FAR *cgiin_stru;
extern HGLOBAL       glbCGIENTRY;
extern int           cgiin_N, cgiin_maxN;

// �@�Ψ禡
                                                 // ���J�ǤJ�Ѽ�(ps:�۰ʰt�m�O����)
BOOL  cgi_load_data( int alloc_N );
                                                 // �� Browser �ǤJ�ѼƥD�{��(ps:�~���O����)
BOOL  cgi_load_data_to( CGIENTRY *html_fild, int fild_N );
                                                 // ��쵲�c�O��������
void  cgi_release_data( void );
                                                 // ���V�Y�� value ����, �H��@�~����
char  FAR *cgiGFD( char *fld_name );
                                                 // �q�Φ^�Ǻ����T��
void  cgi_html_msg( char *message1, char *message2 );
                                                 // ���C��Ʈɤ��C�⤬��
void  cgi_color_switch( BOOL *sw, char *color );

