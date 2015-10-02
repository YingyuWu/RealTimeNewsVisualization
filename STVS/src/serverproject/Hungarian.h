#pragma once

#include <stdio.h>
#include "StoryTelling.h"

class Hungarian
{
public:
	Hungarian(std::vector<ArticleCluster *> source_vector, std::vector<ArticleCluster *> target_vector);
	Hungarian(std::vector<ArticleCluster *> source_vector, std::vector<ArticleCluster *> target_vector, int oldversion);
	~Hungarian(void);

	// like find 7 cluster matched in the top 10 clusters for consistence..
	// find m_matched_result_size in m_matched_consider_size;
	// -----------------------------------------------------------------------------------
	int m_matched_result_size; 
	int m_matched_consider_size; 
};