#pragma once
#include "Article.h"
#include "Delaunay.h"
#include <algorithm>
#include <iostream>

class ArticleCluster
{
public:
	ArticleCluster(size_t clusterID, std::vector<size_t> v_index, std::vector<Article *> v_articles, std::vector<int> currentTime);
	ArticleCluster(const ArticleCluster *source);
	~ArticleCluster(void);

	bool generateKeywordsVector();
	Article *generateCenterArticle();

	double generateSimilarity(Article *target, Article *source);
	void rankArticlesByPageRank();
	
	std::vector<ArticleCluster *> generateSubCluster();

	double purityFromParent();
	bool datetimeParser(std::string pub_date, std::vector<int> &my_date);

	std::vector<int> getCreateDateTime();
	std::vector<int> getMostEarlyTime();

	int m_cluster_id;
	std::vector<size_t> m_v_index;
	std::vector<Article *> m_v_cluster_articles;
	std::vector<Keyword *> m_v_cluster_keywords;
	Article *m_cluster_center_article;
	Article *m_cluster_newest_article;
	Article *m_cluster_oldest_article;
	size_t m_cluster_size;
	std::vector<int> m_currentTime;

	bool m_IsNewCluster;
	int m_matched_id_from_parent;
	std::vector<int> m_v_parent;

	size_t m_newest_index;
	size_t m_oldest_index;
	std::string m_cluster_create_time_string;

	
};
