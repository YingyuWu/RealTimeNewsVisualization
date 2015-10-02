#include "StoryTelling.h"

StoryTelling::StoryTelling(std::vector<Article *> new_v_articles, int currentIndex)
{
	for (size_t i=0; i<m_v_articles.size(); ++i){
		delete m_v_articles[i];
		m_v_articles[i] = NULL;
	}
	m_v_articles.clear();

	m_v_articles = new_v_articles;

	m_article_numbers = 0;
	m_actived_article_numbers = 0;
	m_similarity_matrix = NULL;
	m_dimension = 2;

	m_currentTime.clear();
	m_currentTime.resize(6, 0);
	memcpy(m_currentTime._Myfirst, (m_v_articles[currentIndex]->getDateTime())._Myfirst, sizeof(vector<int>) * 6);
	//
	m_top_cluster_number = 7;
	m_cluster_visualization_threshold = 5;

	// This parameter is used for selecting the top Nth frequent keywords.
	// -----------------------------------------------------------------------------------
	m_top_keyword_number = 100;

	m_slide_window_size = 400;
	m_slide_window_start_index = 0;

	m_cluster_threshold = 0.0;
	m_cluster_threshold_coefficient = 55; // delete 60% small edges.

	m_vertex_set.clear();

	my_trangulation = NULL;
}

StoryTelling::~StoryTelling()
{
	for (size_t i=0; i<m_v_articles.size(); ++i){
		delete m_v_articles[i];
		m_v_articles[i] = NULL;
	}

	m_v_articles.clear();	
}

void StoryTelling::runTo(std::string enddateString)
{
	// ""2013-12-01T00:00:00Z"" Format of "enddateString"
	// -----------------------------------------------------------------------------------
	//std::vector<int> end_date;
	m_currentTime.clear();
	m_str_currentTime = enddateString;
	datetimeParser(enddateString, m_currentTime);
	generateActiveKeywordsVector(m_currentTime);
	updateActiveKeywordsFrequency();
	generateSimilarityMatrix();
	evolveSample(0.3, 0.01, 0.0001, 0.05, 0.4);

	clusterActivedArticle();

	//outputPosition("Point_Position.csv");
	//outputClusterPosition("Cluster_Position.js");
	int stop = 0;

}

void StoryTelling::runTo(std::vector<int> end_date)
{
	// std::vector<int> end_date : Yes, Month, Day, Hour, Minute, Second 
	// -----------------------------------------------------------------------------------
	m_currentTime = end_date;
	datetimeStrParser(end_date, m_str_currentTime);
	generateActiveKeywordsVector(m_currentTime);
	updateActiveKeywordsFrequency();
	generateSimilarityMatrix();
	evolveSample(0.3, 0.01, 0.0001, 0.05, 0.4);
	clusterActivedArticle();

	//outputPosition("Point_Position.csv");
	//outputClusterPosition("Cluster_Position.js");
	int stop = 0;
}

void StoryTelling::runTo(int index)
{
	// std::vector<int> end_date : Yes, Month, Day, Hour, Minute, Second 
	// -----------------------------------------------------------------------------------
	m_str_currentTime = m_v_articles[index]->getDateTimeString();
	datetimeParser(m_str_currentTime, m_currentTime);
	generateActiveKeywordsVector(index);
	updateActiveKeywordsFrequency();
	generateSimilarityMatrix();
	evolveSample(0.3, 0.01, 0.0001, 0.05, 0.4);
	clusterActivedArticle();

	//outputPosition("Point_Position.csv");
	//outputClusterPosition("Cluster_Position.js");
	int stop = 0;
}


bool StoryTelling::runToDateByIndex(int index)
{
	if (index < 0 || index > m_v_articles.size()-1){
		std::cout << "runTo index overflow" << std::endl;
		return false;
	}
	
	//std::string tempDateTime = m_v_articles[index]->getDateTimeString();
	//if (m_str_currentTime == tempDateTime) 
	//	return false;

	//runTo(tempDateTime);

	runTo(index);

	return true;
}


void StoryTelling::setInterval(int day, int hour, int min, int second) 
{
	m_day = day;
	m_hour = hour;
	m_min = min;
	m_second = second;
}

