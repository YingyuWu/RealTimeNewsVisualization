#include "Hungarian.h"

Hungarian::Hungarian(std::vector<ArticleCluster *> source_vector, std::vector<ArticleCluster *> target_vector, int OldVersion)
{

	int source_size = source_vector.size();
	int target_size = target_vector.size();

	int size1 = source_size;
	int size2 = target_size;

	int INF = 0x7FFFFFFF;

	bool verbose = true;
	bool IsInverted = false;

	if (size1 > size2){
		int temp = size1;
		size1 = size2;
		size2 = temp;

		IsInverted = true;
	}

	// This means size1 <= size2;

	int** Array = new int *[size1];
	for (int i=0; i<size1; ++i){
		Array[i] = new int [size2];
	}
	bool** Result = new bool *[size1];
	for (int i=0; i<size1; ++i){
		Result[i] = new bool [size2];
		memset (Result[i], 0x00, sizeof(bool));
	}

	if (IsInverted){
		for (int i=0;i<size1;++i){
			for (int j=0;j<size2;++j){
				Array[i][j]=-1 * source_vector[j]->m_v_parent[i];
			}
		}
	}
	else{
		for (int i=0;i<size1;++i){
			for (int j=0;j<size2;++j){
				Array[i][j]=-1 * source_vector[i]->m_v_parent[j];
			}
		}
	}


	int i,j;
	//int false=0,true=1;

	unsigned int m=size1,n=size2;
	int k;
	int l;
	int s;

	//int col_mate[size1]={0};
	int *col_mate = new int [size1];
	memset (col_mate, 0x00, size1*sizeof(int));

	//int row_mate[size2]={0};
	int *row_mate = new int [size2];
	memset (row_mate, 0x00, size2*sizeof(int));

	//int parent_row[size2]={0};
	int *parent_row = new int [size2];
	memset (parent_row, 0x00, size2*sizeof(int));

	//int unchosen_row[size1]={0};
	int *unchosen_row = new int [size1];
	memset (unchosen_row, 0x00, size1*sizeof(int));

	int t;
	int q;
	//int row_dec[size1]={0};
	int *row_dec = new int [size1];
	memset (row_dec, 0x00, size1*sizeof(int));

	//int col_inc[size2]={0};
	int *col_inc = new int [size2];
	memset (col_inc, 0x00, size2*sizeof(int));

	//int slack[size2]={0};
	int *slack = new int [size2];
	memset (slack, 0x00, size2*sizeof(int));

	//int slack_row[size2]={0};
	int *slack_row = new int [size2];
	memset (slack_row, 0x00, size2*sizeof(int));

	int unmatched;
	int cost=0;

	for (i=0;i<size1;++i)
		for (j=0;j<size2;++j)
			Result[i][j]=false;

	// Begin subtract column minima in order to start with lots of zeroes 12
	//printf("Using heuristic\n");
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
				//if (verbose)
				//	printf("matching col %d==row %d\n",l,k);
				goto row_done;
			}
			col_mate[k]= -1;
			//if (verbose)
			//	printf("node %d: unmatched row %d\n",t,k);
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
		//if (verbose)
		//	printf("Matched %d rows.\n",m-t);
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
									//if (verbose)
									//	printf("node %d: row %d==col %d--row %d\n",
									//	t,row_mate[l],l,k);
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
						//if (verbose)
						//	printf(
						//	"Decreasing uncovered elements by %d produces zero at [%d,%d]\n",
						//	s,k,l);
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
							//if (verbose)
							//	printf("node %d: row %d==col %d--row %d\n",t,row_mate[l],l,k);
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
		//if (verbose)
		//	printf("Breakthrough at node %d of %d!\n",q,t);
		while (1)
		{
			j=col_mate[k];
			col_mate[k]=l;
			row_mate[l]=k;
			//if (verbose)
			//	printf("rematching col %d==row %d\n",l,k);
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
				//if (verbose)
				//	printf("node %d: unmatched row %d\n",t,k);
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
	//printf("Cost is %d\n",cost);

	for (int y=0;y<size1;++y){
		for (int x=0;x<size2;++x){
			if (Result[y][x]){
				if (IsInverted){
					//printf("Inverted New %d is from old %d\n",x,y);
					source_vector[x]->m_matched_id_from_parent = y;//target_vector[y]->m_matched_id_from_parent;
					source_vector[x]->m_cluster_id = target_vector[y]->m_cluster_id;
					source_vector[x]->m_IsNewCluster = false;
				}
				else{
					//printf("New %d is from old %d\n",y,x);
					source_vector[y]->m_matched_id_from_parent = x;//target_vector[x]->m_matched_id_from_parent;;
					source_vector[y]->m_cluster_id = target_vector[x]->m_cluster_id;
					source_vector[y]->m_IsNewCluster = false;
				}
			}
		}
	}

	if (IsInverted){
		std::vector<bool> check_cluster_index;
		for (int i=0; i<size2; ++i){
			check_cluster_index.push_back(false);
		}

		for (int i=0; i<size2; ++i){
			if (source_vector[i]->m_cluster_id >= 0 && source_vector[i]->m_cluster_id < size2){
				check_cluster_index[source_vector[i]->m_cluster_id] = true;
			}	
		}

		for (int i=0, j=0; i<size2; ++i){
			if (source_vector[i]->m_IsNewCluster){
				for(; j<size2; ++j){
					if (!check_cluster_index[j]){
						check_cluster_index[j] = true;
						source_vector[i]->m_cluster_id = j;
						break;
					}
				}
			}
		}
	}

	for (int i=0; i<source_vector.size(); ++i){
		if (source_vector[i]->purityFromParent() < 0.3){
			source_vector[i]->m_IsNewCluster = true;
		}
	}

	int stop = 0;
}

