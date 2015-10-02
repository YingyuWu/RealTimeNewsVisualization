#include "ArticleCluster.h"

ArticleCluster::ArticleCluster(size_t clusterID, std::vector<size_t> v_index, std::vector<Article *> v_articles, std::vector<int> currentTime)
{
	m_cluster_id = clusterID;
	m_cluster_size = v_index.size();
	m_IsNewCluster = true;
	m_matched_id_from_parent = 0x0fffff;

	m_v_index = v_index;
	m_currentTime = currentTime;

	m_newest_index = 0;
	m_oldest_index = 99999999;
	
	for (size_t i=0; i<v_index.size(); ++i){
		m_v_cluster_articles.push_back(v_articles[v_index[i]]);

		if (m_newest_index < m_v_index[i]){
			m_newest_index = m_v_index[i];
		}

		if (m_oldest_index > m_v_index[i]){
			m_oldest_index = m_v_index[i];
		}
	}

	m_cluster_newest_article = v_articles[m_newest_index];
	m_cluster_oldest_article = v_articles[m_oldest_index];

	generateKeywordsVector();
	m_cluster_center_article = generateCenterArticle();

	rankArticlesByPageRank();
	
	m_cluster_create_time_string = "None";
	int stop = 0;
}

ArticleCluster::ArticleCluster(const ArticleCluster *source)
{
	m_cluster_id = source->m_cluster_id;
	m_IsNewCluster = source->m_IsNewCluster;
	m_v_index = source->m_v_index;
	m_v_cluster_articles = source->m_v_cluster_articles;
	m_cluster_center_article = source->m_cluster_center_article;
	m_cluster_newest_article = source->m_cluster_newest_article;
	m_cluster_oldest_article = source->m_cluster_oldest_article;
	m_cluster_size = source->m_cluster_size;
	m_matched_id_from_parent = source->m_matched_id_from_parent;
	m_currentTime = source->m_currentTime;
	m_newest_index = source->m_newest_index;
	m_oldest_index = source->m_oldest_index;

	for (size_t i=0; i<source->m_v_cluster_keywords.size(); ++i){
		Keyword *newItem = new Keyword(source->m_v_cluster_keywords[i]);
		m_v_cluster_keywords.push_back(newItem);
	}

	m_cluster_create_time_string = source->m_cluster_create_time_string;
}

ArticleCluster::~ArticleCluster(void)
{
	for (size_t i=0; i<m_v_cluster_keywords.size(); ++i){
		delete m_v_cluster_keywords[i];
		m_v_cluster_keywords[i] = NULL;
	}
	m_v_cluster_keywords.clear();
}

bool sortKeywordsFunction_ArticleCluster(Keyword *i, Keyword *j){
	return i->occurrence > j->occurrence;
}

bool ArticleCluster::generateKeywordsVector()
{
	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		Article *tempArticle = m_v_cluster_articles[i];

		for (size_t k=0; k<tempArticle->getKeywordsNumber(); ++k){
			Keyword *newTempAticleKeyword = tempArticle->getKeywords()[k];
			bool isNotFound = true;
			for (size_t l=0; l<m_v_cluster_keywords.size(); ++l){
				if (newTempAticleKeyword->value == m_v_cluster_keywords[l]->value){
					m_v_cluster_keywords[l]->occurrence += 1;
					isNotFound = false;
				}
			}

			if (isNotFound){
				Keyword *newActiveKeyword = new Keyword(*newTempAticleKeyword);
				newActiveKeyword->occurrence = 1;
				m_v_cluster_keywords.push_back(newActiveKeyword);
			}
		}
	}

	std::sort (m_v_cluster_keywords.begin(), m_v_cluster_keywords.end(), sortKeywordsFunction_ArticleCluster);

	return true;
}

Article *ArticleCluster::generateCenterArticle()
{
	double x = 0.0;
	double y = 0.0;

	double temp_x = 0.0;
	double temp_y = 0.0;

	double minValue = 2;
	double tempMinValue = 0.0;

	Article *result = NULL;

	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		x += m_v_cluster_articles[i]->getPosition()[0];
		y += m_v_cluster_articles[i]->getPosition()[1];
	}

	x /= m_v_cluster_articles.size();
	y /= m_v_cluster_articles.size();

	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		temp_x = m_v_cluster_articles[i]->getPosition()[0];
		temp_y = m_v_cluster_articles[i]->getPosition()[1];

		tempMinValue = (x-temp_x)*(x-temp_x) + (y-temp_y)*(y-temp_y);

		if (tempMinValue < minValue){
			minValue = tempMinValue;
			result = m_v_cluster_articles[i];
		}
	}

	return result;
}

