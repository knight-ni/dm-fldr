#include "fldr.h" 
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include <sys/time.h>

#define LOBSIZE 51200

/* 
CREATE TABLE FLDRTEST(
USERID INTEGER NOT NULL,
UNAME VARCHAR2(100),
UDATE TIMESTAMP(0),
UDATETIME TIMESTAMP(6),
UFLOAT FLOAT,
UDECIMAL NUMBER(12,4),
UTEXT TEXT,
UCLOB CLOB,
UBYTE BFILE,
UBLOB BLOB)
*/


char* itoa(int src){
    int temp = -1;
    int tv = src>0?src:-src;
    int length = 0;
    while ((tv = tv/10)>0) {
        length++;
    }
    length++;
    tv = src>0?src:-src;
    char* des = (char*)malloc(sizeof(char)*(length+1));
    memset(des, 0, length+1);
    for (int i=0; i<length; i++) {
        int v = 1;
        for (int j=length-i; j>1;j--) {
            v = v*10;
        }
        temp = tv/(v);
        des[i] = (temp + 48);
        if (temp == 0) {
            temp = 1;
        }
        tv = tv%(temp*v);
    }
    des[length] = '\0';
    if (src<0) {
        char* nSrc = (char*)malloc((strlen(des)+2)*sizeof(char));
        sprintf(nSrc, "-%s",des);
        free(des);
        des = nSrc;
    }
    return des;
}

char* append(char *mychar,char *tmpchar){
    char* outstr=malloc(strlen(mychar)+10);
    char* tmp=mychar;
    strcpy(outstr,tmp);
    strcat(outstr,tmpchar);
    return outstr;
}

char *nowtime_us()
{
    struct timeval us;
    gettimeofday(&us,NULL);
    struct tm t;
    static char date_time[64];
    static char usec[64];
    strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", localtime_r(&us.tv_sec, &t));
    sprintf(usec, ".%06ld", us.tv_usec);
    strcat(date_time,usec);
    return date_time;
}

void chkerr(FLDR_RETURN rt,fhinstance instance,int thd_no){
    int errcode;
    int errlen=255;
    char errmsg[errlen];
    int msglen;
    if(rt!=FLDR_SUCCESS && rt!=FLDR_SUCCESS_WITH_INFO){
        rt = fldr_get_diag(instance,thd_no,&errcode,errmsg,errlen,&msglen);
        printf("rc:%d thd_no:%d errmgs:%s\n",rt,thd_no,errmsg);
        fldr_free(instance); 
        exit(9);
    }
}

void echo(unsigned char *sz,int size)
{
	int i = 0;
	while(i<size) {
         	printf("%02x ", sz[i]);
		++i;
                printf("%d\n",i);
	}
	printf("\n");
}

