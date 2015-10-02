#include "ServerProcessing.h"

ServerProcessing::ServerProcessing(int port, std::string ip)
{
	time_t startTime;
	time(&startTime);

	// Initialize websocketserver.
	// -----------------------------------------------------------------------------------
	int server_port = port;	//2014;
	std::string server_ip = ip;	//"127.0.0.1";
	m_socketserver = new websocketserver(server_port, server_ip);

	m_current_index = 30;//110;

	// Initialize Data set.
	// -----------------------------------------------------------------------------------
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/dataset_C++_None.csv"));
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/dataset_C++_Obama.csv"));

	m_storytelling = new StoryTelling(generateDataCollection("../../data/old_dataset/dataset_C++.csv"), m_current_index);
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/dataset_C++NSF600.csv"));
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/public_11.04_11.26_20140919-small.txt"));
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/public_11.04_11.26_20140919.txt"));
	//m_storytelling = new StoryTelling(generateDataCollection("../../data/scotland9.16_17.txt"));

	m_speed = 1;
	m_keyword_size = 10;
	
	m_pause = true;
	m_playback = false;
	m_focus = true;
	m_focus_id = 0;

	m_sentArticleIndex = 0;
	
	m_v_cluster_create_time.resize(200);
	
	time_t endTime;
	time(&endTime);

	std::cout << "Server initialization time : " << (endTime-startTime) << "s" << std::endl;
}

void ServerProcessing::run(int secondInterval)
{
	int interval_day = (int)(secondInterval / 3600 / 24);
	int interval_hours = ((int)(secondInterval / 3600)) % 24;
	int interval_min = (int)((secondInterval % 3600)/60);
	int interval_second = secondInterval % 60;

	m_storytelling->setInterval(interval_day, interval_hours, interval_min, interval_second);

	bool doSendMessage = true;

	m_history_index = 0;

	INT64 waitTime = 10;
	INT64 waitTime2 = 5;

	while (1){
		time_t startTime;
		time_t endTime;


		do{
			
			std::string newMessage = getMessageFromServer();

			if (newMessage == "") continue;
			JSONVALUE newJsonMessage;
			newJsonMessage.Parse(newMessage);
			messageProcessing(newJsonMessage);
		}while(m_pause);

		time(&startTime);

		if (!m_playback){
			//doSendMessage = m_storytelling->runToDateByIndex(m_current_index++);
			doSendMessage = m_storytelling->runToNextInterval();

			if (doSendMessage){
				generateTracingClusterResultForCurrentCluster();
				backupCurrentHistory();
				
				std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
				

				std::string messageOfArticleDetail = generateMessageOfArticleDetail(m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index, m_sentArticleIndex);
				
				m_sentArticleIndex = m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index;

				std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);

				std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);

				sendMessageToClient(messageOfArticleDetail);
				sendMessageToClient(messageOfArticleNodes);
				sendMessageToClient(messageOfClusterResult);

				if (m_focus){			
					sendMessageToClient(messageOfFocusCluster);
				}

				//sendMessageToClient(generateMessageOfArticlePosition());
				//sendMessageToClient(m_storytelling->generateClusterPositionString());
				//Sleep(600);

				++m_history_index;
			}

			//if (m_current_index > 300){

			//	m_current_index = 80;
			//	m_history_index = 0;
			//	for (int i=0; i<m_v_history.size(); ++i){
			//		for (int j=0; j<m_v_history[i].size(); ++j){
			//			delete m_v_history[i][j];
			//		}

			//		m_v_history[i].clear();
			//	}
			//	m_v_history.clear();
			//}
		}
		else{ // is playing back.
			std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
			std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
			std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);

			sendMessageToClient(messageOfArticleNodes);
			sendMessageToClient(messageOfClusterResult);

			if (m_focus){
				sendMessageToClient(messageOfFocusCluster);
			}

			++m_history_index;

			if (m_history_index == m_v_history.size()){
				m_playback = false;
				m_socketserver->sendMessage(generateMessageOfPlayBack(false, m_history_index));
			}
		}

		time(&endTime);
		if ((endTime-startTime)<waitTime){
			Sleep((waitTime - endTime + startTime)*1000); 
		}

		Sleep(waitTime2*1000); 
	}
}

void ServerProcessing::run()
{
	bool doSendMessage = true;

	m_current_index = 0;

	m_history_index = 0;

	while (1){
		time_t startTime;
		time_t endTime;


		do{
			std::string newMessage = getMessageFromServer();

			if (newMessage == "") continue;

			JSONVALUE newJsonMessage;
			newJsonMessage.Parse(newMessage);
			messageProcessing(newJsonMessage);
		}while(m_pause);

		time(&startTime);

		if (!m_playback){
			doSendMessage = m_storytelling->runToDateByIndex(m_current_index++);

			if (doSendMessage){
				generateTracingClusterResultForCurrentCluster();
				backupCurrentHistory();
				time(&endTime);
				if ((endTime-startTime)<3){
					Sleep((3 - endTime + startTime)*0000);
				}

				
				std::string messageOfArticleDetail = generateMessageOfArticleDetail(m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index, m_sentArticleIndex);
				m_sentArticleIndex = m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index;

				sendMessageToClient(messageOfArticleDetail);

				
				std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
				std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
				std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);	

				sendMessageToClient(messageOfArticleNodes);
				sendMessageToClient(messageOfClusterResult);

				if (m_focus){
					
					sendMessageToClient(messageOfFocusCluster);
				}

				//sendMessageToClient(generateMessageOfArticlePosition());
				//sendMessageToClient(m_storytelling->generateClusterPositionString());
				//Sleep(600);

				++m_history_index;
			}

			//if (m_current_index > 300){

			//	m_current_index = 80;
			//	m_history_index = 0;
			//	for (int i=0; i<m_v_history.size(); ++i){
			//		for (int j=0; j<m_v_history[i].size(); ++j){
			//			delete m_v_history[i][j];
			//		}

			//		m_v_history[i].clear();
			//	}
			//	m_v_history.clear();
			//}
		}
		else{ // is playing back.

			time(&endTime);
			if ((endTime-startTime)<5){
				Sleep(5000);
			}

			std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
			std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
			std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);

			sendMessageToClient(messageOfArticleNodes);
			sendMessageToClient(messageOfClusterResult);

			if (m_focus){
				sendMessageToClient(messageOfFocusCluster);
			}

			++m_history_index;

			if (m_history_index == m_v_history.size()){
				m_playback = false;
				m_socketserver->sendMessage(generateMessageOfPlayBack(false, m_history_index));
			}
		}
	}
}

