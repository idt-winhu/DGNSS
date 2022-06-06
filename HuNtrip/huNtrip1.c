

#include <windows.h>
#include "D:\DGNSS\RtkLib\RtkLib.h"

/* function prototypes -------------------------------------------------------*/
static int decode_rtcm2_1(rtcm_t *rtcm);

/* constants -----------------------------------------------------------------*/
#define RTCM2PREAMB 0x66        /* rtcm ver.2 frame preamble */
#define BYTE unsigned char

/*rtkcmn----------------------------------------------------------------------------------------------------------*/
/* extract unsigned/signed bits ------------------------------------------------
 * extract unsigned/signed bits from byte data
 * args   : unsigned char *buff I byte data
 *          int    pos    I      bit position from start of data (bits)
 *          int    len    I      bit length (bits) (len<=32)
 * return : extracted unsigned/signed bits
 *-----------------------------------------------------------------------------*/
static unsigned int getbitu_1(const unsigned char *buff, int pos, int len)
{
    unsigned int bits = 0;
    int i;
    for (i = pos; i<pos + len; i++) bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
    return bits;
}
static int getbits_1(const unsigned char *buff, int pos, int len)
{
    unsigned int bits = getbitu_1(buff, pos, len);
    if (len <= 0 || 32 <= len || !(bits&(1u << (len - 1)))) return (int)bits;
    return (int)(bits | (~0u << len)); /* extend sign */
}
/* decode navigation data word -------------------------------------------------
 * check party and decode navigation data word
 * args   : unsigned int word I navigation data word (2+30bit)
 *                              (previous word D29*-30* + current word D1-30)
 *          unsigned char *data O decoded navigation data without parity
 *                              (8bitx3)
 * return : status (1:ok,0:parity error)
 * notes  : see reference [1] 20.3.5.2 user parity algorithm
 *-----------------------------------------------------------------------------*/
static int decode_word_1(unsigned int word, unsigned char *data)
{
    const unsigned int hamming[] = {
        0xBB1F3480, 0x5D8F9A40, 0xAEC7CD00, 0x5763E680, 0x6BB1F340, 0x8B7A89C0
    };
    unsigned int parity = 0, w;
    int i;
    
    //printf("5, decodeword: word=%08x\n", word);
    
    if (word & 0x40000000) word ^= 0x3FFFFFC0;
    
    for (i = 0; i<6; i++) {
        parity <<= 1;
        for (w = (word&hamming[i]) >> 6; w; w >>= 1) parity ^= w & 1;
    }
    if (parity != (word & 0x3F)) return 0;
    
    for (i = 0; i<3; i++) data[i] = (unsigned char)(word >> (22 - i * 8));
    return 1;
}

/* get current time in utc -----------------------------------------------------
 * get current time in utc
 * args   : none
 * return : current time in utc
 *-----------------------------------------------------------------------------*/
static double timeoffset_1 = 0.0;        /* time offset (s) */

gtime_t timeget_1(void)
{
    gtime_t time;
    double ep[6] = { 0 };
#ifdef WIN32
    SYSTEMTIME ts;
    
    GetSystemTime(&ts); /* utc */
    ep[0] = ts.wYear; ep[1] = ts.wMonth;  ep[2] = ts.wDay;
    ep[3] = ts.wHour; ep[4] = ts.wMinute; ep[5] = ts.wSecond + ts.wMilliseconds*1E-3;
#else
    struct timeval tv;
    struct tm *tt;
    
    if (!gettimeofday(&tv, NULL) && (tt = gmtime(&tv.tv_sec))) {
        ep[0] = tt->tm_year + 1900; ep[1] = tt->tm_mon + 1; ep[2] = tt->tm_mday;
        ep[3] = tt->tm_hour; ep[4] = tt->tm_min; ep[5] = tt->tm_sec + tv.tv_usec*1E-6;
    }
#endif
    time = epoch2time(ep);
    
#ifdef CPUTIME_IN_GPST /* cputime operated in gpst */
    time = gpst2utc(time);
#endif
    return timeadd(time, timeoffset_1);
}

/*rtcm2----------------------------------------------------------------------------------------------------------*/
/* adjust hourly rollover of rtcm 2 time -------------------------------------*/
static void adjhour_1(rtcm_t *rtcm, double zcnt)
{
    double tow, hour, sec;
    int week;
    
    /* if no time, get cpu time */
    if (rtcm->time.time == 0) rtcm->time = utc2gpst(timeget());
    tow = time2gpst(rtcm->time, &week);
    hour = floor(tow / 3600.0);
    sec = tow - hour*3600.0;
    if (zcnt<sec - 1800.0) zcnt += 3600.0;
    else if (zcnt>sec + 1800.0) zcnt -= 3600.0;
    rtcm->time = gpst2time(week, hour * 3600 + zcnt);
}

