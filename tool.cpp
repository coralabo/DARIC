#include "config.h"
#include "DFG.h"
#include "tool.h"
#define LIMIT 64

using namespace std;
DECLARE_string(dfg_file);

bool comp(DFGedge* &a,DFGedge* &b)
{
    return a->latency < b->latency;
}

vector<DFGedge*> getEdgesFromBegin(vector<DFGedge*> edgesList, int beginNo)
{
	vector<DFGedge*> edges;/* 新建一个 */
	for (auto e : edgesList)
	{
		if (e->prenode == beginNo)
		{
			edges.push_back(new DFGedge(e));
		}
	}
	return edges;
}

int calculateBankIndex(int bank)
{
    int index = bank % 120 / 30 * 2 + bank % 2;
    return index;
}


void strtok(string& str, vector<string>& cont, string defstr = " ")
{ 
    cont.clear();
    size_t size = 0;
    size_t begpos = 0;
    size_t endpos = 0;
    begpos = str.find_first_not_of(defstr);
    while (begpos != std::string::npos)
    {
        size++;
        endpos = str.find_first_of(defstr, begpos);

        if (endpos == std::string::npos)
        {
            endpos = str.size();
        }

        string ssubstr = str.substr(begpos, endpos - begpos);

        cont.push_back(ssubstr);
        begpos = str.find_first_not_of(defstr, endpos+1);
    }
}

int ReadDataFromFileLBLIntoString(vector<vector<int>> &DFG_node)
{
    ifstream fin(FLAGS_dfg_file); 
    cout << "********************************" << endl;
    cout << "the name of File:"  << FLAGS_dfg_file << endl;
    cout << "********************************" << endl;
    string s;
    int nodes = 0;
    vector<int> linedata;
    int line = 0;
    while(getline(fin,s))
    {
        vector<string> vec;
        strtok(s,vec,",");
        linedata.clear();
        for(int i = 0; i < vec.size(); i++)
        {
            
           //atoi:ascii to integer
        
           int e = atoi(vec[i].c_str());
            linedata.push_back(e);

        }
        DFG_node.push_back(linedata);
        line++;
    }
    
    fin.close();
    return line;
}



void show(DFG *D, Register *R,  int II,int CGRARow, int CGRAColumn)
{
    
    int column = CGRAColumn + 6;
    // cout<<"CGRARow="<<CGRARow<<"  "<<"column="<<column<<endl;
    int count = 0;
    {
        count = 0;
        for (int k = 0; k < II; k++)
        {
            for(int z = 0; z < column; z++)
            {
                cout << "---";
            }
            cout << "---------";
            cout << "["<< k <<"]";
            for(int z = 0; z < column; z++)
            {
                cout << "---" ;
            }
            cout << "---------";
           
            cout<<endl;                          
            for (int i = 0; i < CGRARow; i++) 
            {//行
                for (int j = 0; j < column; j++)
                {//列
                    for(int m = 0; m < D->DFGnodesList.size(); m++ )
                    {
                        int bindR = D->DFGnodesList[m]->bindResource;/* 绑定的寄存器 */
                        int postion = R->TERnodesList[bindR]->PE;/* 寄存器所在的PE */
                        int op_order = D->DFGnodesList[m]->nodelabel; 
                       
                        int time_step = D->DFGnodesList[op_order]->nodelevel;
                        // cout<<"postion="<<postion<<"  "<<"op_order="<<op_order<<"time_step="<<time_step<<endl;
                        if (time_step == k && postion / column % CGRARow == i && postion % column == j)
                        {
                        
                            count ++;
                            cout << "|"<< op_order << "\t";
                        }

                    }
                    if (count == 0){
                        cout << "|       ";
                    }
                    count = 0;
                }
                cout << "|        " << endl;
            }
        }
        cout << endl;
    } 
} 



bool isConcludeFU(GraphRegister *GR,int path[], int v)
{
    bool flag = false;
    stack<int> q;/* 建立一个栈 */
    int p = v;  /* p用来遍历path数组获得路径点 */
    
    while(path[p]!=-1)
    {
        q.push(path[p]);
        p = path[p];
    }   
    while(!q.empty())
    {
        int temp = q.top();
        
        if(GR->GraphnodesList[temp]->RegisterKind == 4)/* 走过的路包含了FU */
        {
            flag = true;
            break;
        }
        q.pop();
    
    }
    return flag;

}
                    


bool isUsedV(GraphRegister *GR,int path[], int v, int trueTRNum)
{
    
    bool flag = false;
    stack<int> q;/* 建立一个栈 */
    int p = v;  /* p用来遍历path数组获得路径点 */
    
    while(path[p]!=-1)
    {
        q.push(path[p]);
        p = path[p];
    }   

    while(!q.empty())
    {
        int temp = q.top();
        
        // cout<<temp<<" ";
        if((temp % trueTRNum == v % trueTRNum) && GR->GraphnodesList[temp]->RegisterKind != 3)/* 相同不包含bank */
        {
            // cout<<"v="<<v<<" "<<endl;
            flag = true;
            break;
        }
        q.pop();
    
    }
//    cout <<endl;
   
    return flag;
}

bool isLegalfu(GraphRegister *GR,int v,int length)
{
    bool flag = true;
    /* 获得v的时间 */
    
    int kind = GR->GraphnodesList[v]->RegisterKind;
    if(kind == 4)
    {
        
        int time = v / 120;
        if(time != length)
        {
            flag = false;
        }

    }
    return flag;


}




bool isIncludeCandidate(int n,vector<int> &virtualCandidate)
{
    bool flag = false;
    for(int i = 0;i < virtualCandidate.size(); i++)
    {
        if(virtualCandidate[i] == n)
        {
            flag = true;
            // cout<<"flag"<<flag<<endl;
            break;
        }
    }
    return flag;
}

void getPath(int path[], int minReg, vector<int> &pathNode)
{
    
    stack<int> q;/* 建立一个栈 */
    int p = minReg;  /* p用来遍历path数组获得路径点 */
    while(path[p]!=-1)
    {
        q.push(path[p]);
        p = path[p];
    }   
    // cout<<"virual:"<<endl;
    while(!q.empty())
    {
        int temp = q.top();
        // cout<<temp<<" " ;

        pathNode.push_back(temp);
        q.pop();
    }
    // cout<<endl;

}



