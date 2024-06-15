#include "DFG.h"
#include "Register.h"
#include "GraphRegister.h"
#include "config.h"
#include "Path.h"

int calculateBankIndex(int bank);

void setSrcRandRestLantency(Register *R, vector<int>  &SingleTruePath,int &srcR,int latency,int &restLatency,int II);
void show(DFG *D,Register *R,  int II,int CGRARow, int CGRAColumn);
int ReadDataFromFileLBLIntoString(vector<vector<int>> &DFG_node);
bool dijkstra(Dijk *dijk,Register *R,GraphRegister *GR,vector<int> &virtualCandidate,vector<int> &pathNode,vector<vector<int>> sameSrcEdge,vector<int> SingleTruePath,int *inportNum,int *outportNum);
bool isEnoughofBank(Register *R);
vector<DFGedge*> getEdgesFromBegin(vector<DFGedge*> edgesList, int beginNo);
bool isLegalfu(GraphRegister *GR,int v,int length);
bool comp(DFGedge* &a,DFGedge* &b);
bool isIncludeCandidate(int n,vector<int> &virtualCandidate);
bool isUsedV(GraphRegister *GR,int path[], int v, int trueTRNum);
bool hasSuBankLoad(vector<int> pathNode, Register *R);
int getPreLastLU(vector<int> pathNode, Register *R);
void deletePartPath(vector<int> &pathNode, int srcR);
void setVisited(Register *R,vector<int> SingleTruePath);
bool bankLengthIsEvEnOne(Register *R, int i,int *path,int srcTrueTime,int srcR);
void shareRoute(DFG *D, Register *R, int prenodeIndex,int srcR,int II, AllPath *allPathClass,int *inportNum,int *outportNum);
bool compLegal(Register *R,vector<int> sameSrcEdge, vector<int> tempTruePath);
bool isLegalShare(Register *R, int i,int *path,int srcTrueTime,vector<vector<int>> sameSrcEdge,vector<int> tempTruePath,int *inportNum,int *outportNum);
void fixProcess(Register *R,int &srcR,int &restLatency,vector<int> &SingleTruePath,int II,int prelastbankindex,vector<int> &singleTruePathLU);
void showPath(Register *R, vector<int> PathPoint,int II);