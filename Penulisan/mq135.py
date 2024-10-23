import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib import font_manager
import matplotlib.colors as mcolors
import os
import locale

# Set locale to Indonesian
locale.setlocale(locale.LC_TIME, 'id_ID.UTF-8')

# Ensure 'arial' font is available
font_dirs = ['C:/Windows/Fonts']
font_files = font_manager.findSystemFonts(fontpaths=font_dirs)
for font_file in font_files:
    if 'arial' in font_file:
        font_manager.fontManager.addfont(font_file)

# Set default font to 'arial'
plt.rcParams['font.family'] = 'arial'
plt.rcParams['font.size'] = 15  # Default font size

# Read CSV file into a pandas DataFrame
csv_file = 'C:/Users/aryad/Downloads/SCD41/Penulisan/data_mq135.csv'
df = pd.read_csv(csv_file)

# Extract the filename without extension
csv_filename = os.path.splitext(os.path.basename(csv_file))[0]

# Convert 'Timestamp' column to datetime format
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

# Set 'Timestamp' as the DataFrame index for rolling operation
df.set_index('Timestamp', inplace=True)

# Calculate the 8j MA for CO2 and IAQ
df['CO2_rolling'] = df['CO2'].rolling('8h').mean()
df['IAQ_rolling'] = df['IAQ'].rolling('8h').mean()

# Function to blend color with white
def blend_with_white(color, alpha):
    r, g, b = mcolors.to_rgb(color)
    blended_color = [(1 - alpha) * 1.0 + alpha * r, (1 - alpha) * 1.0 + alpha * g, (1 - alpha) * 1.0 + alpha * b]
    return blended_color

# Define pastel colors
pastel_red_line = blend_with_white('red', 0.4)  # Pastel red for the rolling average line
pastel_red_hatch = blend_with_white('red', 0.3)  # Lighter pastel red for hatch

# Define time periods for different parts of the day with blended colors
periode_waktu = [
    (22, 24, blend_with_white('lightblue', 0.4), 'Dini Hari'),
    (0, 5, blend_with_white('lightblue', 0.4), 'Dini Hari'),
    (5, 10, blend_with_white('khaki', 0.4), 'Pagi'),
    (10, 14, blend_with_white('gold', 0.4), 'Siang'),
    (14, 18, blend_with_white('orange', 0.4), 'Sore'),
    (18, 22, blend_with_white('cornflowerblue', 0.4), 'Malam')
]

# Set the y-axis limits based on the entire dataset for consistency
y_min_co2 = df['CO2'].min()
y_max_co2 = df['CO2'].max()
y_min_iaq = df['IAQ'].min()
y_max_iaq = df['IAQ'].max()

# Create figure and axis with size adjusted for A4 width (8.27 inches)
fig, ax1 = plt.subplots(figsize=(11, 6))

# Plot CO2 data on the first y-axis
ax1.plot(df.index[::30], df['CO2'][::30], label='CO2', color='#1E88E5')  # Bright blue for CO2
ax1.plot(df.index[::30], df['CO2_rolling'][::30], label='MA CO2 (8 jam)', color=pastel_red_line)  # Pastel red for rolling average line
ax1.set_xlabel('Waktu')
ax1.set_ylabel('CO2 (ppm)', color='#1E88E5')
ax1.tick_params(axis='y', labelcolor='#1E88E5')
ax1.set_ylim(y_min_co2, y_max_co2)

# Plot IAQ data on the second y-axis
ax2 = ax1.twinx()
ax2.plot(df.index[::30], df['IAQ'][::30], label='IAQ', color='green')  # Green for IAQ
ax2.plot(df.index[::30], df['IAQ_rolling'][::30], label='MA IAQ (8 jam)', color='lightgreen')  # Lighter green for IAQ rolling average
ax2.set_ylabel('IAQ', color='green')
ax2.tick_params(axis='y', labelcolor='green')
ax2.set_ylim(y_min_iaq, y_max_iaq)

# Set major and minor ticks on the x-axis
ax1.xaxis.set_major_locator(mdates.HourLocator(interval=2))
ax1.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
plt.xticks(rotation=0)

# Add shaded areas for different parts of the day
for start_hour, end_hour, color, label in periode_waktu:
    ax1.axvspan(df.index[0] + pd.Timedelta(hours=start_hour), df.index[0] + pd.Timedelta(hours=end_hour), color=color, alpha=1.0, label=label)

# Add hatches for periods where the 8j MA exceeds 1000 ppm
high_co2_periods = df[df['CO2_rolling'] > 1000]
if not high_co2_periods.empty:
    for start, end in zip(high_co2_periods.index[:-1], high_co2_periods.index[1:]):
        ax1.axvspan(start, end, facecolor='none', edgecolor=pastel_red_hatch, hatch='///', linewidth=0, label='MA CO2 >1.000 ppm')

# Ensure each label appears only once in the legend
handles, labels = ax1.get_legend_handles_labels()
unique_handles_labels = dict(zip(labels, handles))

# Sort legend in desired order
order = ["CO2", "MA CO2 (8 jam)", "IAQ", "MA IAQ (8 jam)", "Pagi", "Siang", "Sore", "Malam", "Dini Hari", "MA CO2 >1.000 ppm"]
ordered_handles_labels = {k: unique_handles_labels[k] for k in order if k in unique_handles_labels}

# Add legend with extended right padding
fig.legend(ordered_handles_labels.values(), ordered_handles_labels.keys(), loc='upper right', bbox_to_anchor=(1, 1), fontsize='small', handlelength=2)

# Add title with the formatted date
plt.title(f"Tingkat Konsentrasi CO2 dan IAQ dalam Ruangan ({csv_filename})")

# Adjust layout to provide less space on the left and right
plt.subplots_adjust(left=0.085, right=0.855)  # Adjust the value as needed

# Save plot as PNG file with the filename including parameters and date
output_path = f'C:/Users/aryad/Downloads/SCD41/Penulisan/CO2_IAQ_{csv_filename}.png'
fig.savefig(output_path, dpi=300)

# Show plot
plt.show()
