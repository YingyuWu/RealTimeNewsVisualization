"""
Grace Lu 9/3/14
based on book "Mining the Social Web" by Matthew Russell

Finds tweets containing query words, extracts nouns and verbs from tweets, and puts them into format to be used for Xiaoke's news visualization platform.

Note: 
This version requires a query work to be entered.
Tweets that contain the exact same text (ex. a retweeted tweet but by a different user) are not removed, though this is possible.

Updates:
All words are lower case. 
Query word is not included as a key word.
"""

import twitter
from collections import Counter
import json
import nltk
import cProfile
import tweepy
import ast
import time

#login to Twitter
def oauth_login():
    # XXX: Go to http://twitter.com/apps/new to create an app and get values # for these credentials that you'll need to provide in place of these
    # empty string values that are defined as placeholders.
    # See https://dev.twitter.com/docs/auth/oauth for more information
    # on Twitter's OAuth implementation.
    CONSUMER_KEY = '4FtnZIPVDrsHHj8wO8amoQ'
    CONSUMER_SECRET = '2h08yY97XElbKK8ZNQypI8wJtCewtMDD8j0Xc3DdfkU'
    OAUTH_TOKEN = '582174814-X5NJBNnghiXq5O9XQuCl9NN0GxWa2CyFhLWrwwRG'
    OAUTH_TOKEN_SECRET = 'aqXMX8Jy9uvG04zKB8kboaPUD6a5KfmmZGhfIdN74'
    
    auth = twitter.oauth.OAuth(OAUTH_TOKEN, OAUTH_TOKEN_SECRET,
                               CONSUMER_KEY, CONSUMER_SECRET)
                               
    twitter_api = twitter.Twitter(auth=auth)
    return twitter_api


#query Twitter
def twitter_search(twitter_api, q, max_results=20000, **kw):
    # See https://dev.twitter.com/docs/api/1.1/get/search/tweets and
    # https://dev.twitter.com/docs/using-search for details on advanced
    # search criteria that may be useful for keyword arguments
    # See https://dev.twitter.com/docs/api/1.1/get/search/tweets
    search_results = twitter_api.search.tweets(q=q, count=100, **kw)
    statuses = search_results['statuses']
    # Iterate through batches of results by following the cursor until we
    # reach the desired number of results, keeping in mind that OAuth users
    # can "only" make 180 search queries per 15-minute interval. See
    # https://dev.twitter.com/docs/rate-limiting/1.1/limits
    # for details. A reasonable number of results is ~1000, although
    # that number of results may not exist for all queries.
    # Enforce a reasonable limit

    #max_results = min(1000, max_results)
    for _ in range(100): # 10*100 = 1000
        try:
            next_results = search_results['search_metadata']['next_results']
        except KeyError, e: # No more results when next_results doesn't exist
            break
        # Create a dictionary from next_results, which has the following form: # ?max_id=313519052523986943&q=NCAA&include_entities=1
        kwargs = dict([ kv.split('=')
                       for kv in next_results[1:].split("&") ])
        search_results = twitter_api.search.tweets(**kwargs)
        statuses += search_results['statuses']
                       
        if len(statuses) > max_results:
            break
    return statuses

"""
twitter_api = oauth_login()
query= 'obesity'
statuses= twitter_search(twitter_api, query, max_results=1000) """
#s2= twitter_search(twitter_api, 'weightloss', max_results=1000)
#s3= twitter_search(twitter_api, 'health', max_results=1000)
#s4= twitter_search(twitter_api, 'nutrition', max_results=1000)

#s2= twitter_search(twitter_api, 'diet', max_results=1000)
#s3= twitter_search(twitter_api, 'overweight', max_results=1000)
#s4= twitter_search(twitter_api, 'health', max_results=1000)
#statuses= s1+s2+s3+s4

#finds words that are certain parts of speech after NLTK tokens are created
def findtags(tag_prefix, tagged_text):
    cfd = nltk.ConditionalFreqDist((tag, word) for (word, tag) in tagged_text
                                   if tag.startswith(tag_prefix))
    return dict((tag, cfd[tag].keys()[:15]) for tag in cfd.conditions())

