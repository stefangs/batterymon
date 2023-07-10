import serial

import psycopg2
from tabulate import tabulate

with open("connection.txt", "r") as file:
    connection_string = file.readline().strip()

# Connect to the PostgreSQL database using the URL
conn = psycopg2.connect(connection_string)

# Open a cursor to perform database operations
cursor = conn.cursor()

def handleSample(arguments):
    session = int(arguments[0])
    minutes = int(arguments[1])
    loaded = float(arguments[2]) / 1000.0
    unloaded = float(arguments[3]) / 1000.0
    resistance = 3.3
    print("session:", session, "minutes:", minutes, "unloaded", unloaded, "Loaded:", loaded, "resistance", resistance)
    try:
        cursor.execute("INSERT INTO samples (sess, minute, unloaded, loaded, resistance) VALUES (%s, %s, %s, %s, %s)", 
                       (session, minutes, unloaded, loaded, resistance))
        conn.commit()
        print("inserted")
    except:
        print("Failed to insert")

def handleCommand(command, arguments):
    print("command: " + command + " Arguments: " + str(arguments))
    if command == 'S': handleSample(arguments)


ser = serial.Serial('COM3', 115200, timeout=5.0)

print("connected to: " + ser.portstr)

str(ser.readline(), 'UTF-8')

ser.write(b'R\r\n')

while True:
    line = str(ser.readline(), 'UTF-8')
    if len(line) == 0:
        break
    print(line )
    if line[0] == '>':
        arguments = line[3:].strip().rsplit(sep=',')
        command = line[1]
        handleCommand(command, arguments)

ser.close()
cursor.close()
conn.close()
