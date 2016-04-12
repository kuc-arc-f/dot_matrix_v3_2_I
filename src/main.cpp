//
// LED dot matrix(8x8) for IchigoJam
//
#include "mbed.h"
#include <stdio.h>
#include <string>
#include "font8ex.h"

#define mLED_COL 8

int mFont_min=0x20;
int mFont_max=0x7D;

bool kstate = false;
unsigned char kbuf;

const int mMaxStr=20;

int leds[8][8];
int patterns[2][8][8];

unsigned int pat16[mMaxStr][8];

//int nPattern=0;
int nPatternMax=0;
int mSTAT =0;
int mSTAT_RSV_START=1;
int mSTAT_RSV_END  =2;
int mSTAT_DISP=3;
int iMC=0;

string mReceive ="";
string mReceive2="";
string mResponse="";

//pin
//pin
DigitalOut mPin_1( dp1);
DigitalOut mPin_2( dp2);
DigitalOut mPin_3( dp4);
DigitalOut mPin_4( dp6);
DigitalOut mPin_5( dp5);
DigitalOut mPin_6( dp9);
DigitalOut mPin_7( dp10 );
DigitalOut mPin_8( dp11 );

DigitalOut mPin_9( dp13);
DigitalOut mPin_10(dp14);
DigitalOut mPin_11(dp17);
DigitalOut mPin_12(dp18 );
DigitalOut mPin_13(dp25 );
DigitalOut mPin_14(dp27 );
DigitalOut mPin_15(dp26 );
DigitalOut mPin_16(dp28 );

DigitalOut mCols[8]={mPin_13, mPin_3, mPin_4, mPin_10, mPin_6, mPin_11, mPin_15, mPin_16};
DigitalOut mRows[8]={mPin_9, mPin_14, mPin_8, mPin_12, mPin_1, mPin_7 , mPin_2, mPin_5};


Serial mPc(USBTX, USBRX);


//Util
//
void dump_pattern(int src[8][8]){
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      printf("%d" ,src[i][j]);
    }
      printf(" \n");
  }
  printf("--------\n");
  printf("\n");
}

void dump_pattern16(unsigned int src[8]){
  for(int i=0; i<8; i++){
//      printf("%[08X]" ,src[i]);
      printf("[%02d]" ,src[i]);
  }
  printf("\n");
  printf("--------\n");
  printf("\n");
}

void clear_pattern(){
    for(int i=0;i< mMaxStr; i++){
        for(int j=0; j< mLED_COL; j++){
            pat16[i][j]=0;
        }
    }
}

void setPattern_all(int pattern, int iRow, char *line){
    int buff[8][8];
    for(int i=0; i< mLED_COL; i++){
        if(line[i]=='1'){
            buff[iRow][i]=1;
        }else{
            buff[iRow][i]=0;
        }
    }

    for(int row=0; row< mLED_COL; row++){
        for(int col=0; col< mLED_COL; col++){
            patterns[pattern][col][row] = buff[row][7-col];
        }
    }
}

char* intToBin(int src, int iRow, int pattern){
    char *ret;
        int    iLen=mLED_COL;
        char    buff[iLen+1];
        int     bin;
        int     i1;

        bin = src;
        buff[iLen] = '\0';
        for(int j = 0; j < iLen; j++){
            buff[j]='0';
        }
        
        for(i1 = 0; i1 < iLen; i1++){
                if(i1 != 0 && bin == 0)
                        break;
                if(bin % 2 == 0)
                        buff[(iLen-1)-i1] = '0';
                else
                        buff[(iLen-1)-i1] = '1';
                bin = bin / 2;
        }
//printf(">>>%s\n", buff );
        setPattern_all(pattern, iRow, buff);
        
        ret= &buff[0];
   return ret;
}

unsigned int transUInt(unsigned char c){
    if('0'<=c && '9'>=c) return (c-0x30);//0x30は'0'の文字コード
    if('A'<=c && 'F'>=c) return (c+0x0A-0x41);//0x41は'A'の文字コード
    if('a'<=c && 'f'>=c) return (c+0x0A-0x61);//0x61は'a'の文字コード
    return 0;
}

//
unsigned int pow_get(int src, int iPnum){
    int ret=1;

    if(iPnum > 0){
        ret=src;
        for(int i=0; i<iPnum-1; i++){
          ret=ret * src;
        }
    }
    return ret;
}

//
unsigned int hexToUInt(char *str)
{
// printf("hexToUInt ,ptr=%s\n" , &str[0]);
    unsigned int i,j=0;
    char*str_ptr=str+strlen(str)-1;

    for(i=0;i<strlen(str);i++){
//printf("tr=%d\n" ,pow_get(16, i));
//printf("pow=%d\n" , (unsigned int)pow(16,i));
        j+=transUInt(*str_ptr--)*pow_get(16, i);
//printf("j=%d ,i=%d\n", j, i);
    }
    return j;
}

