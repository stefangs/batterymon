import psycopg2
import matplotlib.pyplot as plt

with open("connection.txt", "r") as file:
    connection_string = file.readline().strip()

# Connect to the PostgreSQL database using the URL
conn = psycopg2.connect(connection_string)

# Open a cursor to perform database operations
cur = conn.cursor()

# Execute a SELECT statement to retrieve the data
cur.execute("SELECT minute, loaded, unloaded FROM samples")

# Fetch all rows and close the cursor
rows = cur.fetchall()
cur.close()

# Extract the x and y values from the rows
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
