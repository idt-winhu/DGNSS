
# ifndef FIELDEF_H
# define FIELDEF_H

#ifdef __cplusplus
extern "C" {
#endif

// DBF/TEXT/SQL/CSV 通用資料庫欄位操控結構
typedef struct
{
//   char    Ename[11];
   char    Ename[21];               // 原長 11 用於 DBF, 改 21 用於其他資料庫, DBF 則自行小心命名不超過
   char    type;
   int     width;
   int     decimals;
   char    Cname[17];
   int     Cname_len;

//   char    value[256];       // DBF 式最多只能 255
//   char    value[1024];      // 採最大化以一併適用於 csvface 及其他可較大資料, DBF 使用時再改回
   char    value[3000];      // 用於 LINE 圖資型態極限(SQL text 型態最大 512byte, 故此用於 csv, SQL需改用 binary 欄位)

                             // 以下三欄本於 PDM 上使用, 但為使欄位統一共用, 此處亦含入
   int     ctrl_mode;        // 輸出入模式( 0/顯示及輸入, 1/僅顯示, 2/隱藏)
   int     modi_mode;        // 自定欄模式( 0/可刪改, 1/可改中文及長度, 2/隱藏)
   int     fld_sec;          // 機密等級( 0/普通, 1/密, 2/機密, 3/極機密... )
}  FIELDEF;

                                       // 欄位設定
void  DBFIELD_SET( FIELDEF *p, char *Ename, char Etype, int Elen, int Edec,
                   char *Cname, int Clen, int CtrlMode, int ModiMode, int FldSec  );
                                       // 設定要操作欄位結構
void  DBFIELD_USE( FIELDEF *FieldStru, int FieldColN );
                                       // 依欄名找出欄位位置
int   DBFIELD_WHERE( char *FieldName );
                                       // 清除緩衝區
void  DBFIELD_CLEAR( void );
                                       // 置入某欄 value 值
void  DBFIELD_REPL( char *FieldName, char *save_value );
                                       // 取出某欄 value 值
char *DBFIELD_GFD( char *FieldName, char *ret_value );

char *DBFIELD_GFV( char *FieldName );


#ifdef __cplusplus
}
#endif

# endif