bool hasSuBankLoad(vector<int> pathNode, Register *R)
{
   
    for(int i = 0; i < pathNode.size() - 2; i++)
    {
        int RegisterKind = R->TERnodesList[pathNode[i]]->RegisterKind;
        if(RegisterKind == 1)/* 碰到了ld */
        {
           
            if(R->TERnodesList[pathNode[i+1]]->RegisterKind == 2)/* 下一个是su */
            {
                if(R->TERnodesList[pathNode[i+2]]->RegisterKind == 3)/* 再下一个是bank */
                {
                    return true;
                }
            }

        }
        else
        {
            continue;/*继续遍历 */
        }
    }
    /* 遍历完了都没有结果 */
    return false;
}


int getPreLastLU(vector<int> pathNode, Register *R)
{

    for(int i = pathNode.size() - 1; i >= 0 ; i--)
    {
        int node = pathNode[i];
        int RegisterKind = R->TERnodesList[node]->RegisterKind;
        if(RegisterKind == 1)
        {
            return node;
            
        }

    }
    return -1;
}





int getLastBankIndex(vector<int> pathNode, Register *R)
{
    for(int i = pathNode.size() - 1; i >= 0 ; i--)
    {
        int node = pathNode[i];
        int RegisterKind = R->TERnodesList[node]->RegisterKind;
        if(RegisterKind == 3)
        {
           int index =  calculateBankIndex(node);
            return index;
            
        }

    }
    return -1;

}



 void deletePartPath(vector<int> &pathNode, int srcR)
 {
    /* 先找到迭代器 */
    for(std::vector<int>::iterator it = pathNode.begin();it != pathNode.end();it++)
    {
        if( *it == srcR)
        {
            pathNode.erase(it, pathNode.end());
            break;
        }

    }
}

/* 将共享的路径设为已经访问，除bank外*/
void setVisited(Register *R,vector<int> SingleTruePath)
{
    for(int i = 0; i < SingleTruePath.size(); i++)
    {
        int RegisterKind = R->TERnodesList[SingleTruePath[i]]->RegisterKind;
        if(RegisterKind == 1 || RegisterKind == 2)/* 只有Lu和SU要彻底设置已经访问 */
        {
            R->TERnodesList[SingleTruePath[i]]->inPort = true;
            R->TERnodesList[SingleTruePath[i]]->isOccupied = true;
        }      
    }
}



bool compLegal(Register *R,vector<int> sameSrcEdge, vector<int> tempTruePath)
{
    int totolpea = 40;
    int longlength;
    int shortlength;
    int length1 = sameSrcEdge.size();
    int length2 = tempTruePath.size();
    if(length1 > length2)
    {
        longlength = length1;
        shortlength = length2;
    }
    else
    {
        longlength = length2;
        shortlength = length1;
    }
    
    // cout<<" i am here"<<endl;
    // // cout<<endl;
    // cout<<"trueNode";
    // for(int i = 0; i < tempTruePath.size(); i++ )
    // {
    //     cout<<tempTruePath[i]<<" ";
    // }
    // cout<<endl;
    
    // cout<<endl;
    // cout<<"sameSrcEdge";
    // for(int i = 0; i < sameSrcEdge.size(); i++ )
    // {
    //     cout<<sameSrcEdge[i]<<" ";
    // }
    // cout<<endl;

    /* 看该条已经布线的路径是否含Bank */
   
    int label = -1;

    for(int i = 0; i < shortlength ; i++ )
    {
        if(sameSrcEdge[i] != tempTruePath[i])/* 部分全相同 */
        {
            label = i;
            break;

        }
    }
   
   

    // cout<<"label"<<label<<endl;
    // cout<<"length1-2="<<length1-2<<endl;
    // cout<<"length2="<<length2<<endl;
    if(label != -1)
    {

        for(int i = label; i < length1-2 ; i++ )
        {
            for(int j = label; j < length2 ; j++ )
            {
                if(sameSrcEdge[i] == tempTruePath[j])
                {
                    if(i-1 >=0 && j-1 >=0)
                    {
                        /* 平面的PE不同 */
                        int samealreadyPEi_1 = R->TERnodesList[sameSrcEdge[i-1]]->PE % totolpea;
                        int samealreadyPEi = R->TERnodesList[sameSrcEdge[i]]->PE % totolpea;
                        int currentPEj_1 = R->TERnodesList[tempTruePath[j-1]]->PE%totolpea;
                        int currentPEj = R->TERnodesList[tempTruePath[j]]->PE%totolpea;


                        if((samealreadyPEi_1!= samealreadyPEi) && (currentPEj_1 != currentPEj ) )
                        {
                            // cout<<"sameSrcEdge[i]="<<tempTruePath[j]<<" "<<"tempTruePath[j]="<<tempTruePath[j]<<endl;
                            // cout<<"R->TERnodesList[sameSrcEdge[i-1]]->PE="<<R->TERnodesList[sameSrcEdge[i-1]]->PE<<"  "<<" R->TERnodesList[tempTruePath[j-1]]->PE2="<< R->TERnodesList[sameSrcEdge[i]]->PE<<endl;
                            return false;
                        }
                        if((samealreadyPEi_1== samealreadyPEi) && (currentPEj_1 == currentPEj ) )
                        {
                            // cout<<"sameSrcEdge[i]="<<tempTruePath[j]<<" "<<"tempTruePath[j]="<<tempTruePath[j]<<endl;
                            // cout<<"R->TERnodesList[sameSrcEdge[i-1]]->PE="<<R->TERnodesList[sameSrcEdge[i-1]]->PE<<"  "<<" R->TERnodesList[tempTruePath[j-1]]->PE2="<< R->TERnodesList[sameSrcEdge[i]]->PE<<endl;
                            return false;
                        }
                    
                        else
                        {
                            /* 检查相似点前后是否有相同点 */
                            if(sameSrcEdge[i+1] ==tempTruePath[j-1])
                                return false;
                        }


                        

                    }
                    
                }
            }
        }
    }


    
    return true;
}
// bool shareisLegal(Register *R, int i,int *path,int srcTrueTime,vector<vector<int>> sameSrcEdge,vector<int> tempTruePath,int v ) 
// {

// }

