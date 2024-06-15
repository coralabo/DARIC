#include "GraphRegister.h"
#include "Register.h"


GraphNode::GraphNode()
{

}
GraphEdge::GraphEdge()
{

}

GraphRegister::~GraphRegister()
{
	for (auto node : GraphnodesList)
	{
		delete node;
	}
 	for (auto edge : GraphedgesList)
	{
		delete edge;
	}
}


bool GraphRegister::graphHasEdge(int begin, int end)
{
	bool flag = false;
	vector<GraphEdge*> edges = this->GraphedgesList;
	for (auto edge : edges)
	{
		if (edge->pre == begin && edge->pos == end)
		{
			flag = true;
			break;
		}
	}
	return flag;
}


int GraphRegister::getEdgeCost(int begin, int end)
{
	vector<GraphEdge*> edges = this->GraphedgesList;
	if(this->graphHasEdge(begin,end))
	{
		for (auto edge : edges)
		{
			if (edge->pre == begin && edge->pos == end)
			{
				return edge->value;
				
			}
		}

	}
	else
	{
		return INF;
	}
}




void GraphRegister::setEdgeValueFromPre(int pre,int value)
{
		
	for(int i = 0; i < this->GraphedgesList.size(); i++)
	{
		if(this->GraphedgesList[i]->pre == pre )
		{
			this->GraphedgesList[i]->value = value;
		}

	
	}
	


}


void GraphRegister::setEdgeValueToPos(int pos,int value)
{
		
	for(int i = 0; i < this->GraphedgesList.size(); i++)
	{
		if(this->GraphedgesList[i]->pos == pos)
		{
			this->GraphedgesList[i]->value = value;
		}

	
	}
	


}


