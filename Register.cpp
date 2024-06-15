#include "Register.h"

Registernode::Registernode()
{

}
Registeredge::Registeredge()
{

}

Register::~Register()
{
	for (auto node : RnodesList)
	{
		delete node;
	}
 	for (auto edge : RedgesList)
	{
		delete edge;
	}
	for (auto tenode : TERnodesList)
	{
		delete tenode;
	}
 	for (auto teedge : TERedgesList)
	{
		delete teedge;
	}
	

}

int Register::getIndex(int R)
{
	for (int i = 0; i < this->TERnodesList.size();i++)
	{
		if (this->TERnodesList[i]->RegisterID == R )
		{
			return i;

		}
	}
}

int Register::getLU(int time,int latency)
{
	for (int i = 0; i < this->TERnodesList.size();i++)
	{
		/* 按顺序的没有被占用的 */
		if (this->TERnodesList[i]->RegisterKind == 1 && this->TERnodesList[i]->time == time && this->TERnodesList[i]->isOccupied == false)/* LU寄存器 */
		{
			/* LU的PE邻居都用完了，获得LU邻居 */
			int lu = this->TERnodesList[i]->RegisterID;
			for(int j = 0; j < this->TERnodesList[lu]->RegisterNeighbors.size(); j++)
			{
				int lunei = this->TERnodesList[lu]->RegisterNeighbors[j];
				// cout<<"lunei="<<lunei<<endl;
				if(this->TERnodesList[lunei]->inPort == false && this->TERnodesList[lunei]->RegisterKind == 0 )
				{
					/* 如果这个邻居和LU在同一个时间步，且lenth==1 */
					if(latency == 1)
					{
						if(this->TERnodesList[lunei]->time == time )
						{
							return lu;
						}
						else
						{
							continue;
						}

					}
					else
					{
						return lu;
					}

				}
			}


			
		}
	}
	return -1;/* 没有LU可使用了 */
}

/* 获得与LU或结果寄存器相连的相连的SU寄存器 */
int Register::getLUorResultSU(int SRC)
{
	for(int i = 0; i < this->TERnodesList[SRC]->RegisterNeighbors.size(); i++)
	{
		
		int neighboreIndex = this->getIndex(this->TERnodesList[SRC]->RegisterNeighbors[i]);
		/* 这个邻居是SU,并且它没有被访问过 */
		if(this->TERnodesList[neighboreIndex]->RegisterKind == 2 && this->TERnodesList[neighboreIndex]->isOccupied == false)
		{
			return this->TERnodesList[SRC]->RegisterNeighbors[i];
		}

	}
	/* 遍历完了，都没有 */
	return -1;
	
}

bool isInPreLU(int lu, vector<int> srcR) 
{
	for(int i = 0; i < srcR.size(); i++)
	{
		if(srcR[i] == lu)
		{
			return true;
		}
	 
	}
	return false;

}



/* 获得与Bank相连的LU */
int Register::getBankLU(int bank,vector<int> srcR)
{
	for(int i = 0; i < this->TERnodesList[bank]->RegisterNeighbors.size(); i++)
	{
		int neighboreIndex = this->getIndex(this->TERnodesList[bank]->RegisterNeighbors[i]);
		if(this->TERnodesList[neighboreIndex]->RegisterKind == 1 && this->TERnodesList[neighboreIndex]->isOccupied == false)
		{
			
			if(isInPreLU(neighboreIndex,srcR))
			{
				continue;
			}
			else
			{
				return neighboreIndex;
			}

		}
	}
	return -1;
}

/* 获得与SU相连的bank */
int Register::getSUBank(int su,int lastBankindex)
{
	for(int i = 0; i < this->TERnodesList[su]->RegisterNeighbors.size(); i++)
	{
		int neighboreIndex = this->getIndex(this->TERnodesList[su]->RegisterNeighbors[i]);
		 
		if(this->TERnodesList[neighboreIndex]->RegisterKind == 3 && this->TERnodesList[neighboreIndex]->isOccupied == false )
		{
			int node = this->TERnodesList[su]->RegisterNeighbors[i];
			int bankIndex = node %  this->RnodesNums / 30 * 2 + node % 2;
			if(bankIndex != lastBankindex)
			{
				
				return node;
			}
		}
		
	}
	return -1;
}