bool isLegalShare(Register *R, int i,int *path,int srcTrueTime,vector<vector<int>> sameSrcEdge,vector<int> tempTruePath,int *inportNum,int *outportNum)
{

    stack<int> q;/* 建立一个栈 */
    int p = i;  /* p用来遍历path数组获得路径点 */
    // cout<<"p"<<p<<endl;
   
    while(path[p]!=-1)
    {
        q.push(path[p]);
        p = path[p];
    }  
    
    // cout<<"temp"<<endl;
    while(!q.empty())
    {
        int temp = q.top();
        int tempTime = temp / R->RnodesNums;
        /* temp是虚拟的点，要将其转换为真实的点 */
        int trueNode   = ( temp - (tempTime* R->RnodesNums) )+ ( (srcTrueTime + tempTime)  * R->RnodesNums ) % (R->II * R->RnodesNums);
        // cout<<temp<<" ";
        // cout<<trueNode<<" ";
        tempTruePath.push_back(trueNode);/*tempTruePath包含了所有路径了  */
        q.pop();
    }
    int iTime = i / R->RnodesNums;
    
    int truei   = ( i - (iTime* R->RnodesNums) )+ ( (srcTrueTime + iTime)  * R->RnodesNums ) % (R->II * R->RnodesNums);
    
    tempTruePath.push_back(truei);


    // cout<<"tempTruePath,.size()="<<tempTruePath.size()<<"  "<<"truei="<<truei<<endl;
    // cout<<endl;

    // for(int i = 0; i < tempTruePath.size(); i++)
    // {
    //     cout<<tempTruePath[i]<<" ";
    // }
    // cout<<endl;


    


    /* tempTruePath放的就是当前真实的路径，少最后一个结果寄存器 */
    /* 遍历前面已经布线的路径 */
    bool isLegal = true;
    // cout<<"sameSrcEdge.size()="<<sameSrcEdge.size()<<endl;
    for(int i = 0; i < sameSrcEdge.size(); i++ )
    {
        
        isLegal = compLegal(R,sameSrcEdge[i],tempTruePath);/* 会被覆盖 */
        if(isLegal == false)
        {
            break;
        }
        // cout<<"isLegal"<<isLegal<<endl;
        
    }
    // cout<<"-----------------"<<endl;
    /* 获得最后一个点的PE */
    int vR = tempTruePath[tempTruePath.size()-1];
    int vRpre = tempTruePath[tempTruePath.size()-2];

    int vPE = R->TERnodesList[vR]->PE;
    int vPEpre = R->TERnodesList[vRpre]->PE;
    if(inportNum[vPE] <= 0 || outportNum[vPE] <= 0)
    {
        isLegal = false;
    }


    return isLegal;

}


/* bank的长度是否没有超过剩余的容量，是否是偶数，是否只包含一个Bank */
bool bankLengthIsEvEnOne(Register *R, int i,int *path,int srcTrueTime)
{
    // for(int i = 0; i < 4; i++)
    // {
    //     cout<<R->bankCapacity[i]<<endl;

    // }
    int count = 0;
    
    stack<int> q;/* 建立一个栈 */
    int p = i;  /* p用来遍历path数组获得路径点 */
    while(path[p]!=-1)
    {
        q.push(path[p]);
        p = path[p];
    }  
    vector<int> tempPath;
    while(!q.empty())
    {
        int temp = q.top();
        tempPath.push_back(temp);
        q.pop();
    }

    int sameBank = -1; 

    // cout<<"temp:";
    for(int i = 0; i< tempPath.size();i++)
    {
        int temp = tempPath[i];
        int tempTime = temp / R->RnodesNums;
        /* temp是虚拟的点，要将其转换为真实的点 */
        int trueNode   = ( temp - (tempTime* R->RnodesNums) )+ ( (srcTrueTime + tempTime)  * R->RnodesNums ) % (R->II * R->RnodesNums);
        // cout<<temp<<" ";

        int RegisterKind = R->TERnodesList[trueNode]->RegisterKind; 
        if(RegisterKind == 3)/* 第一次出现的 */
        {
            
            sameBank = calculateBankIndex(trueNode);
            break;
        }

    }
    // cout<<endl;

    //cout<< "sameBank"<<sameBank<<endl;
    for(int i = 0; i<tempPath.size();i++)
    {
        int temp = tempPath[i];
        int tempTime = temp / R->RnodesNums;
        /* temp是虚拟的点，要将其转换为真实的点 */
        int trueNode   = ( temp - (tempTime* R->RnodesNums) )+ ( (srcTrueTime + tempTime)  * R->RnodesNums ) % (R->II * R->RnodesNums);
        int RegisterKind = R->TERnodesList[trueNode]->RegisterKind; 
        
        if(RegisterKind == 3)
        {
            // cout<<"trueNode"<<trueNode<<endl;
            int anotherBank = calculateBankIndex(trueNode);
            // cout<<"sameBank="<<anotherBank<<" "<<anotherBank<<endl;
            if(sameBank == anotherBank)
                count ++;
            else 
                return false;
        }
    }
    if(sameBank != -1)
    {
        /* 如果bank的数量不是偶数或者数量大于剩余的容量 */
        //cout<<"sameBank"<<sameBank<<"count="<<count<<" "<<R->bankCapacity[sameBank] <<endl;
        // if(count % 2 != 0 || count > R->bankCapacity[sameBank] )
        if(count % 2 != 0 )/* 如果没有bank,也可以通过 */
        {
           
            return false;

        }
        else
        {   
            return true;
        }

    }
    else/* 没有Bamk */
    {
        return true;
    }

}



