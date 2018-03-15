#include "predict.h"
#include <stdio.h>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <time.h>
using namespace std;
const int MAXTyepeVM=30;
int SeverCPU,SeverMEM,SeverDISK;
int Need_Optimize_type=0;//CPU:0  MEM:1
int TypeVM_number;
//Predict PanQiAn LooK That
int PredictNum[MAXTyepeVM];
int PredictTOT=0;
char PredictStartTime[40];
char PredictEndTime[40];
char OutPut[10000000];
//

//ZY
int SeverNum=0;
int DetailSever[50][50]; //[0] size 

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
	if(strcmp(Optimize_type,"MEM")==1)
		Need_Optimize_type=1;
	
	sscanf(info[6+TypeVM_number],"%s",PredictStartTime);  // get 2015-02-20  no hava 00:00:00
	sscanf(info[7+TypeVM_number],"%s",PredictEndTime);	  // and so go
	//printf("%s %s\n",PredictStartTime,PredictEndTime);

}
/*-----*/

void train_input(char * data[MAX_DATA_NUM], int data_num){

}
void train(){

}

void Random(){
	srand(time(0));
	for(int i=0;i<TypeVM_number;i++){
		PredictNum[TypeVmFor[i]]=(rand()%40)+10;
		PredictTOT+=PredictNum[TypeVmFor[i]];
	}
}
void ZY(){
	int RaiseVMNum=PredictTOT;
	int RaiseServerCPU=0,RaiseServerMEM=0;
	/*
	while(RaiseNum!=0){
		RaiseServerCPU=SeverCPU;
		RaiseServerMEM=SeverMEM;

	}
	*/
	//Test
	int tempPredictNum[30];
	for(int i=1;i<MAXTyepeVM;i++){
			tempPredictNum[i]=PredictNum[i];
	}
	for(int j=0;j<PredictTOT;j++){

		for(int i=1;i<MAXTyepeVM;i++){
			if(TypeVmExist[i]==1&&tempPredictNum[i]>0){
				if(RaiseServerCPU>=TypeVMCPU[i]&&RaiseServerMEM>=TypeVMMEM[i]){	
					DetailSever[SeverNum][0]++;
					DetailSever[SeverNum][i]++;
					tempPredictNum[i]--;
					RaiseServerCPU-=TypeVMCPU[i];
					RaiseServerMEM-=TypeVMMEM[i];
					break;
				}
				else {
					SeverNum++;
					RaiseServerCPU=SeverCPU;
					RaiseServerMEM=SeverMEM;
					RaiseServerCPU-=TypeVMCPU[i];
					RaiseServerMEM-=TypeVMMEM[i];
					DetailSever[SeverNum][0]++;
					DetailSever[SeverNum][i]++;
					tempPredictNum[i]--;
					break;
				}
			}
		}
	}
}

void output(){
	int point=0;
	point+=sprintf(OutPut+point,"%d\n",PredictTOT);
	for(int i=1;i<MAXTyepeVM;i++){
		if(TypeVmExist[i]==1){
			point+=sprintf(OutPut+point,"flavor%d %d\n",i,PredictNum[i]);
		}
	}
	point+=sprintf(OutPut+point,"\n");
	point+=sprintf(OutPut+point,"%d\n",SeverNum);
	for(int i=1;i<=SeverNum;i++)
		{
			point+=sprintf(OutPut+point,"%d",i);
			for(int j=1;j<MAXTyepeVM;j++){
				if(DetailSever[i][j]>0){
					point+=sprintf(OutPut+point," flavor%d %d",j,DetailSever[i][j]);
					}
				}
			point+=sprintf(OutPut+point,"\n");
		}
}
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	init_Type_Vmware(); 
	common_input(info);
	
	train_input(data,data_num);
	train();
	Random();
	ZY();
	output();
	char *result_file=OutPut;
	write_result(result_file, filename);
}