/* decode type 1/9: differential gps correction/partial correction set -------*/
static int decode_type1_1(rtcm_t *rtcm)
{
    int i = 48, fact, udre, prn, sat, iod;
    double prc, rrc;
    int sat_id;
    char temp[1000];
    
    sprintf(temp, "4, decode_type1: len=%d\n", rtcm->len);
    //MessageBox(NULL, temp, "4,", MB_OK );
    
    while (i + 40 <= rtcm->len * 8) {
        fact = getbitu_1(rtcm->buff, i, 1); i += 1;
        udre = getbitu_1(rtcm->buff, i, 2); i += 2;
        prn = getbitu_1(rtcm->buff, i, 5); i += 5;
        prc = getbits_1(rtcm->buff, i, 16); i += 16;
        rrc = getbits_1(rtcm->buff, i, 8); i += 8;
        iod = getbits_1(rtcm->buff, i, 8); i += 8;
        if (prn == 0) prn = 32;
        if (prc == 0x80000000 || rrc == 0xFFFF8000) 
        {
            sprintf(temp, "2, rtcm2 1 prc/rrc indicates satellite problem: prn=%d\n", prn);
            //MessageBox(NULL, temp, "2,", MB_OK );

            continue;
        }
        if (rtcm->dgps) {
            sat = satno(SYS_GPS, prn);
            rtcm->dgps[sat - 1].t0 = rtcm->time;
            rtcm->dgps[sat - 1].prc = prc*(fact ? 0.32 : 0.02);
            rtcm->dgps[sat - 1].rrc = rrc*(fact ? 0.032 : 0.002);
            rtcm->dgps[sat - 1].iod = iod;
            rtcm->dgps[sat - 1].udre = udre;
        }
    }
    for (sat_id = 0; sat_id < 32; sat_id++)
    {
        if (rtcm->dgps[sat_id].iod > 0)
        {
            sprintf(temp, "sat:%d\tUDRE:%1.0f\tIOD:%3d\tPRC:%10f\n",
                   sat_id+1,
                   rtcm->dgps[sat_id].udre,
                   rtcm->dgps[sat_id].iod,
                   rtcm->dgps[sat_id].prc);

            //MessageBox(NULL, temp, "sat", MB_OK );
        }
    }
    
    return 7;
}

static int decode_rtcm2_1(rtcm_t *rtcm, double *ret_zcnt)
{
    double zcnt;
    int staid, seqno, stah, ret = 0, type;
    char  temp[1000];

    type = getbitu_1(rtcm->buff, 8, 6);
    sprintf(temp, "3, decode_rtcm2: type=%2d len=%3d\n", type, rtcm->len);
    //MessageBox(NULL, temp, "3,", MB_OK );
    
    if ((zcnt = getbitu_1(rtcm->buff, 24, 13)*0.6) >= 3600.0) {

        sprintf(temp, "2, rtcm2 modified z-count error: zcnt=%.1f\n", zcnt);
        //MessageBox(NULL, temp, "2,", MB_OK );
        return -1;
    }
    *ret_zcnt = zcnt;

    adjhour_1(rtcm, zcnt);
    staid = getbitu_1(rtcm->buff, 14, 10);
    seqno = getbitu_1(rtcm->buff, 37, 3);
    stah = getbitu_1(rtcm->buff, 45, 3);
    if (seqno - rtcm->seqno != 1 && seqno - rtcm->seqno != -7) {
        
    }
    rtcm->seqno = seqno;
    rtcm->stah = stah;
    
    if (rtcm->outtype) {
        sprintf(rtcm->msgtype, "RTCM %2d (%4d) zcnt=%7.1f staid=%3d seqno=%d",
                type, rtcm->len, zcnt, staid, seqno);
    }
    if (type == 3 || type == 22 || type == 23 || type == 24) {
        if (rtcm->staid != 0 && staid != rtcm->staid) {
         
            sprintf(temp, "2, rtcm2 station id changed: %d->%d\n", rtcm->staid, staid);
    //MessageBox(NULL, temp, "2,", MB_OK );
        }
        rtcm->staid = staid;
    }
    if (rtcm->staid != 0 && staid != rtcm->staid) {
        
        sprintf(temp, "2, rtcm2 station id invalid: %d %d\n", staid, rtcm->staid);
    //MessageBox(NULL, temp, "2,", MB_OK );
        return -1;
    }
    switch (type) {
        case  1: ret = decode_type1_1(rtcm); break;
            //case  3: ret = decode_type3(rtcm); break;
        case  9: ret = decode_type1_1(rtcm); break;
            //case 14: ret = decode_type14(rtcm); break;
            //case 16: ret = decode_type16(rtcm); break;
            //case 17: ret = decode_type17(rtcm); break;
            //case 18: ret = decode_type18(rtcm); break;
            //case 19: ret = decode_type19(rtcm); break;
            //case 22: ret = decode_type22(rtcm); break;
            //case 23: ret = decode_type23(rtcm); break; /* not supported */
            //case 24: ret = decode_type24(rtcm); break; /* not supported */
            //case 31: ret = decode_type31(rtcm); break; /* not supported */
            //case 32: ret = decode_type32(rtcm); break; /* not supported */
            //case 34: ret = decode_type34(rtcm); break; /* not supported */
            //case 36: ret = decode_type36(rtcm); break; /* not supported */
            //case 37: ret = decode_type37(rtcm); break; /* not supported */
            //case 59: ret = decode_type59(rtcm); break; /* not supported */
    }
    if (ret >= 0) {
        if (1 <= type&&type <= 99) rtcm->nmsg2[type]++; else rtcm->nmsg2[0]++;
    }
    return ret;
}