bool dijkstra(Dijk *dijk,Register *R,GraphRegister *GR,vector<int> &virtualCandidate,vector<int> &pathNode,vector<vector<int>> sameSrcEdge,vector<int> SingleTruePath,int *inportNum,int *outportNum)
// void dijkstra(Register *R,GraphRegister *GR,int latency,int virtualSrc,vector<int> &virtualCandidate,vector<int> &pathNode,int trueTRNum,int srcTrueTime,vector<vector<int>> sameSrcEdge,vector<int> SingleTruePath)
{

    // cout<<"-----------------"<<endl;
    /*定义 */
    bool *known;
    int *dist;
    int *path;
    int nodeNum = GR->GraphnodesNums;
    // cout<<"nodeNum"<<nodeNum<<endl;
    // cout<<"dijk->virtualSrc"<<dijk->virtualSrc<<endl;
    // cout<<"nodeNum"<<nodeNum<<endl;
    known = new bool[nodeNum];/* 是否已经访问，确定了最短路径的标志 */
    memset(known, 0, nodeNum * sizeof(bool));
    dist = new int[nodeNum];
    memset(dist, 0, nodeNum * sizeof(int));/* 源点到当前点的距离 */
    path = new int[nodeNum];
    memset(path, 0, nodeNum * sizeof(int));/* 前驱节点 */

    int **Graph = new int*[nodeNum]; //开辟行  
    for (int i = 0; i < nodeNum; i++)
        Graph[i] = new int[nodeNum]; //开辟列 
    
    for (int i = 0; i < nodeNum; i++)/* 初始化为无穷大 */
        for(int j = 0;j < nodeNum; j++)
            Graph[i][j] = INF;
     
    for (int i = 0; i < GR->GraphedgesList.size(); i++)
    {
        // if(GR->GraphedgesList[i]->pre == 1 && GR->GraphedgesList[i]->pos == 113 )
        // {
        //     cout<<"GR->GraphedgesList[i]->value"<<GR->GraphedgesList[i]->value<<endl;
        // }
        Graph[GR->GraphedgesList[i]->pre][GR->GraphedgesList[i]->pos] = GR->GraphedgesList[i]->value;

    }
    
    /* to do :实验一下为甚 1-2 31-145(latency =1 ）走不通*/

    if(dijk->latency == 1)
    {
        
        // cout<<"Graph[4][17]="<<Graph[43][132]<<endl;
        // cout<<"Graph[494][615]="<<Graph[4][14]<<endl;
        // cout<<"Graph[494][615]="<<Graph[4][15]<<endl;
        // cout<<"Graph[494][615]="<<Graph[4][16]<<endl;
        
    }


    /* 初始化 */
    // cout<<"dijk->latency="<<dijk->latency<<endl;
    // cout<<"dijk->virtualSrc="<<dijk->virtualSrc<<endl;
    // cout<<"dijk->trueTRNum="<<dijk->trueTRNum<<endl;

    for(int i = 0; i < nodeNum; ++i)
    {
        
        
        known[i] = false;/* 最开始所有点都没有被访问 */
        /* 获得源点到所有点的代价 */
        //dist[i]  = GR->getEdgeCost(virtualSrc,i);
        dist[i]  = Graph[dijk->virtualSrc][i];
        //path[i]  = GR->graphHasEdge(virtualSrc,i) ==  true ? virtualSrc:-1;
        path[i]  = Graph[dijk->virtualSrc][i] <  INF ? dijk->virtualSrc:-1;
    }
    
   
    known[dijk->virtualSrc] = true;
    dist[dijk->virtualSrc]  = 0;
    path[dijk->virtualSrc]  = -1;
    
    // cout<<"dijk->virtualSrc="<<dijk->virtualSrc<<"  "<<"dijk->virtualSrc="<<dijk->virtualSrc<<endl;
    for(int j = 0; j < nodeNum-1 ;j++)
    {
        //找到unknown的dist最小的顶点 
        int v = 0;
        int min = INF;
        for(int i = 0; i < nodeNum; ++i){
            if(!known[i] && (min > dist[i]))/* i没有知道最小的dist */
            {
                min = dist[i];
                v = i;
            }
        }
        known[v] = true;


        //更新与v相邻所有顶点w的dist,path
        for(int i = 0; i < nodeNum;i++){
            if(!known[i] ){     
                if(dist[i] > dist[v] + Graph[v][i] && isUsedV(GR, path, v, dijk->trueTRNum) == false && isLegalShare(R,v,path,dijk->srcTrueTime,sameSrcEdge,SingleTruePath,inportNum,outportNum) == true)              
                // if(dist[i] > dist[v] + Graph[v][i] && isLegalShare(R,v,path,dijk->srcTrueTime,sameSrcEdge,SingleTruePath) == true)              
                {               
                    /* 如果当前的vbu不是倒数第二层，不能是FU */   
                    // if(isLegalfu(GR,v,dijk->latency))
                    // {
                        dist[i] = dist[v] + Graph[v][i];
                        path[i] = v;  
                    // }
                    
                                    
                }
            }
        }
    }
     

    int minRegcost = INF;
    int minReg = dijk->virtualSrc;
    
    for(int i = 0; i < nodeNum; ++i)
    {      
        if(known[i])/* 访问过的 */
        {
           
            if (dist[i] < minRegcost)
            {
               
                if(isIncludeCandidate(i,virtualCandidate) == true )
                {
                  
                    /* mini,找出mini*/
                        minRegcost = dist[i];
                        minReg = i;
            
                }
            }            
        }
            
    } 

    // cout<<"minReg"<<minReg<<endl;
    /* 将mini的路径读出，记在pathNode中 */
    getPath(path, minReg,pathNode);
    if(pathNode.size() == 0)
    {
        return false;
    }
    else
    {
        pathNode.push_back(minReg);
    
        delete [] known; 
        delete [] dist; 
        delete [] path;
        for (int m = 0; m < nodeNum; m++)
            delete[] Graph[m];
        delete[] Graph;
        return true;
        
    }
    
    
}


void setBankVisited(Register *R, int bankIndex)
{
    /* 遍历所有的结点算子 */
    for (int i = 0; i < R->TERnodesList.size();i++)
	{
		if (R->TERnodesList[i]->RegisterKind == 3 )/* bank */
		{
            
            int index = calculateBankIndex(i);
            if(index == bankIndex)
            {
                R->TERnodesList[i]->isOccupied = true;/* 将该索引下所有的bank都设为不可访问 */
            }
			
		}
	}
	
}


bool isEnoughofBank(Register *R)
{
    /* 遍历所有的结点算子 */
    
    for (int i = 0; i < 8;i++)
	{
		if (R->bankCapacity[i] != 0 )/* bank */
		{
            
           return true;
			
		}
	}
    return false;
    
}



