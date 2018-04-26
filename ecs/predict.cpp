#include "predict.h"
#include <bits/stdc++.h>
using namespace std;

const time_t TIME = 5*CLOCKS_PER_SEC; //SA running time
const int VM_TYPE_NUM = 18; 
const int SERVER_TYPE_NUM = 3;
const double eps = 1e-6;
const int INF = 0x7f7f7f7f;

/* ------------------Server-------------------  */
char serverName[SERVER_TYPE_NUM+5][50]={"","General","Large-Memory", "High-Performance"};
int serverCPU[SERVER_TYPE_NUM+5];
int serverMEM[SERVER_TYPE_NUM+5];
int serverDISK[SERVER_TYPE_NUM+5];
int serverNum[SERVER_TYPE_NUM+5];
int serverExist[SERVER_TYPE_NUM+5];
int serverTot=0 ;
struct Server
{
    int type;
    int freeCPU;
    int freeMEM;
    int VMNum[VM_TYPE_NUM+5];
    bool operator < (const Server &tmp) const
    {
        return type < tmp.type;
    }
}serverList[2000];

/*--------------Input&&Output--------------*/
struct TrainData
{
    char type[20];
    char date[20];
    char ID[50];
}data[30000];
char output[10000000];

/*------------------VM---------------------------*/
int VMList[30000];
int VM_type_num;
map<string,int> VMStringToInt;
int VMCPU[VM_TYPE_NUM+5];
int VMMEM[VM_TYPE_NUM+5];
int VMExist[VM_TYPE_NUM+5];
int VMFor[VM_TYPE_NUM+5];

/*-----------------Train-----------------------*/
int trainDays;
int trainLen;
char trainStartDate[20];
char trainEndDate[20];
double trainMVAvg[VM_TYPE_NUM+5];
double trainVMTot[VM_TYPE_NUM+5];
double trainVMDetail[VM_TYPE_NUM+5][1000];
double TrainEndPredictStart;

/*-------------Predict-----------------------*/
double predictDays;
char predictStartDate[20];
char predictEndDate[20];

int predictVMTotNum;
int predictVMNum[VM_TYPE_NUM+5];
int predictVMTotCPU;
int predictVMTotMEM;
int extraVMNum[VM_TYPE_NUM+5];
double  predictpercent;

/*---------------DP---------------------------*/
int dp[VM_TYPE_NUM+5][1000][1000];
int fillVMNum[VM_TYPE_NUM+5][1000][1000];

/*----------------Init-------------------------*/
void Init_VMware()
{
    VMStringToInt["flavor1"]=1; VMCPU[1]=1;  VMMEM[1]=1;
    VMStringToInt["flavor2"]=2; VMCPU[2]=1;  VMMEM[2]=2;
    VMStringToInt["flavor3"]=3; VMCPU[3]=1;  VMMEM[3]=4;
    VMStringToInt["flavor4"]=4; VMCPU[4]=2;  VMMEM[4]=2;
    VMStringToInt["flavor5"]=5; VMCPU[5]=2;  VMMEM[5]=4;
    VMStringToInt["flavor6"]=6; VMCPU[6]=2;  VMMEM[6]=8;
    VMStringToInt["flavor7"]=7; VMCPU[7]=4;  VMMEM[7]=4;
    VMStringToInt["flavor8"]=8; VMCPU[8]=4;  VMMEM[8]=8;
    VMStringToInt["flavor9"]=9; VMCPU[9]=4;  VMMEM[9]=16;
    VMStringToInt["flavor10"]=10; VMCPU[10]=8; VMMEM[10]=8;
    VMStringToInt["flavor11"]=11; VMCPU[11]=8; VMMEM[11]=16;
    VMStringToInt["flavor12"]=12; VMCPU[12]=8; VMMEM[12]=32;
    VMStringToInt["flavor13"]=13; VMCPU[13]=16; VMMEM[13]=16;
    VMStringToInt["flavor14"]=14; VMCPU[14]=16; VMMEM[14]=32;
    VMStringToInt["flavor15"]=15; VMCPU[15]=16; VMMEM[15]=64;
    VMStringToInt["flavor16"]=16; VMCPU[16]=32; VMMEM[16]=32;
    VMStringToInt["flavor17"]=17; VMCPU[17]=32; VMMEM[17]=64;
    VMStringToInt["flavor18"]=18; VMCPU[18]=32; VMMEM[18]=128;
}