int main() 
{ 
 fhinstance instance; 
 FLDR_RETURN ret; 
 char server[20] = "192.168.56.177:5237"; 
 char user[20] = "SYSDBA"; 
 char pwd[20] = "Dameng123"; 
 char table[20] = "FLDRTEST"; 

 int i,fd,status; 
 time_t timep;
 struct tm *tp;

/* variables for thread 1 */

 int c1[10];        //INT
 int c1_len[10]; 

 char c2[10][100];  //VARCHAR
 int c2_len[10]; 

 char c3[10][100];  //DATE
 int c3_len[10]; 

 char c4[10][100];  //DATETIME
 int c4_len[10]; 

 float c5[10];   //FLOAT
 int c5_len[10]; 

 double c6[10];  //DOUBLE
 int c6_len[10]; 

 char c7[10][LOBSIZE];  //TEXT
 int c7_len[10];

 unsigned char c8[10][LOBSIZE];  //CLOB
 int c8_len[10];

 char c9[10][100];  //BFILE
 int c9_len[10];

 unsigned char c10[10][LOBSIZE];
 int c10_len[10];

/* variables for thread 2 */

 int c11[10];        //INT
 int c11_len[10];

 char c12[10][100];  //VARCHAR
 int c12_len[10];

 char c13[10][100];  //DATE
 int c13_len[10];

 char c14[10][100];  //DATETIME
 int c14_len[10];

 float c15[10];   //FLOAT
 int c15_len[10];

 double c16[10];  //DOUBLE
 int c16_len[10];

 char c17[10][LOBSIZE];  //TEXT
 int c17_len[10];

 unsigned char c18[10][LOBSIZE];  //CLOB
 int c18_len[10];

 char c19[10][100];  //BFILE
 int c19_len[10];

 unsigned char c20[10][LOBSIZE];
 int c20_len[10];

 srand((unsigned)time(0));

 FILE *mytext = fopen("/etc/passwd","rt");
 FILE *myclob = fopen("/etc/services","rb");
 FILE *myblob = fopen("cat.jpg","rb");
 //status = fread(lobbuf,sizeof(unsigned char),LOBSIZE,p);
 //echo(lobbuf,status);  // show binary content in lobbuf
/* only work for hex_char convert manually 
 char *lobbuf=(char *)malloc(LOBSIZE*2);
 char mybuf[2];
 unsigned char tmp[4] = {0};
 sprintf(lobbuf,"0x");
 while(!feof(p)){
    int ch = fgetc(p);
    memcpy(tmp,&ch,4);
    //printf("%x %x %x %x",tmp[0],tmp[1],tmp[2],tmp[3]);
    sprintf(mybuf,"%02x",tmp[0]);
    strcat(lobbuf,mybuf);
 }
*/

 for (i = 0; i < 10; i++) 
 { 
 c1[i] = i; 

 sprintf(c2[i],append("TEST STRING",itoa(i)));
 c2_len[i] = strlen(c2[i]); 

 time(&timep);
 tp = gmtime(&timep);
 sprintf(c3[i],"%d-%d-%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
 c3_len[i] = strlen(c3[i]); 

 sprintf(c4[i],nowtime_us());
 c4_len[i] = strlen(c4[i]);

 c5[i] = rand()/(double)(RAND_MAX/100);

 c6[i] = rand()/(double)(RAND_MAX/100);
 
 rewind(mytext);
 status = fread(c7[i],sizeof(char),LOBSIZE,mytext);
 c7_len[i] = status;

 rewind(myclob);
 status = fread(c8[i],sizeof(char),LOBSIZE,myclob);
 c8_len[i] = status;

 sprintf(c9[i],"KNIGHTDIR:cat.jpg");   // NEED DIRECTORY POINT TO HERE CREATED BY YOURSELF 
 c9_len[i] = strlen(c9[i]); 

 rewind(myblob);
 status = fread(c10[i],sizeof(unsigned char),LOBSIZE,myblob);
 c10_len[i] = status; 

 } 

 fclose(mytext);
 fclose(myclob);
 fclose(myblob);
 
 ret = fldr_alloc(&instance); 
 ret = fldr_set_attr(instance,FLDR_ATTR_SERVER,server,20);
 ret = fldr_set_attr(instance,FLDR_ATTR_UID,user,20);
 ret = fldr_set_attr(instance,FLDR_ATTR_PWD,pwd,20);
 ret = fldr_set_attr(instance,FLDR_ATTR_TABLE,table,20);
 
 chkerr(ret,instance,1); //FOR MAIN THREAD
 ret = fldr_initialize(instance, FLDR_TYPE_BIND, NULL, NULL, user, pwd, table);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_bind_nth(instance, 1, FLDR_C_INT, NULL, NULL, c1, sizeof(int), 0, c1_len, 0); 
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_bind_nth(instance, 2, FLDR_C_CHAR, NULL, NULL, c2, 100, 100*10, c2_len, 0); 
 chkerr(ret,instance,1); 

 ret = fldr_bind_nth(instance, 3, FLDR_C_CHAR, "YYYY-MM-DD", NULL, c3, 100, 100*10, c3_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 4, FLDR_C_CHAR, "YYYY-MM-DD HH:MI:SS.FF6", NULL, c4, 100, 100*10, c4_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 5, FLDR_C_FLOAT, NULL, NULL, c5, sizeof(float), 0, c5_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 6, FLDR_C_DOUBLE, NULL, NULL, c6, sizeof(double), 0, c6_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 7, FLDR_C_CHAR, NULL, NULL, c7, LOBSIZE, LOBSIZE*10, c7_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 8, FLDR_C_BINARY, NULL, NULL, c8, LOBSIZE, LOBSIZE*10, c8_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 9, FLDR_C_CHAR, NULL, NULL, c9, 100, 100*10, c9_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 10, FLDR_C_BINARY, NULL, NULL, c10, LOBSIZE, LOBSIZE*10, c10_len, 0); 
 chkerr(ret,instance,1); 

// BIND FOR THREAD 2
 mytext = fopen("/etc/passwd","rt");
 myclob = fopen("/etc/services","rb");
 myblob = fopen("cat1.jpeg","rb");

 for (i = 0; i < 10; i++)
 {
 c11[i] = i;

 sprintf(c12[i],append("TEST STRING",itoa(i)));
 c12_len[i] = strlen(c12[i]);

 time(&timep);
 tp = gmtime(&timep);
 sprintf(c13[i],"%d-%d-%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
 c13_len[i] = strlen(c13[i]);

 sprintf(c14[i],nowtime_us());
 c14_len[i] = strlen(c14[i]);

 c15[i] = rand()/(double)(RAND_MAX/100);

 c16[i] = rand()/(double)(RAND_MAX/100);

 rewind(mytext);
 status = fread(c17[i],sizeof(char),LOBSIZE,mytext);
 c17_len[i] = status;

 rewind(myclob);
 status = fread(c18[i],sizeof(char),LOBSIZE,myclob);
 c18_len[i] = status;

 sprintf(c19[i],"KNIGHTDIR:cat.jpg");   // NEED DIRECTORY POINT TO HERE CREATED BY YOURSELF
 c19_len[i] = strlen(c19[i]);

 rewind(myblob);
 status = fread(c20[i],sizeof(unsigned char),LOBSIZE,myblob);
 c20_len[i] = status;

 }

 
 fclose(mytext);
 fclose(myclob);
 fclose(myblob);

 ret = fldr_bind_nth(instance, 1, FLDR_C_INT, NULL, NULL, c11, sizeof(int), 0, c11_len, 1);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_bind_nth(instance, 2, FLDR_C_CHAR, NULL, NULL, c12, 100, 100*10, c12_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 3, FLDR_C_CHAR, "YYYY-MM-DD", NULL, c13, 100, 100*10, c13_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 4, FLDR_C_CHAR, "YYYY-MM-DD HH:MI:SS.FF6", NULL, c14, 100, 100*10, c14_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 5, FLDR_C_FLOAT, NULL, NULL, c15, sizeof(float), 0, c15_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 6, FLDR_C_DOUBLE, NULL, NULL, c16, sizeof(double), 0, c16_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 7, FLDR_C_CHAR, NULL, NULL, c17, LOBSIZE, LOBSIZE*10, c17_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 8, FLDR_C_BINARY, NULL, NULL, c18, LOBSIZE, LOBSIZE*10, c18_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 9, FLDR_C_CHAR, NULL, NULL, c19, 100, 100*10, c19_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 10, FLDR_C_BINARY, NULL, NULL, c20, LOBSIZE, LOBSIZE*10, c20_len, 1);
 chkerr(ret,instance,1);

 ret = fldr_sendrows_nth(instance, 10, 0, 1); 
 chkerr(ret,instance,1);
 ret = fldr_sendrows_nth(instance, 10, 1, 1); 
 chkerr(ret,instance,2);
 ret = fldr_uninitialize(instance, FLDR_UNINITILIAZE_COMMIT); 
 chkerr(ret,instance,1);
 ret = fldr_free(instance); 
 chkerr(ret,instance,1);
 return 0; 
} 
