//
//(Minimum) Assignment Problem by Hungarian Algorithm
//taken from Knuth's Stanford Graphbase
//
#include <stdio.h>
#include <iostream>

#define INF (0x7FFFFFFF)

#define size1 (7)
#define size2 (7)

#define verbose (1)

int Array[size1][size2];
bool Result[size1][size2];  // used as boolean

void initArray()
{
	int i,j;

	for (i=0;i<size1;++i)
		for (j=0;j<size2;++j)
			Array[i][j]=99999999;
}

void hungarian()
{
	int i,j;
	//int false=0,true=1;

	unsigned int m=size1,n=size2;
	int k;
	int l;
	int s;
	int col_mate[size1]={0};
	int row_mate[size2]={0};
	int parent_row[size2]={0};
	int unchosen_row[size1]={0};
	int t;
	int q;
	int row_dec[size1]={0};
	int col_inc[size2]={0};
	int slack[size2]={0};
	int slack_row[size2]={0};
	int unmatched;
	int cost=0;

	for (i=0;i<size1;++i)
		for (j=0;j<size2;++j)
			Result[i][j]=false;

	// Begin subtract column minima in order to start with lots of zeroes 12
	printf("Using heuristic\n");
	for (l=0;l<n;l++)
	{
		s=Array[0][l];
		for (k=1;k<m;k++)
			if (Array[k][l]<s)
				s=Array[k][l];
		cost+=s;
		if (s!=0)
			for (k=0;k<m;k++)
				Array[k][l]-=s;
	}
	// End subtract column minima in order to start with lots of zeroes 12

	// Begin initial state 16
	t=0;
	for (l=0;l<n;l++)
	{
		row_mate[l]= -1;
		parent_row[l]= -1;
		col_inc[l]=0;
		slack[l]=INF;
	}
	for (k=0;k<m;k++)
	{
		s=Array[k][0];
		for (l=1;l<n;l++)
			if (Array[k][l]<s)
				s=Array[k][l];
		row_dec[k]=s;
		for (l=0;l<n;l++)
			if (s==Array[k][l] && row_mate[l]<0)
			{
				col_mate[k]=l;
				row_mate[l]=k;
				if (verbose)
					printf("matching col %d==row %d\n",l,k);
				goto row_done;
			}
			col_mate[k]= -1;
			if (verbose)
				printf("node %d: unmatched row %d\n",t,k);
			unchosen_row[t++]=k;
row_done:
			;
	}
	// End initial state 16

	// Begin Hungarian algorithm 18
	if (t==0)
		goto done;
	unmatched=t;
	while (1)
	{
		if (verbose)
			printf("Matched %d rows.\n",m-t);
		q=0;
		while (1)
		{
			while (q<t)
			{
				// Begin explore node q of the forest 19
				{
					k=unchosen_row[q];
					s=row_dec[k];
					for (l=0;l<n;l++)
						if (slack[l])
						{
							int del;
							del=Array[k][l]-s+col_inc[l];
							if (del<slack[l])
							{
								if (del==0)
								{
									if (row_mate[l]<0)
										goto breakthru;
									slack[l]=0;
									parent_row[l]=k;
									if (verbose)
										printf("node %d: row %d==col %d--row %d\n",
										t,row_mate[l],l,k);
									unchosen_row[t++]=row_mate[l];
								}
								else
								{
									slack[l]=del;
									slack_row[l]=k;
								}
							}
						}
				}
				// End explore node q of the forest 19
				q++;
			}

			// Begin introduce a new zero into the matrix 21
			s=INF;
			for (l=0;l<n;l++)
				if (slack[l] && slack[l]<s)
					s=slack[l];
			for (q=0;q<t;q++)
				row_dec[unchosen_row[q]]+=s;
			for (l=0;l<n;l++)
				if (slack[l])
				{
					slack[l]-=s;
					if (slack[l]==0)
					{
						// Begin look at a new zero 22
						k=slack_row[l];
						if (verbose)
							printf(
							"Decreasing uncovered elements by %d produces zero at [%d,%d]\n",
							s,k,l);
						if (row_mate[l]<0)
						{
							for (j=l+1;j<n;j++)
								if (slack[j]==0)
									col_inc[j]+=s;
							goto breakthru;
						}
						else
						{
							parent_row[l]=k;
							if (verbose)
								printf("node %d: row %d==col %d--row %d\n",t,row_mate[l],l,k);
							unchosen_row[t++]=row_mate[l];
						}
						// End look at a new zero 22
					}
				}
				else
					col_inc[l]+=s;
			// End introduce a new zero into the matrix 21
		}
breakthru:
		// Begin update the matching 20
		if (verbose)
			printf("Breakthrough at node %d of %d!\n",q,t);
		while (1)
		{
			j=col_mate[k];
			col_mate[k]=l;
			row_mate[l]=k;
			if (verbose)
				printf("rematching col %d==row %d\n",l,k);
			if (j<0)
				break;
			k=parent_row[j];
			l=j;
		}
		// End update the matching 20
		if (--unmatched==0)
			goto done;
		// Begin get ready for another stage 17
		t=0;
		for (l=0;l<n;l++)
		{
			parent_row[l]= -1;
			slack[l]=INF;
		}
		for (k=0;k<m;k++)
			if (col_mate[k]<0)
			{
				if (verbose)
					printf("node %d: unmatched row %d\n",t,k);
				unchosen_row[t++]=k;
			}
			// End get ready for another stage 17
	}
done:

	// Begin doublecheck the solution 23
	for (k=0;k<m;k++)
		for (l=0;l<n;l++)
			if (Array[k][l]<row_dec[k]-col_inc[l])
				exit(0);
	for (k=0;k<m;k++)
	{
		l=col_mate[k];
		if (l<0 || Array[k][l]!=row_dec[k]-col_inc[l])
			exit(0);
	}
	k=0;
	for (l=0;l<n;l++)
		if (col_inc[l])
			k++;
	if (k>m)
		exit(0);
	// End doublecheck the solution 23
	// End Hungarian algorithm 18

	for (i=0;i<m;++i)
	{
		Result[i][col_mate[i]]=true;
		/*TRACE("%d - %d\n", i, col_mate[i]);*/
	}
	for (k=0;k<m;++k)
	{
		for (l=0;l<n;++l)
		{
			/*TRACE("%d ",Array[k][l]-row_dec[k]+col_inc[l]);*/
			Array[k][l]=Array[k][l]-row_dec[k]+col_inc[l];
		}
		/*TRACE("\n");*/
	}
	for (i=0;i<m;i++)
		cost+=row_dec[i];
	for (i=0;i<n;i++)
		cost-=col_inc[i];
	printf("Cost is %d\n",cost);
}