/*---------------Date function-------------*/
bool IsLeap(int year)
{
    return (year % 4 ==0 || year % 400 ==0) && (year % 100 !=0);
}

bool StringToDate(char *date, int& year, int& month, int& day)
{
    sscanf(date,"%d-%d-%d",&year,&month,&day);
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year))
        DAY[1] = 29;
    return year >= 0 && month<=12 && month>0 && day<=DAY[month-1] && day>0;
}

int DayInYear(int year, int month, int day)
{
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year))
        DAY[1] = 29;
    for(int i=0; i<month - 1; ++i)
        day += DAY[i];
    return day;
}

int DaysBetween2Date(char *date1, char *date2)
{
    //取出日期中的年月日
    int year1, month1, day1;
    int year2, month2, day2;
    if(!StringToDate(date1, year1, month1, day1) || !StringToDate(date2, year2,month2,day2))
    {
        cout<<"输入的日期格式不正确";
        return -1;
    }
    if(year1 == year2 && month1 == month2)
        return day1 > day2 ? day1 - day2 : day2 - day1; 
    //如果年相同
    else if(year1 == year2)
    {
        int d1, d2;
        d1 = DayInYear(year1, month1, day1);
        d2 = DayInYear(year2, month2, day2);
        return d1 > d2 ? d1 - d2 : d2 - d1;
        //年月都不相同
    }
    else
    {
        //确保year1年份比year2早
        if(year1 > year2)
        {
            //swap进行两个值的交换
            swap(year1, year2);
            swap(month1, month2);
            swap(day1, day2);
        }
        int d1,d2,d3;
        if(IsLeap(year1))
            d1 = 366 - DayInYear(year1,month1, day1); //取得这个日期在该年还于下多少天
        else
            d1 = 365 - DayInYear(year1,month1, day1);
        d2 = DayInYear(year2,month2,day2); //取得在当年中的第几天
        cout<<"d1:"<<d1<<", d2:"<<d2;
        d3 = 0;
        for(int year = year1 + 1; year < year2; year++)
        {
            if(IsLeap(year))
                d3 += 366;
            else
                d3 += 365;
        }
        return d1 + d2 + d3;
    }
}

/*--------------About Input --------------*/
bool cmp(int a,int b)
{
    return a > b;
}

void common_input(char * info[MAX_INFO_NUM]) 
{
    int real_server_num;
    sscanf(info[0],"%d",&real_server_num);
    for(int i=1; i<=real_server_num; i++)
    {
        char tmpServerName[50];
        int tmpServerCPU,tmpServerMEM,tmpServerDISK;
        sscanf(info[i],"%s %d %d %d",tmpServerName,&tmpServerCPU,&tmpServerMEM,&tmpServerDISK);
        for(int j=1;j<=3;j++)
            if(strcmp(serverName[j],tmpServerName)==0)
            {
                serverCPU[j] = tmpServerCPU;
                serverMEM[j] = tmpServerMEM;
                serverDISK[j] = tmpServerDISK;
                serverExist[j] = 1;
                break;
            }
    }

    sscanf(info[2+real_server_num],"%d",&VM_type_num);
   
    for(int i=0;i<VM_type_num;i++)
    {
        int tempa,tempb;
        char inputtempname[30];
        sscanf(info[3+real_server_num+i],"%s %d %d",inputtempname,&tempa,&tempb);
        string tempname=inputtempname;
        VMExist[VMStringToInt[tempname]]=1;
        VMFor[i]=VMStringToInt[tempname];
    }
    sort(VMFor,VMFor+VM_type_num,cmp);
    sscanf(info[4+real_server_num+VM_type_num],"%s",predictStartDate);  // get 2015-02-20  no hava 00:00:00
    sscanf(info[5+real_server_num+VM_type_num],"%s",predictEndDate);      // and so go
    printf("%s %s\n",predictStartDate,predictEndDate);
    predictDays=DaysBetween2Date(predictStartDate,predictEndDate)+1;

/*    for(int i=1;i<=SERVER_TYPE_NUM;i++)
        if(i==3||i==2)
            serverExist[i] = 0;*/
}

