#pragma once
#include "StoryTelling.h"
#include "Hungarian.h"

class ServerProcessing
{
public:
	ServerProcessing(int port, std::string ip);
	~ServerProcessing(void);

	void run();
	void run(int secondInterval);

private:
	std::vector<Article *> generateDataCollection(std::string filename);
	bool messageProcessing(JSONVALUE newJsonMessage);

	std::string dataTimeToString(std::vector<int>);
	bool generateTracingClusterResultForCurrentCluster();
	
	bool backupCurrentHistory();
	bool clearAllHistory();

	void goAhead();
	void goBack();

	std::string generateMessageOfClusterResult(int index, bool isChangingParameters);
	std::string generateMessageOfClusterResult(std::vector<ArticleCluster *> result_clusters);

	std::string generateMessageOfArticleNodes(int index);
	std::string generateMessageOfArticleNodes(std::vector<ArticleCluster *> result_clusters);
	std::string generateMessageOfArticlePosition();

	std::string generateMessageOfFocusCluster(int clusterID, int historyID);
	std::string generateMessageOfFocusCluster(int historyID);
	std::string generateMessageOfFocusCluster(std::vector<ArticleCluster *> result_clusters);

	std::string generateMessageOfPlayBack(bool IsPlayBacking, int current_history);

	std::string generateMessageOfArticleDetail(int currentIndex, int sentIndex);

private:
	void sendMessageToClient(std::string newMessage);
	std::string getMessageFromServer();

	std::string m_previous_message;
	bool m_pause;
	bool m_playback;
	bool m_focus;

	int m_focus_id;
	int m_sentArticleIndex;

private: 
	int m_history_index;
	int m_current_index;
	int m_speed;

	int m_keyword_size;

	JSONVALUE m_current_message;

	StoryTelling *m_storytelling;
	websocketserver *m_socketserver;

	std::vector<ArticleCluster *> m_v_current_clusters;
	std::vector<std::vector<ArticleCluster *>> m_v_history;
	std::vector<int> m_v_history_index;


	std::vector<std::string> m_v_cluster_create_time;
};
