#include "CGRA.h"

CGRAnode::CGRAnode()
{

}

CGRA::~CGRA()
{
	for (auto node : CGRAnodesList)
	{
		delete node;
	}
}


CGRA::CGRA(int CGRARow, int CGRAColumn)
{   
	int column = CGRAColumn + 6; /* 2bank,2LU,2SU */
    ElmNum = CGRARow *column;//PEA的总数，包括bank和LSU单元	
	
    int i, j, k;           
    for( j = 0; j < CGRARow; j++)
	{
      	for(i = 0; i < column;i++)
		{	
           	CGRAnode* node = new CGRAnode();
			int ElmCurrent = i + j * column;
			node->ElmID = ElmCurrent;/* ElmID从0开始编号 */
			int ElmN = i + ((j - 1 + CGRARow) % CGRARow ) * column;
			int ElmS = i + ((j + 1) % CGRARow) * column;
			int ElmW = ((i - 1) + column) % column + j * column;
			int ElmE = (i + 1) % column + j * column;

			if(i == 0 || i == 1 )//Bank
			{
				
				for(int m = 0; m < 4; m++)
				{
					node->ElmNeighbors.push_back(m * 10 + 2);  
					node->ElmNeighbors.push_back(m * 10 + 4);  
					
				}
				
       		 	node->ElmKind = 3;
			}
			else if(i == 2 || i == 4)//LU
			{
        		node->ElmKind = 1;
				node->ElmNeighbors.push_back(ElmE); /* 连接su */
				if(i == 2)/* 第1列LU */
				{
					node->ElmNeighbors.push_back(ElmCurrent+ 4 ); /* 只连第1列PE */
				}
				else/* 第2列LU */
				{
					node->ElmNeighbors.push_back(ElmCurrent + 3 ); /* 只连第2列E */
				}

			}
			else if(i == 3 || i == 5)//SU
			{
        		node->ElmKind = 2;
				for(int m = 0; m < 4; m++)
				{
					node->ElmNeighbors.push_back(m * 10 );  
					node->ElmNeighbors.push_back(m * 10 + 1);  
					
				}
			}
			else//PE
			{
        		node->ElmKind = 0;
				
				if(j == 0 || j == CGRARow-1)/* 第一行和最后一行 */
				{
					if(i == 6)/* 第一列的PE */
					{
						node->ElmNeighbors.push_back(ElmE); 
						node->ElmNeighbors.push_back(ElmCurrent- 3);/* 只连第一列su */
					}
					else if(i == column - 1)/* 最后一列的PE */
						node->ElmNeighbors.push_back(ElmW);  
					else 
					{
						if(i == 7)/* 第2列的PE */
						{
							node->ElmNeighbors.push_back(ElmCurrent - 2);/* 只连第2列的su */
						}
						node->ElmNeighbors.push_back(ElmW); 
						node->ElmNeighbors.push_back(ElmE); 
					}
					if(j == 0)/* 第1行PE */
					{
						node->ElmNeighbors.push_back(ElmS);

					}
					else if(j == CGRARow-1)/* 最后一行PE */
						node->ElmNeighbors.push_back(ElmN);
				}
				else/* 中间行 */
				{
					node->ElmNeighbors.push_back(ElmN); 
					node->ElmNeighbors.push_back(ElmS); 
					

					if(i == 6)/* PE的第一列 */
					{
						node->ElmNeighbors.push_back(ElmE); 
						node->ElmNeighbors.push_back(ElmCurrent- 3);/* 只连第一列su */
						 

					}
					else if(i == column - 1)
						node->ElmNeighbors.push_back(ElmW);  
					else
					{
						if(i == 7 )/* 第2列PE */
						{
							node->ElmNeighbors.push_back(ElmCurrent - 2);/* 只连第2列的su */
						}
						node->ElmNeighbors.push_back(ElmW);  
						node->ElmNeighbors.push_back(ElmE);  
					}
				}
				


			}

			CGRAnodesList.push_back(node);  
		}
	}
	// cout<<"ElmNum"<<ElmNum<<endl;
	// for(int Z = 0; Z < ElmNum;Z++)
	// {
	// 	cout<<CGRAnodesList[Z]->ElmID<<endl;
	// 	int num = CGRAnodesList[Z]->ElmNeighbors.size();
	// 	cout<<"neighbor:";
	// 	for(int y = 0; y<num;y++)
	// 	{
	// 		cout<< CGRAnodesList[Z]->ElmNeighbors[y]<<" ";

	// 	}
	// 	cout<<endl;

	// }


	
}