bool StoryTelling::runToNextInterval()
{

	// std::vector<int> m_currentTime : Yes, Month, Day, Hour, Minute, Second 
	// -----------------------------------------------------------------------------------
	m_currentTime[5] += m_second;
	m_currentTime[4] += m_min;
	m_currentTime[3] += m_hour;
	m_currentTime[2] += m_day;

	if (m_currentTime[5] > 59) {
		m_currentTime[5] = m_currentTime[5] % 60;
		m_currentTime[4] += 1;
	}

	if (m_currentTime[4] > 59) {
		m_currentTime[4] = m_currentTime[4] % 60;
		m_currentTime[3] += 1;
	}

	if (m_currentTime[3] > 23) {
		m_currentTime[3] = m_currentTime[3] % 24;
		m_currentTime[2] += 1;
	}

	if (m_currentTime[1] == 1 || m_currentTime[1] == 3 || m_currentTime[1] == 5 || m_currentTime[1] == 7 || m_currentTime[1] == 8 || m_currentTime[1] == 10 || m_currentTime[1] == 12) {
		if (m_currentTime[2] > 31) {
			m_currentTime[2] = m_currentTime[2] % 31;
			m_currentTime[1] += 1;
		}
	}
	else if (m_currentTime[1] == 2) {
		if (m_currentTime[2] > 28) {
			m_currentTime[2] = m_currentTime[2] % 28;
			m_currentTime[1] += 1;
		}
	}
	else {
		if (m_currentTime[2] > 30) {
			m_currentTime[2] = m_currentTime[2] % 30;
			m_currentTime[1] += 1;
		}
	}

	if (m_currentTime[1] > 12) {
		m_currentTime[1] = m_currentTime[2] % 12;
		m_currentTime[0] += 1;
	}

	generateActiveKeywordsVector(m_currentTime);
	updateActiveKeywordsFrequency();
	generateSimilarityMatrix();
	evolveSample(0.3, 0.01, 0.0001, 0.05, 0.4);
	clusterActivedArticle();

	//outputPosition("Point_Position.csv");
	//outputClusterPosition("Cluster_Position.js");
	int stop = 0;

	return true;
}

std::vector<Article *> StoryTelling::getArticleVector()
{
	return m_v_articles;
}

size_t StoryTelling::getActivedNumber()
{
	return m_actived_article_numbers;
}

bool StoryTelling::generateActiveKeywordsVector(std::vector<int> end_date)
{
	time_t startTime;
	time(&startTime);

	for (size_t i=0; i<m_v_active_keywords.size(); ++i){
		delete m_v_active_keywords[i];
		m_v_active_keywords[i] = NULL;
	}
	m_v_active_keywords.clear();

	for (size_t i=0; i<m_v_articles.size(); ++i){
		Article *tempArticle = m_v_articles[i];

		// only the date earlier than "enddateString" are computed for the active keywords matrix.
		// -----------------------------------------------------------------------------------
		if (compareDate(tempArticle->getDateTime(), end_date)){
			m_actived_article_numbers = i;
		}
		else{
			m_actived_article_numbers = i;
			break;
		}
	}

	if (m_actived_article_numbers > m_slide_window_size) {
		m_slide_window_start_index = m_actived_article_numbers - m_slide_window_size;
		m_actived_article_numbers = m_slide_window_size;
	}

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		Article *tempArticle = m_v_articles[i+m_slide_window_start_index];

		for (size_t k=0; k<tempArticle->getKeywordsNumber(); ++k){
			Keyword *newTempAticleKeyword = tempArticle->getKeywords()[k];
			bool isNotFound = true;
			for (size_t l=0; l<m_v_active_keywords.size(); ++l){
				if (newTempAticleKeyword->value == m_v_active_keywords[l]->value){
					isNotFound = false;
				}
			}

			if (isNotFound){
				Keyword *newActiveKeyword = new Keyword(*newTempAticleKeyword);
				m_v_active_keywords.push_back(newActiveKeyword);
			}
		}
	}

	time_t endTime;
	time(&endTime);

	//std::cout << "generateActiveKeywordsVector time : " << (endTime-startTime) << "s" << std::endl;
	return true;
}

bool StoryTelling::generateActiveKeywordsVector(int index)
{
	time_t startTime;
	time(&startTime);

	for (size_t i=0; i<m_v_active_keywords.size(); ++i){
		delete m_v_active_keywords[i];
		m_v_active_keywords[i] = NULL;
	}
	m_v_active_keywords.clear();


	for (size_t i=0; i<=index; ++i){
		Article *tempArticle = m_v_articles[i];

		for (size_t k=0; k<tempArticle->getKeywordsNumber(); ++k){
			Keyword *newTempAticleKeyword = tempArticle->getKeywords()[k];
			bool isNotFound = true;
			for (size_t l=0; l<m_v_active_keywords.size(); ++l){
				if (newTempAticleKeyword->value == m_v_active_keywords[l]->value){
					isNotFound = false;
				}
			}

			if (isNotFound){
				Keyword *newActiveKeyword = new Keyword(*newTempAticleKeyword);
				m_v_active_keywords.push_back(newActiveKeyword);
			}
		}

		m_actived_article_numbers = i;
	}

	time_t endTime;
	time(&endTime);

	//std::cout << "generateActiveKeywordsVector time : " << (endTime-startTime) << "s" << std::endl;
	return true;
}

// Used for sort Keywords Vector by using TF-IDF.
// TF = occurrence;
// IDF = weight;
// -----------------------------------------------------------------------------------
bool sortKeywordsFunction_StoryTelling(Keyword *i, Keyword *j){
	return (i->occurrence)*(i->weight) > (j->occurrence)*(j->weight);
}

