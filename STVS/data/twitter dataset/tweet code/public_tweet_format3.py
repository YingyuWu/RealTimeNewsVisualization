# -*- coding: utf-8 -*-

# twitter client
import tweepy
import json
import nltk

def findtags(tag_prefix, tagged_text):
    cfd = nltk.ConditionalFreqDist((tag, word) for (word, tag) in tagged_text
                                   if tag.startswith(tag_prefix))
    return dict((tag, cfd[tag].keys()[:15]) for tag in cfd.conditions())

def format_date(original):
    date= original[0:10]
    time= original[11:19]
   
    return date + "T" + time + "Z"

def replace_char(str):
    return str.replace("\\xa0", " ").replace("\\xa9", "(c)").replace("\\xae", "(r)").replace("\\xb7", "*").replace("\\u2018", "'").replace("\\u2019", "'").replace("\\u201c", '"').replace("\\u201d", '"').replace("\\u2026", "...").replace("\\u2002", " ").replace("\\u2003", " ").replace("\\u2009", " ").replace("\\u2013", "-").replace( "\\u2014", "--")

def find_keywords(tweet_text):
    #print unicode(tweet_text.encode('UTF-8'), 'ascii', 'ignore')
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
            #print u
            if u == 'are' or u == 'am' or u == 'is' or u == 'be' or u == 'have' or u == 'had' or u == 'having' or u == 'has' or u == 'was' or u == 'were' or u == 'do' or u == 'did' or u == 'does'or u == 'done' or u == '\'s' or u == '\'re' or u == 'it' or u == 'nah' or u == 'uh' or u == 'lol' or u == '\'m':
                tagged[t].remove(u)
            else:
                u= u.encode('UTF-8')
                if '\u' in u:
                    tagged[t].remove(u)
                elif 't.co' in u:
                    tagged[t].remove(u)
                #elif 'http' in u:
                #tagged[t].remove(u)
                elif u == 'rt' or u == 'http' or u == '@' or u == '(' or  u == ')' or u == '{' or  u == '}' or u == '|' or u == '=' or u == '[' or u == ']' or u == '-' or u == "...":
                    tagged[t].remove(u)
                elif u == 'RT' or u == 'http' or u == '@' or u == '(' or  u == ')' or u == '{' or  u == '}' or u == '|' or u == '=' or u == '[' or  u == ']' or u == '-':
                    tagged[t].remove(u)
                else:
                    #print u
                    bare= u.rstrip(".").rstrip("...").rstrip(";").rstrip("%").rstrip("\'")
                    if bare == 'are' or bare == 'am' or bare == 'is' or bare == 'be' or bare == 'have' or bare == 'had' or bare == 'having' or bare == 'has' or bare == 'was' or bare == 'were' or bare == 'do' or bare == 'did' or bare == 'does' or bare == 'done' or bare == '\'s' or bare == '\'re' or bare == 'it' or bare == 'nah' or bare == 'bareh' or bare == 'lol' or bare == 'the' or bare == '\'m' or u == 'rt' or u == 'http' or u == '@' or u == '(' or  u == ')' or u == '{' or  u == '}' or u == '|' or u == '=' or u == '[' or u == ']' or u == '-' or u == "..." or u == "im" or u == "lh":
                        continue
                    elif unicode(bare, 'ascii', 'ignore') == "":
                        continue
                    elif len(bare) == 1:
                        continue
                    else:
                        tmp_kword["name"]= t
                        tmp_kword["value"]= bare.rstrip(".").rstrip("...").rstrip(";").rstrip("%")
                        #print unicode(tmp_kword["value"], 'ascii', 'ignore')
                        #print tmp_kword["value"]
                        tmp_klist.append(tmp_kword)
                        tmp_kword={}
                    
                    """
                    tmp_kword["name"]= t
                    tmp_kword["value"]= u.rstrip(".").rstrip("...").rstrip(";").rstrip("%")
                    #print tmp_kword
                    tmp_klist.append(tmp_kword)
                    tmp_kword={}
                    """
    return tmp_klist

class StreamWatcherHandler(tweepy.StreamListener):
    """ Handles all incoming tweets as discrete tweet objects.
        """

    
    def on_status(self, status):
        """Called when status (tweet) object received.
            
            See the following link for more information:
            https://github.com/tweepy/tweepy/blob/master/tweepy/models.py
            """
        """try:
            tid = status.id_str
            usr = status.author.screen_name.strip()
            txt = status.text.strip()
            in_reply_to = status.in_reply_to_status_id
            coord = status.coordinates
            src = status.source.strip()
            cat = status.created_at"""
        
        try:
            if status.lang == "en":
                tempItem={}
                tempItem["headline"]= status.user.screen_name
            #print tempItem["headline"]
                tempItem["keywords"]= find_keywords(replace_char(status.text).lower())
            #print json.dumps(tempItem["keywords"], ensure_ascii=False)
                tempItem["lead_paragraph"]= replace_char(status.text)
                tempItem["pub_date"]= format_date(str(status.created_at))
                print json.dumps(tempItem, encoding="utf8", ensure_ascii=False)
            
            #print tempItem["pub_date"]
            #print format_date(str(status.created_at))
            #print json.dumps(tempItem)
            #print json.dumps(tempItem["lead_paragraph"], ensure_ascii=False)
            #print json.dumps(tempItem)
            #print json.dumps(tempItem)
            #print str(status.created_at)[0:4]
            #print tempItem["lead_paragraph"]
            #print json.dumps(tempItem, ensure_ascii=False)
            #print "{\"headline\": \"" + tempItem["headline"] + "\", \"keywords\": " + json.dumps(tempItem["keywords"], ensure_ascii=False) + ", \"lead_paragraph\": " + json.dumps(tempItem["lead_paragraph"], ensure_ascii=False) + ", \"pub_date\": \"" + tempItem["pub_date"] + "\"}"
            """
            print "{\"headline\": \"" + tempItem["headline"] + "\", \"keywords\": "
            print json.dumps(tempItem["keywords"], ensure_ascii=False)
            print ", \"lead_paragraph\": " + json.dumps(tempItem["lead_paragraph"], ensure_ascii=False) + ", \"pub_date\": \"" + tempItem["pub_date"] + "\"}"
            """
        #print json.dumps(tempItem, encoding="utf8", ensure_ascii=False)


        except Exception as e:
            # Most errors we're going to see relate to the handling of UTF-8 messages (sorry)
            print(e)
    
    def on_error(self, status_code):
        print('An error has occured! Status code = %s' % status_code)
        return True

def main():
    #num_tweets=0
    # establish stream
    consumer_key = '4FtnZIPVDrsHHj8wO8amoQ'
    consumer_secret = '2h08yY97XElbKK8ZNQypI8wJtCewtMDD8j0Xc3DdfkU'
    auth1 = tweepy.auth.OAuthHandler(consumer_key, consumer_secret)
    
    access_token = '582174814-X5NJBNnghiXq5O9XQuCl9NN0GxWa2CyFhLWrwwRG'
    access_token_secret = 'aqXMX8Jy9uvG04zKB8kboaPUD6a5KfmmZGhfIdN74'
    auth1.set_access_token(access_token, access_token_secret)
    
    print "Establishing stream...",
    stream = tweepy.Stream(auth1, StreamWatcherHandler(), timeout=None)
    #stream.filter(track=['flyers'])
        #stream.filter(languages = ["en"])
    #stream.filter(languages = ["en"])
    print "Done"
    
    
    # Start pulling our sample streaming API from Twitter to be handled by StreamWatcherHandler
    stream.sample()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print "Disconnecting from database... ",
        print "Done"