void train_input(char * Data[MAX_DATA_NUM], int data_num)
{
    for(int i=0;i<data_num;i++)
        sscanf(Data[i],"%s %s %s",data[i].ID,data[i].type,data[i].date);

    strcpy(trainStartDate,data[0].date);
    strcpy(trainEndDate,data[0].date);
    for(int i=0;i<data_num;i++)
    {
        if(strcmp(trainStartDate,data[i].date)>0)
            strcpy(trainStartDate,data[i].date);
        if(strcmp(trainEndDate,data[i].date)<0)
            strcpy(trainEndDate,data[i].date);
    }
    trainLen=DaysBetween2Date(trainStartDate,trainEndDate);
    for(int i=0;i<data_num;i++)
    {
        int tempday = DaysBetween2Date(data[i].date,trainEndDate);
        trainVMDetail[VMStringToInt[string(data[i].type)]][tempday]++;
    }
    TrainEndPredictStart=DaysBetween2Date(predictStartDate,trainEndDate);
    trainLen = min(trainLen,60);
}

void Denoise()
{
    double avg[VM_TYPE_NUM+5]={0};
    double sigma[VM_TYPE_NUM+5]={0};
    double amount[VM_TYPE_NUM+5]={0};
    double w=4;
    
    
    for(int i=1;i<=VM_TYPE_NUM;i++)
    {
        for(int j=0;j<trainLen;j++)
            if(trainVMDetail[i][j])
            {
                avg[i] += trainVMDetail[i][j];
                amount[i]++;
            }
    
        avg[i] /= amount[i];
        for(int j=0;j<trainLen;j++)
            if(trainVMDetail[i][j])
                sigma[i] += pow(avg[i]-trainVMDetail[i][j],2);
        sigma[i] = sqrt(sigma[i]/(amount[i] - 1));
        for(int j=0;j<trainLen;j++)    
            if(trainVMDetail[i][j]>avg[i]+sigma[i]*w)
                trainVMDetail[i][j] = avg[i] + sigma[i]*w;
    }
}

void Show()
{
    printf("predictVMTotNum:%d\n",predictVMTotNum);
    for(int i=1;i<=VM_TYPE_NUM;i++)
        printf("flavor%d:%d\n",i,predictVMNum[i]);
    printf("serverTot:%d\n",serverTot);
    for(int i=0;i<serverTot;i++)
        printf("%s freeCPU:%d freeMEM:%d\n",serverName[serverList[i].type],serverList[i].freeCPU,serverList[i].freeMEM);
      
}

void CalculatePercent()
{
    int serverTotCPU = 0;
    int serverTotMEM = 0;
    for(int i=1;i<=SERVER_TYPE_NUM;i++)
        if(serverExist[i])
        {
            printf("serverCPU:%d serverMEM:%d\n",serverCPU[i],serverMEM[i]);
            serverTotCPU += serverCPU[i]*serverNum[i];
            serverTotMEM += serverMEM[i]*serverNum[i];
        }
    printf("predictVMTotCPU:%d predictVMTotMEM:%d\n",predictVMTotCPU,predictVMTotMEM);
    printf("serverTotCPU:%d serverTotMEM:%d\n",serverTotCPU,serverTotMEM);
    printf("CPUPercent:%.6f MEMPercent:%.6f\n",predictVMTotCPU*1.0/serverTotCPU,predictVMTotMEM*1.0/serverTotMEM);
    printf("avgPercent:%.6f\n",(predictVMTotCPU*1.0/serverTotCPU+predictVMTotMEM*1.0/serverTotMEM)/2);
    printf("PREIDICT PERCENT:%lf\n",predictpercent);
}

double everVMTot[VM_TYPE_NUM+5]; 