/* 获得寄存器的种类 */
int Register::getNodeKind(int nodeLabel)
{
    vector<Registernode*> nodesList = this->TERnodesList;
	for (auto node : nodesList)
	{
		if (node->RegisterID == nodeLabel)
		{
			return node->RegisterKind;
		}
	}
	return -1;

}

/* 获得与LU相连的PE寄存器 */
int Register::getLUR(int LU)
{
	for (int i = 0; i < this->TERnodesList.size();i++)
	{
		if (this->TERnodesList[i]->RegisterID == LU )
		{
			for(int j = 0; j<this->TERnodesList[i]->RegisterNeighbors.size(); j++)
			{
				
				if(this->getNodeKind(this->TERnodesList[i]->RegisterNeighbors[j]) == 0)
					return this->TERnodesList[i]->RegisterNeighbors[j];

			}
		}
	}
	
}
	



void Register::getSUSet(int time, vector<int> &candidateR)
{

	for (auto node : this->TERnodesList)
	{
		if (node->RegisterKind == 2 && node->time == time &&  node->isOccupied == false)
		{
			candidateR.push_back(node->RegisterID);
		}

	}
}


void Register::getResultRSet(int time,vector<int> &candidataR)
{
	
	for (auto node : this->TERnodesList)
	{
		if (node->RegisterKind == 5 && node->time == time && node->isOccupied == false)/* 结果寄存器 */
		{
			candidataR.push_back(node->RegisterID);
		}

	}
}