void fixProcess(Register *R,int &srcR,int &restLatency,vector<int> &SingleTruePath,int II,int prelastbankindex,vector<int> &singleTruePathLU)
{

    SingleTruePath.push_back(srcR);/* 先放进去 */
    int  su = R->getLUorResultSU(srcR);/* 再找SU */
    SingleTruePath.push_back(su);/* 放进去 */
    int lu ;
    int BankNumtotoal = 0;
    int bank = R->getSUBank(su,prelastbankindex);/* 2 bank了*/

 
    /* 看看它的容量 */
    int bankIndex = calculateBankIndex(bank);
    int Capacity = R->bankCapacity[bankIndex];
    // cout<<"bankIndex="<<bankIndex<<"    Capacity="<<Capacity<<endl;

    /* 需要切多少？ */
    int BankNum = floor((restLatency- 4)/II);
   
    /* 容量不够 */
    if(BankNum > Capacity)
    {
        BankNum = Capacity;/* 先把Capacity占满 */
        /* 把所有属于这bankIndex的 层数 的bank都设置为 已经访问 */
        setBankVisited(R,bankIndex);
    }
    else/* 容量够 */
    {
        R->bankCapacity[bankIndex] = Capacity - BankNum;/* 否则还是把它剩余的容量标记下 */
        R->TERnodesList[bank]->isOccupied = true;/* 当前层的bank不可用，其他层的不知道 */
        
    }

    if(BankNum % 2 != 0)
    {
        BankNum = BankNum - 1;
    }
            
    BankNumtotoal = (BankNum - 1)*II + 1; /* 在bank中消耗的时间 */

    SingleTruePath.push_back(bank);/* 进bank前先放置单个寄存器 */
    /* 放置 BankNum*II个相同的BANK */
    for(int M = 0; M < BankNumtotoal; M++)
    {
        SingleTruePath.push_back(bank);/* 先放进去 */
    }
    SingleTruePath.push_back(bank);/* 出bank前放置单个寄存器 */

    restLatency = restLatency - BankNumtotoal - 4;

    lu = R->getBankLU(bank,singleTruePathLU);/* 这个就是在CGRAPE内部布线的Load */
    srcR = lu;
    singleTruePathLU.push_back(lu);

}

