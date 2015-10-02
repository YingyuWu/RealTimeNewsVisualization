#pragma once

#include "Json.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

class Keyword;

class Article
{
public:
	Article(JSONVALUE newJSONArticle, int newArticleID);
	~Article();

	void setActiveKeywordVector(std::vector<Keyword *>);

	bool isExistIndex(size_t index);

	std::string getHeadline();
	std::string getLeadParagraph();
	std::vector<int> getDateTime();
	std::string getDateTimeString();
	std::vector<Keyword *> getKeywords();
	std::vector<size_t> getKeywordsIndex();
	size_t getKeywordsNumber();
	std::vector<double> &getPosition();
	std::vector<double> &getVel();
	std::vector<double> &getAcc();
	std::vector<double> &getForce();
	std::vector<double> &getAccSign();
	double getMass();	

	bool m_InCluster;

	int m_cluster_id;
	int m_foci_id;

	double m_pagerank_probability;
	double m_pagerank_previous_probability;

private:
	int m_article_id;

	std::string m_headline;
	std::string m_lead_paragraph;

	std::vector<Keyword *> m_v_keywords;
	std::vector<int> m_pub_date; // year, month, day, hour, min, sec.
	std::string m_pub_date_string;
	size_t m_keywords_number;

	std::vector<double> m_position;
	std::vector<double> m_vel;
	std::vector<double> m_acc;
	std::vector<double> m_force;
	std::vector<double> m_accSign;

	std::vector<size_t> m_v_keywords_index;

	double m_mass;

	int m_dimension;

private:
	bool datetimeParser(std::string pub_date, std::vector<int> &my_date);
	bool keywordsParser(JSONVALUE keywords, std::vector<Keyword *> &my_keywords);
	
};

class Keyword
{
public:
	std::string value;
	int rank;
	std::string is_major;
	std::string name;

	bool isExist;
	double hot_value;
	double occurrence;
	double weight;

	Keyword(){
		value = "";
		rank = 0;
		is_major = "";
		name = "";

		isExist = false;
		hot_value = 1;
		occurrence = 1;
		weight = 0.0;
	}

	Keyword(const Keyword& newKeyword){
		value = newKeyword.value;
		rank = newKeyword.rank;
		is_major = newKeyword.is_major;
		name = newKeyword.name;

		isExist = false;
		hot_value = 1;
		occurrence = 1;
		weight = 0.0;
	}

	Keyword(const Keyword *newKeyword){
		value = newKeyword->value;
		rank = newKeyword->rank;
		is_major = newKeyword->is_major;
		name = newKeyword->name;

		isExist = false;
		hot_value = newKeyword->hot_value;
		occurrence = newKeyword->occurrence;
		weight = newKeyword->weight;
	}

	Keyword(std::string newValue, std::string newRank, std::string newName, std::string newIs_major){
		if (newValue != "")
			value = std::string(++newValue.begin(), --newValue.end());
		//if (newName != "")
			name = "name";//std::string(++newName.begin(), --newName.end());

		//if (newIs_major != "")
			is_major = "Y";//std::string(++newIs_major.begin(), --newIs_major.end());
		
		//if (newRank != "")
			rank = 0;//atoi(std::string(++newRank.begin(), --newRank.end()).c_str());

		isExist = false;
		hot_value = 1;
		occurrence = 1;
		weight = 0.0;
	}

	~Keyword(){}
};