std::vector<Article *> ServerProcessing::generateDataCollection(std::string filename)
{
	// create an new vector for new data collection.
	std::vector<Article *> result_v_articles;

	// Input Data Set.
	std::ifstream inputFile(filename.c_str());

	std::string line;

	// Parse the first line "query_word start_data end_data"
	// -----------------------------------------------------------------------------------

	std::getline (inputFile, line, '\n'); 
	std::istringstream linestream(line);
	std::string query_word;
	std::string start_data;
	std::string end_data;

	getline(linestream, query_word, ',');
	getline(linestream, start_data, ',');
	getline(linestream, end_data, ',');

	int articleID = 0;

	// Create the Vector for all Articles.
	// -----------------------------------------------------------------------------------

	while(std::getline (inputFile, line, '\n')){
		JSONVALUE newJSONArticle;
		newJSONArticle.Parse(line);

		Article *newArticle = new Article(newJSONArticle, articleID);

		result_v_articles.push_back(newArticle);

		++articleID;
	}

	inputFile.close();

	return result_v_articles;
}

bool ServerProcessing::messageProcessing(JSONVALUE newJsonMessage)
{
	bool result = true;
	// message processing.
	// use messageType to do something base on the messageContent.
	std::string messageType = "";
	newJsonMessage["messageType"].ToString(messageType, 2);

	JSONVALUE messageContent = newJsonMessage["messageContent"];
	// -----------------------------------------------------------------------------------
	if (!messageType.compare(std::string("\"VIS_Pause_Continue\""))){

		std::string currentStatus;
		messageContent.ToString(currentStatus);
		if (currentStatus == "\"playing\""){
			m_pause = true;
		}
		else if (currentStatus == "\"pausing\""){
			m_pause = false;
		}

		JSONVALUE JS_Result;
		JSONVALUE JS_MessageContent;
		JS_MessageContent = m_pause ? "pausing" : "playing";
		JS_Result["messageType"] = "VIS_Pause_Continue";
		JS_Result["messageContent"] = JS_MessageContent;

		std::string messageString;
		JS_Result.ToString(messageString);
		m_socketserver->sendMessage(messageString);
	}
	else if (!messageType.compare(std::string("\"VIS_Play_Back\""))){
		
		m_history_index = messageContent.val.numVal.llVal;
		m_playback = true;
		m_socketserver->sendMessage(generateMessageOfPlayBack(true, m_history_index));

		int stop = 0;
	}
	else if (!messageType.compare(std::string("\"VIS_Focus_Context\""))){

		m_focus = messageContent["isFocus"].val.boolVal;
		m_focus_id = messageContent["focusID"].val.numVal.llVal;

		//m_history_index = historyID;
		//m_playback = true;
		//m_socketserver->sendMessage(generateMessageOfPlayBack(true, m_history_index));

		//send the focus information to client;
		//m_socketserver->sendMessage(generateMessageOfFocusCluster(focusID, m_history_index));

		int stop = 0;
	}
	else if (!messageType.compare(std::string("\"VIS_Play_Control\""))){
		if (m_pause == false){
			return result;
		}

		int tempType = messageContent["d"].val.numVal.llVal;
		int count = messageContent["c"].val.numVal.llVal;

		if (tempType == 1){
			goAhead();
		}
		else{
			goBack();
		}
		

		int stop = 0;
	}
	else if (!messageType.compare(std::string("\"VIS_CLUSTER_COEFFICIENT\""))){
		int currentHistoryIndex = m_history_index - 1;
		if (currentHistoryIndex < 0) {
			currentHistoryIndex = 0;
		}
		m_storytelling->setClusterThreshouldCoefficient(messageContent.val.numVal.llVal);

		m_storytelling->clusterActivedArticle();
		generateTracingClusterResultForCurrentCluster();

		std::string messageOfClusterResult = generateMessageOfClusterResult(m_v_current_clusters);
		std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_v_current_clusters);
		std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_v_current_clusters);

		sendMessageToClient(messageOfArticleNodes);
		sendMessageToClient(messageOfClusterResult);
		sendMessageToClient(messageOfFocusCluster);
	}
	else if (!messageType.compare(std::string("\"VIS_CLUSTER_SIZE\""))){
		int currentHistoryIndex = m_history_index - 1;
		if (currentHistoryIndex < 0) {
			currentHistoryIndex = 0;
		}
		m_storytelling->setClusterSizeThreshouldCoefficient(messageContent.val.numVal.llVal);

		m_storytelling->clusterActivedArticle();
		generateTracingClusterResultForCurrentCluster();

		std::string messageOfClusterResult = generateMessageOfClusterResult(m_v_current_clusters);
		std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_v_current_clusters);
		std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_v_current_clusters);

		sendMessageToClient(messageOfArticleNodes);
		sendMessageToClient(messageOfClusterResult);
		sendMessageToClient(messageOfFocusCluster);
	}
	else if (!messageType.compare(std::string("newMessage"))){
		
	}

	return result;
}