def format_date(original):
    """
    year=original[26:30]
    month=original[4:7]
    day=original[8:10]
    time=original[11:19]
    
    month_new=""
    if month == "Jan":
        month_new += "01-"
    elif month == "Feb":
        month_new += "02-"
    elif month == "Mar":
        month_new += "03-"
    elif month == "Apr":
        month_new += "04-"
    elif month == "May":
        month_new += "05-"
    elif month == "Jun":
        month_new += "06-"
    elif month == "Jul":
        month_new += "07-"
    elif month == "Aug":
        month_new += "08-"
    elif month == "Sep":
        month_new += "09-"
    elif month == "Oct":
        month_new += "10-"
    elif month == "Nov":
        month_new += "11-"
    elif month == "Dec":
        month_new += "12-"
        """
    date= original[0:10]
    time= original[11:20]
    return date + "T" + time + "Z"

def replace_char(str):
    return str.replace("\\xa0", " ").replace("\\xa9", "(c)").replace("\\xae", "(r)").replace("\\xb7", "*").replace("\\u2018", "'").replace("\\u2019", "'").replace("\\u201c", '"').replace("\\u201d", '"').replace("\\u2026", "...").replace("\\u2002", " ").replace("\\u2003", " ").replace("\\u2009", " ").replace("\\u2013", "-").replace( "\\u2014", "--")

def find_keywords(tweet_texti):
    tweet_text= replace_char(tweet_texti).lower()
    #print tweet_text
    tmp_klist=[]
    tmp_kword={}
    c=0
    tokens=nltk.word_tokenize(tweet_text)
    tag=nltk.pos_tag(tokens)
    tagged= findtags('NN', tag)
    taggedv=  findtags('VB', tag)
    tagged.update(taggedv)
    key_temp=""
    #print tagged
    for t in tagged:
        #print t
        for u in tagged[t]:
            #print u.lower()
            #print u
            #u=u.lower()
            #u.rstrip(".").rstrip("...").rstrip(";").rstrip("%")
            if u == query.lower() or u == query.title():
                tagged[t].remove(u)
            elif u == 'are' or u == 'am' or u == 'is' or u == 'be' or u == 'have' or u == 'had' or u == 'having' or u == 'has' or u == 'was' or u == 'were' or u == 'do' or u == 'did' or u == 'does'or u == 'done' or u == '\'s' or u == '\'re' or u == 'it' or u == 'nah' or u == 'uh' or u == 'lol' or u == '\'m':
                #print u
                tagged[t].remove(u)
            else:
                #u= u.encode('UTF-8')
                #u= replace_char(u)
                if 't.co' in u:
                    tagged[t].remove(u)
                #elif 'http' in u:
                #tagged[t].remove(u)
                elif u == 'rt' or u == 'http' or u == '@' or u == '(' or  u == ')' or u == '{' or  u == '}' or u == '|' or u == '=' or u == '[' or u == ']' or u == '-' or u == "...":
                    tagged[t].remove(u)
                else:
                    #print =
                    bare= u.rstrip(".").rstrip("...").rstrip(";").rstrip("%").rstrip("\'")
                    if bare == 'are' or bare == 'am' or bare == 'is' or bare == 'be' or bare == 'have' or bare == 'had' or bare == 'having' or bare == 'has' or bare == 'was' or bare == 'were' or bare == 'do' or bare == 'did' or bare == 'does'or bare == 'done' or bare == '\'s' or bare == '\'re' or bare == 'it' or bare == 'nah' or bare == 'bareh' or bare == 'lol' or bare == '\'m' or u == 'rt' or u == 'http' or u == '@' or u == '(' or  u == ')' or u == '{' or  u == '}' or u == '|' or u == '=' or u == '[' or u == ']' or u == '-' or u == "..." or u == "im" or u == "lh":
                        continue
                    else:
                        tmp_kword["name"]= t
                        tmp_kword["value"]= bare.rstrip(".").rstrip("...").rstrip(";").rstrip("%")
                        #print tmp_kword["value"]
                        #print tmp_kword
                        tmp_klist.append(tmp_kword)
                        tmp_kword={}
    return tmp_klist

