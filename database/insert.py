'''
Author : Ishan Maduranga
Last Modified : 29/06/2023
'''

'''
This python script will be used to insert data to the MySQL database
As this will subscribe to UOP/CO326/E18/11/+ and this will retrieve all
messages published.And based on these published data this will update 
the database entries automatically
'''

# importing the required modules
import paho.mqtt.client as mqtt
import mysql.connector
import json 

# importing the credentials file
from credentials import credentials

# function to retrieve all the data from the database
def update_db(topic , payload):
    
    # Database connection details
    host = credentials['host']
    port = credentials['port']
    user = credentials['user']
    password = credentials['password']
    database = credentials['database']

    # Create a connection to the MySQL database
    conn = mysql.connector.connect(
        host=host,
        port=port,
        user=user,
        password=password,
        database=database
    )

    # Create a cursor object to execute SQL queries
    cursor = conn.cursor()

    # SQL query to retrieve data
    # if it is a Buzzer records it will be in the actuator records table or otherwise in sensordata table
    query = f"INSERT INTO {'actuator_records' if topic == 'Buzzer' else 'sensordata'} VALUES(%s , %s , %s , %s)"

    # Data to be inserted
    data = (payload['userID'] , topic, payload['message'] , payload['updatedAt'])
    
    # Execute the query
    cursor.execute(query , data)
    conn.commit()

    # Close the cursor and the database connection
    cursor.close()
    conn.close()


# ------------------------------------------------------------------------------------------------

# Callback function when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code: {rc}")
    # Subscribe to a topic upon successful connection
    client.subscribe("UOP/CO326/E18/11/+")

# Callback function when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()}")     
    # print(msg.topic)
    
    # get the payload and the topic
    topic = msg.topic
    payload = msg.payload.decode()
    
    # split the topic by slashes
    # eg : UOP/CO326/E18/11/Temperature
    topic_arr = topic.split('/')
    
    # split the payload by | 
    # eg : 410101|11:26:59|22.12
    payload_arr = payload.split('|')
    
    json_payload = {
        "userID" : payload_arr[0],
        "topic" : topic_arr[-1],
        "message" : payload_arr[-1],
        "updatedAt" : payload_arr[1]
    }
    
    # Convert the payload to JSON
    try:
        # json_payload = json.loads(payload)
        
        # Print the JSON data
        print(json_payload)
        
        # update the database
        update_db(topic_arr[-1] , json_payload)   
            
    except json.JSONDecodeError:
        print(f"Invalid JSON payload: {payload}")

# listen to the latest send
def listen_latest():
    # MQTT broker details
    broker_address = credentials['broker']
    broker_port = credentials['broker_port']

    # Create an MQTT client instance
    client = mqtt.Client()

    # Set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # Connect to the MQTT broker
    client.connect(broker_address, broker_port)

    # Loop continuously to process MQTT network traffic, but avoid blocking the main thread
    client.loop_start()

    # Keep the program running until interrupted
    try:
        while True:
            pass
    except KeyboardInterrupt:
        pass

    # Disconnect from the MQTT broker
    client.loop_stop()
    client.disconnect()


# start executing the function
listen_latest()