/* input rtcm 2 message from stream --------------------------------------------
 * fetch next rtcm 2 message and input a message from byte stream
 * args   : rtcm_t *rtcm IO   rtcm control struct
 *          unsigned char data I stream data (1 byte)
 * return : status (-1: error message, 0: no message, 1: input observation data,
 *                  2: input ephemeris, 5: input station pos/ant parameters,
 *                  6: input time parameter, 7: input dgps corrections,
 *                  9: input special message)
 * notes  : before firstly calling the function, time in rtcm control struct has
 *          to be set to the approximate time within 1/2 hour in order to resolve
 *          ambiguity of time in rtcm messages.
 *          supported msgs RTCM ver.2: 1,3,9,14,16,17,18,19,22
 *          refer [1] for RTCM ver.2
 *-----------------------------------------------------------------------------*/
static int input_rtcm2_1(rtcm_t *rtcm, unsigned char data, double *zcnt)
{
    unsigned char preamb;
    int i;
    char  temp[1000];
    
    //printf("5, input_rtcm2: data=%02x\n", data);
    
    if ((data & 0xC0) != 0x40) return 0; /* ignore if upper 2bit != 01 */
    
    for (i = 0; i<6; i++, data >>= 1) { /* decode 6-of-8 form */
        rtcm->word = (rtcm->word << 1) + (data & 1);
        
        /* synchronize frame */
        if (rtcm->nbyte == 0) {
            preamb = (unsigned char)(rtcm->word >> 22);
            if (rtcm->word & 0x40000000) preamb ^= 0xFF; /* decode preamble */
            if (preamb != RTCM2PREAMB) continue;
            
            /* check parity */
            if (!decode_word_1(rtcm->word, rtcm->buff)) continue;
            rtcm->nbyte = 3; rtcm->nbit = 0;
            continue;
        }
        if (++rtcm->nbit<30) continue; else rtcm->nbit = 0;
        
        /* check parity */
        if (!decode_word_1(rtcm->word, rtcm->buff + rtcm->nbyte)) {

            sprintf(temp, "2, rtcm2 partity error: i=%d word=%08x\n", i, rtcm->word);
    //MessageBox(NULL, temp, "2,", MB_OK );
            rtcm->nbyte = 0; rtcm->word &= 0x3;
            continue;
        }
        rtcm->nbyte += 3;
        if (rtcm->nbyte == 6) rtcm->len = (rtcm->buff[5] >> 3) * 3 + 6;
        if (rtcm->nbyte<rtcm->len) continue;
        rtcm->nbyte = 0; rtcm->word &= 0x3;
        
        /* decode rtcm2 message */
        return decode_rtcm2_1(rtcm, zcnt);
    }
    return 0;
}

int Ntrip1_deocder(char *getBuffer, int dwRead, char *ret_str)
{
    BYTE   szBuffer[30000];
    int    ret = -1;
    rtcm_t rtcm1 = { 0 };
    int    ii;
    int    sat_id;
    char   sstr[1000];
    dgps_t dgps[32];
    double  zcnt;
    BOOL    have7 = FALSE;

    //MessageBox(NULL, getBuffer, "0,", MB_OK );

    init_rtcm(&rtcm1);
    rtcm1.dgps = dgps;

    if (dwRead > 0)
    {
        memcpy(szBuffer, getBuffer, dwRead);
        
        for (ii = 0; ii < dwRead;ii++)
        {
            ret = input_rtcm2_1(&rtcm1, szBuffer[ii], &zcnt);
            if( ret == 7 )
            {
                have7 = TRUE;
            }
        }
    }

    // 組成 json 回傳
    strcpy( ret_str, "{\"err_msg\":\"\",\"rtcm2type1\":[");

    if( have7 )
    {
        for (sat_id = 0,ii=0; sat_id < 32; sat_id++)
        {
            if ( abs(dgps[sat_id].prc) > 0 && abs(dgps[sat_id].prc) < 10000)
            {
                sprintf(sstr, "\n{\"prn\":\"%d\",\n\"prc\":\"%10f\",\n\"rrc\":\"%10f\",\n\"zcount\":\"%7f\"}",
                    sat_id+1,
                    dgps[sat_id].prc,
                    dgps[sat_id].rrc,
                    zcnt
                );
                if( ii !=0 )
                    strcat(ret_str, ",");
                strcat( ret_str, sstr );

                ii++;
            }
        }
        ret = ii;
    }
    else
    {
        ret = -1;
    }
    strcat( ret_str, "]}");

return( ret );
}