bool StoryTelling::updateActiveKeywordsFrequency()
{
	time_t startTime;
	time(&startTime);

	for (size_t i=0; i<m_v_active_keywords.size(); ++i){
		m_v_active_keywords[i]->occurrence = 0.0;
	}

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		Article *tempArticle = m_v_articles[i+m_slide_window_start_index];

		// calculate the vector keywords of each Article
		// -----------------------------------------------------------------------------------
		tempArticle->setActiveKeywordVector(m_v_active_keywords);

		// calculate the frequence of each keywords
		// -----------------------------------------------------------------------------------
		std::vector<size_t> tempIndexVector = tempArticle->getKeywordsIndex();
		for (size_t k=0; k<tempIndexVector.size(); ++k){
			++(m_v_active_keywords[tempIndexVector[k]]->occurrence);
		}
	}

	calculateKeywordsWeight();

	// Sort Keywords by TF-IDF // IDF = weight, TF = Occurence.
	// Store only the top "m_top_keyword_number" keywords.
	// -----------------------------------------------------------------------------------
	std::sort (m_v_active_keywords.begin(), m_v_active_keywords.end(), sortKeywordsFunction_StoryTelling);

	//m_top_keyword_number = (int)m_v_active_keywords.size() / 10;
	//m_top_keyword_number = 10;

	if (m_v_active_keywords.size() > m_top_keyword_number){
		m_v_active_keywords.erase(m_v_active_keywords.begin()+m_top_keyword_number, m_v_active_keywords.end());
	}

	// calculate the vector keywords of each Article base on Sorted Keyword Vector.
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<m_actived_article_numbers; ++i){
		m_v_articles[i+m_slide_window_start_index]->setActiveKeywordVector(m_v_active_keywords);
	}

	time_t endTime;
	time(&endTime);

	//std::cout << "updateActiveKeywordsFrequency time : " << (endTime-startTime) << "s" << std::endl;
	return true;
}

bool StoryTelling::calculateKeywordsWeight()
{
	// calculate the weight of each keywords
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<m_v_active_keywords.size(); ++i){
		m_v_active_keywords[i]->weight = log((double)m_v_active_keywords.size()/m_v_active_keywords[i]->occurrence)/log(2.0)*m_v_active_keywords[i]->hot_value;
	}

	return true;
}

bool StoryTelling::generateSimilarityMatrix()
{
	time_t startTime;
	time(&startTime);
	// calculate the similarity matirx for Articles
	// -----------------------------------------------------------------------------------

	if (m_similarity_matrix != NULL){
		delete [] m_similarity_matrix; 
		m_similarity_matrix = NULL;
	}

	m_similarity_matrix = new double [m_actived_article_numbers*m_actived_article_numbers];
	memset(m_similarity_matrix, 0x00, m_actived_article_numbers*m_actived_article_numbers*8);

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		for (size_t j=0; j<m_actived_article_numbers; ++j){
			double sum1=0;
			double sum2=0;
			double sum3=0;

			if (i == j){
				m_similarity_matrix[i*m_actived_article_numbers+j] = 1.0;
			}
			else if (i > j){
				m_similarity_matrix[i*m_actived_article_numbers+j] = m_similarity_matrix[j*m_actived_article_numbers+i];
			}
			else{
				std::vector<size_t> v_article_index_i = m_v_articles[i+m_slide_window_start_index]->getKeywordsIndex();
				std::vector<size_t> v_article_index_j = m_v_articles[j+m_slide_window_start_index]->getKeywordsIndex();

				for (size_t k=0; k<v_article_index_i.size(); ++k){
					sum2 += m_v_active_keywords[v_article_index_i[k]]->weight * m_v_active_keywords[v_article_index_i[k]]->weight;

					for (size_t l=0; l<v_article_index_j.size(); ++l){
						if (v_article_index_i[k] == v_article_index_j[l]){
							size_t index = v_article_index_i[k];
							sum1 += m_v_active_keywords[index]->weight * m_v_active_keywords[index]->weight;
						}
					}
				}

				for (size_t k=0; k<v_article_index_j.size(); ++k){
					sum3 += m_v_active_keywords[v_article_index_j[k]]->weight * m_v_active_keywords[v_article_index_j[k]]->weight;
				}

				if (sum2*sum3 == 0){
					m_similarity_matrix[i*m_actived_article_numbers+j] = 0.0;
				}
				else{
					m_similarity_matrix[i*m_actived_article_numbers+j] = sum1 / sqrt(sum2) / sqrt(sum3);
				}
			}
		}
	}

	time_t endTime;
	time(&endTime);

	//std::cout << "generateSimilarityMatrix time : " << (endTime-startTime) << "s" << std::endl;
	return true;
}

