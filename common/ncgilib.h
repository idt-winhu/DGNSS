
# ifndef NCGILIB_H
# define NCGILIB_H

#ifdef __cplusplus
extern "C" {
#endif

// �@�Ψ禡
                                                 // ���J�ǤJ���
int    cgi_load_data( void );
                                                 // ���J���O��������
void   cgi_release_data( void );
                                                 // ���V�Y�� value ����, �H��@�~����
char  *cgiGFD( char *fld_name );
                                                 // html �覡�^��
void   cgi_html_msg( char *message1, char *message2 );
                                                 // �Y���Ƥj�p
long   cgiGFD_LEN( char *fld_name );

#ifdef __cplusplus
}
#endif

# endif
