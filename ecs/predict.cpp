#include "predict.h"
#include <stdio.h>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <time.h>
#include <iostream>
using namespace std;
const int MAXTyepeVM=30;
struct TrainData{
	string type;
	string date;
	string ID;
}Data[50000];
int SeverCPU,SeverMEM,SeverDISK;
const int XXX=5;
int Need_Optimize_type=0;//CPU:0  MEM:1
int TypeVM_number;
//Predict PanQiAn LooK That
int PredictNum[MAXTyepeVM];
int PredictTOT=0;
int ListCPU[20]={0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};	//1:4
int ListMEM[20]={0,15,14,12,13,11,9,10,8,6,7,5,3,4,2,1};
int ListCPU2[20]={0,13,14,15,10,11,12,7,8,9,4,5,6,1,2,3};	//1:1
int ListMEM2[20]={0,15,12,14,9,11,13,6,8,10,3,5,7,2,4,1};
int ListCPU3[20]={0,14,13,15,11,10,12,8,7,9,5,4,6,2,1,3};	//1:2
int PredictDay=0; //days of Predict
string PredictStartTime;
string PredictEndTime;
int sumCPU=0,sumMEM=0;
char OutPut[10000000];
//
double ZYNUM[40];

//ZY
int SeverNum=0;
int DetailSever[300][300]; //[0] size 
double const eps=1e-6;
/*---------模拟退火--------*/
// list

int VMtotal=0;
int VMLIST[30000];
struct Server {
	int free_mem;   // 物理服务器剩余可用内存
	int free_cpu;   // 物理服务器剩余可用CPU
}Servers[10000];
int ServersNUM=0;

/*-------------------*/
/*----About TypeVM By ZY---*/
map<string,int> TypeVMStringToInt;
int TypeVMCPU[MAXTyepeVM],TypeVMMEM[MAXTyepeVM],TypeVmExist[MAXTyepeVM],TypeVmFor[MAXTyepeVM]; // Predict -> PredictNum[] ->Answer
void init_Type_Vmware(){
	memset(TypeVmExist,0,sizeof(int)*MAXTyepeVM);
	TypeVMStringToInt["flavor1"]=1;  TypeVMCPU[1]=1;  TypeVMMEM[1]=1;
	TypeVMStringToInt["flavor2"]=2;  TypeVMCPU[2]=1;  TypeVMMEM[2]=2;
	TypeVMStringToInt["flavor3"]=3;  TypeVMCPU[3]=1;  TypeVMMEM[3]=4;
	TypeVMStringToInt["flavor4"]=4;  TypeVMCPU[4]=2;  TypeVMMEM[4]=2;
	TypeVMStringToInt["flavor5"]=5;  TypeVMCPU[5]=2;  TypeVMMEM[5]=4;
	TypeVMStringToInt["flavor6"]=6;  TypeVMCPU[6]=2;  TypeVMMEM[6]=8;
	TypeVMStringToInt["flavor7"]=7;  TypeVMCPU[7]=4;  TypeVMMEM[7]=4;
	TypeVMStringToInt["flavor8"]=8;  TypeVMCPU[8]=4;  TypeVMMEM[8]=8;
	TypeVMStringToInt["flavor9"]=9;  TypeVMCPU[9]=4;  TypeVMMEM[9]=16;
	TypeVMStringToInt["flavor10"]=10;TypeVMCPU[10]=8; TypeVMMEM[10]=8;
	TypeVMStringToInt["flavor11"]=11;TypeVMCPU[11]=8; TypeVMMEM[11]=16;
	TypeVMStringToInt["flavor12"]=12;TypeVMCPU[12]=8; TypeVMMEM[12]=32;
	TypeVMStringToInt["flavor13"]=13;TypeVMCPU[13]=16;TypeVMMEM[13]=16;
	TypeVMStringToInt["flavor14"]=14;TypeVMCPU[14]=16;TypeVMMEM[14]=32;
	TypeVMStringToInt["flavor15"]=15;TypeVMCPU[15]=16;TypeVMMEM[15]=64;
}
/*-------*/