void get_font8_line(int iSt, int pattern){
    char  *ret;
    unsigned int  buff[mLED_COL+1];
    int iPos=iSt-mFont_min;
    buff[0]= (unsigned int)font8ex[iPos *mLED_COL];
    buff[1]= (unsigned int)font8ex[iPos *mLED_COL +1];
    buff[2]= (unsigned int)font8ex[iPos *mLED_COL +2];
    buff[3]= (unsigned int)font8ex[iPos *mLED_COL +3];
    buff[4]= (unsigned int)font8ex[iPos *mLED_COL +4];
    buff[5]= (unsigned int)font8ex[iPos *mLED_COL +5];
    buff[6]= (unsigned int)font8ex[iPos *mLED_COL +6];
    buff[7]= (unsigned int)font8ex[iPos *mLED_COL +7];
//printf("buff[0]=%08d\n" , buff[0] );
//printf("buff[1]=%08d\n" , buff[1] );
//printf("buff[2]=%08X\n" , buff[2] );
//printf("buff[3]=%08X\n" , buff[3] );
    pat16[pattern][0]=buff[0];
    pat16[pattern][1]=buff[1];
    pat16[pattern][2]=buff[2];
    pat16[pattern][3]=buff[3];
    pat16[pattern][4]=buff[4];
    pat16[pattern][5]=buff[5];
    pat16[pattern][6]=buff[6];
    pat16[pattern][7]=buff[7];
}

void get_font8(int iSt, int pattern){
    char *line;
    int iPos=iSt;
    if( iSt <mFont_min){
        iPos =mFont_min;
//printf( "get_font8.St=%d\n",iSt);
    }
    if( iSt >mFont_max){
        iPos =mFont_min;
//printf( "get_font8.mx.iSt=%X\n",iSt);
    }
    //
    get_font8_line(iPos,  pattern);
    //dump_pattern(patterns[pattern]);
}

int drawtext(const char *ss){
  unsigned char c;
  int ret=0;
  int iPos=0;
  int i=0;
  int iNum=0;
  char buff[2+1];
  while(ss[i] != '\0'){
    sprintf(buff, "%02X", ss[i]);
    unsigned int iNum_1 = hexToUInt(buff);
//printf("iNum=%d\n" ,iNum_1);
//if(!((iNum_1==19) || (iNum_1==16))){
    if(!((ss[i]=='\r') || (ss[i]=='\n'))){
        get_font8( iNum_1 , i);
        iPos++;
    }
    i++;
  }
//printf("iSz=%d\n" ,i);
//printf("iPos=%d\n" ,iPos);
  ret=iPos;
  return ret;
}

//
void init_proc(){
    for (int i = 1; i <= 8; i++) {
        mCols[i-1]=0;
    }
    for (int i = 1; i <= 8; i++) {
        mRows[i-1]=0;
    }   
}
//
void display_proc(int pattern){
    for(int k=0; k < mLED_COL; k++){
        for(int i=0; i < 10; i++){     // Loop-LED-ON
            for(int col=0; col<8 ; col++){
                mRows[col]=0;
                for(int row=0; row<8; row++){
                    if(leds[col][row]==1){
                        mCols[row]=1;
                    }
                    wait_us(200);
                    if(leds[col][row]==1){
                        mCols[row]=0;
                    }
                    mCols[row]=0;
                }
                mRows[col]=1;
            }
        }
        //slide
//mPc.printf("pattern=%d\n" ,pattern);
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
              if(j ==7){
 // mPc.printf("dat-7=%d\n" ,patterns[nPattern +1][i][iCt]);
               leds[i][7] = patterns[1][i][k];
              }else{
               leds[i][j] = leds[i][j+1];
              }
          }
        }
    } //for_k
// mPc.printf("display_proc.nPattern=%d ,mSTAT=%d, k=%d\n", nPattern ,mSTAT, k);
}

//
bool Is_validLine(string src){
    int ret=false;
    int nMax=32;
    int iLen= strlen(src.c_str() );
    string sRes32="";
    if(iLen > nMax){
      sRes32= src.substr(0, nMax);
    }else{
      sRes32=src;
    }
    char sLine[nMax];
//    int iLen= strlen(sLine);
    if(iLen>= 2){
        sprintf(sLine, "%s" ,sRes32.c_str());
//printf("valid.iLen=%d\n" , iLen );
//printf("x=%02X\n", sLine[iLen-2] );   
//printf("x=%02X\n", sLine[iLen-1] ); 
        if(sLine[iLen-2]=='\r'){
            if(sLine[iLen-1]=='\n'){
//    printf("[sE2].x=%02X\n", sLine[iLen-2] );   
//    printf("[sE1].x=%02X\n", sLine[iLen-1] ); 
                ret=true;
            }                        
        }
    }
    return ret;
}

