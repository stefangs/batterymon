import sys
import psycopg2
import fire
from tabulate import tabulate
import matplotlib.pyplot as plt
import serial

class Batt(object):
  """
  Battery health tracking app.
  
  The app collects battery discharge data from a battery discharging device and stores and presents the collected data
  
  """

  def __connectDatabase(self):
    with open("connection.txt", "r") as file:
      connection_string = file.readline().strip()
    conn = psycopg2.connect(connection_string)
    cursor = conn.cursor()
    return (conn, cursor)

  def list(self):
    """Lists all discharge sessions"""
    conn, cursor = self.__connectDatabase()
    cursor.execute("select sess, round(AVG(loaded), 2) AS AVGV, round(SUM(loaded) * 1000 / 60 / 3.3, 0) AS mAh, ROUND(SUM(loaded * loaded / 3.4) * 60, 0) AS Joule from samples group by sess order by AVGV DESC, mAh DESC;")
    rows = cursor.fetchall()
    cursor.close()
    conn.close()
    headers = ["Session", "Voltage", "mAh", "Joule"]
    print(tabulate(rows, headers=headers))

  def plot(self, session=0):
    """
    Plots the discharge graph for a session
    
    Args:
      session (integer): Identity of the session to plot
    """
    conn, cur = self.__connectDatabase()
    # Execute a SELECT statement to retrieve the data
    cur.execute("SELECT minute, loaded, unloaded FROM samples where sess=%s;", (session,))
    rows = cur.fetchall()
    cur.close()
    x = [row[0] for row in rows]
    loaded = [row[1] for row in rows]
    unloaded = [row[2] for row in rows]

    # Create a scatter plot of the data
    plt.plot(x, loaded, color='red', label='Loaded')
    plt.plot(x, unloaded, color='blue', label='Unoaded')
    plt.xlabel("Time (minutes)")
    plt.ylabel("Voltage")
    plt.title("Discharge voltage")
    plt.ylim(0)
    plt.legend()
    plt.show()

    # Close the database connection
    conn.close()

  def __insertSamples(self, sessionNr, samples, battery):
    conn, cursor = self.__connectDatabase()
    try:
      cursor.execute("INSERT INTO sessions (id, battery) VALUES (%s, %s)", 
                  (sessionNr, battery))
    except:
      print("Failed to insert session", sessionNr)
    for sample in samples:
      session = int(sample[0])
      minutes = int(sample[1])
      loaded = float(sample[2]) / 1000.0
      unloaded = float(sample[3]) / 1000.0
      resistance = 3.3
      try:
        cursor.execute("INSERT INTO samples (sess, minute, unloaded, loaded, resistance) VALUES (%s, %s, %s, %s, %s)", 
                       (session, minutes, unloaded, loaded, resistance))
        print(f"Inserted sample: {minutes}", end='\r')
      except:
        print("Failed to insert sample", minutes, "from session", session)
    print()
    conn.commit()
    cursor.close()
    conn.close()

  
  def read(self, battery=0, port='COM3'):
    """
    Reads a discharge session from serial port and inserts in database
    
    Args:
      battery (integer): The identity of the battery beeing discharged in the session
      port (string): The name of the COM port that the discharger is connected to
    
    """
    try:
      ser = serial.Serial(port, 115200, timeout=5.0)
    except:
      exc_type, exc_value, exc_traceback = sys.exc_info()
      print("Failed to open serial port", port, "Error:", exc_value)
      return

    # Wait until sampler prints startup line
    str(ser.readline(), 'UTF-8')

    # Write the report command
    ser.write(b'R\r\n')

    samples = []
    sessionNr = 0

    while True:
      line = str(ser.readline(), 'UTF-8')
      if len(line) == 0:
          break
      if line[0] == '>':
          arguments = line[3:].strip().rsplit(sep=',')
          command = line[1]
          if command == 'S':
            samples.append(arguments)
          if command == 'E':
            sessionNr = int(arguments[0])
            break

    print(len(samples), "samples read from session", sessionNr)

    self.__insertSamples(sessionNr, samples, battery)

  def delete(self, session):
    """
    Deletes a session and all its samples
    
    Args:
      session (integer): Identity of the session to delete
    """
    conn, cursor = self.__connectDatabase()
    cursor.execute("select count(*) from samples where sess=%s;", (session,))
    count = cursor.fetchone()[0]
    response = input("Do you want to delete session " + str(session) + " with " + str(count) + " samples ? (Y/N)")
    if response.upper() != "Y":
      print("Not deleting")
      return
    try:
      cursor.execute("delete from samples where sess=%s;", (session,))
      deleted1 = cursor.rowcount
      cursor.execute("delete from sessions where id=%s;", (session,))
      conn.commit()
      print("Deleted", deleted1, "samples")
    except:
        exc_type, exc_value, exc_traceback = sys.exc_info()
        print("Failed to delete session", session, "Error:", exc_value)
    cursor.close()
    conn.close()

  
if __name__ == '__main__':
  fire.Fire(Batt)