std::string ServerProcessing::dataTimeToString(std::vector<int> my_date)
{
	std::string result = "";
	//"2013-01-04T00:00:00Z"

	if (my_date.size() < 6) return "NULL";

	char buffer[32];
	int size_n = 0;

	size_n = sprintf(buffer, "%d-%02d-%02dT%02d:%02d:%02dZ", my_date[0],my_date[1],my_date[2],my_date[3],my_date[4],my_date[5]);

	result = std::string(buffer, size_n);

	return result;
}

bool ServerProcessing::generateTracingClusterResultForCurrentCluster()
{
	// get a deep copy of current cluster result
	// -----------------------------------------------------------------------------------
	m_v_current_clusters = m_storytelling->getArticleClusterVector();

	if (m_v_history.size() == 0){
		// generate tracing cluster result for starting time which has not previous time stamp.
		// -----------------------------------------------------------------------------------
		for (size_t i=0; i<m_v_current_clusters.size(); ++i){
			ArticleCluster *source = m_v_current_clusters[i];
			source->m_IsNewCluster = true;
			source->m_v_parent.clear();
			source->m_cluster_id = i;
			m_v_cluster_create_time[source->m_cluster_id] = dataTimeToString(source->m_currentTime);
			source->m_cluster_create_time_string = m_v_cluster_create_time[source->m_cluster_id];
		}
	}
	else {
		// 1/2 generate similar Number data structure for all clusters of current time stamp.
		// -----------------------------------------------------------------------------------
		
		// generate the tracing cluster result. Target is the previous cluster history.
		// -----------------------------------------------------------------------------------
		std::vector<ArticleCluster *> targetHistory = *--m_v_history.end();

		for (size_t s=0; s<m_v_current_clusters.size(); ++s){
			ArticleCluster *source = m_v_current_clusters[s];

			for (size_t t=0; t<targetHistory.size(); ++t){
				ArticleCluster *target = targetHistory[t];
				int similarNumber = 0;
				std::vector<size_t> targetVecotr = target->m_v_index;
				std::vector<size_t> sourceVector = source->m_v_index;

				size_t size_i = targetVecotr.size();
				size_t size_j = sourceVector.size();

				for (size_t i=0; i<size_i; ++i){
					for (size_t j=0; j<size_j; ++j){
						if (targetVecotr[i] == sourceVector[j]){
							similarNumber += 1;
						}
					}
				}

				source->m_v_parent.push_back(similarNumber);
			}
		}


		// 2/2 processing clusters by using the similar Number data structure.
		// Using hungarian Algorithm
		// -----------------------------------------------------------------------------------
		Hungarian temp_Hungarian(m_v_current_clusters, targetHistory);

		for (size_t s=0; s<m_v_current_clusters.size(); ++s){
			ArticleCluster *tempCluster = m_v_current_clusters[s];
			if(tempCluster->m_IsNewCluster){
				m_v_cluster_create_time[tempCluster->m_cluster_id] = dataTimeToString(tempCluster->m_currentTime);
			}

			tempCluster->m_cluster_create_time_string = m_v_cluster_create_time[tempCluster->m_cluster_id];
		}
	}

	return true;
}

bool ServerProcessing::backupCurrentHistory()
{
	if (m_v_current_clusters.size() == 0)
		return false;

	m_v_history.push_back(m_v_current_clusters);
	//int test = m_v_history.size();
	//m_v_history_index.push_back(m_current_index);
	return true;
}

bool ServerProcessing::clearAllHistory()
{
	for (size_t i=0; i<m_v_history.size(); ++i){
		for (size_t j=0; j<m_v_history[i].size(); ++j){
			delete m_v_history[i][j];
			m_v_history[i][j] = NULL;
		}
		m_v_history[i].clear();
	}

	m_v_history.clear();
	m_v_history_index.clear();

	return true;
}

void ServerProcessing::goAhead()
{
	bool doSendMessage = true;

	if (!m_playback){
		do{
			//doSendMessage = m_storytelling->runToDateByIndex(m_current_index++);
			doSendMessage = m_storytelling->runToNextInterval();
		}while(!doSendMessage);

		if (doSendMessage){
			generateTracingClusterResultForCurrentCluster();
			backupCurrentHistory();

			std::string messageOfArticleDetail = generateMessageOfArticleDetail(m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index, m_sentArticleIndex);
			m_sentArticleIndex = m_storytelling->getActivedNumber()+m_storytelling->m_slide_window_start_index;
			sendMessageToClient(messageOfArticleDetail);


			std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
			std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
			std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);


			sendMessageToClient(messageOfArticleNodes);
			sendMessageToClient(messageOfClusterResult);

			if (m_focus){				
				sendMessageToClient(messageOfFocusCluster);
			}
			++m_history_index;
		}
	}
	else{ // is playing back.

		std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
		std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
		std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);

		sendMessageToClient(messageOfArticleNodes);
		sendMessageToClient(messageOfClusterResult);

		if (m_focus){
			sendMessageToClient(messageOfFocusCluster);
		}

		++m_history_index;

		if (m_history_index == m_v_history.size()){
			m_playback = false;
			m_socketserver->sendMessage(generateMessageOfPlayBack(false, m_history_index));
		}
	}
}

void ServerProcessing::goBack()
{
	m_playback = true;
	m_history_index -= 2;

	if (m_history_index < 0){
		m_history_index = 0;
		return;
	}

	m_socketserver->sendMessage(generateMessageOfPlayBack(true, m_history_index));

	std::string messageOfClusterResult = generateMessageOfClusterResult(m_history_index, false);
	std::string messageOfArticleNodes = generateMessageOfArticleNodes(m_history_index);
	std::string messageOfFocusCluster = generateMessageOfFocusCluster(m_history_index);

	sendMessageToClient(messageOfArticleNodes);
	sendMessageToClient(messageOfClusterResult);


	if (m_focus){
		sendMessageToClient(messageOfFocusCluster);
	}

	++m_history_index;

	if (m_history_index == m_v_history.size()){
		m_playback = false;
		m_socketserver->sendMessage(generateMessageOfPlayBack(false, m_history_index));
	}
}

