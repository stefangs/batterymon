import psycopg2
import fire
from tabulate import tabulate
import matplotlib.pyplot as plt

class Batt(object):
  """Battery health app"""

  def __connectDatabase(self):
    with open("connection.txt", "r") as file:
      connection_string = file.readline().strip()
    conn = psycopg2.connect(connection_string)
    cursor = conn.cursor()
    return (conn, cursor)

  def list(self):
    """List all discharge sessions"""
    conn, cursor = self.__connectDatabase()
    cursor.execute("select sess, round(AVG(loaded), 2) AS AVGV, round(SUM(loaded) * 1000 / 60 / 3.3, 0) AS mAh, ROUND(SUM(loaded * loaded / 3.4) * 60, 0) AS Joule from samples group by sess order by AVGV DESC, mAh DESC;")
    rows = cursor.fetchall()
    cursor.close()
    conn.close()
    headers = ["Session", "Voltage", "mAh", "Joule"]
    print(tabulate(rows, headers=headers))

  def plot(self, session=0):
    """Plots the discharge graph for a session"""
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
  
  def read(self, battery=0, port='COM3'):
    """NYI Reads a discharge session from serial port and inserts in database"""
    return 2
  
if __name__ == '__main__':
  fire.Fire(Batt)