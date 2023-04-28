import sys
import ssl
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import json
import time
import pymongo
import pandas as pd
import random
import time
from forecast import forecast_item
#Setting global variables
global message_environment
message_environment = ""
#Connection to a specific Collection of Mongodb
MONGO_URL = 'mongodb+srv://larrowl:Lsh680911@daps.h6g0bwp.mongodb.net/test'
client = pymongo.MongoClient(MONGO_URL)
db = client['IOT_data']
dc = db['environment_data']
dd = db['stock_data']
dc.drop()#Reset Collection with each restart
dd.drop()


#Setup our MQTT client and security certificates
#Make sure your certificate names match what you downloaded from AWS IoT

mqttc = AWSIoTMQTTClient("1234")

#Use the endpoint from the settings page in the IoT console
mqttc.configureEndpoint("a3l5bqqa7x3v5l-ats.iot.eu-west-2.amazonaws.com",8883)
mqttc.configureCredentials("./assets/lab1/AmazonRootCA1.pem","./assets/lab1/privateKey.pem","./assets/lab1/certificate.pem")

#Function to encode a payload into JSON
def json_encode(string):
        return json.dumps(string)

mqttc.json_encode=json_encode

#This sends our test message to the iot topic
def send(topic, message):
    mqttc.publish(topic, message, 0)
    #print("Message Published" + str(topic))
    
def customCallback_env(client, userdata, message):
    ###解析环境传感器/数据拆分转发/数据存入数据库
    #Parsing of environmental sensor data / data splitting and forwarding / data storage in database
    
    global message_environment
    message_environment=message.payload.decode('utf-8')
    y=eval(message_environment)
    for topic,message in y.items():
        topic = 'arduino/' + str(topic)
        send(topic, message)
    dc.insert_one(y)
    
def customCallback_item(client, userdata, message):
    ###解析Stock数据/数据拆分，重新聚合，转发/数据存入数据库
    #Parsing Stock data / data splitting, re-aggregation, forwarding / data storage in database
    global message_environment
    message_environment=message.payload.decode('utf-8')
    y=eval(message_environment)
    stock_total = mqttc.json_encode(y)
    send('arduino/Stock_total', stock_total)
    
    stock = {key: value for key, value in y.items() if key in ["Timestamp","Milk_Stock", "Coke_Stock","Cheese_Stock"]}
    stock = mqttc.json_encode(stock)
    send('arduino/Stock', stock)
    
    price = {key: value for key, value in y.items() if key in ["Timestamp","Milk_Price", "Coke_Price","Cheese_Price"]}
    price = mqttc.json_encode(price)
    send('arduino/Price', price)
    
    baditem = {key: value for key, value in y.items() if key in ["Timestamp",
                                                                "Milk_Baditem", "Coke_Baditem","Cheese_Baditem"]}
    baditem = mqttc.json_encode(baditem)
    send('arduino/Bad_item', baditem)
    
    dd.insert_one(y)
    
def customCallback_command(client, userdata, message):
    ###解析接受nodered端指令/转发至arduino
    #Parsing to accept nodered side commands/forwarding to arduino
    global message_environment
    message_environment=message.payload.decode('utf-8')
    y=eval(message_environment)
    message = mqttc.json_encode(y)
    send('client/Command', message)

def customCallback_report(client, userdata, message):
    ###解析接受nodered端报告生成指令/计算报告数据/返回数据至nodered
    #Parsing accepts nodered-side report generation instructions/computes report data/returns data to nodered
    print('Start')
    #message = forecast_item()
    message = {
                "Best_selling": "Cheese",
                "Milk_forecast": 123.91232876712331,
                "Cheese_forecast": 125.57808219178087,
                "Coke_forecast": 125.51780821917808
                }
    message = mqttc.json_encode(message)
    send('server/Report', message)
    print('True')
    
def sub_env():
    ###订阅环境传感器数据
    #Subscribe to environmental sensor data
    mqttc.subscribe("arduino/Environment", 0, customCallback_env)
    
def sub_item():
    ###订阅Stock器数据
    #Subscribe to stock data
    mqttc.subscribe("arduino/Item", 0, customCallback_item)
    
def sub_command():
    ###订阅用户对arduino指令数据
    #Subscribe to user data on arduino commands
    mqttc.subscribe("client1/Command", 0, customCallback_command)
    mqttc.subscribe("client2/Command", 0, customCallback_command)
    
def sub_report():
    ###订阅用户对报告指令数据
    #Subscribe to user data on reporting instructions
    mqttc.subscribe("client/Report", 0, customCallback_report)
    
#Connect to the gateway
mqttc.connect()
print("Connected")
#sub_report()
#Loop until terminated
while True:
    sub_report()
    sub_env()
    sub_item()
    sub_command()
    time.sleep(5)
mqttc.disconnect()