double TentrainVMTot[VM_TYPE_NUM+5];
void Train()
{
    trainDays = 25;
    for(int i=1;i<=VM_TYPE_NUM;i++)
        for(int j=0;j<trainDays;j++)
            trainVMTot[i] += trainVMDetail[i][j];
   int ExtrainDays = min(60,trainLen);
    for(int i=1;i<=VM_TYPE_NUM;i++)
        for(int j=0;j<ExtrainDays;j++)
            everVMTot[i] += trainVMDetail[i][j];

   double sum=0;
    for(int i=1;i<=VM_TYPE_NUM;i++){
        trainMVAvg[i] = 1.0*trainVMTot[i]/trainDays;
        everVMTot[i] = 1.0*everVMTot[i]/ExtrainDays;
        sum+=everVMTot[i];
      
    }
    
   double p=1,a=0.5;
    for(int i=1;i<=VM_TYPE_NUM;i++){
        p=1.2+min((((TrainEndPredictStart+TrainEndPredictStart+predictDays)/2.0)/7.0)*0.25,2.0);//2*0.25 +  4*0.25
        trainMVAvg[i]= (trainMVAvg[i]*a+everVMTot[i]*(1-a))*p;
    }
}


void Predict()
{
    int a[]={0, 0, 0, 0, 0, 0, 0, 115, 179, 144, 0, 0, 0, 0, 26, 194, 0, 23};
    int b[]={76,89,14,35,92,12,10,140,37,4,43,35,13,13,5,14,7,0};
    for(int i=1;i<=VM_TYPE_NUM;i++)
        if(VMExist[i])
        {
           
            predictVMNum[i] = trainMVAvg[i] * predictDays  +0.5;
            a[i-1]=predictVMNum[i];
            //predictVMNum[i] = 420;
            predictVMTotNum += predictVMNum[i];
            extraVMNum[i] = predictVMNum[i]*0.3;
            predictVMTotCPU += predictVMNum[i]*VMCPU[i];
            predictVMTotMEM += predictVMNum[i]*VMMEM[i];
        }
    double A=0,B=0,C=0,tot=0;
    for(int i=0;i<=VM_TYPE_NUM-1;i++){
        if(VMExist[i+1]){
            tot++;
            A+=(a[i]-b[i])*(a[i]-b[i]);
            B+=a[i]*a[i];
            C+=b[i]*b[i];
      //      printf("%d %d\n",a[i],b[i]);
        }
    }
    predictpercent=(1-sqrt(A/tot)/(sqrt(B/tot)+sqrt(C/tot)));

}

void Random()
{}

void FillServer(Server &server, int *VMNum, int &VMTotCPU, int &VMTotMEM, int pre = -1)
{
    int ok = 1;
    if(pre != -1 && serverList[pre].type == 1)
    {
        for(int i=1;i<=VM_TYPE_NUM;i++)
            if(serverList[pre].VMNum[i]>VMNum[i])
            {   
                ok = 0;
                break;
            }
    }
    if(ok && pre != -1 && serverList[pre].type == 1)
    {
        for(int VMType=VM_TYPE_NUM;VMType>0;VMType--)
        {
            int num = serverList[pre].VMNum[VMType];
            VMNum[VMType] -= num;
            server.VMNum[VMType] += num;
            server.freeCPU -= VMCPU[VMType] * num;
            server.freeMEM -= VMMEM[VMType] * num;
            VMTotCPU -= VMCPU[VMType] * num;
            VMTotMEM -= VMMEM[VMType] * num;
        }
        return ;
    }


    for(int i=1;i<=VM_TYPE_NUM;i++)
        for(int j=1;j<=server.freeCPU;j++)
            for(int k=1;k<=server.freeMEM;k++)
            {
                dp[i][j][k]=0;
                fillVMNum[i][j][k]=0;
            }
    for(int VMType=1;VMType<=VM_TYPE_NUM;VMType++)
        for(int i=0;i<=VMNum[VMType];i++)
        {
            int totCPU = VMCPU[VMType]*i;
            int totMEM = VMMEM[VMType]*i;
            int value = totCPU + totMEM;
            for(int cpu=server.freeCPU;cpu>=totCPU;cpu--)
                for(int mem=server.freeMEM;mem>=totMEM;mem--)
                    if(dp[VMType][cpu][mem] <= dp[VMType-1][cpu-totCPU][mem-totMEM]+value)
                    {
                        dp[VMType][cpu][mem] = dp[VMType-1][cpu-totCPU][mem-totMEM]+value;
                        fillVMNum[VMType][cpu][mem] = i; 
                    }
        }
    for(int VMType=VM_TYPE_NUM;VMType>0;VMType--)
    {
        int num = fillVMNum[VMType][server.freeCPU][server.freeMEM];
        VMNum[VMType] -= num;
        server.VMNum[VMType] += num;
        server.freeCPU -= VMCPU[VMType] * num;
        server.freeMEM -= VMMEM[VMType] * num;
        VMTotCPU -= VMCPU[VMType] * num;
        VMTotMEM -= VMMEM[VMType] * num;
    }
}