void StoryTelling::evolveSample(double dt, double alpha, double stopDistance, double minDistance, double maxDistance)
{
	time_t startTime;
	time(&startTime);

	// Doing the simulation of Articles. 
	// dt is the differential of time.
	// alpha is the parameter of Force Computing.
	// stopDistance is the threshold of stopping the simulation
	// minDistance and maxDistance is used for generate position of Articles. ActualDist = (1-similarity) * (maxDistance - minDistance) + minDistance;
	// -----------------------------------------------------------------------------------

	int step = 0;
	bool checkSimulationStop = true;
	while (checkSimulationStop){
		checkSimulationStop = false;

		computerForces(alpha, minDistance, maxDistance);

		// Calculate Velocity and Acceleration of Articles
		// -----------------------------------------------------------------------------------
		for (size_t i=0; i<m_actived_article_numbers; ++i){
			for (int k=0; k<m_dimension; ++k){
				m_v_articles[i+m_slide_window_start_index]->getVel()[k] = m_v_articles[i+m_slide_window_start_index]->getVel()[k] +
											0.5 * (m_v_articles[i+m_slide_window_start_index]->getForce()[k] / m_v_articles[i+m_slide_window_start_index]->getMass() + m_v_articles[i+m_slide_window_start_index]->getAcc()[k]) *
											dt;
				m_v_articles[i+m_slide_window_start_index]->getAcc()[k] = m_v_articles[i+m_slide_window_start_index]->getForce()[k] / m_v_articles[i+m_slide_window_start_index]->getMass();
											
			}
		}

		// Initialize Acceleration Sign
		// -----------------------------------------------------------------------------------
		if (step == 0){

			for (size_t i=0; i<m_actived_article_numbers; ++i){
				for (int k=0; k<m_dimension; ++k){
					if (m_v_articles[i+m_slide_window_start_index]->getAcc()[k] >= 0.0){
						m_v_articles[i+m_slide_window_start_index]->getAccSign()[k] = 1.0;
					}
					else{
						m_v_articles[i+m_slide_window_start_index]->getAccSign()[k] = -1.0;
					}
				}
			}
		}

		// Invert AccSign and Set Velocity to 0, if Acceleration changes direction.
		// -----------------------------------------------------------------------------------
		for (size_t i=0; i<m_actived_article_numbers; ++i){
			for (int k=0; k<m_dimension; ++k){
				if (m_v_articles[i+m_slide_window_start_index]->getAcc()[k] * m_v_articles[i+m_slide_window_start_index]->getAccSign()[k] < 0){
					m_v_articles[i+m_slide_window_start_index]->getVel()[k] = 0.0;
					m_v_articles[i+m_slide_window_start_index]->getAccSign()[k] *= -1.0;
				}
			}
		}

		// Calculate Position of Articles
		// -----------------------------------------------------------------------------------
		double dis = 0;

		for (size_t i=0; i<m_actived_article_numbers; ++i){
			dis = 0.0;
			for (int k=0; k<m_dimension; ++k){
				double tempDis = dt * m_v_articles[i+m_slide_window_start_index]->getVel()[k] +
					0.5 * dt * dt * m_v_articles[i+m_slide_window_start_index]->getAcc()[k];
				//m_v_articles[i+m_slide_window_start_index]->getPosition()[k] += tempDis;
				dis += tempDis * tempDis;
			}

			if (dis > stopDistance){
				checkSimulationStop = true;
			}
		}

		if (checkSimulationStop == true) {
			for (size_t i=0; i<m_actived_article_numbers; ++i){
				for (int k=0; k<m_dimension; ++k){
					double tempDis = dt * m_v_articles[i+m_slide_window_start_index]->getVel()[k] +
						0.5 * dt * dt * m_v_articles[i+m_slide_window_start_index]->getAcc()[k];
					m_v_articles[i+m_slide_window_start_index]->getPosition()[k] += tempDis;
				}
			}
		}

		// refold position for Articles.	Guarantee the position in the range [0, 1].
		// -----------------------------------------------------------------------------------
		for (size_t i=0; i<m_actived_article_numbers; ++i){
			for (int k=0; k<m_dimension; ++k){
				if (m_v_articles[i+m_slide_window_start_index]->getPosition()[k] > 1.0){
					m_v_articles[i+m_slide_window_start_index]->getPosition()[k] -= 1.0;
				}

				if (m_v_articles[i+m_slide_window_start_index]->getPosition()[k] < 0.0){
					m_v_articles[i+m_slide_window_start_index]->getPosition()[k] += 1.0;
				}
			}
		}

		++step;
	}

	time_t endTime;
	time(&endTime);

	//std::cout << "evolveSample time : " << (endTime-startTime) << "s" << std::endl;

	//std::cout << "Step : " << step << std::endl;
	std::cout << "Articles Number : " << m_slide_window_start_index << " + " << m_actived_article_numbers << std::endl;
}

