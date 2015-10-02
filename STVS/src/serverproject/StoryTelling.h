#pragma once
#include "websocketserver.h"
#include "Json.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <algorithm>

#include "Delaunay.h"
#include "Article.h"
#include "ArticleCluster.h"

class StoryTelling
{
public:
	StoryTelling(std::vector<Article *> new_v_articles, int currentIndex);
	~StoryTelling();

	void runTo(std::string enddateString);
	void runTo(std::vector<int> end_date);
	void runTo(int index);
	bool runToDateByIndex(int index);
	bool runToNextInterval();

	void setInterval(int day, int hour, int min, int second);

	int m_day;
	int m_hour;
	int m_min;
	int m_second;

	std::vector<Article *> getArticleVector();
	size_t getActivedNumber();

	std::string generateClusterPositionString();

	void setClusterThreshouldCoefficient(double newThresholdCoefficient);
	void setClusterSizeThreshouldCoefficient(double newSizeThresholdCoefficient);

	std::vector<ArticleCluster *> getArticleClusterVector();

	std::vector<int> m_currentTime;
	std::string m_str_currentTime;
public:
	bool generateActiveKeywordsVector(std::vector<int> end_date);
	bool generateActiveKeywordsVector(int index);
	bool updateActiveKeywordsFrequency();
	bool calculateKeywordsWeight();
	bool generateSimilarityMatrix();

	void evolveSample(double dt, double alpha, double stopDistance, double minDistance, double maxDistance);
	void computerForces(double alpha, double minDistance, double maxDistance);

	void clusterActivedArticle();
	void initilizeVertexset();
	void clusterFromOneEdge(edge startEdge, std::vector<size_t> &one_cluster, std::vector<edge> &temp_v_edgeset);

	// Test Function
	void outputPosition(std::string outputFileName);
	void outputClusterPosition(std::string outputFileName);

	void generateTheMostSimilarCluster(std::vector<ArticleCluster *> &matched_v_clusters);
	double calculateTheSimilarityOfClusters(ArticleCluster *target, ArticleCluster *Source, int similarityType);

private:
	bool datetimeParser(std::string pub_date, std::vector<int> &my_date);
	bool datetimeStrParser(std::vector<int> my_date, std::string &pub_date);
	bool compareDate(const std::vector<int> earlierDate, std::vector<int> olderDate); //True : earlierDate <= olderDate. False : earlierDate > olderDate
	double dotProduct(double arr1[],double arr2[],int length);
	

public:
	std::vector<Article *> m_v_articles;
	std::vector<Article *> m_v_current_articles;

	std::vector<Keyword *> m_v_active_keywords;
	int m_article_numbers;
	size_t m_actived_article_numbers;
	double *m_similarity_matrix;

	int m_dimension;
	double m_Energy;

	size_t m_slide_window_start_index;
	int m_slide_window_size;
	int m_top_keyword_number;
	int m_top_cluster_number;

private:
	// Triangulation Related Variable.
	vertexSet m_vertex_set;
	Delaunay *my_trangulation;
	triangleSet my_triangleset;
	edgeSet my_edgeset;

	size_t m_cluster_visualization_threshold;
	double m_cluster_threshold;
	double m_cluster_threshold_coefficient;
	std::vector<vector<size_t>> m_v_index_clusters;
	std::vector<ArticleCluster *> m_v_clusters;
	std::vector<ArticleCluster *> m_v_previous_clusters;
};