//
void  set_nextString(const char *ss){
  int i=0;
  while(ss[i] != '\0'){
//    sprintf(buff, "%02X", ss[i]);
    if(!((ss[i]=='\r') || (ss[i]=='\n'))){
        mReceive2+=ss[i];
    }
    i++;
  }
//printf("mReceive2=%s\n" , mReceive2.c_str() );  
}

//
void send_nextMatrix(string src, string sNo){
  int iStr =src.length();
  string buff_1="";
  string buff_2="";
  string sSend="";
  if(iStr > 10){
    buff_1=src.substr(0,10);
    sSend=sNo + buff_1;
    mPc.printf(sSend.c_str() );
    wait_ms(100);
    buff_2=src.substr(10);
    //buff_2=buff_2+'\r'+'\n';
    mPc.printf("%s\r\n" ,buff_2.c_str());
  }else{
    buff_1=src;
    //sSend=sNo + buff_1+'\r'+'\n';
    sSend=sNo + buff_1;
    mPc.printf("%s\r\n" , sSend.c_str());
  }
}
// 
void proc_uart(){
    if( mPc.readable()) {
        char c= mPc.getc();
        mResponse+= c;
    }else{
                if(mSTAT ==mSTAT_RSV_START){
                  if(mResponse.length() > 3){
//printf("mResponse=%s,resLen%d\n" ,mResponse.c_str() ,mResponse.length() );
                    string sStat=mResponse.substr(2,1);
//printf("sStat=%s\n" ,sStat.c_str() );
                    if(sStat== "1"){
                        string sNo=mResponse.substr(0,2);
//printf("sNo=%s\n" , sNo.c_str() );
                        iMC = atoi( sNo.c_str() );
                        mReceive =mResponse.substr(3 );
//printf("mReceive=%s\n" ,mReceive.c_str() );
                        if(iMC <2){
//                          mReceive =" "+ mReceive;
                          mReceive =" "+ mReceive;
                        }
printf("mReceive[B]1=%s\n" ,mReceive.c_str() );
                        if(mReceive.length() >= 5 ){
                            if(iMC <2){
                              mReceive+=" ";
                            }                            
                            mSTAT =mSTAT_DISP;
                        }else{
                            mSTAT =mSTAT_RSV_END;
                        }
                        /*
                        if(Is_validLine(mResponse)==true ){
                            if(iMC <2){
                              mReceive+=" ";
                            }                            
                            mSTAT =mSTAT_DISP;
                        }else{
                            mSTAT =mSTAT_RSV_END;
                        }
                        */
                        mResponse="";
                    }else{
                        mResponse="";                        
                    }
                  }
                }else if(mSTAT ==mSTAT_RSV_END){
                  if(mResponse.length() > 0){
//printf("[b]mResponse=%s,resLen%d\n" ,mResponse.c_str() ,mResponse.length() );
                        mReceive += mResponse;
printf("[b]mReceive2=%s\n" ,mReceive.c_str()  );
                        if(Is_validLine(mResponse)==true ){
                            if(iMC <2){
                               mReceive+=" ";
                            }
                            mSTAT =mSTAT_DISP;
                        }
                        mResponse="";
                  }
                } //if_mStat

    }// if_read                
}

//
int main() {
    mSTAT = mSTAT_RSV_START;

//    mPc.baud(9600 );
    mPc.baud( 115200 );
    mPc.printf("#Start-main\n");
    init_proc();
    while(1){
        wait_ms(5);
//wait_ms(100);
//printf("mSTAT=%d\n", mSTAT);
        if(mSTAT != mSTAT_DISP){
            proc_uart();
        }else{            
//            string sDisp=mReceive.substr(0, mReceive.length()-2);
            set_nextString(mReceive.c_str());
            nPatternMax =drawtext(mReceive2.c_str() );
printf("mReceive2=%s\n", mReceive2.c_str() );
printf("mReceive2.length()=%d\n", mReceive2.length());
            for(int k=0; k < nPatternMax; k++){
//mPc.printf("main.k=%d\n", k );
                for(int iPos=0; iPos < 2; iPos++){
                    for(int row=0;row < mLED_COL; row++ ){
                        char *bit= intToBin(pat16[k+iPos][row], row, iPos);
                    }
                }
//debug
//for(int iPos=0; iPos < 2; iPos++){
//      dump_pattern(patterns[iPos]);
//}
//mPc.printf("main.k=%d\n", k );
              display_proc(k);
              if(k==0){
                  string sBuff=mReceive2;
                  //if(iMC < 2){
                  //    sBuff=mReceive2.substr(1, mReceive2.length()-1 );
                  //}
                  char buff_no[4];
                  iMC=iMC+1;
                  sprintf(buff_no, "%02d1", iMC);
                  send_nextMatrix(sBuff, buff_no);
              }                                          
            }//for_k
            mSTAT = mSTAT_RSV_START;
            clear_pattern();
//printf("#End-dispkay.len=%d \n", mReceive2.length() );
 printf("#End-dispkay\n"  );
            mReceive="";
            mReceive2="";
        }
    }
    printf("#End-main \n");
}