void StoryTelling::computerForces(double alpha, double minDistance, double maxDistance)
{
	// Rij are the different of position vector of article i and article j.
	// Rsqij = sqrt(Rij*Rij).
	// phi is the energy.
	// dphi is the force.
	// dist is the ideal distance between two articles.
	// -----------------------------------------------------------------------------------

	double *Rij = new double[m_dimension];
	double Rsqij, phi, dphi, dist;

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		for (int k=0; k<m_dimension; ++k){
			m_v_articles[i+m_slide_window_start_index]->getForce()[k] = 0.0;
		}
	}

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		for (size_t j=i+1; j<m_actived_article_numbers; ++j){
			for (int k=0; k<m_dimension; ++k){
				Rij[k] = m_v_articles[i+m_slide_window_start_index]->getPosition()[k] - m_v_articles[j+m_slide_window_start_index]->getPosition()[k];
			}

			Rsqij = sqrt(dotProduct(Rij, Rij, m_dimension)); 
			dist = (maxDistance - minDistance) * (1.0 - m_similarity_matrix[i*m_actived_article_numbers+j]) + minDistance;

			phi = alpha * pow((dist - Rsqij), 2);
			dphi = alpha * 2 * (dist - Rsqij);

			for (int k=0; k<m_dimension; ++k){
				if (Rsqij == 0){
					double randomValue = rand()%10/10.0;
					m_v_articles[i+m_slide_window_start_index]->getForce()[k] += randomValue;
					m_v_articles[j+m_slide_window_start_index]->getForce()[k] -= randomValue;
				}else{
					m_v_articles[i+m_slide_window_start_index]->getForce()[k] += dphi * Rij[k] / Rsqij;
					m_v_articles[j+m_slide_window_start_index]->getForce()[k] -= dphi * Rij[k] / Rsqij;
				}
			}
		}
	}

	delete [] Rij;
	Rij = NULL;
}

void StoryTelling::outputPosition(std::string outputFileName)
{
	std::ofstream outputFile(outputFileName.c_str());

	outputFile << "Article_ID,x,y" << std::endl;

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		outputFile << i << "," << m_v_articles[i]->getPosition()[0] << "," << m_v_articles[i]->getPosition()[1] << std::endl;
	}

	outputFile.close();
}

void StoryTelling::outputClusterPosition(std::string outputFileName)
{
	std::ofstream outputFile(outputFileName.c_str());
	outputFile << "var dataset = ";
	
	JSONVALUE JS_result;
	std::string result;

	for (size_t i=0; i<m_top_cluster_number; ++i){
		ArticleCluster *tempCluster = m_v_clusters[i];
		JSONVALUE JS_tempCluster;

		JSONARRAY JS_position;
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[0]);
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[1]);
		JS_tempCluster["position"] = JS_position;

		JS_tempCluster["size"] = tempCluster->m_cluster_size;
		JS_tempCluster["id"] = tempCluster->m_cluster_id;
		JS_tempCluster["IsNew"] = tempCluster->m_IsNewCluster;
	
		JS_tempCluster["headline"] = tempCluster->m_cluster_center_article->getHeadline();

		JSONARRAY JS_keywords;
		for (size_t j=0; j<10; ++j){
			JS_keywords.push_back(tempCluster->m_v_cluster_keywords[j]->value);
		}

		JS_tempCluster["keywords"] = JS_keywords;

		JS_result.Push(JS_tempCluster);
	}

	JS_result.ToString(result);
	outputFile << result << std::endl;

	outputFile.close();
}

void StoryTelling::generateTheMostSimilarCluster(std::vector<ArticleCluster *> &matched_v_clusters)
{
	if (m_v_clusters.size() == 0) return ;
	if (m_v_previous_clusters.size() == 0) return;

	size_t previous_size = m_v_previous_clusters.size();
	size_t current_size = m_v_clusters.size();

	if (previous_size > m_top_cluster_number) previous_size = m_top_cluster_number;
	if (current_size > m_top_cluster_number) current_size = m_top_cluster_number;

	for (size_t i=0; i<current_size; ++i){
		// find the most similar index, swap the index and i of matched_v_clusters.
		if (i <= previous_size-1)
		{
			ArticleCluster *target = m_v_previous_clusters[i];
			double maxSimilarity = 0.0;
			size_t index = i;

			for (size_t j=i; j<current_size; ++j){
				ArticleCluster *source = matched_v_clusters[j];
				double tempSimilarity = calculateTheSimilarityOfClusters(target, source, 1);
				if (maxSimilarity < tempSimilarity){
					maxSimilarity = tempSimilarity;
					index = j;
				}
			}
			//size_t index = findIndexOfTheMostSimilarCluster(i, current_size);
			ArticleCluster *temp = matched_v_clusters[i];
			matched_v_clusters[i] = matched_v_clusters[index];
			matched_v_clusters[i]->m_cluster_id = i;
			matched_v_clusters[i]->m_IsNewCluster = false;

			if (maxSimilarity < 0.1) matched_v_clusters[i]->m_IsNewCluster = true;
			matched_v_clusters[index] = temp;
		}
		else{
			matched_v_clusters[i]->m_cluster_id = i;
			matched_v_clusters[i]->m_IsNewCluster = true;
		}
	}
}