void setSrcRandRestLantency(Register *R, vector<int>  &SingleTruePath,int &srcR,int latency,int &restLatency,int II,vector<int> &singleTruePathLU)
{
    int lastBankindex;

    if(SingleTruePath.size() != 0 && hasSuBankLoad(SingleTruePath,R) == true)/* 如果上条路径不为空 并且它含store->bank-load固定模式 */
    {
        srcR = getPreLastLU(SingleTruePath,R);/* 获得该条路径上的最后一个LU,从它开始寻路  */
        // cout<<"SingleTruePath.size()="<<SingleTruePath.size()<<endl;
        deletePartPath(SingleTruePath,srcR);/* 那么就将它的最后load作为起始点。后面的删除（包括这个新srcR） */
        
        /* delete的功能:记录下srcR的索引，在索引之前实际消耗了多少层，用虚拟的路径判断 */
        /* 其实这个如果没有普通算子长依赖，没有别要设置。如果要共享普通的路径,重新找就是了，不将其设置为已经访问，因为它没走固定模式 */

        // cout<<"after SingleTruePath.size()="<<SingleTruePath.size()<<endl;
        /* 并将删除后剩下的前面的设为为已经访问 防止在一条路线中，后面重复前面的路线 */
        /* 如果上一条就有固定模式了，那就后面会一直沿用这个固定模式，所以就不能重复了 */
        setVisited(R,SingleTruePath);/* 可能会让固定模式重复设置为已经访问，但没关系 */
        /* 记下最近的bank */
        lastBankindex = getLastBankIndex(SingleTruePath,R);
        // cout<<"new srcR="<<srcR<<endl;


    }      
    else
    {
        SingleTruePath.clear();/* 不能共享的全部清除置0 */
    }

        
    restLatency = latency  - SingleTruePath.size();/* 除掉共享的，剩余的长度 */
    // cout<<"SingleTruePath.size()="<<SingleTruePath.size()<<endl;
    // cout<<"latency="<<latency<<endl;
    // cout<<"restLatency="<<restLatency<<endl;

    if(restLatency > LIMIT)/* 共享之后,或者第一条边没有被共享，仍然还蛮大的话，就要准备切了 */
    {
        /* 切第一次 */
        /* 走ld-su-单个寄存器-bank-单个寄存器-ld  这条路 */
        SingleTruePath.push_back(srcR);/* 先把这个新的srcR放进去 */ 
        singleTruePathLU.push_back(srcR);/* 把这个新的LU放入 */

        int su = R->getLUorResultSU(srcR);/* 获得与LU或者与结果寄存器或者其他寄存器（可能长依赖的源点是计算算子）相连的SU */
        SingleTruePath.push_back(su);
        /* 按顺序获得BANK,LU可以与所有bank互连 */
        int bank = R->getSUBank(su,lastBankindex);/* 获得与SU相连的Bank */
        /* 将这个bank的TECRegister的访问设置为true */

        /* 获得与该bank相连的下一bank */
        // R->getNextBank(bank);
        // cout<<"su="<<su<<"  "<<"bank="<<bank<<endl;

        
        /* 占用的BANK的容量 数量 */
        /* 当前边的总latency - 已经共享的
        因为还要再切或者第一次切，必定到经过 ld-su-单个寄存器 -bank(要求的) -单个寄存器 -ld - dijksta短路径*/
        int BankNum = floor((latency - SingleTruePath.size() - 4) / II); /* 即将占用的bank容量 */

        if(BankNum % 2 != 0)/* BankNum要是偶数 */
        {
            BankNum = BankNum -1;
        }

        /* BankNum为暂定要占用的容量
        1.如果当前的bank的容量大于 BankNum，就继续
        2.如果小于，就先占着，占了之后，看是否需要切下一个Bank */
        int bankIndex = calculateBankIndex(bank);
        int Capacity = R->bankCapacity[bankIndex];/* 该bank存在的容量 */

        // cout<<"bankIndex="<<bankIndex<<"  "<<"Capacity="<<Capacity<<"BankNum="<<BankNum<<endl;
        
        int lu ;/* 确定新的lu，是第一次切的，还是后面切的 */
        int BankNumtotoal = 0;
        
        if(BankNum > Capacity )/* 第一次准备切，发现Bank容量不够,那就要找下一个bank借一下 */
        {
            /* 先把Capacity的容量占着 */
            if(Capacity % 2 != 0)
            {
                BankNum = Capacity - 1;
            }
            else
            {
                BankNum = Capacity;
            }
            
            BankNumtotoal = (BankNum - 1)*II + 1;/* 占Capacity个Bank中的寄存器，在Bank中消耗Capacity *II个时钟周期 */
            
            /* 收集当前路径中所有的LU */
           


            /* 进bank前要锁存一下 */
            SingleTruePath.push_back(bank);/* 这个单寄存器就将它设为bank号吧 */
            /* 放置 BankNum*II个相同的BANK */
            for(int M = 0; M < BankNumtotoal; M++)
            {
                SingleTruePath.push_back(bank);/* 先放进去 */
            }
            SingleTruePath.push_back(bank);/* 出Bank要一个，这个单寄存器就将它设为bank号吧 */

            /* 当前层的这个Bank不能再访问了，其他层可以 */
            R->TERnodesList[bank]->isOccupied = true;
            R->bankCapacity[bankIndex] = R->bankCapacity[bankIndex] - BankNum;

            // cout<<"bankIndex="<<bankIndex<<"  "<<"R->bankCapacity[bankIndex]="<<R->bankCapacity[bankIndex]<<endl;

            
            // cout<<"restLatency="<<restLatency<<endl;
            restLatency = restLatency - BankNumtotoal - 4;/* 占了bank剩余的最大容量后Capacity ，还减了两个单，lu-su,剩下的时钟周期*/
            // cout<<"after restLatency="<<restLatency<<endl;

            /* 和前面的不要重复了 */
            /* 如果原来的srcR是Load结点，就不要和下面的重复了 */
            
            lu = R->getBankLU(bank,singleTruePathLU);/*切了一次后新的srcR */
            // cout<<"lu="<<lu<<"  "<<"bank="<<bank<<endl;
           
            srcR = lu;/* 不打算借的话就src就定了 */
            singleTruePathLU.push_back(srcR);
            lastBankindex = bankIndex;
            while(restLatency > LIMIT)
            {
                /* 如果没有足够的bank可以用了 */
                if(!isEnoughofBank(R))
                {
                    cout<<"Capacity not enough"<<endl;
                }

                fixProcess(R,srcR,restLatency,SingleTruePath,II,lastBankindex,singleTruePathLU);
                lastBankindex = getLastBankIndex(SingleTruePath,R);
                
            }

            // if(restLatency > LIMIT)
            // {
            //     /* 切第一次，然后容量不够，先占满后，后面要借的多不多 dijkst
            //     发现还是需要借蛮多*/
            //     /* 获得上一个bank的bank索引号 */
            //     int bankIndex =calculateBankIndex(bank);
            //     fixProcess(R,srcR,restLatency,SingleTruePath,II,bankIndex);
            //     lastBankindex = getLastBankIndex(SingleTruePath,R);

            //     if(restLatency > LIMIT)/* 占满了bank2，还是很长，那就要搬出bank3了 */
            //     {
            //         fixProcess(R,srcR,restLatency,SingleTruePath,II,lastBankindex);

            //         if(restLatency > LIMIT)/* 占满了最后一个bank,还是不够的话 */
            //         {
            //             cout<<"Capacity not enough"<<endl;
            //         }
                    
                    
            //     }
            // }
            // else{
            //     /* do nothing*/
            // }
            
        }
        else/* 第一次切后，Bank中的容量还充足，直接放 */
        {

            BankNumtotoal = (BankNum - 1)*II + 1;/* 在bank中消耗的时间 */
            /* 放置 BankNum*II个相同的BANK */
            // cout<<"BankNumtotoal="<<BankNumtotoal<<"  "<<"BankNum="<<BankNum<<endl;
            /* 进bank前要锁存一下 */
            R->TERnodesList[bank]->isOccupied = true;
            SingleTruePath.push_back(bank);
            for(int M = 0; M < BankNumtotoal; M++)
            {
                SingleTruePath.push_back(bank);
            }
            SingleTruePath.push_back(bank);/* 出Bank要一个单寄存器 */

            R->bankCapacity[bankIndex] = R->bankCapacity[bankIndex] - BankNum;
            
            restLatency = restLatency - BankNumtotoal - 4;/* 剩下的用dijstra算法寻路,减去的2是 ld-su ，两个单寄存器*/
            // cout<<"srcR="<<srcR<<endl;
            lu = R->getBankLU(bank,singleTruePathLU);/* 这个就是在CGRAPE内部布线的Load */
            // cout<<"lu="<<lu<<endl;
            srcR = lu;
        }
    }
    else
    {
        
        /* 共享之后的restLatency不大，就用 共享路径的最后一个Load srcR 或者原来的src*/
    }

}

