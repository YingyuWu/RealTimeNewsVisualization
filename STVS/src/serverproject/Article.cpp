#include "Article.h"

Article::Article(JSONVALUE newJSONArticle, int newArticleID)
{
	m_dimension = 2;
	m_cluster_id = -1;
	m_foci_id = -1;

	// Initial ID
	// -----------------------------------------------------------------------------------
	m_article_id = newArticleID;

	// Initial Headline
	// -----------------------------------------------------------------------------------
	newJSONArticle["headline"].ToString(m_headline); 

	// Initial Lead Paragraph
	// -----------------------------------------------------------------------------------
	newJSONArticle["lead_paragraph"].ToString(m_lead_paragraph);

	// Initial DateTime
	// -----------------------------------------------------------------------------------
	newJSONArticle["pub_date"].ToString(m_pub_date_string);
	datetimeParser(m_pub_date_string, m_pub_date); 

	// Initial Keywords list
	// -----------------------------------------------------------------------------------
	keywordsParser(newJSONArticle["keywords"], m_v_keywords);
	m_keywords_number = m_v_keywords.size();

	for (int i=0; i<m_dimension; ++i){
		m_position.push_back(0.0);
		m_vel.push_back(0.0);
		m_acc.push_back(0.0);
		m_force.push_back(0.0);
		m_accSign.push_back(1.0);
	}

	m_mass = 1.0;

	m_pagerank_probability = 0;
	m_pagerank_previous_probability = 0;
	m_InCluster = false;	
}

Article::~Article()
{
	int stop = 0;
}

void Article::setActiveKeywordVector(std::vector<Keyword *> newActiveKeywordVector)
{
	m_v_keywords_index.clear();

	for (size_t i=0; i<newActiveKeywordVector.size(); ++i){

		for (size_t j=0; j<m_v_keywords.size(); ++j){
			if (newActiveKeywordVector[i]->value == m_v_keywords[j]->value){
				m_v_keywords_index.push_back(i);
				break;
			}
		}
	}
}

bool Article::isExistIndex(size_t index)
{
	for (size_t i=0; i<m_v_keywords_index.size(); ++i){
		if (index == m_v_keywords_index[i])
			return true;
	}

	return false;
}

std::string Article::getHeadline()
{
	return m_headline;
}

std::string Article::getLeadParagraph()
{
	return m_lead_paragraph;
}

std::vector<int> Article::getDateTime()
{
	return m_pub_date;
}

std::string Article::getDateTimeString()
{
	return m_pub_date_string;
}

std::vector<Keyword *> Article::getKeywords()
{
	return m_v_keywords;
}

std::vector<size_t> Article::getKeywordsIndex()
{
	return m_v_keywords_index;
}

size_t Article::getKeywordsNumber()
{
	return m_keywords_number;
}

std::vector<double> &Article::getPosition()
{
	return m_position;
}

std::vector<double> &Article::getVel()
{
	return m_vel;
}
std::vector<double> &Article::getAcc()
{
	return m_acc;
}

std::vector<double> &Article::getForce()
{
	return m_force;
}

std::vector<double> &Article::getAccSign()
{
	return m_accSign;
}

double Article::getMass()
{
	return m_mass;
}

bool Article::datetimeParser(std::string pub_date, std::vector<int> &my_date) 
{
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

bool Article::keywordsParser(JSONVALUE keywords, std::vector<Keyword *> &my_keywords)
{	
	for (int i=0; i<keywords.Size(); i++){
		JSONVALUE oneJSONWord;

		if (keywords.At(i, oneJSONWord)){
			/*
			std::string tempvalue;
			std::string temprank;
			std::string tempname;
			std::string tempis_major;

			oneJSONWord["value"].ToString(tempvalue);
			oneJSONWord["rank"].ToString(temprank);
			oneJSONWord["name"].ToString(tempname);
			oneJSONWord["is_major"].ToString(tempis_major);

			if (tempvalue == "") continue;
			if (tempvalue == "\"Obama, Barack\"") {
				continue;
			}
			if (tempvalue == "\"comput\"") {
				continue;
			}
			if (tempvalue == "\"obesity\"" || tempvalue == "\"Obesity\"") {
				continue;
			}

			Keyword *oneword = new Keyword(tempvalue, temprank, tempname, tempis_major);

			my_keywords.push_back(oneword);
			*/

			std::string tempvalue;

			oneJSONWord.ToString(tempvalue);

			if (tempvalue == "") continue;
			if (tempvalue == "\"Obama, Barack\"") {
				continue;
			}
			if (tempvalue == "\"comput\"") {
				continue;
			}
			if (tempvalue == "\"obesity\"" || tempvalue == "\"Obesity\"") {
				continue;
			}

			Keyword *oneword = new Keyword(tempvalue, " ", " ", " ");

			my_keywords.push_back(oneword);
		}

	}
	return true;
}