double StoryTelling::calculateTheSimilarityOfClusters(ArticleCluster *target, ArticleCluster *source, int similarityType)
{
	// 0 is the keyworks; 1 is the Article id;
	if (similarityType == 0){
		double result = 0.0;
		double maxresult = 0.0;
		std::vector<Keyword *> targetVecotr = target->m_v_cluster_keywords;
		std::vector<Keyword *> sourceVector = source->m_v_cluster_keywords;
		size_t size_i = 10;
		size_t size_j = 10;

		if (size_i > targetVecotr.size()) size_i = targetVecotr.size();
		if (size_j > sourceVector.size()) size_j = sourceVector.size();

		for (size_t i=0; i<size_i; ++i){
			maxresult += (size_i-i+1);

			for (size_t j=0; j<size_j; ++j){
				if (targetVecotr[i]->value == sourceVector[j]->value){
					result += (size_i-i+1);
					break;
				}
			}
		}
		return result/maxresult;
	}
	else if (similarityType == 1){
		double result = 0.0;
		double maxresult = 0.0;
		std::vector<size_t> targetVecotr = target->m_v_index;
		std::vector<size_t> sourceVector = source->m_v_index;

		size_t size_i = targetVecotr.size();
		size_t size_j = sourceVector.size();

		for (size_t i=0; i<size_i; ++i){
			maxresult += 1;

			for (size_t j=0; j<size_j; ++j){
				if (targetVecotr[i] == sourceVector[j]){
					result += 1;
				}
			}
		}
		return result;

	}

	return 0;
}

std::string StoryTelling::generateClusterPositionString()
{
	JSONVALUE JS_result;
	JSONVALUE JS_Cluster;
	JSONVALUE JS_DateTime;
	std::string result;

	size_t cluster_size = m_top_cluster_number;

	if (m_v_clusters.size() < cluster_size){
		cluster_size = m_v_clusters.size();
	}

	// Store the previous cluster result and reorder the first 10 clusters in the cluster vector
	// which is used for matching cluster result.
	// -----------------------------------------------------------------------------------

	std::vector<ArticleCluster *> matched_v_clusters;

	for (size_t i=0; i<cluster_size; ++i){
		matched_v_clusters.push_back(m_v_clusters[i]);
	}


	//generateTheMostSimilarCluster(matched_v_clusters);

	// Backup the previous cluster information.
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<m_v_previous_clusters.size(); ++i){
		delete m_v_previous_clusters[i];
		m_v_previous_clusters[i] = NULL;
	}

	m_v_previous_clusters.clear();

	for (size_t i=0; i<matched_v_clusters.size(); ++i){
		ArticleCluster * newItem = new ArticleCluster(matched_v_clusters[i]);
		m_v_previous_clusters.push_back(newItem);
	}

	// Generate the Cluster String and send it to web.
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<cluster_size; ++i){
		ArticleCluster *tempCluster = matched_v_clusters[i];
		JSONVALUE JS_tempCluster;

		JSONARRAY JS_position;
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[0]);
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[1]);
		JS_tempCluster["position"] = JS_position;

		JS_tempCluster["size"] = tempCluster->m_cluster_size / (double)this->m_actived_article_numbers;
		JS_tempCluster["id"] = tempCluster->m_cluster_id;

		JS_tempCluster["IsNew"] = tempCluster->m_IsNewCluster;

		JS_tempCluster["headline"] = tempCluster->m_cluster_center_article->getHeadline();

		JSONARRAY JS_keywords;
		size_t keyword_size = 10;
		if (tempCluster->m_v_cluster_keywords.size() < keyword_size){
			keyword_size = tempCluster->m_v_cluster_keywords.size();
		}
		for (size_t j=0; j<keyword_size; ++j){
			JS_keywords.push_back(tempCluster->m_v_cluster_keywords[j]->value);
		}

		JS_tempCluster["keywords"] = JS_keywords;

		JS_Cluster.Push(JS_tempCluster);
	}

	for (size_t i=0; i<m_currentTime.size(); ++i){
		JS_DateTime.Push(m_currentTime[i]);
	}

	JS_result["cluster"] = JS_Cluster;
	JS_result["messageContent"] = JS_DateTime;
	JS_result["messageType"] = "VIS_Current_Date";

	JS_result.ToString(result);
	return result;
}

void StoryTelling::setClusterThreshouldCoefficient(double newThresholdCoefficient)
{
	m_cluster_threshold_coefficient = newThresholdCoefficient;
}

void StoryTelling::setClusterSizeThreshouldCoefficient(double newThresholdCoefficient)
{
	m_cluster_visualization_threshold = newThresholdCoefficient;
}

std::vector<ArticleCluster *> StoryTelling::getArticleClusterVector()
{
	std::vector<ArticleCluster *> result;
	int tempSize = m_cluster_visualization_threshold;
	if (tempSize > m_v_clusters.size()) {
		tempSize = m_v_clusters.size();
	}

	for (size_t i=0; i<tempSize; ++i){
		if (m_v_clusters[i]->m_cluster_size < 7)
		{
			continue;
		}
		ArticleCluster *newItem = new ArticleCluster(m_v_clusters[i]);
		result.push_back(newItem);
	}

	return result;
}

// Used for sort Clusters
// -----------------------------------------------------------------------------------
bool sortClustersFunction(std::vector<size_t> i, std::vector<size_t> j){
	return i.size() > j.size();
}
bool sortEdgeFunctionForCluster(edge i, edge j) // descending
{
	return i.getLength() > j.getLength();
}