/* 同源的边一起布线
prenodeIndex：所有同源边的前驱
srcR:该前驱绑定的资源 */
void shareRoute(DFG *D, Register *R, int prenodeIndex, int srcR, int II, AllPath *allPathClass,int *inportNum,int *outportNum)
{

    vector<DFGedge*>  DFGedgeFromLoad = getEdgesFromBegin(D->DFGedgesList,prenodeIndex);/* 获得以该Node为源头的边 */
    // cout<<"DFGedgeFromLoad.size()"<<DFGedgeFromLoad.size()<<endl;
    sort(DFGedgeFromLoad.begin(),DFGedgeFromLoad.end(),comp); /* 将同源边根据latency的大小排一个序,小的在前 */
    
    vector<int> SingleTruePath;/* 单条真实的路径 */
    vector<vector<int>> sameSrcEdge; /* 已经布线了同源边 集合 */
   
    for(int i = 0; i < DFGedgeFromLoad.size(); i++) /* 从小到大一条条布线 */
    {
        // cout<<"------------------------------------------------"<<endl;
        // cout<<"prenode="<<DFGedgeFromLoad[i]->prenode<<"      posenode="<<DFGedgeFromLoad[i]->posnode<<"       latency"<<DFGedgeFromLoad[i]->latency<<endl;
        vector<int> singleTruePathLU;
        if(R->TERnodesList[srcR]->RegisterKind == 1)
        {
            singleTruePathLU.push_back(srcR);
        }
        int posNode = DFGedgeFromLoad[i]->posnode;/* 边的后驱 */
        int posnodeIndex  = D->getIndex(posNode);/* 边的后驱索引 */
        int latency = DFGedgeFromLoad[i]->latency;/* 当前边的总latency */
        int restLatency = 0;

        int desKind = D->getNodeKind(posNode);    /* 获得DFG目标算子的种类 */  
        int desTime = D->getNodeModuleTime(posNode);/* 获得目的算子的折叠后的时间步 */
       
        /* ----------------------确定在dijkstra算法的 srcR, restLatency（构建图，虚拟层数），目标点，已经存在的共享路径点 */
        

        /* ------------ srcR, restLatency-------------- */
        /* to do :把singleTruePathLU当作参数传进去 */
        setSrcRandRestLantency(R, SingleTruePath,srcR,latency,restLatency,II,singleTruePathLU);
        



        /* ------------目标点-------------------------- */
        vector<int> candidataR;/* 存放真实的目的点集合 */

        if(D->DFGnodesList[posnodeIndex]->isBind == false)/* 没有被绑定 */
        {
            if(desKind == 2)/* 目标算子是store算子，需要放在SU上 */
            {
                /* 获取所有在当前时间步的没有被占用的SU单元 */
                R->getSUSet(desTime,candidataR);
            }
            else if(desKind == 0)/* 否则就是普通的计算算子 */ /* 需要放在真实结果寄存器中 */
            {
                R->getResultRSet(desTime,candidataR);
                // cout<<"candidataR.size()="<<candidataR.size()<<endl;
            }
            
        }
        else/* 绑定过了的 */
        {
            int des = D->DFGnodesList[posnodeIndex]->bindResource;
            // cout<<"des="<<des<<endl;
            R->TERnodesList[des]->inPort = false;/* 防止因为设置过已经访问，而无法再到达Result */
            R->TERnodesList[des-1]->inPort = false;/* 防止因为设置过已经访问，而无法再到达FU */
            candidataR.push_back(des);/* des是结果寄存器 */
            
        }
        // cout<<"srcR="<<srcR<<endl;



        int srcTrueTime = R->TERnodesList[srcR]->time;/* 当前源点所在的真实层数 */
       
        /* -------------构建虚拟图来寻路------------------------ */
        GraphRegister *GR = new GraphRegister(restLatency + 1, R, srcTrueTime, II);/* latency+1层 */
        

        /* -----------------------为dijkstra准备参数-------------------------- */
       
        /* 将srcR真实的，转为虚拟的,虚拟的在第一层 */
        int virtualSrc = srcR % R->RnodesNums; 

        /* 将candidataR真实的，也转为虚拟的，层数在虚拟的最后一层 */
        vector<int> virtualCandidate;
        for(int m = 0; m < candidataR.size(); m++)
        {
            int virtualDes = candidataR[m] % R->RnodesNums + restLatency * R->RnodesNums;
            virtualCandidate.push_back(virtualDes);
        }
        

        vector<int> SingleVirtualPath;/* 存放在单条虚拟的在dijkstra算法的路径，可能是不完整路径 */ 
        Dijk *dijk = new Dijk();
        /* 创建dijstra传入参数的类 */
        dijk->latency = restLatency;
        dijk->virtualSrc = virtualSrc;
        dijk->trueTRNum = R->TERnodesNums;
        dijk->srcTrueTime = srcTrueTime;
        dijk->nodeDesKind = desKind;
        bool dijSuccess = false;
        
        
        dijSuccess = dijkstra(dijk,R,GR,virtualCandidate,SingleVirtualPath,sameSrcEdge,SingleTruePath,inportNum,outportNum);/* 将candidataR传入，i/92 == latency，代表最后一层，并且它在candidaeR里就比较*/            
        // cout<<"dijSuccess="<<dijSuccess<<endl;
        if(dijSuccess == false)
        {
            cout<<"#################################"<<endl;
            cout<<"dijkstra short can't find path"<<endl;
            cout<<"#################################"<<endl;
        }
        
       
        delete dijk;

        int countBank[4];
        for(int m = 0; m < 4; m++)
        {
            countBank[m] = 0;
            // cout<<"bankCapacity["<<m<<"]="<<R->bankCapacity[m]<<endl;
        }

        // cout<<"temp:";
        for(int m = 0; m < SingleVirtualPath.size(); m++)
        {
            int count = 0;
            int virtualTime = SingleVirtualPath[m] / R->RnodesNums;
            int TruePath = ( SingleVirtualPath[m] - (virtualTime * R->RnodesNums) )+ ( (srcTrueTime + virtualTime)  * R->RnodesNums ) % (II * R->RnodesNums);
            int kind = R->TERnodesList[TruePath]->RegisterKind;
            // cout<<SingleVirtualPath[m] <<" "<<TruePath<<" ";
            if(kind == 3)/* 顺便检查一下在dijktra算法中有没有用bank */
            {
                int bankNo = calculateBankIndex(R->TERnodesList[TruePath]->RegisterID);
                countBank[bankNo] ++;
            }
           
            SingleTruePath.push_back(TruePath);/* 将在dijkstra算法中找的部分路径转为真实的路径添加到全部路径中去 */
           
        }
        // cout<<endl;
       
        
        /*  */
        for(int i = 0; i< 4 ;i++)
        {
            int one = R->bankCapacity[i];
            int two = countBank[i];
            R->bankCapacity[i]  = one - two;

            if(R->bankCapacity[i] < II)
            {
                setBankVisited(R,i);             
            }
        }


        /* 单条真实的布好的线 */
        // showPath(R,SingleTruePath,II); 
       
        /* 创建一个Path对象，将vector的数据复制 */
        Path* path = new Path();
		path->DFGpre = prenodeIndex;
        path->DFGpos = posNode;
        path->point = SingleTruePath;
        path->latency = latency;
        
        allPathClass->PathsList.push_back(path);/* 总路线 */

        sameSrcEdge.push_back(SingleTruePath);/* 只包含同源路线 */

        /* if SingleTruePath是最后一条了，在这里设置访问，*/
        /* to do:设置为已经访问，主要是ld，su.  bank，PE内的寄存器都暂时不需要 */
        if( i == DFGedgeFromLoad.size() - 1)
        {
            setVisited(R,SingleTruePath);
        }

        delete GR;/* 某条边已经布好线了 */

        int lastnodeInPath = SingleTruePath[SingleTruePath.size() - 1];/* 这个是结果寄存器或SU */
        // cout<<"lastnodeInPath="<<lastnodeInPath<<endl;
        // cout<<"posnodeIndex="<<posnodeIndex<<endl;
        if(desKind == 0)
        {
            R->TERnodesList[lastnodeInPath-1]->inPort = true;
        }
        R->TERnodesList[lastnodeInPath]->inPort = true;
        
        D->DFGnodesList[posnodeIndex]->bindResource = lastnodeInPath;
        // cout<<"posnodeIndex="<<posnodeIndex<<"  "<<"D->DFGnodesList[posnodeIndex]->bindResource ="<<D->DFGnodesList[posnodeIndex]->bindResource <<endl;
        D->DFGnodesList[posnodeIndex]->isBind = true;
  
        SingleVirtualPath.clear();
        candidataR.clear();
 
    }//for:同源的边
   
    SingleTruePath.clear();

    /* 同源的布完线后，重新设置一下TERegister的访问。pe1->pe2的边，pe2经历过从外面得到数据，因此入PE2的外界边的权值都要设为infine */
    /* 遍历每一条同源的路径，如果是PE-PE,就要将 TERegister对应的点的属性设置一下*/
    // sameSrcEdge vector<vector<int>> sameSrcEdge;
    // cout<<"-----------------------"<<endl;
    /* 不是粗暴的将某个寄存器设为不能访问，因为它可能还能旁路 */

    int totolpea = 40;
    map<int, int> m1;
    for(int i = 0; i < sameSrcEdge.size(); i++)/* 多少条同源 */
    {
        for(int j = 0; j < sameSrcEdge[i].size() - 1; j++)
        {
            int node1 = sameSrcEdge[i][j];
            int node2 = sameSrcEdge[i][j+1];
            int node1PE = R->TERnodesList[node1]->PE;
            int node2PE = R->TERnodesList[node2]->PE;
            if(m1.count(node1)==0)
            {
                m1.insert ( pair <int, int> ( node1, node2 ) );
                if(R->TERnodesList[node1]->RegisterKind == 0 && R->TERnodesList[node2]->RegisterKind == 0 )/* 普通寄存器-》普通寄存器 */
                {
                    if(node1PE % totolpea != node2PE % totolpea )/* 数据传出去了 */
                    {
                        /* 获得当前的PE */
                        inportNum[node2PE]--;/* 输入端口减1 */
                        outportNum[node1PE]--;/* 输出端口减1 */
                    }

                }
                if(R->TERnodesList[node1]->RegisterKind == 5 && R->TERnodesList[node2]->RegisterKind == 0 )/* 普通寄存器-》普通寄存器 */
                {
                    if(node1PE % totolpea != node2PE % totolpea )/* 数据传出去了 */
                    {
                        /* 获得当前的PE */
                        inportNum[node2PE]--;/* 输入端口减1 */
                        outportNum[node1PE]--;/* 输出端口减1 */
                    }

                }
                if(R->TERnodesList[node1]->RegisterKind == 1 && R->TERnodesList[node2]->RegisterKind == 0 )/* LU-》普通寄存器 */
                {
                    
                    inportNum[node2PE]--;/* 输入端口减1 */          
                }
                if(R->TERnodesList[node1]->RegisterKind == 0 && R->TERnodesList[node2]->RegisterKind == 2 )/* 普通寄存器-->SU */
                {
                    
                    outportNum[node1PE]--;/* 输出端口减1 */          
                }



                if((R->TERnodesList[node1]->time == R->TERnodesList[node2]->time)  && R->TERnodesList[node2]->RegisterKind == 0)/* PE内寄存器 */
                {
                    // cout<<"node1=" <<node1 <<"  " <<"node2=" <<node2 <<endl;
                    R->TERnodesList[node2]->usedBypass = true;
                    
                
                }
                else if((R->TERnodesList[node1]->time != R->TERnodesList[node2]->time)  && R->TERnodesList[node2]->RegisterKind == 0)
                {
                    R->TERnodesList[node2]->usedTimeTrans = true;
                }

                if(R->TERnodesList[node2]->usedBypass && R->TERnodesList[node2]->usedTimeTrans)
                {
                    R->TERnodesList[node2]->inPort = true;/* 已经访问了，不能外来数据了 */

                }


            }
            else
            {
                continue;
            }
            
        }
     
    }


    /* 每步完所有同源的，查看一下，inport */
    for(int i = 0; i < R->TERnodesNums; i++)
    {
        /* 还能用 */
        if(R->TERnodesList[i]->inPort == false)
        {
            // cout<<"R->TERnodesList[i]->RegisterID="<<R->TERnodesList[i]->RegisterID<<"  "<<"R->TERnodesList[i]->inPort="<<R->TERnodesList[i]->inPort<<endl;
        }
     
    }





    for (auto edge : DFGedgeFromLoad)
    {
        delete edge;
    }
 

}