void CutServer(Server &server, int *VMNum, int &VMTotCPU, int &VMTotMEM)
{
    for(int i=1;i<=VM_TYPE_NUM;i++)
    {
        VMNum[i] += server.VMNum[i];
        VMTotCPU += VMCPU[i] * server.VMNum[i];
        VMTotMEM += VMMEM[i] * server.VMNum[i];
        server.VMNum[i] = 0;
    }
}

void PredictServerNum(int VMTotCPU,int VMTotMEM,int *serverNum)
{
    int limitServerNum[SERVER_TYPE_NUM+5];
    for(int i=1;i<=SERVER_TYPE_NUM;i++)
        if(serverExist[i])
            limitServerNum[i] = max((VMTotCPU+serverCPU[i]-1)/serverCPU[i],(VMTotMEM+serverMEM[i]-1)/serverMEM[i]);
        else
            limitServerNum[i] = 0;
    int loss = INF;
    for(int i=0;i<=limitServerNum[1];i++)
        for(int j=0;j<=limitServerNum[2];j++)
        {
            int k = max(0,(VMTotCPU - serverCPU[1]*i - serverCPU[2]*j + serverCPU[3] - 1)/serverCPU[3]);
            k = max(k, (VMTotMEM - serverMEM[1]*i - serverMEM[2]*j+ serverMEM[3] -1)/serverMEM[3]);
            k = min(k,limitServerNum[3]);
            int lossCPU = i*serverCPU[1] + j*serverCPU[2] + k*serverCPU[3] - VMTotCPU;
            int lossMEM = i*serverMEM[1] + j*serverMEM[2] + k*serverMEM[3] - VMTotMEM;
            if(lossCPU >=0 && lossMEM>=0 && lossCPU+lossMEM<=loss)
            {
                loss = lossCPU+lossMEM;
                serverNum[1]=i;
                serverNum[2]=j;
                serverNum[3]=k;
            }
        }
}

void InitAns()
{
    memset(serverList,0,sizeof(serverList));
    int len = 0;
    int tmpVMNum[VM_TYPE_NUM+5];
    for(int i=1;i<=VM_TYPE_NUM;i++)
        tmpVMNum[i] =predictVMNum[i];
    int flag = -1;
    while(1)
    {
    
        int tmpVMTotCPU=0, tmpVMTotMEM=0;
        for(int i=1;i<=VM_TYPE_NUM;i++)
            if(tmpVMNum[i] > 0)
            {
                tmpVMTotCPU += tmpVMNum[i] * VMCPU[i];
                tmpVMTotMEM += tmpVMNum[i] * VMMEM[i];
            }
        if(tmpVMTotCPU==0) break;
        int tmpServerNum[SERVER_TYPE_NUM+5];
        int overflow = tmpVMTotMEM - tmpVMTotCPU*serverMEM[1]/serverCPU[1];
        //printf("outer: overflow:%d\n", overflow);
        if(serverExist[2] && (flag == 2 || overflow>=serverMEM[2]-serverCPU[2]*serverMEM[1]/serverCPU[1]))
            PredictServerNum(serverCPU[2],serverMEM[2],tmpServerNum);
        else if(serverExist[3] && (flag == 3 || overflow<=serverMEM[3]-serverCPU[3]*serverMEM[1]/serverCPU[1])) 
            PredictServerNum(serverCPU[3],serverMEM[3],tmpServerNum);
        else
            PredictServerNum(tmpVMTotCPU,tmpVMTotMEM,tmpServerNum);
        flag = -1;
        int ok = 0;
        for(int i=2;i<=SERVER_TYPE_NUM;i++)
        {
            for(int j=1;j<=tmpServerNum[i];j++)
            {
                serverNum[i]++;
                serverList[serverTot].type = i;
                serverList[serverTot].freeCPU = serverCPU[i];
                serverList[serverTot].freeMEM = serverMEM[i];
                FillServer(serverList[serverTot],tmpVMNum,tmpVMTotCPU,tmpVMTotMEM);
                serverTot++;  
                len = serverTot;
                ok = 1;
                break;
            }
            if(ok)
                break;
        }
        if(ok)
            continue;
        for(int i=1;i<=tmpServerNum[1];i++)
        {
            serverNum[1]++;
            serverList[serverTot].type = 1;
            serverList[serverTot].freeCPU = serverCPU[1];
            serverList[serverTot].freeMEM = serverMEM[1];
            FillServer(serverList[serverTot],tmpVMNum,tmpVMTotCPU,tmpVMTotMEM,serverTot-1);
            serverTot++;

            overflow = tmpVMTotMEM - tmpVMTotCPU*serverMEM[1]/serverCPU[1];
            //printf("in: overflow:%d\n",overflow);
            if(serverExist[2] && overflow>=serverMEM[2]-serverCPU[2]*serverMEM[1]/serverCPU[1])
                flag = 2;
            else if(serverExist[3] && overflow<=serverMEM[3]-serverCPU[3]*serverMEM[1]/serverCPU[1]) 
                flag = 3;

            if(flag != -1)
            {
                for(;serverTot>len;serverTot--)
                {
                    serverNum[1]--;
                    CutServer(serverList[serverTot-1],tmpVMNum,tmpVMTotCPU,tmpVMTotMEM);
                }
                break;
            }
        }
        len = serverTot;
    }
}

