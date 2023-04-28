# Description of the project
Developed a Smart Supermarket shelf, an IoT system, for increasing supermarket system overall efficiency, through providing the functionalities of 

  - quality monitoring
  - remote control and notification
  - product report review
  - user-friendly interface
  - customer feedback
  - Energy-efficient system

The IoT system is based on the concepts of edge-cloud collaboration and AI models. 

The edge devices, the IoT devices, are made up with arduino devices, in Fig:
![image](https://github.com/mujiexu2/ELEC0130-assignment22-23/blob/main/images/components.png)

The cloud system, AWS EC2 instance work as the server, MongoDB and DynamoDB does storaging, MongoDB works as the primary database and DynamoDB is the backup database.

The graphical user interface (GUI) is made up with the EC2 instance accessed NodeRed. User can do remote control and notification, product report review, stock condition, and further remote access through phones, as well as integrating with IOS Homekit and Siri.

AWS IoT core works as the MQTT broker.

The system architecture can be shown in the Fig down below:
1) In Theory
![image](https://github.com/mujiexu2/ELEC0130-assignment22-23/blob/main/images/system%20architecture%20-1.png)

2) In Practice
![image](https://github.com/mujiexu2/ELEC0130-assignment22-23/blob/main/images/system%20architecture%20-inpractice%20-2.png)

# Role of each file
Two Directories are included, 
  - arduino is for arduino programs, 
  
      AWS_IoT_WiFi.ino: main program, including all logics, sensors, and connections
      
      arduino_secrets.h: network configuration, including wifi configuration and MQTT configuration.
      
  - Cloud9 is for the python files working in Cloud9 IDE in AWS EC2 instance, mainly does data forwarding, data splitting, data preprocessing, data storaging and data analysis.
  
# Software used

- Arduino files run in Arduino IDE
![image](https://github.com/mujiexu2/ELEC0130-assignment22-23/blob/main/images/arduinoide.png)

- Python files run in Cloud9 IDE in AWS EC2 instance.
![image](https://github.com/mujiexu2/ELEC0130-assignment22-23/blob/main/images/cloud9.png)