std::string ServerProcessing::generateMessageOfClusterResult(int index, bool isChangingParameters)
{
	if (index >= m_v_history.size()){
		return "History index overflow.";
	}

	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Cluster;
	JSONVALUE JS_DateTime;

	std::vector<ArticleCluster *> result_clusters = m_v_history[index];

	if (result_clusters.size() == 0){
		return "ERROR in history clusters.";
	}

	for (size_t i=0; i<result_clusters.size(); ++i){
		ArticleCluster *tempCluster = result_clusters[i];
		JSONVALUE JS_tempCluster;
		JSONARRAY JS_position;
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[0]);
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[1]);

		JS_tempCluster["position"] = JS_position;
		JS_tempCluster["size"] = tempCluster->m_cluster_size;
		JS_tempCluster["id"] = tempCluster->m_cluster_id;
		JS_tempCluster["IsNew"] = tempCluster->m_IsNewCluster;
		JS_tempCluster["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
		JS_tempCluster["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
		JS_tempCluster["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();

		JSONARRAY JS_keywords;
		size_t keyword_size = m_keyword_size;
		if (tempCluster->m_v_cluster_keywords.size() < keyword_size){
			keyword_size = tempCluster->m_v_cluster_keywords.size();
		}
		for (size_t j=0; j<keyword_size; ++j){
			JSONVALUE JS_information_keywords;
			JS_information_keywords["value"] = tempCluster->m_v_cluster_keywords[j]->value;
			JS_information_keywords["occurrence"] = tempCluster->m_v_cluster_keywords[j]->occurrence;

			JS_keywords.push_back(JS_information_keywords);
			//JS_keywords.push_back(tempCluster->m_v_cluster_keywords[j]->value);
		}

		JS_tempCluster["keywords"] = JS_keywords;

		JSONARRAY JS_parents;
		for (size_t j=0; j<tempCluster->m_v_parent.size(); ++j){
			JS_parents.push_back(tempCluster->m_v_parent[j]);
		}
		JS_tempCluster["parents"] = JS_parents;

		JSONARRAY JS_articles;
		for (size_t j=0; j<tempCluster->m_v_index.size(); ++j){
			JSONVALUE tempNode;
			tempNode["index"] = tempCluster->m_v_index[j];
			tempNode["id"] = i;
			JS_articles.push_back(tempNode);
		}
		JS_tempCluster["articles"] = JS_articles;

		JSONARRAY JA_clusterCreateTime;
		JSONARRAY JA_lastUpdateTime;
		JSONARRAY JA_mostEarlyTime;

		std::vector<int> tempCreateTime = tempCluster->getCreateDateTime();

		for (int tempi = 0; tempi < tempCreateTime.size(); ++tempi) {
			JA_clusterCreateTime.push_back(tempCreateTime[tempi]);
		}

		std::vector<int> templastUpdateTime = tempCluster->m_cluster_newest_article->getDateTime();

		for (int tempi = 0; tempi < templastUpdateTime.size(); ++tempi) {
			JA_lastUpdateTime.push_back(templastUpdateTime[tempi]);
		}

		std::vector<int> tempmostEarlyTime = tempCluster->m_cluster_oldest_article->getDateTime();

		for (int tempi = 0; tempi < tempmostEarlyTime.size(); ++tempi) {
			JA_mostEarlyTime.push_back(tempmostEarlyTime[tempi]);
		}

		JS_tempCluster["clusterCreateTime"] = JA_clusterCreateTime;
		JS_tempCluster["lastUpdateTime"] = JA_lastUpdateTime;
		JS_tempCluster["mostEarlyTime"] = JA_mostEarlyTime;
		JS_Cluster.Push(JS_tempCluster);
	}

	std::vector<int> currentTime = result_clusters[0]->m_currentTime;

	for (size_t i=0; i<currentTime.size(); ++i){
		JS_DateTime.Push(currentTime[i]);
	}

	JS_MessageContent["clusterData"] = JS_Cluster;
	JS_MessageContent["dateTime"] = JS_DateTime;
	JS_MessageContent["changeParameter"] = isChangingParameters;

	JS_Result["messageType"] = "VIS_Cluster_Data";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfClusterResult(std::vector<ArticleCluster *> result_clusters)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Cluster;
	JSONVALUE JS_DateTime;

	if (result_clusters.size() == 0){
		return "ERROR in history clusters.";
	}

	for (size_t i=0; i<result_clusters.size(); ++i){
		ArticleCluster *tempCluster = result_clusters[i];
		JSONVALUE JS_tempCluster;
		JSONARRAY JS_position;
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[0]);
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[1]);

		JS_tempCluster["position"] = JS_position;
		JS_tempCluster["size"] = tempCluster->m_cluster_size;
		JS_tempCluster["id"] = tempCluster->m_cluster_id;
		JS_tempCluster["IsNew"] = tempCluster->m_IsNewCluster;
		JS_tempCluster["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
		JS_tempCluster["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
		JS_tempCluster["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();

		JSONARRAY JS_keywords;
		size_t keyword_size = m_keyword_size;
		if (tempCluster->m_v_cluster_keywords.size() < keyword_size){
			keyword_size = tempCluster->m_v_cluster_keywords.size();
		}
		for (size_t j=0; j<keyword_size; ++j){
			JSONVALUE JS_information_keywords;
			JS_information_keywords["value"] = tempCluster->m_v_cluster_keywords[j]->value;
			JS_information_keywords["occurrence"] = tempCluster->m_v_cluster_keywords[j]->occurrence;

			JS_keywords.push_back(JS_information_keywords);
			//JS_keywords.push_back(tempCluster->m_v_cluster_keywords[j]->value);
		}

		JS_tempCluster["keywords"] = JS_keywords;

		JSONARRAY JS_parents;
		for (size_t j=0; j<tempCluster->m_v_parent.size(); ++j){
			JS_parents.push_back(tempCluster->m_v_parent[j]);
		}
		JS_tempCluster["parents"] = JS_parents;

		JSONARRAY JS_articles;
		for (size_t j=0; j<tempCluster->m_v_index.size(); ++j){
			JSONVALUE tempNode;
			tempNode["index"] = tempCluster->m_v_index[j];
			tempNode["id"] = i;
			JS_articles.push_back(tempNode);
		}
		JS_tempCluster["articles"] = JS_articles;

		JSONARRAY JA_clusterCreateTime;
		JSONARRAY JA_lastUpdateTime;
		JSONARRAY JA_mostEarlyTime;

		std::vector<int> tempCreateTime = tempCluster->getCreateDateTime();

		for (int tempi = 0; tempi < tempCreateTime.size(); ++tempi) {
			JA_clusterCreateTime.push_back(tempCreateTime[tempi]);
		}

		std::vector<int> templastUpdateTime = tempCluster->m_cluster_newest_article->getDateTime();

		for (int tempi = 0; tempi < templastUpdateTime.size(); ++tempi) {
			JA_lastUpdateTime.push_back(templastUpdateTime[tempi]);
		}

		std::vector<int> tempmostEarlyTime = tempCluster->m_cluster_oldest_article->getDateTime();

		for (int tempi = 0; tempi < tempmostEarlyTime.size(); ++tempi) {
			JA_mostEarlyTime.push_back(tempmostEarlyTime[tempi]);
		}

		JS_tempCluster["clusterCreateTime"] = JA_clusterCreateTime;
		JS_tempCluster["lastUpdateTime"] = JA_lastUpdateTime;
		JS_tempCluster["mostEarlyTime"] = JA_mostEarlyTime;
		JS_Cluster.Push(JS_tempCluster);
	}

	std::vector<int> currentTime = result_clusters[0]->m_currentTime;

	for (size_t i=0; i<currentTime.size(); ++i){
		JS_DateTime.Push(currentTime[i]);
	}

	JS_MessageContent["clusterData"] = JS_Cluster;
	JS_MessageContent["dateTime"] = JS_DateTime;
	JS_MessageContent["changeParameter"] = true;

	JS_Result["messageType"] = "VIS_Cluster_Data";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
	/*
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Cluster;
	JSONVALUE JS_DateTime;

	if (result_clusters.size() == 0){
		return "ERROR in history clusters.";
	}

	for (size_t i=0; i<result_clusters.size(); ++i){
		ArticleCluster *tempCluster = result_clusters[i];
		JSONVALUE JS_tempCluster;
		JSONARRAY JS_position;
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[0]);
		JS_position.push_back(tempCluster->m_cluster_center_article->getPosition()[1]);

		JS_tempCluster["position"] = JS_position;
		JS_tempCluster["size"] = tempCluster->m_cluster_size;
		JS_tempCluster["id"] = tempCluster->m_cluster_id;
		JS_tempCluster["IsNew"] = tempCluster->m_IsNewCluster;
		JS_tempCluster["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
		JS_tempCluster["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
		JS_tempCluster["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();

		JSONARRAY JS_keywords;
		size_t keyword_size = m_keyword_size;
		if (tempCluster->m_v_cluster_keywords.size() < keyword_size){
			keyword_size = tempCluster->m_v_cluster_keywords.size();
		}
		for (size_t j=0; j<keyword_size; ++j){
			JSONVALUE JS_information_keywords;
			JS_information_keywords["value"] = tempCluster->m_v_cluster_keywords[j]->value;
			JS_information_keywords["occurrence"] = tempCluster->m_v_cluster_keywords[j]->occurrence;

			JS_keywords.push_back(JS_information_keywords);
			//JS_keywords.push_back(tempCluster->m_v_cluster_keywords[j]->value);
		}

		JS_tempCluster["keywords"] = JS_keywords;

		JSONARRAY JS_parents;
		for (size_t j=0; j<tempCluster->m_v_parent.size(); ++j){
			JS_parents.push_back(tempCluster->m_v_parent[j]);
		}
		JS_tempCluster["parents"] = JS_parents;

		JSONARRAY JS_articles;
		for (size_t j=0; j<tempCluster->m_v_index.size(); ++j){
			JSONVALUE tempNode;
			tempNode["index"] = tempCluster->m_v_index[j];
			tempNode["id"] = i;
			JS_articles.push_back(tempNode);
		}
		JS_tempCluster["articles"] = JS_articles;


		JS_tempCluster["clusterCreateTime"] = tempCluster->m_cluster_create_time_string;
		JS_tempCluster["lastUpdateTime"] = tempCluster->m_cluster_newest_article->getDateTimeString();
		JS_Cluster.Push(JS_tempCluster);
	}

	std::vector<int> currentTime = result_clusters[0]->m_currentTime;

	for (size_t i=0; i<currentTime.size(); ++i){
		JS_DateTime.Push(currentTime[i]);
	}

	JS_MessageContent["clusterData"] = JS_Cluster;
	JS_MessageContent["dateTime"] = JS_DateTime;
	JS_MessageContent["changeParameter"] = true;

	JS_Result["messageType"] = "VIS_Cluster_Data";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
	*/
}

std::string ServerProcessing::generateMessageOfArticleNodes(int index)
{
	if (index >= m_v_history.size()){
		return "History index overflow.";
	}

	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_ArticleNodes;
	JSONARRAY JS_FocI;

	int temp_size = m_storytelling->getActivedNumber();
	std::vector<Article *> temp_data = m_storytelling->getArticleVector();

	std::vector<ArticleCluster *> result_clusters = m_v_history[index];

	for (size_t i=0; i<temp_data.size(); ++i){
		temp_data[i]->m_cluster_id = -1;
		temp_data[i]->m_foci_id = -1;

	}

	if (result_clusters.size() == 0){
		return "ERROR in history clusters.";
	}

	for (size_t i=0; i<result_clusters.size(); ++i){
		ArticleCluster *tempCluster = result_clusters[i];

		JSONVALUE tempClusterPosition;
		tempClusterPosition["x"] = tempCluster->m_cluster_center_article->getPosition()[0];
		tempClusterPosition["y"] = tempCluster->m_cluster_center_article->getPosition()[1];

		JS_FocI.push_back(tempClusterPosition);

		for (size_t j=0; j<tempCluster->m_v_index.size(); ++j){
			temp_data[tempCluster->m_v_index[j]]->m_cluster_id = tempCluster->m_cluster_id;
			temp_data[tempCluster->m_v_index[j]]->m_foci_id = i;
		}
	}



	for (size_t i=0; i<temp_data.size(); ++i){
		JSONVALUE JS_node;
		JSONARRAY JS_node_position;
		JS_node_position.push_back(temp_data[i]->getPosition()[0]);
		JS_node_position.push_back(temp_data[i]->getPosition()[1]);
		JS_node["id"] = temp_data[i]->m_foci_id;
		JS_node["cid"] = temp_data[i]->m_cluster_id;
		JS_node["pos"] = JS_node_position;
		if (i >= temp_size - 4 && i < temp_size){
			JS_node["age"] = 1.0;
		}
		else if (temp_size - 4 > i && i >= temp_size - 8){
			JS_node["age"] = 0.9;
		}
		else if (temp_size - 8 > i && i >= temp_size - 12){
			JS_node["age"] = 0.7;
		}
		else{
			JS_node["age"] = 0.6;
		}

		JS_ArticleNodes.Push(JS_node);
	}

	JS_MessageContent["articleNodes"] = JS_ArticleNodes;
	JS_MessageContent["foci"] = JS_FocI;

	JS_Result["messageType"] = "VIS_Article_Nodes";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
}


std::string ServerProcessing::generateMessageOfArticleNodes(std::vector<ArticleCluster *> result_clusters)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_ArticleNodes;
	JSONARRAY JS_FocI;

	int temp_size = m_storytelling->getActivedNumber();
	std::vector<Article *> temp_data = m_storytelling->getArticleVector();

	for (size_t i=0; i<temp_data.size(); ++i){
		temp_data[i]->m_cluster_id = -1;
		temp_data[i]->m_foci_id = -1;

	}

	if (result_clusters.size() == 0){
		return "ERROR in history clusters.";
	}

	for (size_t i=0; i<result_clusters.size(); ++i){
		ArticleCluster *tempCluster = result_clusters[i];

		JSONVALUE tempClusterPosition;
		tempClusterPosition["x"] = tempCluster->m_cluster_center_article->getPosition()[0];
		tempClusterPosition["y"] = tempCluster->m_cluster_center_article->getPosition()[1];

		JS_FocI.push_back(tempClusterPosition);

		for (size_t j=0; j<tempCluster->m_v_index.size(); ++j){
			temp_data[tempCluster->m_v_index[j]]->m_cluster_id = tempCluster->m_cluster_id;
			temp_data[tempCluster->m_v_index[j]]->m_foci_id = i;
		}
	}



	for (size_t i=0; i<temp_data.size(); ++i){
		JSONVALUE JS_node;
		JSONARRAY JS_node_position;
		JS_node_position.push_back(temp_data[i]->getPosition()[0]);
		JS_node_position.push_back(temp_data[i]->getPosition()[1]);
		JS_node["id"] = temp_data[i]->m_foci_id;
		JS_node["cid"] = temp_data[i]->m_cluster_id;
		JS_node["pos"] = JS_node_position;
		if (i >= temp_size - 4 && i < temp_size){
			JS_node["age"] = 1.0;
		}
		else if (temp_size - 4 > i && i >= temp_size - 8){
			JS_node["age"] = 0.8;
		}
		else if (temp_size - 8 > i && i >= temp_size - 12){
			JS_node["age"] = 0.7;
		}
		else{
			JS_node["age"] = 0.3;
		}

		JS_ArticleNodes.Push(JS_node);
	}

	JS_MessageContent["articleNodes"] = JS_ArticleNodes;
	JS_MessageContent["foci"] = JS_FocI;
	JS_MessageContent["changeParameter"] = true;

	JS_Result["messageType"] = "VIS_Article_Nodes";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfArticlePosition()
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_ArticlePosition;

	int temp_size = m_storytelling->getActivedNumber();
	std::vector<Article *> temp_data = m_storytelling->getArticleVector();

	for (size_t i=0; i<temp_size; ++i){
		JSONARRAY JS_node;
		JS_node.push_back(temp_data[i]->getPosition()[0]);
		JS_node.push_back(temp_data[i]->getPosition()[1]);
		if (i >= temp_size - 5){
			JS_node.push_back(3);
		}
		else if (temp_size - 5 > i && i >= temp_size - 10){
			JS_node.push_back(2);
		}
		else if (temp_size - 10 > i && i >= temp_size - 15){
			JS_node.push_back(1);
		}
		else{
			JS_node.push_back(0);
		}
		
		JS_ArticlePosition.Push(JS_node);
	}

	JS_MessageContent["articlePosition"] = JS_ArticlePosition;

	JS_Result["messageType"] = "VIS_Article_Data";
	JS_Result["messageContent"] = JS_MessageContent;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfFocusCluster(int clusterID, int historyID)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Information;

	ArticleCluster* targetCluster = NULL;
	ArticleCluster* previousCluster = NULL;

	for(int i=0; i<m_v_history[historyID].size(); ++i){
		if (m_v_history[historyID][i]->m_cluster_id == clusterID){
			targetCluster = m_v_history[historyID][i];
		}
	}

	if (historyID > 0){
		for(int i=0; i<m_v_history[historyID-1].size(); ++i){
			if (m_v_history[historyID-1][i]->m_cluster_id == clusterID){
				previousCluster = m_v_history[historyID-1][i];
			}
		}
	}

	if (targetCluster == NULL){
		JS_Result["messageType"] = "VIS_Focus_Context";
		JS_Information["isExist"] = false;
		JS_Result["messageContent"] = JS_Information;
	}else{
		JS_Result["messageType"] = "VIS_Focus_Context";
		JS_Information["isExist"] = true;
		if (previousCluster == NULL){
			JS_Information["previousIsExist"] = false;
		}
		else{
			JS_Information["previousIsExist"] = true;
		}

		JSONVALUE JS_Keywords_target;
		JSONVALUE JS_Keywords_previous;

		size_t keyword_size = m_keyword_size;
		if (targetCluster->m_v_cluster_keywords.size() < keyword_size){
			keyword_size = targetCluster->m_v_cluster_keywords.size();
		}

		for (int i=0; i<keyword_size; ++i){
			JSONVALUE keywordItem;
			keywordItem["occurence"] = targetCluster->m_v_cluster_keywords[i]->occurrence;
			keywordItem["value"] = targetCluster->m_v_cluster_keywords[i]->value;
			JS_Keywords_target.Push(keywordItem);
		}

		if (previousCluster != NULL){
			size_t keyword_size = m_keyword_size;
			if (previousCluster->m_v_cluster_keywords.size() < keyword_size){
				keyword_size = previousCluster->m_v_cluster_keywords.size();
			}

			for (int i=0; i<keyword_size; ++i){
				JSONVALUE keywordItem;
				keywordItem["occurence"] = previousCluster->m_v_cluster_keywords[i]->occurrence;
				keywordItem["value"] = previousCluster->m_v_cluster_keywords[i]->value;
				JS_Keywords_previous.Push(keywordItem);
			}
		}

		//targetCluster->rankArticlesByPageRank();

		JSONVALUE JS_Sub_Cluster;
		std::vector<ArticleCluster *> subClusters = targetCluster->generateSubCluster();
		// DO SOMETHING FOR SUB CLUSTER.
		/*
		{
		"name": "root",
		"children": [
		{
		"name": "keyword1",
		"children": [


		{"name": "AgglomerativeCluster", "size": 3938},
		{"name": "CommunityStructure", "size": 3812},
		{"name": "HierarchicalCluster", "size": 6714},
		{"name": "MergeEdge", "size": 743}


		]
		},
		{
		"name": "graph",
		"children": [

		{"name": "BetweennessCentrality", "size": 3534},
		{"name": "LinkDistance", "size": 5731},
		{"name": "MaxFlowMinCut", "size": 7840},
		{"name": "ShortestPaths", "size": 5914},
		{"name": "SpanningTree", "size": 3416}
		]
		}

		]
		}
		*/
		
		JSONVALUE JS_Sub_Children;

		for (size_t i=0; i<subClusters.size(); ++i){
			ArticleCluster *tempCluster = subClusters[i];
			JSONVALUE JS_item;
			JS_item["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
			//tempCluster->rankArticlesByPageRank();
			JS_item["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
			JS_item["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();
			JS_item["size"] = tempCluster->m_v_cluster_articles.size();
			JS_item["subID"] = i;
			JS_item["lastupdateTime"] = tempCluster->m_cluster_newest_article->getDateTimeString();

			JSONVALUE JS_sub_keywords;

			size_t tempSize = 5;
			if (tempSize > tempCluster->m_v_cluster_keywords.size()){
				tempSize = tempCluster->m_v_cluster_keywords.size();
			}

			for (size_t j=0; j<tempSize; ++j){
				JS_sub_keywords.Push(tempCluster->m_v_cluster_keywords[j]->value);
			}
			JS_item["keywords"] = JS_sub_keywords;
			JS_Sub_Children.Push(JS_item);
		}

		JS_Sub_Cluster["f_title"] = targetCluster->m_cluster_center_article->getHeadline();
		JS_Sub_Cluster["p_title"] = targetCluster->m_v_cluster_articles[0]->getHeadline();
		JS_Sub_Cluster["g_title"] = targetCluster->m_cluster_newest_article->getHeadline();
		JS_Sub_Cluster["children"] = JS_Sub_Children;


		JS_Information["currentCluster"] = JS_Keywords_target;
		JS_Information["previousCluster"] = JS_Keywords_previous;
		JS_Information["subCluster"] = JS_Sub_Cluster;
		JS_Result["messageContent"] = JS_Information;
	}



	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfFocusCluster(std::vector<ArticleCluster *> result_clusters)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Information;

	ArticleCluster* targetCluster = NULL;
	ArticleCluster* previousCluster = NULL;

	JS_Result["messageType"] = "VIS_Focus_Context";

	for(int i=0; i<result_clusters.size(); ++i){
		targetCluster = result_clusters[i];

		JSONVALUE JS_Sub_Cluster;
		JSONVALUE JS_Sub_Children;

		std::vector<ArticleCluster *> subClusters = targetCluster->generateSubCluster();		

		for (size_t i=0; i<subClusters.size(); ++i){
			ArticleCluster *tempCluster = subClusters[i];
			JSONVALUE JS_item;
			JS_item["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
			JS_item["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
			JS_item["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();
			JS_item["size"] = tempCluster->m_v_cluster_articles.size();
			JS_item["subID"] = i;
			JS_item["lastupdateTime"] = tempCluster->m_cluster_newest_article->getDateTimeString();

			JSONVALUE JS_sub_keywords;

			size_t tempSize = 5;
			if (tempSize > tempCluster->m_v_cluster_keywords.size()){
				tempSize = tempCluster->m_v_cluster_keywords.size();
			}

			for (size_t j=0; j<tempSize; ++j){
				JS_sub_keywords.Push(tempCluster->m_v_cluster_keywords[j]->value);
			}
			JS_item["keywords"] = JS_sub_keywords;
			JS_Sub_Children.Push(JS_item);
		}

		JS_Sub_Cluster["f_title"] = targetCluster->m_cluster_center_article->getHeadline();
		JS_Sub_Cluster["p_title"] = targetCluster->m_v_cluster_articles[0]->getHeadline();
		JS_Sub_Cluster["g_title"] = targetCluster->m_cluster_newest_article->getHeadline();
		JS_Sub_Cluster["children"] = JS_Sub_Children;

		if (targetCluster != NULL) {
			JS_Information[std::to_string(result_clusters[i]->m_cluster_id)] = JS_Sub_Cluster;
		}
	}

	JS_Result["messageContent"] = JS_Information;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfFocusCluster(int historyID)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Information;

	ArticleCluster* targetCluster = NULL;
	ArticleCluster* previousCluster = NULL;

	JS_Result["messageType"] = "VIS_Focus_Context";

	if (m_v_history.size() == 0) return "";

	for(int i=0; i<m_v_history[historyID].size(); ++i){
		targetCluster = m_v_history[historyID][i];

		JSONVALUE JS_Sub_Cluster;
		JSONVALUE JS_Sub_Children;

		std::vector<ArticleCluster *> subClusters = targetCluster->generateSubCluster();		

		for (size_t i=0; i<subClusters.size(); ++i){
			ArticleCluster *tempCluster = subClusters[i];
			JSONVALUE JS_item;
			JS_item["f_title"] = tempCluster->m_cluster_center_article->getHeadline();
			JS_item["p_title"] = tempCluster->m_v_cluster_articles[0]->getHeadline();
			JS_item["g_title"] = tempCluster->m_cluster_newest_article->getHeadline();
			JS_item["size"] = tempCluster->m_v_cluster_articles.size();
			JS_item["subID"] = i;
			JS_item["lastupdateTime"] = tempCluster->m_cluster_newest_article->getDateTimeString();

			JSONVALUE JS_sub_keywords;

			size_t tempSize = 5;
			if (tempSize > tempCluster->m_v_cluster_keywords.size()){
				tempSize = tempCluster->m_v_cluster_keywords.size();
			}

			for (size_t j=0; j<tempSize; ++j){
				JS_sub_keywords.Push(tempCluster->m_v_cluster_keywords[j]->value);
			}
			JS_item["keywords"] = JS_sub_keywords;
			JS_Sub_Children.Push(JS_item);
		}

		JS_Sub_Cluster["f_title"] = targetCluster->m_cluster_center_article->getHeadline();
		JS_Sub_Cluster["p_title"] = targetCluster->m_v_cluster_articles[0]->getHeadline();
		JS_Sub_Cluster["g_title"] = targetCluster->m_cluster_newest_article->getHeadline();
		JS_Sub_Cluster["children"] = JS_Sub_Children;

		if (targetCluster != NULL) {
			JS_Information[std::to_string(m_v_history[historyID][i]->m_cluster_id)] = JS_Sub_Cluster;
		}
	}

	JS_Result["messageContent"] = JS_Information;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfPlayBack(bool IsPlayBacking, int current_history)
{
	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Information;

	JS_Result["messageType"] = "VIS_Play_Back";
	JS_Information["isPlayBacking"] = IsPlayBacking;
	JS_Information["currentHistoryIndex"] = current_history;
	JS_Result["messageContent"] = JS_Information;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

std::string ServerProcessing::generateMessageOfArticleDetail(int currentIndex, int sentIndex)
{



	JSONVALUE JS_Result;
	JSONVALUE JS_MessageContent;
	JSONVALUE JS_Information;

	JS_Result["messageType"] = "VIS_Article_Detail";

	std::vector<Article *> temp_article_vector = m_storytelling->getArticleVector();

	for (size_t i=sentIndex; i<currentIndex; ++i){
		JSONARRAY tempItem;
		//tempItem.push_back(i);
		JSONARRAY tempDateTime;
		for (int tempi=0; tempi<temp_article_vector[i]->getDateTime().size(); ++tempi) {
			tempDateTime.push_back(temp_article_vector[i]->getDateTime()[tempi]);
		}
		tempItem.push_back(tempDateTime); // 0
		tempItem.push_back(temp_article_vector[i]->getHeadline()); // 1

		std::vector<Keyword *> temp_keyword_vector = temp_article_vector[i]->getKeywords();
		JSONARRAY tempKeywordItem;
		for (size_t j=0; j<temp_keyword_vector.size(); ++j){
			tempKeywordItem.push_back(temp_keyword_vector[j]->value);
		}
		tempItem.push_back(tempKeywordItem); // 2
		//tempItem.push_back(temp_article_vector[i]->getDateTimeString());
		tempItem.push_back(temp_article_vector[i]->getLeadParagraph()); // 3

		tempItem.push_back(temp_article_vector[i]->m_cluster_id); //4

		JS_Information.Push(tempItem);
	}

	JS_Result["messageContent"] = JS_Information;

	std::string result;
	JS_Result.ToString(result);
	return result;
}

ServerProcessing::~ServerProcessing(void)
{
}

void ServerProcessing::sendMessageToClient(std::string newMessage)
{
	m_socketserver->sendMessage(newMessage);
}

std::string ServerProcessing::getMessageFromServer()
{
	std::string message = m_socketserver->getMessage();

	if (m_previous_message == message){
		return "";
	}
	else{
		m_previous_message = message;
		return message;
	} 
}