void FinalFill()
{
    int tmpVMNum[VM_TYPE_NUM+5];
    int tmpVMTotCPU = 0, tmpVMTotMEM = 0;
    for(int i=1;i<=VM_TYPE_NUM;i++)
    {
        tmpVMNum[i] = extraVMNum[i];
        tmpVMTotCPU += extraVMNum[i] * VMCPU[i];
        tmpVMTotMEM += extraVMNum[i] * VMMEM[i];
    }
    for(int i=0;i<serverTot;i++)
        FillServer(serverList[i], tmpVMNum, tmpVMTotCPU, tmpVMTotMEM);
    for(int i=1;i<=VM_TYPE_NUM;i++)
        printf("extraVMNum[%d]:%d tmpVMNum[%d]:%d\n",i,extraVMNum[i],i,tmpVMNum[i]);
    for(int i=1;i<=VM_TYPE_NUM;i++)
    {
        int num = extraVMNum[i] - tmpVMNum[i];
        predictVMNum[i] += num;
        predictVMTotNum += num;
        predictVMTotCPU += num * VMCPU[i];
        predictVMTotMEM += num * VMMEM[i];
    }
}

void Output()
{
    int point = 0;
    point += sprintf(output+point,"%d\n",predictVMTotNum);
    for(int i=1;i<=VM_TYPE_NUM;i++)
        if(VMExist[i]==1)
            point += sprintf(output+point,"flavor%d %d\n",i,predictVMNum[i]);
    point += sprintf(output+point,"\n");

    int len = 0;
    sort(serverList,serverList+serverTot);
    for(int i=1;i<=SERVER_TYPE_NUM;i++)
    {
        if(serverNum[i] == 0) continue;
        point += sprintf(output+point,"%s %d\n",serverName[i],serverNum[i]);
        for(int j=1;j<=serverNum[i];j++)
        {
            point += sprintf(output+point,"%s-%d",serverName[i],j);
            for(int k=1;k<=VM_TYPE_NUM;k++)
                if(serverList[len].VMNum[k]>0)
                    point += sprintf(output+point," flavor%d %d",k,serverList[len].VMNum[k]);
            len++;
            point += sprintf(output+point,"\n");
        }
        point += sprintf(output+point,"\n");
    }
}

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
    srand(time(NULL));
    Init_VMware(); 
    common_input(info);
    train_input(data,data_num);
    Denoise();
    Train();
    Predict();
    InitAns();
    //FinalFill();
    Show();
    CalculatePercent();
    Output();
    write_result(output, filename);
}