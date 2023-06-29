'''
Author : Ishan Maduranga
Last Modified : 29/06/2023
'''

'''
This python script is subscribed to UOP/CO326/E18/11/Latest
and this will retrieve the latest 15 records from the database
and then it will going to publish to the topic UOP/CO326/E18/11/Latest_get

'''
# importing the required modules
import paho.mqtt.client as mqtt
import mysql.connector
import json 
from datetime import datetime

# importing the credentials file
from credentials import credentials


# function to retrieve all the data from the database
def retrieve_latest():
    
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
    query = "SELECT * FROM sensordata ORDER BY updatedAt DESC LIMIT 15"

    # Execute the query
    cursor.execute(query)

    # Fetch all the rows from the result set
    rows = cursor.fetchall()

    # Process the retrieved data
    results = []
    for row in rows:

        # each row will be a tuple so we need to access each property and create a json object
        # as the time returns as timedelta function it needs to be converetd to str before publishing
        result = {
            "topic" : row[1],
            "value" : row[2],
            "updatedAt" : str(row[3])   
        }
        results.append(result)

    json_data = json.dumps(results)

    print(json_data)

    # Close the cursor and the database connection
    cursor.close()
    conn.close()
    
    # return the results
    return json_data



# publish to the latest one
def publish_latest():
    
    # store the results here
    results = retrieve_latest()
    
    # MQTT broker details
    broker_address = credentials['broker']
    broker_port = credentials['broker_port']

    # Callback function when the client connects to the MQTT broker
    def on_connect(client, userdata, flags, rc):
        print(f"Connected to MQTT broker with result code: {rc}")

    # Create an MQTT client instance
    client = mqtt.Client()

    # Set the callback functions
    client.on_connect = on_connect

    # Connect to the MQTT broker
    client.connect(broker_address, broker_port)

    # Loop continuously to process MQTT network traffic, but avoid blocking the main thread
    client.loop_start()

    # Publish a message to a topic
    client.publish("UOP/CO326/E18/11/Latest_get", results)

    # Disconnect from the MQTT broker
    client.disconnect()
    client.loop_stop()


# -------------------------------------------------------------------------------------------------------------------------


# Callback function when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code: {rc}")
    # Subscribe to a topic upon successful connection
    client.subscribe("UOP/CO326/E18/11/Latest")

# Callback function when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()}")
    
    if(msg.payload.decode() == 'Send'):
        publish_latest()  


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