Hungarian::Hungarian(std::vector<ArticleCluster *> source_vector, std::vector<ArticleCluster *> target_vector)
{

	int source_size = source_vector.size();
	int target_size = target_vector.size();

	int size1 = source_size;
	int size2 = target_size;

	int INF = 0x7FFFFFFF;

	bool verbose = true;
	bool IsInverted = false;

	if (size1 > size2){
		size2 = size1;
		IsInverted = true;
	}
	else {
		size1 = size2;
	}

	// This means size1 <= size2;

	int** Array = new int *[size1];
	for (int i=0; i<size1; ++i){
		Array[i] = new int [size2];
	}
	bool** Result = new bool *[size1];
	for (int i=0; i<size1; ++i){
		Result[i] = new bool [size2];
		memset (Result[i], 0x00, sizeof(bool));
	}

	for (int i=0;i<size1;++i){
		for (int j=0;j<size2;++j){
			if (i<source_size && j<target_size){
				Array[i][j] = -1 * source_vector[i]->m_v_parent[j];
			}
			else{
				Array[i][j] = 0;
			}
		}
	}


	int i,j;
	//int false=0,true=1;

	unsigned int m=size1,n=size2;
	int k;
	int l;
	int s;

	//int col_mate[size1]={0};
	int *col_mate = new int [size1];
	memset (col_mate, 0x00, size1*sizeof(int));

	//int row_mate[size2]={0};
	int *row_mate = new int [size2];
	memset (row_mate, 0x00, size2*sizeof(int));

	//int parent_row[size2]={0};
	int *parent_row = new int [size2];
	memset (parent_row, 0x00, size2*sizeof(int));

	//int unchosen_row[size1]={0};
	int *unchosen_row = new int [size1];
	memset (unchosen_row, 0x00, size1*sizeof(int));

	int t;
	int q;
	//int row_dec[size1]={0};
	int *row_dec = new int [size1];
	memset (row_dec, 0x00, size1*sizeof(int));

	//int col_inc[size2]={0};
	int *col_inc = new int [size2];
	memset (col_inc, 0x00, size2*sizeof(int));

	//int slack[size2]={0};
	int *slack = new int [size2];
	memset (slack, 0x00, size2*sizeof(int));

	//int slack_row[size2]={0};
	int *slack_row = new int [size2];
	memset (slack_row, 0x00, size2*sizeof(int));

	int unmatched;
	int cost=0;

	for (i=0;i<size1;++i)
		for (j=0;j<size2;++j)
			Result[i][j]=false;

	// Begin subtract column minima in order to start with lots of zeroes 12
	//printf("Using heuristic\n");
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
				//if (verbose)
				//	printf("matching col %d==row %d\n",l,k);
				goto row_done;
			}
			col_mate[k]= -1;
			//if (verbose)
			//	printf("node %d: unmatched row %d\n",t,k);
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
		//if (verbose)
		//	printf("Matched %d rows.\n",m-t);
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
									//if (verbose)
									//	printf("node %d: row %d==col %d--row %d\n",
									//	t,row_mate[l],l,k);
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
						//if (verbose)
						//	printf(
						//	"Decreasing uncovered elements by %d produces zero at [%d,%d]\n",
						//	s,k,l);
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
							//if (verbose)
							//	printf("node %d: row %d==col %d--row %d\n",t,row_mate[l],l,k);
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
		//if (verbose)
		//	printf("Breakthrough at node %d of %d!\n",q,t);
		while (1)
		{
			j=col_mate[k];
			col_mate[k]=l;
			row_mate[l]=k;
			//if (verbose)
			//	printf("rematching col %d==row %d\n",l,k);
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
				//if (verbose)
				//	printf("node %d: unmatched row %d\n",t,k);
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
	//printf("Cost is %d\n",cost);
	
	// Sort Done
	// ----------------------------------------------------------------------------

	for (int y=0;y<size1;++y){
		for (int x=0;x<size2;++x){
			if (Result[y][x] & y<source_size && x<target_size){
				source_vector[y]->m_matched_id_from_parent = x;//target_vector[x]->m_matched_id_from_parent;;
				source_vector[y]->m_cluster_id = target_vector[x]->m_cluster_id;
				source_vector[y]->m_IsNewCluster = false;
			}
		}
	}

	if (IsInverted){
		std::vector<bool> check_cluster_index;
		for (int i=0; i<size2; ++i){
			check_cluster_index.push_back(false);
		}

		for (int i=0; i<size2; ++i){
			if (!source_vector[i]->m_IsNewCluster && source_vector[i]->m_cluster_id < source_size){
				check_cluster_index[source_vector[i]->m_cluster_id] = true;
			}	
		}

		for (int i=0, j=0; i<size2; ++i){
			if (source_vector[i]->m_IsNewCluster){
				for(; j<size2; ++j){
					if (!check_cluster_index[j]){
						check_cluster_index[j] = true;
						source_vector[i]->m_cluster_id = j;
						break;
					}
				}
			}
		}
	}

	for (int i=0; i<source_vector.size(); ++i){
		if (source_vector[i]->purityFromParent() < 0.15){
			source_vector[i]->m_IsNewCluster = true;
		}
	}
}

Hungarian::~Hungarian(void)
{
}

