#include "fldr.h" 
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <pthread.h> 
 
#define LOBSIZE 51200
#define ROWS 10

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

typedef struct {
 int c1[ROWS];        //INT
 int c1_len[ROWS];

 char c2[ROWS][100];  //VARCHAR
 int c2_len[ROWS];

 char c3[ROWS][100];  //DATE
 int c3_len[ROWS];

 char c4[ROWS][100];  //DATETIME
 int c4_len[ROWS];

 float c5[ROWS];   //FLOAT
 int c5_len[ROWS];

 double c6[ROWS];  //DOUBLE
 int c6_len[ROWS];

 char c7[ROWS][LOBSIZE];  //TEXT
 int c7_len[ROWS];

 unsigned char c8[ROWS][LOBSIZE];  //CLOB
 int c8_len[ROWS];

 char c9[ROWS][100];  //BFILE
 int c9_len[ROWS];

 unsigned char c10[ROWS][LOBSIZE];
 int c10_len[ROWS];
} mydata_t;

typedef struct {
  fhinstance instance;
  mydata_t data;
} para_t;

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

void loaddata(mydata_t *data){
 int i,status;
 time_t timep;
 struct tm *tp;

 FILE *mytext = fopen("/etc/passwd","rt");
 FILE *myclob = fopen("/etc/services","rb");
 FILE *myblob = fopen("cat.jpg","rb");

 for (i = 0; i < ROWS; i++) {
 data->c1[i] = i;

 sprintf(data->c2[i],append("TEST STRING",itoa(i)));
 data->c2_len[i] = strlen(data->c2[i]);

 time(&timep);
 tp = gmtime(&timep);
 sprintf(data->c3[i],"%d-%d-%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
 data->c3_len[i] = strlen(data->c3[i]);

 sprintf(data->c4[i],nowtime_us());
 data->c4_len[i] = strlen(data->c4[i]);

 data->c5[i] = rand()/(double)(RAND_MAX/100);

 data->c6[i] = rand()/(double)(RAND_MAX/100);

 rewind(mytext);
 status = fread(data->c7[i],sizeof(char),LOBSIZE,mytext);
 data->c7_len[i] = status;

 rewind(myclob);
 status = fread(data->c8[i],sizeof(char),LOBSIZE,myclob);
 data->c8_len[i] = status;

 sprintf(data->c9[i],"KNIGHTDIR:cat.jpg");   // NEED DIRECTORY POINT TO HERE CREATED BY YOURSELF
 data->c9_len[i] = strlen(data->c9[i]);

 rewind(myblob);
 status = fread(data->c10[i],sizeof(unsigned char),LOBSIZE,myblob);
 data->c10_len[i] = status;
 }

 fclose(mytext);
 fclose(myclob);
 fclose(myblob);
}

void myth1(para_t *mypara){
 FLDR_RETURN ret;
 fhinstance instance;
 mydata_t *data;
 instance = mypara->instance;
 data = &mypara->data;

 ret = fldr_bind_nth(instance, 1, FLDR_C_INT, NULL, NULL, data->c1, sizeof(int), 0, data->c1_len, 0);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_bind_nth(instance, 2, FLDR_C_CHAR, NULL, NULL, data->c2, 100, 100*ROWS, data->c2_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 3, FLDR_C_CHAR, "YYYY-MM-DD", NULL, data->c3, 100, 100*ROWS, data->c3_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 4, FLDR_C_CHAR, "YYYY-MM-DD HH:MI:SS.FF6", NULL, data->c4, 100, 100*ROWS, data->c4_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 5, FLDR_C_FLOAT, NULL, NULL, data->c5, sizeof(float), 0, data->c5_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 6, FLDR_C_DOUBLE, NULL, NULL, data->c6, sizeof(double), 0, data->c6_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 7, FLDR_C_CHAR, NULL, NULL, data->c7, LOBSIZE, LOBSIZE*ROWS, data->c7_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 8, FLDR_C_BINARY, NULL, NULL, data->c8, LOBSIZE, LOBSIZE*ROWS, data->c8_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 9, FLDR_C_CHAR, NULL, NULL, data->c9, 100, 100*ROWS, data->c9_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_bind_nth(instance, 10, FLDR_C_BINARY, NULL, NULL, data->c10, LOBSIZE, LOBSIZE*ROWS, data->c10_len, 0);
 chkerr(ret,instance,1);

 ret = fldr_sendrows_nth(instance, ROWS, 0, 1);
 chkerr(ret,instance,1);
}


int main() 
{ 
 fhinstance instance; 
 FLDR_RETURN ret; 
 char server[20] = "192.168.56.177:5237"; 
 char user[20] = "SYSDBA"; 
 char pwd[20] = "Dameng123"; 
 char table[20] = "FLDRTEST"; 

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

 ret = fldr_alloc(&instance); 
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_set_attr(instance,FLDR_ATTR_SERVER,server,20);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_set_attr(instance,FLDR_ATTR_UID,user,20);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_set_attr(instance,FLDR_ATTR_PWD,pwd,20);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 ret = fldr_set_attr(instance,FLDR_ATTR_TABLE,table,20);
 chkerr(ret,instance,1); //FOR MAIN THREAD
 
 ret = fldr_initialize(instance, FLDR_TYPE_BIND, NULL, NULL, user, pwd, table);
 chkerr(ret,instance,1); //FOR MAIN THREAD

 mydata_t data = {0,'0','0','0',0,0,'0',0,'0',0};
 loaddata(&data); //make multi data struct for multi thread work

 para_t mypara = {0,0};
 mypara.instance = instance;
 mypara.data = data;

 pthread_t th1;
 pthread_create(&th1,NULL,(void*)myth1,&mypara);
 pthread_join(th1,NULL);

 ret = fldr_uninitialize(instance, FLDR_UNINITILIAZE_COMMIT); 
 chkerr(ret,instance,1);

 ret = fldr_free(instance); 
 chkerr(ret,instance,1);
 return 0; 
} 