double ArticleCluster::purityFromParent()
{
	if (m_matched_id_from_parent == 0x0fffff){
		return 0.0;
	}
	else{
		return (double)m_v_parent[m_matched_id_from_parent] / (double)m_cluster_size;
	}
}

double ArticleCluster::generateSimilarity(Article *target, Article *source)
{
	double result = 0.0;
	double result_total = 0.0;
	std::vector<Keyword *> targetKeywords = target->getKeywords();
	std::vector<Keyword *> sourceKeywords = source->getKeywords();

	if (targetKeywords.size() == 0 || sourceKeywords.size() == 0) return 0.0;

	if (targetKeywords.size() >= sourceKeywords.size()){
		result_total = targetKeywords.size();
	}
	else{
		result_total = sourceKeywords.size();
	}

	for (size_t i=0; i<targetKeywords.size(); ++i){
		for (size_t j=0; j<sourceKeywords.size(); ++j){
			if (targetKeywords[i]->value == sourceKeywords[j]->value){
				result += 1;
			}
		}
	}
	
	return result / result_total;
}

bool pageRankSortFunction(Article *a, Article *b){
	return a->m_pagerank_probability > b->m_pagerank_probability;
}

void ArticleCluster::rankArticlesByPageRank()
{
	std::vector<vector<double>> sim;

	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		std::vector<double> row;
		for (size_t j=0; j<m_v_cluster_articles.size(); ++j){
			if (i == j){
				row.push_back(1);
			}
			else{
				row.push_back(generateSimilarity(m_v_cluster_articles[i], m_v_cluster_articles[j]));
			}
		}

		sim.push_back(row);
	}

	std::vector<int> linkNumber;
	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		linkNumber.push_back(1);
	}

	bool doIteration = true;
	int iterator_time = 0;

	float deviation = 0.0;
	float maximal_deviation = 0.0;
	float iterator_threshold = 0.001;
	float d = 0.85;
	float simvalue = 0.0;
	float simthreshold = 0.1;
	size_t nodenumber = m_v_cluster_articles.size();

	while (doIteration){
		maximal_deviation = 0.0;
		iterator_time += 1;

		for (size_t i=0; i<nodenumber; ++i){
			if (iterator_time == 1){
				m_v_cluster_articles[i]->m_pagerank_probability = 1/(double)nodenumber;
				m_v_cluster_articles[i]->m_pagerank_previous_probability = 0;
			}
			else{
				m_v_cluster_articles[i]->m_pagerank_probability = (1-d)/(double)nodenumber;

				for (size_t j=0; j<nodenumber; ++j){
					simvalue = sim[i][j];

					if (simvalue >= simthreshold){
						m_v_cluster_articles[i]->m_pagerank_probability 
							+= d*m_v_cluster_articles[j]->m_pagerank_probability/linkNumber[j];
						
						if (iterator_time == 2){
							linkNumber[i] = linkNumber[i] + 1;
						}
					}
					else{
						continue;
					}
				}
			}

			deviation = m_v_cluster_articles[i]->m_pagerank_previous_probability - m_v_cluster_articles[i]->m_pagerank_probability;
			m_v_cluster_articles[i]->m_pagerank_previous_probability = m_v_cluster_articles[i]->m_pagerank_probability;

			if (deviation < 0) {
				deviation *= -1;
			}

			if (maximal_deviation < deviation){
				maximal_deviation = deviation;
			}
		}

		if (maximal_deviation < iterator_threshold){
			doIteration = false;
		}	
	}

	sort(m_v_cluster_articles.begin(), m_v_cluster_articles.end(), pageRankSortFunction);

	int stop = 0;
}

bool sortEdgeFunctionForSubCluster(edge i, edge j) // descending
{
	return i.getLength() > j.getLength();
}

bool sortClustersFunctionForSubCluster(std::vector<size_t> i, std::vector<size_t> j){
	return i.size() > j.size();
}

