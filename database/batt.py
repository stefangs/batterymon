import psycopg2
import fire
from tabulate import tabulate

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

  def plot(self, session=0, battery=0):
    """Plots the discharge graph, either for a session or all sessions for a battery"""
    return 2
  
  def read(self, battery=0, port='COM3'):
    """Reads a discharge session from serial port and inserts in database"""
    return 2
  
if __name__ == '__main__':
  fire.Fire(Batt)