void StoryTelling::clusterActivedArticle()
{
	time_t startTime;
	time(&startTime);

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		m_v_articles[i+m_slide_window_start_index]->m_InCluster = false;
	}

	// Generate VertexSet by using Articles
	// -----------------------------------------------------------------------------------
	initilizeVertexset();

	my_triangleset.clear();
	my_edgeset.clear();

	if (my_trangulation != NULL){
		delete my_trangulation;
		my_trangulation = NULL;
	}

	my_trangulation = new Delaunay();

	my_trangulation->Triangulate(m_vertex_set, my_triangleset);
	my_trangulation->TrianglesToEdges(my_triangleset, my_edgeset);

	m_v_index_clusters.clear();

	// Generate Vector edge. And delete the edge which is larger then cluster_threshold.
	// -----------------------------------------------------------------------------------
	m_cluster_threshold = m_cluster_threshold_coefficient / 100;
	//double tempThresholdHelp = 0.004;
	//double tempThreshold = ((int)(m_cluster_threshold*100))/100.0 + tempThresholdHelp;
	//if (m_cluster_threshold > tempThreshold){
	//	m_cluster_threshold = tempThreshold;
	//}
	//else{
	//	m_cluster_threshold = tempThreshold - tempThresholdHelp;
	//}

	// Used for restore one cluster.
	// -----------------------------------------------------------------------------------
	std::vector<size_t> one_cluster;

	std::vector<edge> temp_v_edgeset;

	for (edgeSet::iterator iter=my_edgeset.begin(); iter!=my_edgeset.end(); iter++){
			temp_v_edgeset.push_back(*iter);
	}

	sort(temp_v_edgeset.begin(), temp_v_edgeset.end(), sortEdgeFunctionForCluster);
	temp_v_edgeset.erase(temp_v_edgeset.begin(), temp_v_edgeset.begin() + (int)(temp_v_edgeset.size()*m_cluster_threshold));

	// Here we build the clusters by traversing all the edges, if the edge is less than the 
	// threshold then we add the two articles to the same cluster
	// -----------------------------------------------------------------------------------
	while (temp_v_edgeset.size() != 0){	
		edge startEdge = temp_v_edgeset[0];

		temp_v_edgeset.erase(temp_v_edgeset.begin());

		clusterFromOneEdge(startEdge, one_cluster, temp_v_edgeset);

		if (one_cluster.size() != 0){
			m_v_index_clusters.push_back(one_cluster);
		}

		one_cluster.clear();
	}


	// Sort Clusters By size of cluster.
	// -----------------------------------------------------------------------------------
	sort (m_v_index_clusters.begin(), m_v_index_clusters.end(), sortClustersFunction);

	// Generate the Abstract Information of Cluster.
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<m_v_clusters.size(); ++i){
		delete m_v_clusters[i];
		m_v_clusters[i] = NULL;
	}

	m_v_clusters.clear();

	for (size_t i=0; i<m_v_index_clusters.size(); ++i){
		ArticleCluster *newItem = new ArticleCluster(i, m_v_index_clusters[i], m_v_articles, m_currentTime);
		m_v_clusters.push_back(newItem);
	}

	time_t endTime; 
	time(&endTime);

	//std::cout << "clusterActivedArticle time : " << (endTime-startTime) << "s" << std::endl;
}

/*
void StoryTelling::clusterActivedArticle()
{
	time_t startTime;
	time(&startTime);

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		m_v_articles[i]->m_InCluster = false;
	}

	// Generate VertexSet by using Articles
	// -----------------------------------------------------------------------------------
	initilizeVertexset();

	my_triangleset.clear();
	my_edgeset.clear();

	if (my_trangulation != NULL){
		delete my_trangulation;
		my_trangulation = NULL;
	}

	my_trangulation = new Delaunay();

	my_trangulation->Triangulate(m_vertex_set, my_triangleset);
	my_trangulation->TrianglesToEdges(my_triangleset, my_edgeset);

	m_v_index_clusters.clear();

	// Generate Vector edge. And delete the edge which is larger then cluster_threshold.
	// -----------------------------------------------------------------------------------
	m_cluster_threshold = m_cluster_threshold_coefficient / (m_actived_article_numbers+1) / 6;
	double tempThresholdHelp = 0.004;
	double tempThreshold = ((int)(m_cluster_threshold*100))/100.0 + tempThresholdHelp;
	if (m_cluster_threshold > tempThreshold){
		m_cluster_threshold = tempThreshold;
	}
	else{
		m_cluster_threshold = tempThreshold - tempThresholdHelp;
	}

	// Used for restore one cluster.
	// -----------------------------------------------------------------------------------
	std::vector<size_t> one_cluster;

	std::vector<edge> temp_v_edgeset;

	for (edgeSet::iterator iter=my_edgeset.begin(); iter!=my_edgeset.end(); iter++){
		if (iter->getLength() < m_cluster_threshold){
			temp_v_edgeset.push_back(*iter);
		}
	}

	// Here we build the clusters by traversing all the edges, if the edge is less than the 
	// threshold then we add the two articles to the same cluster
	// -----------------------------------------------------------------------------------
	while (temp_v_edgeset.size() != 0){	
		edge startEdge = temp_v_edgeset[0];

		temp_v_edgeset.erase(temp_v_edgeset.begin());

		clusterFromOneEdge(startEdge, one_cluster, temp_v_edgeset);

		if (one_cluster.size() != 0){
			m_v_index_clusters.push_back(one_cluster);
		}

		one_cluster.clear();
	}


	// Sort Clusters By size of cluster.
	// -----------------------------------------------------------------------------------
	sort (m_v_index_clusters.begin(), m_v_index_clusters.end(), sortClustersFunction);

	// Generate the Abstract Information of Cluster.
	// -----------------------------------------------------------------------------------
	for (size_t i=0; i<m_v_clusters.size(); ++i){
		delete m_v_clusters[i];
		m_v_clusters[i] = NULL;
	}

	m_v_clusters.clear();
	
	for (size_t i=0; i<m_v_index_clusters.size(); ++i){
		ArticleCluster *newItem = new ArticleCluster(i, m_v_index_clusters[i], m_v_articles, m_currentTime);
		m_v_clusters.push_back(newItem);
	}

	time_t endTime; 
	time(&endTime);

	//std::cout << "clusterActivedArticle time : " << (endTime-startTime) << "s" << std::endl;
}
*/

