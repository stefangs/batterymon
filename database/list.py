import psycopg2
from tabulate import tabulate

with open("connection.txt", "r") as file:
    connection_string = file.readline().strip()

# Connect to the PostgreSQL database using the URL
conn = psycopg2.connect(connection_string)

# Open a cursor to perform database operations
cursor = conn.cursor()

column1_name = "Session"
column2_name = "Voltage"
column3_name = "mAh"
column4_name = "Joule"

# Execute the SQL query to fetch the data
cursor.execute("select sess, round(AVG(loaded), 2) AS AVGV, round(SUM(loaded) * 1000 / 60 / 3.3, 0) AS mAh, ROUND(SUM(loaded * loaded / 3.4) * 60, 0) AS Joule from samples group by sess order by AVGV DESC, mAh DESC;")

# Fetch all the rows from the result set
rows = cursor.fetchall()

# Close the cursor and the connection
cursor.close()
conn.close()

# Print the data in a nicely formatted table
headers = [column1_name, column2_name, column3_name, column4_name]
print(tabulate(rows, headers=headers))
