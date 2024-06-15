#include "DFG.h"
#include "CGRA.h"
#include "config.h"
#include "Register.h"
#include "tool.h"
#include "GraphRegister.h"
#include "Path.h"

DEFINE_string(dfg_file, "data/test.txt", "dfg file.");
DEFINE_int32(II, 3, "initiation interval");
DEFINE_int32(pea_column, 4, "CGRA columns");//列数，包含LSU
DEFINE_int32(pea_row, 4, "CGRA rows");//行数
DEFINE_int32(childNum, 4, "childNum");//DFGs算子的最大子节点数量
DEFINE_int32(Rnum, 5, "Register Num in PE");//PE内的寄存器数量，默认为5

using namespace std;
int main(int argc, char** argv)
{
    #ifdef EXE
    google::ParseCommandLineFlags(&argc, &argv, true);
    #endif
    LOG(INFO) << "GLOG TEST!";
    int level_flag;		
    int *flag;	
    flag = & level_flag;			  
    clock_t start, end;/* 记录事件 */
    timeval tv, tv1;    
    gettimeofday(&tv, 0);
 
    DFG *D = new DFG(FLAGS_II,FLAGS_childNum); /* 创建DFG */
    CGRA *C = new CGRA(FLAGS_pea_row,FLAGS_pea_column);/* 创建CGRA */
    Register *R = new Register(FLAGS_Rnum,C);/* 创建平面寄存器 */
    R->CreatTER(FLAGS_II); /*  创建时间扩展的TEC寄存器 */

    int *inportNum;
    int *outportNum;
    int TECPENUM = C->ElmNum *FLAGS_II;
    inportNum = new int[TECPENUM];
    outportNum = new int[TECPENUM];
    memset(inportNum, 0, TECPENUM * sizeof(int));
    memset(outportNum, 0, TECPENUM * sizeof(int));
    for(int i = 0; i < TECPENUM; i++)
    {
        inportNum[i] = 5;
        outportNum[i] = 6;
    }

    
    /* 输出信息 */
    cout << "the number of DFG's nodes:" << D->numDFGnodes << endl;
    cout << "the number of DFG's edges:" << D->numDFGedges << endl;
    cout << "II:" << FLAGS_II << endl;

    /* DFG折叠 */
    D->CreatMDFG(FLAGS_II);
   
    *flag = D->Constraint_Level(FLAGS_II);/* 资源判断 LSU和load,PE和普通算子,store和SU */
    CHECK_EQ(*flag, -1) << "Modulo resource constraints are not satisffied!";/* 不满足时输出 */
    AllPath *allPathClass = new AllPath();/* 存放最终所有边的路径对象 */
    
    /* 遍历DFG的每一条边 */
    // cout<<"D->numDFGedgesc<="<<D->numDFGedges<<endl;
    for(int i = 0; i < D->numDFGedges; i++)
    {
        int srcR;
        int prenodeIndex  = D->getIndex(D->DFGedgesList[i]->prenode);
        int posnodeIndex  = D->getIndex(D->DFGedgesList[i]->posnode);
        // cout<<"pre:"<<D->DFGnodesList[prenodeIndex]->nodelabel<<" ------- pos:"<<D->DFGnodesList[posnodeIndex]->nodelabel<<"  ------- preIsRouted:"<<D->DFGnodesList[prenodeIndex]->isRoute<<endl;

        /* 相同源点的多条边一起布线 */
        /* ---------------------------------------------------------确定源点-------------------------------------------------------------------------------- */ 
        if(D->DFGnodesList[prenodeIndex]->kind == 1 && D->DFGnodesList[prenodeIndex]->isRoute == false )/* 边的前驱是load算子,且没有被布线过*/
        {
            /* 获得一个没有被占用的LU,后面有相应的Bank，容量处理。
            isOccupied：1.如果Bank容量为0不可再使用;2如果某一层（II层）的bank已经使用过，那么该层不能再被使用  */
            /* 获得首结点的时间步 */
            int time = D->DFGnodesList[prenodeIndex]->nodelevel;
            int latency  = D->DFGedgesList[i]->latency;
            int LUR = R->getLU(time,latency);
            srcR = LUR ;/* 该LU就是真实的源头寄存器 */
            // cout<<"srcR="<<srcR<<endl;
           
        }
        else if(D->DFGnodesList[prenodeIndex]->kind == 0 && D->DFGnodesList[prenodeIndex]->isRoute == false) /* 依赖边的前端是普通算子，且没有被访问过 */
        {
            srcR = D->DFGnodesList[prenodeIndex]->bindResource;/* 普通算子肯定已经布局了 */
        }
        else/* 剩下的就是前驱结点已经被访问过了，同源边已经处理了 */
        {
            continue;
        }
        shareRoute(D, R, prenodeIndex,srcR,FLAGS_II,allPathClass,inportNum,outportNum);
        D->DFGnodesList[prenodeIndex]->isRoute = true;/* 将边的前驱设为已经访问 */
        D->DFGnodesList[prenodeIndex]->bindResource = srcR;/* 为边的前驱绑定好资源 */
    }

    
    
   
    // for(int i = 0; i < D->DFGnodesList.size(); i++ )
    // {
     
    //     cout<<i<<"   "<<D->DFGnodesList[i]->bindResource<<endl;
    // }
     /* to do:node的位置用界面展示 */
    show(D, R,FLAGS_II,FLAGS_pea_row,FLAGS_pea_column);
    cout<<"--------------------------------------------"<<endl;
    
    for(int i = 0; i < allPathClass->PathsList.size(); i++ )
    {
        cout<<"Path["<<i<<"]:"<<endl;
        cout<<"pre:"<<allPathClass->PathsList[i]->DFGpre<<"  ------  pos:"<<allPathClass->PathsList[i]->DFGpos<<"  -----  latency:"<<allPathClass->PathsList[i]->latency<<endl;
        showPath(R,allPathClass->PathsList[i]->point,FLAGS_II); 
    }

    cout<<"--------------------------------------------"<<endl;
    for(int m = 0; m < 8; m++)
    {
       
        cout<<"bankCapacity["<<m<<"]="<<R->bankCapacity[m]<<endl;
    }

    for(int i = 0; i < TECPENUM; i++)
    {
        if(C->CGRAnodesList[i%C->ElmNum]->ElmKind == 0)
        {
            cout<<i<<" "<<inportNum[i]<<" "<<outportNum[i]<<endl;

        }
    }

    gettimeofday(&tv1, 0); 
    cout<<"--------------------------------------------"<<endl;
    cout<< "spand time:" <<(tv1.tv_sec - tv.tv_sec + (double)(tv1.tv_usec - tv.tv_usec) / CLOCKS_PER_SEC)<<endl; 
     

    delete C;
    //delete T;
    delete D;
    delete allPathClass;
    return 0;
}