void StoryTelling::initilizeVertexset()
{
	m_vertex_set.clear();

	for (size_t i=0; i<m_actived_article_numbers; ++i){
		vertex newVertex(m_v_articles[i+m_slide_window_start_index]->getPosition()[0], m_v_articles[i+m_slide_window_start_index]->getPosition()[1], i+m_slide_window_start_index);
		m_vertex_set.insert(newVertex);
	}
}

void StoryTelling::clusterFromOneEdge(edge startEdge, std::vector<size_t> &one_cluster, std::vector<edge> &temp_v_edgeset)
{
	int test = -1;
	if (!m_v_articles[startEdge.m_pV0->m_index]->m_InCluster){
		test = startEdge.m_pV0->m_index;
		one_cluster.push_back(startEdge.m_pV0->m_index);
		m_v_articles[startEdge.m_pV0->m_index]->m_InCluster = true;

	}

	if (!m_v_articles[startEdge.m_pV1->m_index]->m_InCluster){
		test = startEdge.m_pV1->m_index;
		one_cluster.push_back(startEdge.m_pV1->m_index);
		m_v_articles[startEdge.m_pV1->m_index]->m_InCluster = true;
	}

	if (one_cluster.size() == 0)
		return;

	size_t index = 0;

	while (index < one_cluster.size()){
		for (size_t i=0; i<temp_v_edgeset.size(); ++i){
			edge itemEdge = temp_v_edgeset[i];

			if (itemEdge.m_pV0->m_index == one_cluster[index] && !m_v_articles[itemEdge.m_pV1->m_index]->m_InCluster){
				m_v_articles[itemEdge.m_pV1->m_index]->m_InCluster = true;
				one_cluster.push_back(itemEdge.m_pV1->m_index);

				temp_v_edgeset.erase(temp_v_edgeset.begin()+i);
				--i;
			}

			if (itemEdge.m_pV1->m_index == one_cluster[index] && !m_v_articles[itemEdge.m_pV0->m_index]->m_InCluster){
				m_v_articles[itemEdge.m_pV0->m_index]->m_InCluster = true;
				one_cluster.push_back(itemEdge.m_pV0->m_index);

				temp_v_edgeset.erase(temp_v_edgeset.begin()+i);
				--i;
			}
		}

		++index;
	}
}

bool StoryTelling::datetimeParser(std::string pub_date, std::vector<int> &my_date) 
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

bool StoryTelling::datetimeStrParser(std::vector<int> my_date, std::string &pub_date)
{
	// ""2013-01-04T00:00:00Z"" Format
	char buffer[32];
	int size_n = 0;

	size_n = sprintf(buffer, "\"%d-%02d-%02dT%02d:%02d:%02dZ\"", my_date[0],my_date[1],my_date[2],my_date[3],my_date[4],my_date[5]);
	
	pub_date = std::string(buffer, size_n);
	return true;
}

bool StoryTelling::compareDate(std::vector<int> earlierDate, std::vector<int> olderDate)
{
	//True : earlierDate <= olderDate. False : earlierDate > olderDate
	// -----------------------------------------------------------------------------------
	size_t i = 0;
	size_t m = earlierDate.size();

	for (i=0; i<m; ++i){
		if (earlierDate[i] > olderDate[i]){
			return false;
		}
		else if (earlierDate[i] < olderDate[i]){
			return true;
		}
	}

	return true;
}

double StoryTelling::dotProduct(double arr1[],double arr2[],int length)
{
	double mult = 0;
	for(int i = 0 ; i < length ; i++)
		mult += arr1[i] * arr2[i];
	return mult;

}