/* 创建平面Register */
Register::Register(int Rnum, CGRA *C)
{   
	int peNumInRow = 10;
	int notpeInRow = 6;
	int regInPE = 6;
    int count = 0;
	bankCapacity[0] = 0;/* 第一个已经默认装满了 */
	for(int i = 1; i < 8; i++)/* 剩余7个 */
	{
		bankCapacity[i] = 256;/* 容量为256个int型数据 */
	}
	
 
	for(int i = 0; i < C->CGRAnodesList.size(); i++)
	{
		int neighNum = C->CGRAnodesList[i]->ElmNeighbors.size();
		
		if(C->CGRAnodesList[i]->ElmKind == 3)/* BANK */
		{
			Registernode* node = new Registernode();
			node->RegisterID = count;
			node->PE = C->CGRAnodesList[i]->ElmID;
			node->RegisterKind = 3;
			node->isOccupied = false;
			
			for(int m = 0; m < neighNum; m++)
			{
				int nei = C->CGRAnodesList[i]->ElmNeighbors[m];
			 	int r = nei/peNumInRow * 30 + nei % peNumInRow;
				node->RegisterNeighbors.push_back(r);
			}

			RnodesList.push_back(node);
			count++;
		}
		else if(C->CGRAnodesList[i]->ElmKind == 1)/* LU */
		{
			
			Registernode* node = new Registernode();
			node->RegisterID = count;
			node->PE =  C->CGRAnodesList[i]->ElmID;
			node->RegisterKind = 1;
			node->isOccupied = false;
			for(int m = 0; m < neighNum; m++)
			{
				int nei = C->CGRAnodesList[i]->ElmNeighbors[m];
			 	if(C->CGRAnodesList[nei]->ElmKind == 2)/* 邻居的种类是SU */
				{
					node->RegisterNeighbors.push_back(node->RegisterID + 1);
				}
				else if(C->CGRAnodesList[nei]->ElmKind == 0)/* 如果邻居是PE */
				{
					int baseR = (nei % peNumInRow-6 +nei/peNumInRow*4)*regInPE + (nei/peNumInRow+1) * notpeInRow + 2;
					for(int m = 0; m < 4; m++)
					{
						node->RegisterNeighbors.push_back(baseR + m);
					}

				}

				
			}

			RnodesList.push_back(node); 
			count++;
		}
		else if(C->CGRAnodesList[i]->ElmKind == 2)/* SU */
		{
			Registernode* node = new Registernode();
			node->RegisterID = count;
			node->PE =  C->CGRAnodesList[i]->ElmID;
			node->RegisterKind = 2;
			node->isOccupied = false;
			for(int m = 0; m < neighNum; m++)
			{
				int nei = C->CGRAnodesList[i]->ElmNeighbors[m];
			 	int b = nei/peNumInRow * 30 + nei % peNumInRow;/* bank */
				node->RegisterNeighbors.push_back(b);
			}
			RnodesList.push_back(node); 
			count++;
		}
		
		else if(C->CGRAnodesList[i]->ElmKind == 0)/* PE */
		{
			/* 先FU */
			Registernode* node = new Registernode();
			node->RegisterID = count;
			node->PE = C->CGRAnodesList[i]->ElmID;
			node->RegisterKind = 4;/* fu */
			node->isOccupied = false;
			node->RegisterNeighbors.push_back(node->RegisterID + 1);
			RnodesList.push_back(node); 
			count++;

	
			/* 再其他寄存器 */
			for(int j = 0; j < Rnum ; j++)
			{
				Registernode* node = new Registernode();
				node->RegisterID = count;
				node->PE = C->CGRAnodesList[i]->ElmID;
				int baseR = (node->PE % peNumInRow-6 + node->PE/peNumInRow*4)*notpeInRow + (node->PE/peNumInRow+1) * notpeInRow;
				if(node->RegisterID == baseR + 1 )
				{
					node->RegisterKind = 5;/* 结果寄存器 */
				}
				else
				{
					node->RegisterKind = 0;/* 普通寄存器 */			
				}

				node->isOccupied = false;
				
				int neighNum = C->CGRAnodesList[i]->ElmNeighbors.size();
				
						
				if( node->RegisterID + 1 < baseR + Rnum + 1)
				{
					node->RegisterNeighbors.push_back(node->RegisterID + 1 );/* 同一PE的下一个寄存器 */
						
				}
					

				for(int m = 0; m < neighNum; m++)
				{
					int neighbor = C->CGRAnodesList[i]->ElmNeighbors[m];
					
					if(C->CGRAnodesList[neighbor]->ElmKind == 0)/* 如果邻居是PE */
					{
						/* 该邻居的前四个寄存器是 邻居 */
						/* 该PE邻居 */
						int base = (neighbor%peNumInRow-notpeInRow + neighbor/peNumInRow*4)*regInPE + (neighbor/peNumInRow+1) * notpeInRow + 1;
						for(int n = 1; n < Rnum; n++)
						{
							node->RegisterNeighbors.push_back(base + n);
						}
				
					}
					if(C->CGRAnodesList[neighbor]->ElmKind == 2)/* 如果邻居是SU*/
					{
						int SUR = neighbor/peNumInRow * 30 + neighbor % peNumInRow ;
						node->RegisterNeighbors.push_back(SUR);/* neighbor是pe，改为寄存器 */
					}

				}

				/* 邻居还有FU,结果寄存器的邻居也有FU */
				node->RegisterNeighbors.push_back(baseR);
				
				RnodesList.push_back(node); 
				count++;
			}

			
			
		}
		else
		{


		}
		
	}
	RnodesNums = count;
	// /* 寄存器点的输出 */
	// for(int x = 0; x < RnodesList.size(); x++)
	// {
	// 	cout<<"RID="<<RnodesList[x]->RegisterID<<" R->PE="<<RnodesList[x]->PE<<" R->Kind"<<RnodesList[x]->RegisterKind<<endl;
	// 	cout<<" neghbor:";
	// 	for(int y = 0; y < RnodesList[x]->RegisterNeighbors.size(); y++)
	// 	{
	// 		cout<< RnodesList[x]->RegisterNeighbors[y]<<" ";

	// 	}
	// 	cout<<endl;
	// }


	/* 寄存器边 */
	count = 0;
	for(int x = 0; x < RnodesList.size(); x++)
	{	
		for(int y = 0; y < RnodesList[x]->RegisterNeighbors.size(); y++)
		{
			Registeredge* edge = new Registeredge();

			edge->edgeId = count;
			edge->srcReg = RnodesList[x]->RegisterID;
			edge->tgtReg = RnodesList[x]->RegisterNeighbors[y];
			RedgesList.push_back(edge); 
			count ++;

		}
	}
	RedgesNums = count;
	// for(int x = 0; x < RedgesList.size(); x++)
	// {
	// 	cout<<RedgesList[x]->edgeId<<" "<<RedgesList[x]->srcReg<<" "<<RedgesList[x]->tgtReg<<endl;
	// }
	
}

