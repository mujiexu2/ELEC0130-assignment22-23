import pandas as pd
import numpy as np
import pymongo

def item_sell():
    
    MONGO_URL = 'mongodb+srv://larrowl:Lsh680911@daps.h6g0bwp.mongodb.net/test'
    client = pymongo.MongoClient(MONGO_URL)
    db = client['IOT_data']
    dc = db['sell_item']
    
    df = pd.DataFrame(list(dc.find()))
    df = df.drop(columns='_id')
    #将数据集转换为时间序列
    # Convert datasets to time series
    df["date"] = pd.to_datetime(df["date"])
    df.set_index("date", inplace=True)
    
    #筛选指定时间区间内的销售数据
    #Filter sales data for a specified time interval
    start_date = "2022-01-01"
    end_date = "2022-03-01"
    
    df_period = df.loc[start_date:end_date]
    
    #计算每种商品的总销售量
    # Calculate the total sales volume of each item
    sales_totals = df_period.sum()
    
    #找到销售量最高和利润最高的商品
    #Find the highest selling and most profitable items
    best_selling_product = sales_totals.idxmax()

    return best_selling_product