/*---about  Get DayS---*/
//IsLeap函数判断一个年份是否为闰年，方法如下:
bool IsLeap(int year)
{
   return (year % 4 ==0 || year % 400 ==0) && (year % 100 !=0);
}

//上面的StringToDate函数用于取出日期中的年月日并判断日期是否合法
//从字符中最得年月日 规定日期的格式是yyyy-mm-dd
bool StringToDate(string date, int& year, int& month, int& day)
{
    year = atoi((date.substr(0,4)).c_str());
    month = atoi((date.substr(5,2)).c_str());
    day = atoi((date.substr(8,2)).c_str());
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year)){
         DAY[1] = 29;
    }
    return year >= 0 && month<=12 && month>0 && day<=DAY[month-1] && day>0;
}
//DayInYear能根据给定的日期，求出它在该年的第几天，代码如下
int DayInYear(int year, int month, int day)
{
    //int _day = 0;
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year))
        DAY[1] = 29;
    for(int i=0; i<month - 1; ++i)
    {
        day += DAY[i];
    }
    return day;
}
int DaysBetween2Date(string date1, string date2)
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
     {
          return day1 > day2 ? day1 - day2 : day2 - day1;
      
     //如果年相同
     }else if(year1 == year2)
     {
         int d1, d2;
         d1 = DayInYear(year1, month1, day1);
         d2 = DayInYear(year2, month2, day2);
         return d1 > d2 ? d1 - d2 : d2 - d1;
      
     //年月都不相同
     }else{
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

/*-----*/

bool cmp(int a,int b){
	return a > b;
}

/*----About Input -*/
void common_input(char * info[MAX_INFO_NUM]) {
	sscanf(info[0],"%d %d %d",&SeverCPU,&SeverMEM,&SeverDISK);
	//printf("%d %d %d\n",bagCpu,bagMem,bagDisk);	
	sscanf(info[2],"%d",&TypeVM_number);
	//printf("%d",TypeVM_number);
	
	for(int i=0;i<TypeVM_number;i++)
	{
		int tempa,tempb;
		char inputtempname[30];
		sscanf(info[3+i],"%s %d %d",inputtempname,&tempa,&tempb);
		string tempname=inputtempname;
		TypeVmExist[TypeVMStringToInt[tempname]]=1;
		TypeVmFor[i]=TypeVMStringToInt[tempname];
		//printf("%s %d\n",inputtempname,TypeVMStringToInt[tempname]);
	}
	sort(TypeVmFor,TypeVmFor+TypeVM_number,cmp);
//	for(int i=0;i<TypeVM_number;i++){
//		printf("%d ",TypeVmFor[i]);
//	}
	char Optimize_type[30];
	sscanf(info[4+TypeVM_number],"%s",Optimize_type);
	if(strcmp(Optimize_type,"CPU")==0)
		Need_Optimize_type=0;
	if(strcmp(Optimize_type,"MEM")==0)
		Need_Optimize_type=1;
	//printf("%s\n",Optimize_type );
	PredictStartTime.resize(100);
	PredictEndTime.resize(100);
	sscanf(info[6+TypeVM_number],"%s",&PredictStartTime[0]);  // get 2015-02-20  no hava 00:00:00
	sscanf(info[7+TypeVM_number],"%s",&PredictEndTime[0]);	  // and so go
	//cout<<PredictStartTime<<" "<<PredictEndTime<<endl;
	PredictDay=DaysBetween2Date(PredictStartTime,PredictEndTime);
	//printf("%d\n",PredictDay);
}


/*----/*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*/

int num[30]={0};
double DataDetailVMwareNum[600][20]={0};
double PredictDataDetailVMwareNum[600][20]={0};
double FPVMwareNum[600][20]={0};
double PredictFPVMwareNum[600][20]={0};
double TWOPredictMwareNum[600][20]={0};
double ThreePredictMwareNum[600][20]={0};
int FPDay=1;
int cmp2(TrainData a,TrainData b){
	return a.date<b.date;
}
void train_input(char * data[MAX_DATA_NUM], int data_num){
	double RealAlpha[20]={0.1,0.825, //1
							  0.725, //2
							  0.725, //3
							  0.725, //4
							  0.725, //5
							  0.725, //6
							  0.725, //7
							  0.735, //8
							  0.725, //9
							  0.725, //10
							  0.725, //11
							  0.725, //12
							  0.725, //13
							  0.725, //14
							  0.725}; //指数平滑法 参数
	/*for(int i=1;i<=15;i++){
		RealAlpha[i]=0.725;
	}*/
	int DAYDAY=15;
	int days=1.5*PredictDay;
	FPDay=PredictDay;
	char name[50];
	for(int i=0;i<data_num;i++){
		Data[i].ID.resize(100);
		Data[i].date.resize(100);
		Data[i].type.resize(100);
		sscanf(data[i],"%s %s %s",&Data[i].ID[0],&Data[i].type[0],&Data[i].date[0]);
	}
	// small -> big
	sort(Data,Data+data_num,cmp2);
	for(int i=0;i<data_num;i++){
		int tempday= DaysBetween2Date(Data[i].date,Data[0].date);
		DataDetailVMwareNum[tempday][TypeVMStringToInt[Data[i].type.c_str()]]++;

	}
	int endday=DaysBetween2Date(PredictEndTime,Data[0].date);
	int startday=DaysBetween2Date(PredictStartTime,Data[0].date);

	int TestDataStart=0;
	int WHATGETDAY=60;
	if(startday>=WHATGETDAY){
		TestDataStart=startday-WHATGETDAY;
	}
	else TestDataStart=0;

	double S_[20]={0};
	double S_u[20]={0};
	const double w=3.5;
	for(int k=TestDataStart;k<startday;k++){
		for(int i=0;i<20;i++){
			S_[i]=0;
			S_u[i]=0;
		}
		for(int i=TestDataStart;i<startday;i++)
			for(int j=1;j<=15;j++){
				S_[j]+=DataDetailVMwareNum[i][j];
			}
		for(int j=1;j<=15;j++){
			S_[j]=S_[j]/startday;
			//printf("S_[j]:%lf\n",S_[j]);
		}
		for(int i=TestDataStart;i<startday;i++)
			for(int j=1;j<=15;j++){
				S_u[j]+=pow(DataDetailVMwareNum[i][j]-S_[j],2);
			}
		for(int j=1;j<=15;j++){
			S_u[j]=sqrt(S_u[j]/(startday-TestDataStart));
			//printf("S_u:%lf\n",S_u[j]);
			if(DataDetailVMwareNum[k][j]>S_u[j]*w+S_[j]){
				printf("k:%d yichang:%lf\n",k,DataDetailVMwareNum[k][j]);
				DataDetailVMwareNum[k][j]=(int)(S_u[j]*w+S_[j]);
				
			}
			else if(DataDetailVMwareNum[k][j]<S_[j]-S_u[j]*w){
				if(S_[j]-S_u[j]*w>0)
					DataDetailVMwareNum[k][j]=S_[j]-S_u[j]*w;
				else 
					DataDetailVMwareNum[k][j]=0;
			}
		}
	}





/*	double tempstart=startday%FPDay;
	for(int i=tempstart;i<startday;i+=FPDay){
		int pos=(i-tempstart)/FPDay;
		for(int j=0;j<FPDay;j++){
			for(int k=1;k<=15;k++){
				FPVMwareNum[pos][k]+=DataDetailVMwareNum[i+j][k];
			}
		}
	}*/

	for(int i=TestDataStart;i<startday;i++){
		for(int j=1;j<=15;j++){
			int pos=i-TestDataStart;
			if(pos==0)
				FPVMwareNum[pos][j]+=DataDetailVMwareNum[i][j];
			else 
				FPVMwareNum[pos][j]+=FPVMwareNum[pos-1][j]+DataDetailVMwareNum[i][j];
		}
	}


	int Len=startday-TestDataStart-1;
	for(int i=1;i<=15;i++){
		for(int j=0;j<=0&&j<=Len;j++){

			PredictFPVMwareNum[0][i]+=FPVMwareNum[j][i];
			TWOPredictMwareNum[0][i]+=FPVMwareNum[j][i];
			ThreePredictMwareNum[0][i]+=FPVMwareNum[j][i];
		}
/*		if(2<=Len){
			PredictFPVMwareNum[0][i]=PredictFPVMwareNum[0][i]/3;
			TWOPredictMwareNum[0][i]=TWOPredictMwareNum[0][i]/3;
			ThreePredictMwareNum[0][i]=ThreePredictMwareNum[0][i]/3;
		}
		else {
			PredictFPVMwareNum[0][i]=PredictFPVMwareNum[0][i]/(Len+1);
			TWOPredictMwareNum[0][i]=TWOPredictMwareNum[0][i]/(Len+1);
			ThreePredictMwareNum[0][i]=ThreePredictMwareNum[0][i]/(Len+1);
		}*/
	}

	double minFangCha=2000000000;
	double K=0;
	//get Good alpha
	double MFC[16];
	for(int j=1;j<=15;j++){
		minFangCha=2000000000;
		for(double alpha=0.001;alpha<=(1-eps);alpha+=0.001){ //0.0001->0.001
			double Fangcha=0;
			for(int i=1;i<=Len;i++){
					PredictFPVMwareNum[i][j]=alpha*FPVMwareNum[i][j]+(1-alpha)*PredictFPVMwareNum[i-1][j];	
					TWOPredictMwareNum[i][j]=alpha*PredictFPVMwareNum[i][j]+(1-alpha)*TWOPredictMwareNum[i-1][j];
					ThreePredictMwareNum[i][j]=alpha*TWOPredictMwareNum[i][j]+(1-alpha)*ThreePredictMwareNum[i-1][j];
					double AT=3*PredictFPVMwareNum[i-1][j]-3*TWOPredictMwareNum[i-1][j]+ThreePredictMwareNum[i-1][j];
					double BT=(alpha/((1-alpha)*(1-alpha)*2))*((6-5*alpha)*PredictFPVMwareNum[i-1][j]-2*(5-4*alpha)*TWOPredictMwareNum[i-1][j]+(4-3*alpha)*ThreePredictMwareNum[i-1][j]);
					double CT=((alpha*alpha)/(2*(1-alpha)*(1-alpha)))*(PredictFPVMwareNum[i-1][j]-2*TWOPredictMwareNum[i-1][j]+ThreePredictMwareNum[i-1][j]);
					double PreANS=AT+BT+CT;
					if(Len-i<=21)
					Fangcha+=pow((PreANS-FPVMwareNum[i][j]),2);
			}
			if(Fangcha<minFangCha-eps){
					minFangCha=Fangcha;
					RealAlpha[j]=alpha;
					//printf("Fangcha:%lf\n",alpha);
			}
		}
		MFC[j]=minFangCha;
		K=K+MFC[j];
		printf("Fangcha:%lf\n",minFangCha);
	}
	for(int i=1;i<=15;i++){
		printf("Alpha %d %lf\n",i,RealAlpha[i]);
	}
	for(int i=1;i<=Len;i++){
		for(int j=1;j<=15;j++){
			PredictFPVMwareNum[i][j]=RealAlpha[j]*FPVMwareNum[i][j]+(1-RealAlpha[j])*PredictFPVMwareNum[i-1][j];	
			TWOPredictMwareNum[i][j]=RealAlpha[j]*PredictFPVMwareNum[i][j]+(1-RealAlpha[j])*TWOPredictMwareNum[i-1][j];
			ThreePredictMwareNum[i][j]=RealAlpha[j]*TWOPredictMwareNum[i][j]+(1-RealAlpha[j])*ThreePredictMwareNum[i-1][j];
		}
	}
	for(int j=1;j<=15;j++){
		double AT=3*PredictFPVMwareNum[Len][j]-3*TWOPredictMwareNum[Len][j]+ThreePredictMwareNum[Len][j];
		double BT=(RealAlpha[j]/((1-RealAlpha[j])*(1-RealAlpha[j])*2))*((6-5*RealAlpha[j])*PredictFPVMwareNum[Len][j]-2*(5-4*RealAlpha[j])*TWOPredictMwareNum[Len][j]+(4-3*RealAlpha[j])*ThreePredictMwareNum[Len][j]);
		double CT=((RealAlpha[j]*RealAlpha[j])/(2*(1-RealAlpha[j])*(1-RealAlpha[j])))*(PredictFPVMwareNum[Len][j]-2*TWOPredictMwareNum[Len][j]+ThreePredictMwareNum[Len][j]);
		double PreANS=AT+BT*PredictDay+CT*PredictDay*PredictDay;

		double AT2=3*PredictFPVMwareNum[Len-1][j]-3*TWOPredictMwareNum[Len-1][j]+ThreePredictMwareNum[Len-1][j];
		double BT2=(RealAlpha[j]/((1-RealAlpha[j])*(1-RealAlpha[j])*2))*((6-5*RealAlpha[j])*PredictFPVMwareNum[Len-1][j]-2*(5-4*RealAlpha[j])*TWOPredictMwareNum[Len-1][j]+(4-3*RealAlpha[j])*ThreePredictMwareNum[Len-1][j]);
		double CT2=((RealAlpha[j]*RealAlpha[j])/(2*(1-RealAlpha[j])*(1-RealAlpha[j])))*(PredictFPVMwareNum[Len-1][j]-2*TWOPredictMwareNum[Len-1][j]+ThreePredictMwareNum[Len-1][j]);
		double PreANS2=AT2+BT2+CT2;

		printf("A B C %lf %lf %lf %lf\n",AT,BT,CT,PreANS-PreANS2);
		if(PreANS-PreANS2>=0)
		ZYNUM[j]=PreANS-PreANS2;
	}
/*	for(int i=1;i<=startday;i++){
		for(int j=1;j<=15;j++){
			PredictDataDetailVMwareNum[i][j]=RealAlpha[j]*DataDetailVMwareNum[i-1][j]+(1-RealAlpha[j])*PredictDataDetailVMwareNum[i-1][j];
			if(i==startday){
				ZYNUM[j]=PredictDataDetailVMwareNum[i][j];
				printf("%lf\n", ZYNUM[j]);
			}
		}
	}*/


	int TEMPZYNUM[20];
	for(int i=1;i<=15;i++){
		TEMPZYNUM[i]=0;
	}


	for(int i=startday-days;i<startday;i++)
		for(int j=1;j<=15;j++){
			TEMPZYNUM[j]+=DataDetailVMwareNum[i][j];
		}

		for(int i=1;i<=15;i++){
			if(MFC[i]>=(K/15)){
			ZYNUM[i]=TEMPZYNUM[i]*PredictDay/days;
			
		}
		printf("%lf\n", ZYNUM[i]);
	}
/*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*//*-----*/
}
void train(){

}


int LASTBAGNUM[16]={0};
int LASTBAGTOT=0;
void Random(){
	srand(time(0));
	for(int i=0;i<TypeVM_number;i++){
		PredictNum[TypeVmFor[i]]=(int)((ZYNUM[TypeVmFor[i]])+0.5);
		//PredictNum[TypeVmFor[i]]=100;
		LASTBAGNUM[TypeVmFor[i]]=PredictNum[TypeVmFor[i]]*0.145;
		LASTBAGTOT+=LASTBAGNUM[i];
		PredictTOT+=PredictNum[TypeVmFor[i]];
	}
	for(int i=1;i<=15;i++){
		printf("%d\n",LASTBAGNUM[i]);

	}
}
void ZY(){
	//int RaiseVMNum=PredictTOT;
	int RaiseServerCPU=0,RaiseServerMEM=0;
	/*
	while(RaiseNum!=0){
		RaiseServerCPU=SeverCPU;
		RaiseServerMEM=SeverMEM;

	}
	*/
	//Test
	int flag=0,i;
	int tempPredictNum[30];
	for(int i=1;i<MAXTyepeVM;i++){
			tempPredictNum[i]=PredictNum[i];
	}
	for(int j=0;j<PredictTOT;j++){
		flag=1;
		for(int k=1;k<=15;k++){
			
				{
					double temp1=1.00*RaiseServerMEM/RaiseServerCPU;
					if(temp1>2.0+eps)	//1:2-1:4
					//if(RaiseServerCPU<=(1.1*SeverCPU/SeverMEM)*SeverMEM)
					i=ListCPU[k];
					else if(temp1>1+eps){
					i=ListCPU3[k];
					}
					else{
					i=ListCPU2[k];
					}
				}
			if(TypeVmExist[i]==1&&tempPredictNum[i]>0){
				if(RaiseServerCPU>=TypeVMCPU[i]&&RaiseServerMEM>=TypeVMMEM[i]){
					VMLIST[VMtotal]=i;
					VMtotal++;
					//DetailSever[SeverNum][0]++;
					//DetailSever[SeverNum][i]++;
					tempPredictNum[i]--;
					RaiseServerCPU-=TypeVMCPU[i];
					RaiseServerMEM-=TypeVMMEM[i];
					sumCPU+=TypeVMCPU[i];
					sumMEM+=TypeVMMEM[i];
					flag=0;
					break;
				}
			}
		}
		if(flag==1){
					printf("Raise:%d %d\n",RaiseServerCPU,RaiseServerMEM);
					SeverNum++;
					RaiseServerCPU=SeverCPU;
					RaiseServerMEM=SeverMEM;
					j--;

		}
					
		
	}
	printf("Raise:%d %d\n",RaiseServerCPU,RaiseServerMEM);
}

void output(){
	int point=0;
	point+=sprintf(OutPut+point,"%d\n",PredictTOT);
	for(int i=1;i<=15;i++){
		if(TypeVmExist[i]==1){
			point+=sprintf(OutPut+point,"flavor%d %d\n",i,PredictNum[i]);
		}
	}
	point+=sprintf(OutPut+point,"\n");
	point+=sprintf(OutPut+point,"%d\n",ServersNUM);
	for(int i=1;i<=ServersNUM;i++)
		{
			point+=sprintf(OutPut+point,"%d",i);
			for(int j=1;j<=15;j++){
				if(DetailSever[i][j]>0){
					point+=sprintf(OutPut+point," flavor%d %d",j,DetailSever[i][j]);
					}
				}
			point+=sprintf(OutPut+point,"\n");
		}
}

double ANS[30000];
void monituihuo(){
		srand(time(0));
		double T = 300.0;  //模拟退火初始温度
	    double Tmin = 1;   //模拟退火终止温度
        double r = 0.99999; //温度下降系数
        double ANSpercent=2100000000;
        int pos=0;//存储VMLIST当前指向 <VMtotal;
        int i;
        double minpercent=2100000000;
        while (T > Tmin) {
        	//init
        	int temp_sumCPU=0;
        	int temp_sumMEM=0;
        	pos=0;
        	i=0;
        	ServersNUM=0;

        	int a=rand()%VMtotal,b=rand()%VMtotal;//退火需要交换顺序的虚拟机
        	swap(VMLIST[a],VMLIST[b]);

  			//放置虚拟机主要逻辑
			//如果当前所有服务器都放不下虚拟机，就新建一个服务器用于存放
			while(pos<VMtotal){
				int TEMP_VMTYPE=VMLIST[pos];

				for(i=0;i<ServersNUM;i++){
					if(Servers[i].free_cpu>=TypeVMCPU[TEMP_VMTYPE]&&Servers[i].free_mem>=TypeVMMEM[TEMP_VMTYPE]){
						//放置
						Servers[i].free_cpu-=TypeVMCPU[TEMP_VMTYPE];
						Servers[i].free_mem-=TypeVMMEM[TEMP_VMTYPE];
						temp_sumCPU+=TypeVMCPU[TEMP_VMTYPE];
						temp_sumMEM+=TypeVMMEM[TEMP_VMTYPE];
						pos++;
						break;
					}
				}
	        	if(i==ServersNUM){
	        		Servers[ServersNUM].free_cpu=SeverCPU;
	        		Servers[ServersNUM].free_mem=SeverMEM;
	        		ServersNUM++;
				}
        		//printf("%d\n",temp_sumCPU);
        		//printf("%d\n",temp_sumMEM);
        		//printf("%d\n",ServersNUM);
        	}
        	//减去最后一个服务器使用的资源
			double UseServerCPU=(SeverCPU-Servers[ServersNUM-1].free_cpu);
			double UseServerMEM=(SeverMEM-Servers[ServersNUM-1].free_mem);
        	//count
        	double percentCPU=ServersNUM+UseServerCPU/SeverCPU;
        	double percentMEM=ServersNUM+UseServerMEM/SeverMEM;
        //	printf("%lf %lf\n",percentCPU,percentMEM);
        	double percent;
        	if(Need_Optimize_type==0) percent=percentCPU;
        	else percent=percentMEM; 
        	//退火
        	int ok=0;
              if(percent<ANSpercent){
                ANSpercent=percent;
		        		for(int i=0;i<VMtotal;i++){
		        			ANS[i]=VMLIST[i];
		        		}
              }
        	if((minpercent-percent)>-(1e-9)){
        		//save;
        		minpercent=percent;
        		ok=1;
        	}
	       	else {
	       			double a=exp(((minpercent-percent)*10000000 )/ T);
	        		if ( a > (rand() / RAND_MAX)) {
		        		minpercent=percent;
		        		//printf("a:%lf   %lf \n",a,percent - maxpercent);
						ok=1;
					}
	        	}
        	if(ok==0)
        		swap(VMLIST[a],VMLIST[b]);
        //	printf("%lf %lf\n",minpercent,ANSpercent);
        	//break;
        	T=T*r;
        }

}
	int F[16][200][200]={0};
	int X[16][200][200]={0};	
	int Y[16][200][200]={0};
	int BAGNUM[16][200][200]={0};
void ZYZ(){
	int pos=0;        
	int i,j;
	int temp_sumCPU=0;
	int temp_sumMEM=0;
		ServersNUM=0;

	int Value[16];
	printf("\n");
	while(pos<VMtotal){
				int TEMP_VMTYPE=VMLIST[pos];

				for(i=0;i<ServersNUM;i++){
					if(Servers[i].free_cpu>=TypeVMCPU[TEMP_VMTYPE]&&Servers[i].free_mem>=TypeVMMEM[TEMP_VMTYPE]){
						//放置
						//printf("%d",i);

						Servers[i].free_cpu-=TypeVMCPU[TEMP_VMTYPE];
						Servers[i].free_mem-=TypeVMMEM[TEMP_VMTYPE];
						temp_sumCPU+=TypeVMCPU[TEMP_VMTYPE];
						temp_sumMEM+=TypeVMMEM[TEMP_VMTYPE];
						DetailSever[i+1][0]++;
					 	DetailSever[i+1][TEMP_VMTYPE]++;
						pos++;
						break;
					}
				}
	        	if(i==ServersNUM){
	        			printf("TWORaise: %d %d\n",Servers[ServersNUM].free_cpu,Servers[ServersNUM].free_mem);
	        		Servers[ServersNUM].free_cpu=SeverCPU;
	        		Servers[ServersNUM].free_mem=SeverMEM;
	        		ServersNUM++;
				}
        		//printf("%d\n",temp_sumCPU);
        		//printf("%d\n",temp_sumMEM);
        		//printf("%d\n",ServersNUM);
        	}
	int RaiseServerCPU=Servers[ServersNUM-1].free_cpu;
	int RaiseServerMEM=Servers[ServersNUM-1].free_mem;
	for(int i=1;i<=15;i++){
		if(Need_Optimize_type==0)
			Value[i]=TypeVMCPU[i];
		else
			Value[i]=TypeVMMEM[i];
	}
		//init
	for(int i=1;i<=15;i++)
		for(int j=0;j<=RaiseServerCPU;j++)
			for(int k=0;k<=RaiseServerMEM;k++){
				F[i][j][k]=0;
			}

		for(int i=1;i<=15;i++)
			for(int j=0;j<=RaiseServerCPU;j++)
				for(int k=0;k<=RaiseServerMEM;k++){
					for(int l=0;l<=LASTBAGNUM[i];l++){
						if(j-l*TypeVMCPU[i]>=0&&k-l*TypeVMMEM[i]>=0)
						{
							if(F[i][j][k]<F[i-1][j-l*TypeVMCPU[i]][k-l*TypeVMMEM[i]]+l*Value[i]){
								F[i][j][k]=F[i-1][j-l*TypeVMCPU[i]][k-l*TypeVMMEM[i]]+l*Value[i];
								X[i][j][k]=j-l*TypeVMCPU[i];
								Y[i][j][k]=k-l*TypeVMMEM[i];
								BAGNUM[i][j][k]=l;
							}
						}
					}	
				}
		int XX=RaiseServerCPU,YY=RaiseServerMEM;

	//	printf("XY %d %d\n",X[15][34][60],Y[15][34][60]);
		double SHEQI,SHEQIMAX;
		if(Need_Optimize_type == 0){
				SHEQI=RaiseServerCPU;
				SHEQIMAX=SeverCPU;
		}
		else {
				SHEQI=RaiseServerMEM;
				SHEQIMAX=SeverMEM;
		}
		if(SHEQI<=0.8*SHEQIMAX||DetailSever[ServersNUM][0]>=7){
		for(int i=15;i>=1;i--){
			printf("%d BAG:%d\n",i,BAGNUM[i][XX][YY]);
			temp_sumCPU+=TypeVMCPU[i]*BAGNUM[i][XX][YY];
			temp_sumMEM+=TypeVMMEM[i]*BAGNUM[i][XX][YY];
			DetailSever[ServersNUM][0]+=BAGNUM[i][XX][YY];
			DetailSever[ServersNUM][i]+=BAGNUM[i][XX][YY];
			PredictNum[i]+=BAGNUM[i][XX][YY];
			PredictTOT+=BAGNUM[i][XX][YY];
			//printf("XY %d %d\n",X[i][XX][YY],Y[i][XX][YY]);
			int tempX=XX,tempY=YY;
			XX=X[i][tempX][tempY];
			YY=Y[i][tempX][tempY];
		}
		}
		else {
		for(int i=1;i<=15;i++){
			temp_sumCPU-=TypeVMCPU[i]*DetailSever[ServersNUM][i];
			temp_sumMEM-=TypeVMMEM[i]*DetailSever[ServersNUM][i];
			PredictNum[i]-=DetailSever[ServersNUM][i];
			PredictTOT-=DetailSever[ServersNUM][i];
			DetailSever[ServersNUM][i]=0;
		}
		ServersNUM--;
	}
		printf("??\n%d %d\n",RaiseServerCPU,RaiseServerMEM);
		printf("F[i][j][k]:%d\n",F[15][RaiseServerCPU][RaiseServerMEM]);
	/*for(int i=15;i>=1;i--)
	for(int j=1;j<=LASTBAGNUM[i];j++)
		if(RaiseServerCPU>=TypeVMCPU[i]&&RaiseServerMEM>=TypeVMMEM[i])
	{
			RaiseServerCPU-=TypeVMCPU[i];
			RaiseServerMEM-=TypeVMMEM[i];
			temp_sumCPU+=TypeVMCPU[i];
			temp_sumMEM+=TypeVMMEM[i];
			DetailSever[ServersNUM][0]+=1;
			DetailSever[ServersNUM][i]+=1;
			PredictNum[i]+=1;
			PredictTOT+=1;

	}*/
	


	//  if(RaiseServerCPU>=2) exit(0);
	  double percentCPU= (temp_sumCPU*1.0)/((ServersNUM)*SeverCPU);
      double percentMEM= (temp_sumMEM*1.0)/((ServersNUM)*SeverMEM);

      printf("%d,CPU:%lf MEM:%lf\n",ServersNUM,percentCPU,percentMEM);
}

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	init_Type_Vmware(); 
	common_input(info);
	train_input(data,data_num);
	
	train();
	Random();
//	if(PredictNum[11]==0) return ;
	ZY();
	monituihuo();
		for(int i=0;i<VMtotal;i++){
		printf("%d ",VMLIST[i]);
	}
	ZYZ();
	output();

	char *result_file=OutPut;
	write_result(result_file, filename);
}