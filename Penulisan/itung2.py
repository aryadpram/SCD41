import pandas as pd
import numpy as np

# Load your data into a DataFrame
# Assuming your data is in a CSV file named 'data.csv' with columns 'Timestamp' and 'CO2'
csv_file = 'C:/Users/aryad/Downloads/SCD41/Penulisan/data_eldas.csv'
df = pd.read_csv(csv_file)

# Ensure Timestamp is in datetime format
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

# Set Timestamp as the DataFrame index
df.set_index('Timestamp', inplace=True)

# Calculate basic statistics
max_co2 = df['CO2'].max()
min_co2 = df['CO2'].min()
mean_co2 = df['CO2'].mean()
std_dev_co2 = df['CO2'].std()

# Calculate the 8-hour rolling window statistics
rolling_8h = df['CO2'].rolling('8h').mean()

max_rolling_8h = rolling_8h.max()
min_rolling_8h = rolling_8h.min()
mean_rolling_8h = rolling_8h.mean()
average_8h_ma = mean_rolling_8h.mean()

# Create a new DataFrame to store the results
results = pd.DataFrame({
    'Max CO2': [max_co2],
    'Min CO2': [min_co2],
    'Mean CO2': [mean_co2],
    'Std Dev CO2': [std_dev_co2],
    'Max Rolling 8H CO2': [max_rolling_8h.max()],
    'Min Rolling 8H CO2': [min_rolling_8h.min()],
    'Average 8H MA CO2': [average_8h_ma]
})

# Print the results
print(results)