consumer_key = '4FtnZIPVDrsHHj8wO8amoQ'
consumer_secret = '2h08yY97XElbKK8ZNQypI8wJtCewtMDD8j0Xc3DdfkU'
access_token = '582174814-X5NJBNnghiXq5O9XQuCl9NN0GxWa2CyFhLWrwwRG'
access_token_secret = 'aqXMX8Jy9uvG04zKB8kboaPUD6a5KfmmZGhfIdN74'

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)
api = tweepy.API(auth)

#tweet_keeper=[]
tempItem={}
query= "Obama"

# above omitted for brevity
c = tweepy.Cursor(api.search,
                  q=query,
                  since="2014-09-16",
                  until="2014-09-17",
                  lang="en").items()
while True:
    try:
        status = c.next()
        #print tweet
        tempItem["headline"]= status.user.screen_name
        tempItem["keywords"]= find_keywords(replace_char(status.text))
        tempItem["lead_paragraph"]= replace_char(status.text)
        tempItem["pub_date"]= format_date(str(status.created_at))
        #print tempItem["pub_date"]
        j= json.dumps(tempItem, encoding="utf8", ensure_ascii=False)
        print j
        #tweet_keeper.append(ast.literal_eval(j))
        #f= open('obama.csv', 'w')
        #f.write(j)
        #f.close()
        #print "success"

    # Insert into db
    except tweepy.TweepError:
        time.sleep(60 * 15 + 5)
        continue

    except StopIteration:
        print "here"
        break

"""
sorted_tk= sorted(tweet_keeper, key=lambda k: k['pub_date'], reverse=False)

for t in sorted_tk:
    print t
"""

"""
for status in tweepy.Cursor(api.search,
                           q= query,
                            #since_id= 441632741352681472,
                            #max_id= 512331669622521856,
                            since="2014-09-16",
                            until="2014-09-17",
                           lang="en").items(1000):
    #find_keywords(replace_char(status.text))
    #print status.id
    #print status
    
    #print format_date(str(status.created_at))
    #print status.created_at
    tempItem["headline"]= status.user.screen_name
    tempItem["keywords"]= find_keywords(replace_char(status.text))
    tempItem["lead_paragraph"]= replace_char(status.text)
    tempItem["pub_date"]= format_date(str(status.created_at))
    j= json.dumps(tempItem, encoding="utf8", ensure_ascii=False)
    tweet_keeper.append(ast.literal_eval(j))
    #print j
    #print json.dumps(tempItem, encoding="utf8", ensure_ascii=False)


sorted_tk= sorted(tweet_keeper, key=lambda k: k['pub_date'], reverse=False)

for t in sorted_tk:
    print t
"""


"""
    c = tweepy.Cursor(api.search,
    q=search,
    include_entities=True).items()
    while True:
    try:
    tweet = c.next()
    # Insert into db
    except tweepy.TweepError:
    time.sleep(60 * 15)
    continue
    except StopIteration:
    break
"""
"""
for t in sorted_tk:
    print t["pub_date"]
    #newt= ast.literal_eval(t)
    #print type(t)

for t in tweet_keeper:
    print t
"""
"""
for status in statuses:
    tempItem["headline"]= status['user']['screen_name']
    #print json.dumps(tempItem["headline"], ensure_ascii=False).encode('utf8')
    tempItem["keywords"]= find_keywords(replace_char(status['text']))
    #print json.dumps(tempItem["keywords"], ensure_ascii=False)
    tempItem["lead_paragraph"]= replace_char(status['text'])
    #print tempItem["lead_paragraph"]
    tempItem["pub_date"]= format_date(status['created_at'])
#print json.dumps(tempItem)
    print json.dumps(tempItem, encoding="utf8", ensure_ascii=False)
#print json.dumps(tempItem, ensure_ascii=False).encode('utf8')


#print json.dumps(replace_char(status['text']), ensure_ascii=False).encode('utf8')


"""