void Register::CreatTER(int II)
{
	int totalPENum = 40;
	int count = 0;/* 记录扩展后的寄存器的数量 */
	this->II = II;

	for(int i = 0; i < II; i++)/* 遍历所有的寄存器节点，平面的 */
	{
		for(int j = 0; j < this->RnodesNums; j++)
		{

			Registernode* tenode = new Registernode();
			tenode->RegisterID = count;
			tenode->inPort = false;/* 默认输入端口还能用,TRUE=1就不能用了 */
			

			if(this->RnodesList[j]->RegisterKind == 3)/* 如果当前寄存器是BANK */
			{
				
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 3;
				/* 将0 bank设为已经访问，不可再访问 */
				
				int bankindex = tenode->RegisterID%this->RnodesNums  /30 *2  + tenode->RegisterID % 2;
				if(bankindex == 0 )
				{
					tenode->isOccupied = true;	
					tenode->inPort = true;	
				}
				else
				{
					tenode->isOccupied = false;				
				}
						
				tenode->time = i;
				
				/* 添加邻居 ，下一层邻居*/
				if(II!=1)
					tenode->RegisterNeighbors.push_back(tenode->RegisterID % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* Bank不动，也就是自己到自己 */
				for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* 下一层 */
				} 
				
			}
			else if(this->RnodesList[j]->RegisterKind == 1)/* LU */
			{
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 1;
				tenode->isOccupied = false;		
				tenode->time = i;
				
				/* 自己到自己的 *//* LU它可以连接到下一层的自己，代表下一层不动 */ 

				/* lu可以旁路到PE内的寄存器，不能旁路到SU
				但PE内的寄存器不能旁路到FU */
				
				
				for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					int nei = this->RnodesList[j]->RegisterNeighbors[n];
					if(this->RnodesList[nei]->RegisterKind != 2)
					{
						tenode->RegisterNeighbors.push_back(nei% this->RnodesNums + (i%II) * this->RnodesNums % this->RnodesNums + (i%II) * this->RnodesNums);/* 下一层 */
					}
				} 
				if(II != 1)
					tenode->RegisterNeighbors.push_back(tenode->RegisterID %this->RnodesNums + ((i+1)%II) * this->RnodesNums);
				
				for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* 下一层 */
				} 			
			}
			else if(this->RnodesList[j]->RegisterKind == 2)/* SU */
			{
				
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 2;
				tenode->isOccupied = false;
				tenode->time = i;
				if(II != 1)
					tenode->RegisterNeighbors.push_back(tenode->RegisterID %this->RnodesNums + ((i+1)%II) * this->RnodesNums);
				for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* 下一层 */
				}
				
			}
			else if(this->RnodesList[j]->RegisterKind == 4)/* 先FU */
			{
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 4;
				tenode->isOccupied = false;
				tenode->time = i;
				/* 邻居不能自己到自己，可以被旁路进来，不可以被旁路出去 */
				/* 只能是同层的结果寄存器 */			
				tenode->RegisterNeighbors.push_back(tenode->RegisterID + 1);/*  */


			}
			else if(this->RnodesList[j]->RegisterKind == 5)/* 再结果寄存器 */
			{
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 5;
				tenode->isOccupied = false;
				tenode->time = i;

				if(II != 1)
				{
					/* 旁路只能邻居间的旁路，:不能自己内部间的旁路。只能PE->PE ; */
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						
						int neighborIndex = this->RnodesList[j]->RegisterNeighbors[n];
						/* 结果寄存器不能旁路到SU,FU */
						if( this->RnodesList[neighborIndex]->RegisterKind != 2 && this->RnodesList[neighborIndex]->RegisterKind != 4 )/* 结果寄存器只能旁路到邻居寄存器 */
						{
							if((this->RnodesList[neighborIndex]->PE + i* totalPENum)!= tenode->PE)
								tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + (i%II) * this->RnodesNums % this->RnodesNums + (i%II) * this->RnodesNums);/* 平面的同一层 */
						}
						
					} 
					/* 最好不要让结果寄存器停留？因为当前的FU就不能使用了 */ 
					// if(II != 1)
					// /* 自己到自己 */
					// 	tenode->RegisterNeighbors.push_back(tenode->RegisterID % this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);
					/* 下一层 */
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* 下一层 */

					} 

				}
				else
				{
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						int neighborIndex = this->RnodesList[j]->RegisterNeighbors[n];
						if(this->RnodesList[neighborIndex]->RegisterKind != 4)/* 当II=1的时候，不可能到同层的FU */
							tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]);
					}

				}


			}
			else if(this->RnodesList[j]->RegisterKind == 0)/* PE内的寄存器 */
			{
				
				tenode->PE = RnodesList[j]->PE + i * totalPENum;
				tenode->RegisterKind = 0;
				tenode->isOccupied = false;				
				tenode->time = i;
				if(II != 1)
				{
					/* 旁路只能邻居间的旁路，TO DO:不能自己内部间的旁路。PE->PE 以及 到FU; */
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						
						int neighborIndex = this->RnodesList[j]->RegisterNeighbors[n];
						/* 普通寄存器不能旁路到自己内部的寄存器，不能旁路到FU和SU */
						if( this->RnodesList[neighborIndex]->RegisterKind != 2 && this->RnodesList[neighborIndex]->RegisterKind != 4)
						{
							if( (this->RnodesList[neighborIndex]->PE + i* totalPENum)!= tenode->PE)
							{
								tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + (i%II) * this->RnodesNums % this->RnodesNums + (i%II) * this->RnodesNums);/* 平面的同一层 */
							}

						}
						
					} 
					if(II != 1)
					/* 自己到自己 */
						tenode->RegisterNeighbors.push_back(tenode->RegisterID % this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);
					/* 下一层 */
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]% this->RnodesNums + ((i+1)%II) * this->RnodesNums % this->RnodesNums + ((i+1)%II) * this->RnodesNums);/* 下一层 */

					} 
				}
				else/* 如果II等于1 */
				{
					for(int n = 0; n < this->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						tenode->RegisterNeighbors.push_back(this->RnodesList[j]->RegisterNeighbors[n]);
					}

				}
			}
			
			TERnodesList.push_back(tenode);
			count++;

			
		}
			
	}
	this->TERnodesNums = count;


	/* 扩展寄存器的输出 */
	// for(int x = 0; x < TERnodesList.size(); x++)
	// {
	// 	cout<<"ID:"<<TERnodesList[x]->RegisterID<<"   PE: "<<TERnodesList[x]->PE<<"   kind: "<<TERnodesList[x]->RegisterKind<<"   time: "<<TERnodesList[x]->time<<"   inPort: "<<TERnodesList[x]->inPort<<endl;
	// 	cout<<" neghbor:";
	// 	for(int y = 0; y < TERnodesList[x]->RegisterNeighbors.size(); y++)
	// 	{
	// 		cout<< TERnodesList[x]->RegisterNeighbors[y]<<" ";

	// 	}
	// 	cout<<endl;
	// }



	/* 寄存器边 */
	count = 0;
	for(int x = 0; x < TERnodesList.size(); x++)
	{	
		for(int y = 0; y < TERnodesList[x]->RegisterNeighbors.size(); y++)
		{
			Registeredge* edge = new Registeredge();

			edge->edgeId = count;
			edge->srcReg = TERnodesList[x]->RegisterID;
			edge->tgtReg = TERnodesList[x]->RegisterNeighbors[y];
			TERedgesList.push_back(edge); 
			count ++;

		}
	}
	this->TERedgesNums = count;
	// for(int x = 0; x < TERedgesList.size(); x++)
	// {
	// 	cout<<TERedgesList[x]->edgeId<<" "<<TERedgesList[x]->srcReg<<" "<<TERedgesList[x]->tgtReg<<endl;
	// }

}




