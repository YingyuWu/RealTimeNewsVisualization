import ast

tweet_keeper= []

with open('obama9.16_17.14.txt') as f:
    content= f.readlines()

for i in range(len(content)):
    tweet_keeper.append(ast.literal_eval(content[i]))

sorted_tk= sorted(tweet_keeper, key=lambda k: k['pub_date'], reverse=False)
print sorted_tk

#print tweet_keeper[0]
#print len(content)
#print content
#for l in content.length:
#print l['pub_date']
#print content
"""
tweet_keeper= []
for l in content:
    tweet_keeper.append(ast.literal_eval(l))
    print tweet_keeper
#tweet_keeper.append(l)

sorted_tk= sorted(tweet_keeper, key=lambda k: k['pub_date'], reverse=False)

for t in sorted_tk:
    print t

"""