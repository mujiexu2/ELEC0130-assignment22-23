import pymongo
import pandas as pd
import random
import time

MONGO_URL = 'mongodb+srv://larrowl:Lsh680911@daps.h6g0bwp.mongodb.net/test'
client = pymongo.MongoClient(MONGO_URL)
db = client['DAPS_data']
dc = db['stock_covid']
DF = pd.DataFrame(list(dc.find()))
DF = DF.drop(columns='_id')
print(DF)