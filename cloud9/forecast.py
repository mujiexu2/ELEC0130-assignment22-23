import sys
import ssl
import json
import time
import pandas as pd
import numpy as np
import pymongo
import matplotlib.pyplot as plt
from itemsell import item_sell
from statsmodels.tsa.seasonal import seasonal_decompose
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from pmdarima.arima import auto_arima


def seasonal_decompose_func(ts):
    ###对时间序列进行模式分解
    #Performing pattern decomposition on time series
    decomposition = seasonal_decompose(ts, model="additive")
    trend = decomposition.trend
    seasonal = decomposition.seasonal
    residual = decomposition.resid
    return trend, seasonal, residual
def json_encode(string):
    ###编码字符串为json文件
    #Encoding strings as json files
        return json.dumps(string)

def forecast_item():
    
    MONGO_URL = 'mongodb+srv://larrowl:Lsh680911@daps.h6g0bwp.mongodb.net/test'
    client = pymongo.MongoClient(MONGO_URL)
    db = client['IOT_data']
    dc = db['sell_item']
    
    ###数据集转换为时间序列
    #Data set conversion to time series
    df = pd.DataFrame(list(dc.find()))
    df = df.drop(columns='_id')
    df["date"] = pd.to_datetime(df["date"])
    df.set_index("date", inplace=True)
    ###对每个商品进行周期性分析
    
    trend1, seasonal1, residual1 = seasonal_decompose_func(df["Milk"])
    trend2, seasonal2, residual2 = seasonal_decompose_func(df["Cheese"])
    trend3, seasonal3, residual3 = seasonal_decompose_func(df["Coke"])
    
    ###对每个商品进行时间序列模型拟合，使用ARIMA模型，选择最优模型并进行预测：
    #A time series model was fitted to each commodity, using an ARIMA model, to select the optimal model and to forecast.
    
    model1 = auto_arima(df["Milk"], seasonal=True, m=12, suppress_warnings=True, error_action="ignore")
    model2 = auto_arima(df["Cheese"], seasonal=True, m=12, suppress_warnings=True, error_action="ignore")
    model3 = auto_arima(df["Coke"], seasonal=True, m=12, suppress_warnings=True, error_action="ignore")
    
    #预测下一周期的各商品销量
    # Forecast sales of each commodity for the next cycle
    forecast1 = model1.predict(n_periods=1)
    forecast2 = model2.predict(n_periods=1)
    forecast3 = model3.predict(n_periods=1)

    best_selling_product = item_sell()
    
    #Declaring variables
    message ={
      'Best_selling': best_selling_product,
      'Milk_forecast': forecast1[0],
      "Cheese_forecast": forecast2[0],
      "Coke_forecast": forecast3[0]
    }
    
    return message
    