void main()
{
	int y,x,i;

	initArray();
	//
	for (y=0;y<size1;++y)
		for (x=0;x<size2;++x)
			Array[y][x]=0;

	Array[0][0]=-59;
	Array[0][1]=0;
	Array[0][2]=0;
	Array[0][3]=0;
	Array[0][4]=0;
	Array[0][5]=0;

	Array[1][0]=0;
	Array[1][1]=-54;
	Array[1][2]=0;
	Array[1][3]=0;
	Array[1][4]=0;
	Array[1][5]=0;

	Array[2][0]=0;
	Array[2][1]=0;
	Array[2][2]=-24;
	Array[2][3]=0;
	Array[2][4]=0;
	Array[2][5]=0;

	Array[3][0]=0;
	Array[3][1]=0;
	Array[3][2]=0;
	Array[3][3]=-19;
	Array[3][4]=0;
	Array[3][5]=0;

	Array[4][0]=0;
	Array[4][1]=0;
	Array[4][2]=0;
	Array[4][3]=0;
	Array[4][4]=-19;
	Array[4][5]=0;

	Array[5][0]=0;
	Array[5][1]=0;
	Array[5][2]=0;
	Array[5][3]=0;
	Array[5][4]=0;
	Array[5][5]=-1;

	Array[6][0]=0;
	Array[6][1]=0;
	Array[6][2]=0;
	Array[6][3]=0;
	Array[6][4]=0;
	Array[6][5]=-10;

	//
	/* Instance from Papadimitriou & Steiglitz, size1=size2=5
	Array[0][0]=7;
	Array[0][1]=2;
	Array[0][2]=1;
	Array[0][3]=9;
	Array[0][4]=4;
	Array[1][0]=9;
	Array[1][1]=6;
	Array[1][2]=9;
	Array[1][3]=5;
	Array[1][4]=5;
	Array[2][0]=3;
	Array[2][1]=8;
	Array[2][2]=3;
	Array[2][3]=1;
	Array[2][4]=8;
	Array[3][0]=7;
	Array[3][1]=9;
	Array[3][2]=4;
	Array[3][3]=2;
	Array[3][4]=2;
	Array[4][0]=8;
	Array[4][1]=4;
	Array[4][2]=7;
	Array[4][3]=4;
	Array[4][4]=8;
	*/

	hungarian();

	for (y=0;y<size1;++y)
		for (x=0;x<size2;++x)
			if (Result[y][x])
				printf("Old %d and New %d are connected in the assignment\n",y,x);

	system("pause");
}