/* to do:最后一层不必要要返回来 */
GraphRegister::GraphRegister(int length, Register *R,int SrcTrueTime,int II)
{
	int totolpea = 40;

	int count = 0;
	for(int i = 0; i < length; i++)/* length为层数 */
	{
		for(int j = 0; j < R->RnodesNums; j++)
		{
			GraphNode* node = new GraphNode();
			node->GNodeID = count;
			if(R->RnodesList[j]->RegisterKind == 3)/* BANK */
			{
				node->RegisterKind = 3;

				int nextLevelNeighbor = node->GNodeID % R->RnodesNums + ((i + 1) % length) * R->RnodesNums;
				if(i != (length-1) )
				{
					node->GNodeNeighbors.push_back(nextLevelNeighbor);/* 自己到下一层自己 */
					for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* 下一层 */
					}
				}
			}
			else if(R->RnodesList[j]->RegisterKind == 1)/* LU */
			{
				node->RegisterKind = 1;
				for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					int neighborIndex = R->getIndex(R->RnodesList[j]->RegisterNeighbors[n]);
					
					if( R->RnodesList[neighborIndex]->RegisterKind != 2 &&  R->RnodesList[neighborIndex]->RegisterKind != 4)/* 旁路不能从结果寄存器到su和FU */
					{
						
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + (i%length) * R->RnodesNums % R->RnodesNums + (i%length) * R->RnodesNums);/* 平面的同一层 */
					}
					
				} 


				if(i != (length-1))
				{
					/* LU还是不要自己到自己，122，2‘2为最后一个Load */
					// node->GNodeNeighbors.push_back(node->GNodeID % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* 下一层的自己 */
					
					for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)/* 下一层的邻居 */
					{
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* ��һ�� */
					} 

				}
				
			}
			else if(R->RnodesList[j]->RegisterKind == 2)/* SU */
			{
				node->RegisterKind = 2;
				if(i != (length-1))
				{
					node->GNodeNeighbors.push_back(node->GNodeID % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* 下一层的自己 */
					for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)/* 下一层的邻居 */
					{
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* ��һ�� */
					}
				}
			}
			else if(R->RnodesList[j]->RegisterKind == 4)/* 先FU */
			{	
				node->RegisterKind = 4;	
				/* 只能同层的结果寄存器 */
				node->GNodeNeighbors.push_back(node->GNodeID + 1);/*  */
			}

			else if(R->RnodesList[j]->RegisterKind == 5)/* 再结果寄存器 */
			{		
				node->RegisterKind = 5;
				/* 旁路只能邻居间的旁路，TO DO:不能自己内部间的旁路。PE->PE 以及 到FU; */
				for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					
					int neighborIndex = R->getIndex(R->RnodesList[j]->RegisterNeighbors[n]);
					
					if( R->RnodesList[neighborIndex]->RegisterKind != 2 &&  R->RnodesList[neighborIndex]->RegisterKind != 4)/* 旁路不能从结果寄存器到su和FU */
					{
						if((R->RnodesList[neighborIndex]->PE + i* totolpea)!= (R->RnodesList[j]->PE + i* totolpea))
							node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + (i%length) * R->RnodesNums % R->RnodesNums + (i%length) * R->RnodesNums);/* 平面的同一层 */
					}
					
				} 

				if(i != (length-1))
				{
					// /* 自己到下一层自己 */
					// node->GNodeNeighbors.push_back(node->GNodeID % R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);
					/* 下一层 */
					
					for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums  + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* 下一层 */
						// if(j == 58)
						// {	
						// 	int mmm = R->RnodesList[58]->RegisterNeighbors[0]% R->RnodesNums  + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums;
						// 	cout<<"mmmm"<<mmm<<endl;
						// 	cout<<"R->RnodesList[j]->RegisterNeighbors[0]"<<R->RnodesList[58]->RegisterNeighbors[0]<<endl;
						// }
					} 
				}
			}
			else if(R->RnodesList[j]->RegisterKind == 0)/* PE内的寄存器 */
			{
				node->RegisterKind = 0;
				/* 旁路只能邻居间的旁路，TO DO:不能自己内部间的旁路。PE->PE 以及 到FU; */
				for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
				{
					
					int neighborIndex = R->getIndex(R->RnodesList[j]->RegisterNeighbors[n]);
					/* 普通寄存器可以旁路到FU和邻居寄存器 */
					// if( (R->RnodesList[neighborIndex]->PE + i* totolpea) != (R->RnodesList[j]->PE + i*totolpea) || R->RnodesList[neighborIndex]->RegisterKind == 4)
					if( ((R->RnodesList[neighborIndex]->PE + i* totolpea) != (R->RnodesList[j]->PE + i* totolpea)) && R->RnodesList[neighborIndex]->RegisterKind != 2 )
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + (i%length) * R->RnodesNums % R->RnodesNums + (i%length) * R->RnodesNums);/* 平面的同一层 */
					
				} 
				if(i != (length-1))
				{
					/* 自己到下一层自己 */
					node->GNodeNeighbors.push_back(node->GNodeID % R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);
					
					/* 下一层 */
					for(int n = 0; n < R->RnodesList[j]->RegisterNeighbors.size(); n++)
					{
						
						node->GNodeNeighbors.push_back(R->RnodesList[j]->RegisterNeighbors[n]% R->RnodesNums + ((i+1)%length) * R->RnodesNums % R->RnodesNums + ((i+1)%length) * R->RnodesNums);/* 下一层 */

					} 
				}
				


			}
			GraphnodesList.push_back(node); 
			count++;
			
		}
			
	}
	GraphnodesNums = count;

	
	// for(int x = 0; x < GraphnodesList.size(); x++)
	// {
	// 	cout<<"ID:"<<GraphnodesList[x]->GNodeID<<endl;
	// 	cout<<" neghbor:";
	// 	for(int y = 0; y < GraphnodesList[x]->GNodeNeighbors.size(); y++)
	// 	{
	// 		cout<< GraphnodesList[x]->GNodeNeighbors[y]<<" ";

	// 	}
	// 	cout<<endl;
	// }
	

	

	/*  */
	count = 0;
	for(int x = 0; x < GraphnodesList.size(); x++)
	{	
		for(int y = 0; y < GraphnodesList[x]->GNodeNeighbors.size(); y++)
		{
			GraphEdge* edge = new GraphEdge();

			edge->GEdgeId = count;
			edge->pre = GraphnodesList[x]->GNodeID;
			edge->pos = GraphnodesList[x]->GNodeNeighbors[y];

			edge->value = 2;/* 都有一个初值 */
			GraphedgesList.push_back(edge); 
			count ++;

		}
	}
	GraphedgesNums = count;


	/* 为GraphRegister的边赋值 */
	for(int x = 0; x < GraphedgesList.size(); x++)
	{

		int preNode = GraphedgesList[x]->pre;
		int posNode = GraphedgesList[x]->pos;
		int preTime = preNode  / R->RnodesNums;
		int posTime = posNode  / R->RnodesNums;
		

		/* 从虚拟的点到真实的点 */
		int firstR   = ( preNode - (preTime* R->RnodesNums) )+ ( (SrcTrueTime + preTime)  * R->RnodesNums ) % (II * R->RnodesNums);
		int secondR  = ( posNode - (posTime* R->RnodesNums) )+ ( (SrcTrueTime + posTime)  * R->RnodesNums ) % (II * R->RnodesNums);
		// cout<<"preNode="<<preNode<<"   posNode="<<posNode<<"   firstR="<<firstR<<"   secondR="<<secondR<<endl;
		

		/* 先正常的设置，
		1.Bank->bank,1
		2.bank->load,2
		3.load->store,2
		4.load-pe,2
		5.store->bank,2
		6.pe->pe2(同一PE)，PE->PE(不同PE)3,pe->store
		 */
		if(R->TERnodesList[firstR]->RegisterKind == 3 && R->TERnodesList[secondR]->RegisterKind == 3  )/* 如果是BANK->BANK*/
		{
			GraphedgesList[x]->value = 1;
		}
		else if( R->TERnodesList[firstR]->RegisterKind == 5 && R->TERnodesList[secondR]->RegisterKind == 0 )/* 如果是PE->PE */
		{
			if(R->TERnodesList[firstR]->PE% totolpea != R->TERnodesList[secondR]->PE % totolpea  )/* 不同PE */
			{
				
				if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
				{
					
					
					GraphedgesList[x]->value = INF;
				}
				else{
					GraphedgesList[x]->value = 40;
				}

			}
			else/* 相同PE（不同层次） */
			{
				if(II != 1)/* 如果II！=1,自己到自己更好 */
				{
					/* II层下的 */
					if(firstR % R->RnodesNums == secondR % R->RnodesNums)
					{
						GraphedgesList[x]->value = 2;
					}
					else
					{
						GraphedgesList[x]->value = 5;
					}
					

				}
				else{
					GraphedgesList[x]->value = 5;
				}
			}

		}

		else if(R->TERnodesList[firstR]->RegisterKind == 0 && R->TERnodesList[secondR]->RegisterKind == 0 )/* 如果是PE->PE */
		{
			if(R->TERnodesList[firstR]->PE% totolpea  != R->TERnodesList[secondR]->PE % totolpea )/* 不同PE */
			{
				
				if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
				{
					
					
					GraphedgesList[x]->value = INF;
				}
				else{
					GraphedgesList[x]->value = 40;
				}

			}
			else/* 相同PE（不同层次） */
			{
				if(II != 1)/* 如果II！=1,自己到自己更好 */
				{
					/* II层下的 */
					if(firstR % R->RnodesNums == secondR % R->RnodesNums)
					{
						GraphedgesList[x]->value = 2;
					}
					else
					{
						GraphedgesList[x]->value = 5;
					}
					

				}
				else{
					GraphedgesList[x]->value = 5;
				}
			}

		}





		else if(R->TERnodesList[firstR]->RegisterKind == 1 && R->TERnodesList[secondR]->RegisterKind == 1)/* LD->LD */
		{
			GraphedgesList[x]->value = 20;
		}
		
		else if(R->TERnodesList[firstR]->RegisterKind == 1 && R->TERnodesList[secondR]->RegisterKind == 0)/* LD->PE */
		{
			// cout<<"R->TERnodesList[secondR]->inPort="<<R->TERnodesList[secondR]->inPort<<endl;
			if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
			{
				// cout<<"firstR="<<firstR<<"  "<<"secondR="<<secondR<<endl;
				GraphedgesList[x]->value = INF;
			}
			else{
				GraphedgesList[x]->value = 4;
			}

			
		}
		else if(R->TERnodesList[firstR]->RegisterKind == 1 && R->TERnodesList[secondR]->RegisterKind == 2)/* LD->SU */
		{
			/* ld-su，一般是切的，如果latency较少，最好在PE内去流动 */
			GraphedgesList[x]->value = 100;
		}


		else if(R->TERnodesList[firstR]->RegisterKind == 0 && R->TERnodesList[secondR]->RegisterKind == 4)/*pe->FU */
		{
			if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
			{
				// cout<<"firstR="<<firstR<<"  "<<"secondR="<<secondR<<endl;
				GraphedgesList[x]->value = INF;
			}
			else
			{
				GraphedgesList[x]->value = 50;

			}

		}
		else if(R->TERnodesList[firstR]->RegisterKind == 0 && R->TERnodesList[secondR]->RegisterKind == 2)/*pe->SU*/
		{
			GraphedgesList[x]->value = 100;
		}
		else if(R->TERnodesList[firstR]->RegisterKind == 2 && R->TERnodesList[secondR]->RegisterKind == 3)/*SU->BANK*/
		{
			if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
			{
				// cout<<"firstR="<<firstR<<"  "<<"secondR="<<secondR<<endl;
				GraphedgesList[x]->value = INF;
			}
			else
			{
				GraphedgesList[x]->value = 100;
			}
		}
		else if(R->TERnodesList[firstR]->RegisterKind == 5 && R->TERnodesList[secondR]->RegisterKind == 4)/* Result->FU */
		{
			if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
			{
				// cout<<"firstR="<<firstR<<"  "<<"secondR="<<secondR<<endl;
				GraphedgesList[x]->value = INF;
			}
			else
			{
				GraphedgesList[x]->value = 50;

			}
		}

		else if(R->TERnodesList[firstR]->RegisterKind == 4 && R->TERnodesList[secondR]->RegisterKind == 5)/* Result->FU */
		{
			if(R->TERnodesList[secondR]->inPort == true)/* 末端的输入端口被占 */
			{
				// cout<<"firstR="<<firstR<<"  "<<"secondR="<<secondR<<endl;
				GraphedgesList[x]->value = INF;
			}
			else
			{
				GraphedgesList[x]->value = 50;

			}

		}



		else
		{
			GraphedgesList[x]->value = 5;
		}

		/* 根据时间扩展的R实时调整权值 */

		// if(R->TERnodesList[firstR]->isOccupied == true)
		// {
		// 	// cout<<"firstR="<<firstR<<endl;
		// 	/* 将以edge->pre为源点的 边设置为 无穷大 */
		// 	// this->setEdgeValueFromPre(preNode,INF);
		// 	// /* 将以edge->pre为目标点的 边设置为 无穷大 */
		// 	// this->setEdgeValueToPos(preNode,INF);
		// }
		// if(R->TERnodesList[secondR]->isOccupied == true)
		// {
		// 	// cout<<"secondR="<<secondR<<endl;
		// 	/* 将以edge->pos为源点的 边设置为 无穷大 */
		// 	// this->setEdgeValueFromPre(posNode,INF);
		// 	// /* 将以edge->pos为目标点的 边设置为 无穷大 */
		// 	// this->setEdgeValueToPos(posNode,INF);
		// }
		// cout<<GraphedgesList[x]->GEdgeId<<" "<<GraphedgesList[x]->pre<<" "<<GraphedgesList[x]->pos<<" "<<GraphedgesList[x]->value<<endl;
	}

}




