#ifndef REGISTER_H
#define REGISTER_H
#include "config.h"
#include "CGRA.h"
using namespace std;

class Registernode{      
public:
    Registernode();    
    int RegisterID; /* �Ĵ�����ID */  
    int PE;  /*�Ĵ���������PE*/ 
    int RegisterKind;/* �Ĵ���������,0����R��1����load,2����store�Ĵ�����3����FIFO,4����FU��5��������Ĵ���*/
    bool isOccupied;
    
    int time;
    bool inPort;//输入端口是否已经被占。
    bool usedBypass;
    bool usedTimeTrans;
    
    vector<int> RegisterNeighbors;
    string info;
};

class Registeredge{      
public:
    Registeredge();    
    int edgeId; /* �ߵ�ID */   
   	int srcReg; /*Դ�Ĵ���*/
	  int tgtReg; /*Ŀ��Ĵ���*/
};

class Register
{
public:
    Register(int Rnum,CGRA *C);
    int RnodesNums;
		int RedgesNums;
    int II;
    int TERnodesNums;
    int TERedgesNums;
    int bankCapacity[8];/* 8个bank,每个bank的容量256 */
    
    int Rnum;//ÿ��PE�ļĴ�������
		vector<Registernode*> RnodesList;
		vector<Registeredge*> RedgesList;
   
    vector<Registernode*> TERnodesList;
    vector<Registeredge*> TERedgesList;
    
   
   
    int getLU(int time,int latency);
    int getNodeKind(int nodeLabel);
    int getSUBank(int su,int lastBankindex);
    int getIndex(int R);
    int getLUR(int LU);
    int getLUorResultSU(int SRC);
    int getBankLU(int bank,vector<int> srcR);
    void getSUSet(int time,vector<int> &candidateR);
    void getResultRSet(int time,vector<int> &candidataR);
    void CreatTER(int II);
    ~Register();
};
#endif