void showPath(Register *R, vector<int> PathPoint,int II)
{
    bool alreadyPrint = false;
    int num = 0;
    bool bankVisited[4] = {false,false,false,false};
    int bankIndex = 0;
    for(int j = 0; j < PathPoint.size(); j++ )
    {
        
        int node = PathPoint[j];
       
        if(R->TERnodesList[node]->RegisterKind == 3)
        {
            bankIndex = calculateBankIndex(R->TERnodesList[node]->RegisterID);
        }
        // cout<<"bankVisited"<<bankVisited<<endl;
        
        if(R->TERnodesList[node]->RegisterKind == 3 && bankVisited[bankIndex] == false)/* 如果种类是bank的第一个 */
        {
            cout<<node<<" ";
            num ++;
            bankVisited[bankIndex] = true;
            
        }
        else if(R->TERnodesList[node]->RegisterKind == 3 && bankVisited[bankIndex] == true)/* 如果种类是bank */
        {  

            num ++;
            bankVisited[bankIndex] = true;
            alreadyPrint = false;
        }
        
        else if(R->TERnodesList[node]->RegisterKind != 3 &&num !=0 && alreadyPrint == false)
        {
            cout<<"("<<(num -3)/II<<"*"<<II<<"+1+2)  ";/* 包含输出的第一个？ */
           
            cout<<node<<" ";
            alreadyPrint = true;
        }
        else 
        {
            bankVisited[bankIndex] = false;
            num = 0;
            cout<<node<<" ";
        }
    }
    cout<<endl;


}