std::vector<ArticleCluster *> ArticleCluster::generateSubCluster()
{
	std::vector<ArticleCluster *> result;
	std::vector<std::vector<size_t>> result_index;

	Delaunay *sub_trangulation = new Delaunay();
	vertexSet sub_vertexset;
	triangleSet sub_trangulaset;
	edgeSet sub_edgeset;
	std::vector<edge> sub_edgevector;

	double threshold = 0.62;

	std::vector<bool> checkInCluster;

	for (size_t i=0; i<m_v_cluster_articles.size(); ++i){
		vertex newVertex(m_v_cluster_articles[i]->getPosition()[0], m_v_cluster_articles[i]->getPosition()[1], i);
		sub_vertexset.insert(newVertex);
		checkInCluster.push_back(false);
	}

	sub_trangulation->Triangulate(sub_vertexset, sub_trangulaset);
	sub_trangulation->TrianglesToEdges(sub_trangulaset, sub_edgeset);

	for (edgeSet::iterator iter=sub_edgeset.begin(); iter!=sub_edgeset.end(); iter++){
		sub_edgevector.push_back(*iter);
	}

	sort(sub_edgevector.begin(), sub_edgevector.end(), sortEdgeFunctionForSubCluster);
	sub_edgevector.erase(sub_edgevector.begin(), sub_edgevector.begin() + (int)(sub_edgevector.size()*threshold));
	
	std::vector<size_t> one_cluster;

	while (sub_edgevector.size() != 0){	
		edge startEdge = sub_edgevector[0];

		sub_edgevector.erase(sub_edgevector.begin());

		// Begin clustering from on Edge
		// -----------------------------------------------------------------------------------

		if (!checkInCluster[startEdge.m_pV0->m_index]){
			one_cluster.push_back(startEdge.m_pV0->m_index);
			checkInCluster[startEdge.m_pV0->m_index] = true;

		}

		if (!checkInCluster[startEdge.m_pV1->m_index]){
			one_cluster.push_back(startEdge.m_pV1->m_index);
			checkInCluster[startEdge.m_pV1->m_index] = true;
		}

		if (one_cluster.size() == 0)
			continue;

		size_t index = 0;

		while (index < one_cluster.size()){
			for (size_t i=0; i<sub_edgevector.size(); ++i){
				edge itemEdge = sub_edgevector[i];

				if (itemEdge.m_pV0->m_index == one_cluster[index] && !checkInCluster[itemEdge.m_pV1->m_index]){
					checkInCluster[itemEdge.m_pV1->m_index] = true;
					one_cluster.push_back(itemEdge.m_pV1->m_index);

					sub_edgevector.erase(sub_edgevector.begin()+i);
					--i;
				}

				if (itemEdge.m_pV1->m_index == one_cluster[index] && !checkInCluster[itemEdge.m_pV0->m_index]){
					checkInCluster[itemEdge.m_pV0->m_index] = true;
					one_cluster.push_back(itemEdge.m_pV0->m_index);

					sub_edgevector.erase(sub_edgevector.begin()+i);
					--i;
				}
			}

			++index;
		}

		// -----------------------------------------------------------------------------------
		// End clustering from on Edge
		
		if (one_cluster.size() != 0){
			result_index.push_back(one_cluster);
		}

		one_cluster.clear();
	}

	sort (result_index.begin(), result_index.end(), sortClustersFunctionForSubCluster);

	//std::cout << result_index.size() << std::endl;

	size_t outputSize = 3;
	if (outputSize > result_index.size()){
		outputSize = result_index.size();
	}

	for (size_t i=0; i<outputSize; ++i){
		ArticleCluster *newItem = new ArticleCluster(i, result_index[i], m_v_cluster_articles, m_currentTime);
		result.push_back(newItem);
	}

	return result;
}

bool ArticleCluster::datetimeParser(std::string pub_date, std::vector<int> &my_date) 
{

	if (pub_date == "ERROR") return false;
	if (pub_date == "") return false;

	my_date.clear();
	// ""2013-01-04T00:00:00Z"" Format
	// -----------------------------------------------------------------------------------
	std::istringstream linestream(pub_date);
	std::string item;

	// For '"' Symbol
	// -----------------------------------------------------------------------------------
	if (pub_date[0] == '"'){
		std::getline(linestream, item, '"');
	}

	// Year
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, '-');
	my_date.push_back(atoi(item.c_str()));

	// Month
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, '-');
	my_date.push_back(atoi(item.c_str()));

	// Day
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, 'T');
	my_date.push_back(atoi(item.c_str()));

	// Hour
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, ':');
	my_date.push_back(atoi(item.c_str()));

	// Minute
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, ':');
	my_date.push_back(atoi(item.c_str()));

	// Second
	// -----------------------------------------------------------------------------------
	std::getline(linestream, item, 'Z');
	my_date.push_back(atoi(item.c_str()));

	return true;
}

std::vector<int> ArticleCluster::getCreateDateTime() 
{
	std::vector<int> my_date;
	datetimeParser(m_cluster_create_time_string, my_date);

	return my_date;
}

std::vector<int> ArticleCluster::getMostEarlyTime()
{
	std::vector<int> my_date;


	return my_date;
}
