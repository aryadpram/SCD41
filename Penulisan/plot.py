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
csv_file = 'C:/Users/aryad/Downloads/SCD41/Penulisan/data_eldas.csv'
df = pd.read_csv(csv_file)

# Extract the filename without extension
csv_filename = os.path.splitext(os.path.basename(csv_file))[0]

# Convert 'Timestamp' column to datetime format
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

# Function to blend color with white
def blend_with_white(color, alpha):
    r, g, b = mcolors.to_rgb(color)
    blended_color = [(1 - alpha) * 1.0 + alpha * r, (1 - alpha) * 1.0 + alpha * g, (1 - alpha) * 1.0 + alpha * b]
    return blended_color

# Define time periods for different parts of the day with blended colors
periode_waktu = [
    (22, 24, blend_with_white('lightblue', 0.4), 'Dini Hari'),
    (0, 5, blend_with_white('lightblue', 0.4), 'Dini Hari'),
    (5, 10, blend_with_white('khaki', 0.4), 'Pagi'),
    (10, 14, blend_with_white('gold', 0.4), 'Siang'),
    (14, 18, blend_with_white('orange', 0.4), 'Sore'),
    (18, 22, blend_with_white('cornflowerblue', 0.4), 'Malam')
]

# IDA levels based on EN 13779:2007
ida_levels = {
    'IDA 1': (0, 800, '#4CAF50'),      # Green
    'IDA 2': (800, 1000, '#FFEB3B'),   # Yellow
    'IDA 3': (1000, 1400, '#FF9800'),  # Orange
    'IDA 4': (1400, 5000, '#F44336')   # Red
}

# Set the y-axis limits based on the entire dataset for consistency
y_min_co2 = df['CO2'].min()
y_max_co2 = df['CO2'].max()

# Iterate over each day in the data
for day in pd.date_range(df['Timestamp'].min().normalize(), df['Timestamp'].max().normalize()):
    # Filter data for the current day
    day_start = day + pd.Timedelta(hours=5)  # Start at 5 AM
    day_end = day + pd.Timedelta(days=1) + pd.Timedelta(hours=5)  # End at 5 AM next day
    daily_data = df[(df['Timestamp'] >= day_start) & (df['Timestamp'] < day_end)]
    
    # Create figure and axis with size adjusted for A4 width (8.27 inches)
    fig, ax1 = plt.subplots(figsize=(10, 6))

    # Plot CO2 data on the first y-axis
    ax1.plot(daily_data['Timestamp'][::30], daily_data['CO2'][::30], label='CO2', color='#1E88E5')  # Bright blue for CO2
    ax1.set_xlabel('Waktu')
    ax1.set_ylabel('CO2 (ppm)', color='#1E88E5')
    ax1.tick_params(axis='y', labelcolor='#1E88E5')
    ax1.set_ylim(y_min_co2, y_max_co2)

    # Set major and minor ticks on the x-axis
    ax1.xaxis.set_major_locator(mdates.HourLocator(interval=2))
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    plt.xticks(rotation=0)

    # Add shaded areas for different parts of the day
    for start_hour, end_hour, color, label in periode_waktu:
        start_time = day + pd.Timedelta(hours=start_hour)
        end_time = day + pd.Timedelta(hours=end_hour)
        if start_hour == 0:  # Adjust end_time for periods that go past midnight
            end_time += pd.Timedelta(days=1)
        if start_time <= daily_data['Timestamp'].max() and end_time >= daily_data['Timestamp'].min():
            ax1.axvspan(max(start_time, daily_data['Timestamp'].min()), min(end_time, daily_data['Timestamp'].max()), color=color, alpha=1.0, label=label)

    # Add horizontal dotted lines for relevant IDA levels
    relevant_ida_levels = {level: (start, end, color) for level, (start, end, color) in ida_levels.items() if end >= y_min_co2 and start <= y_max_co2}
    for level, (start, end, color) in list(relevant_ida_levels.items()):
        ax1.axhline(y=start, color=color, linestyle='--', linewidth=2)

    # Create a second y-axis for IDA level labels
    ax2 = ax1.twinx()
    ax2.set_ylim(ax1.get_ylim())
    ida_labels = []
    for level, (start, end, color) in relevant_ida_levels.items():
        if level == 'IDA 1':
            mid_point = (800 + y_min_co2) / 2
        elif level == 'IDA 2':
            mid_point = 800 + (1000 - 800) / 2
        elif level == 'IDA 3':
            mid_point = 1000 + (1400 - 1000) / 2
        elif level == 'IDA 4':
            mid_point = 1400 + (y_max_co2 - 1400) / 2
        ida_labels.append(mid_point)

    ax2.set_yticks(ida_labels)
    ax2.set_yticklabels(relevant_ida_levels.keys(), fontsize=16)
    ax2.tick_params(axis='y', labelsize=16)
    ax2.set_ylabel('Tingkat IDA', color='black')

    # Set the color of IDA levels axis labels to match corresponding lines
    for label, (level, (start, end, color)) in zip(ax2.get_yticklabels(), relevant_ida_levels.items()):
        label.set_color(color)

    # Ensure each label appears only once in the legend
    handles1, labels1 = ax1.get_legend_handles_labels()

    handles = handles1
    labels = labels1

    unique_handles_labels = {label: handle for handle, label in zip(handles, labels)}

    # Sort legend in desired order
    order = ["CO2", "Pagi", "Siang", "Sore", "Malam", "Dini Hari"]
    ordered_handles_labels = {k: unique_handles_labels[k] for k in order if k in unique_handles_labels}

    # Add legend
    fig.legend(ordered_handles_labels.values(), ordered_handles_labels.keys(), loc='upper right', bbox_to_anchor=(1, 1), fontsize='small', handlelength=2)

    # Format the title date in Indonesian
    day_name = day.strftime("%A")
    day_formatted = day.strftime("%d %B %Y")
    title_date = f"Tingkat Konsentrasi CO2 dalam Ruangan \n pada Hari {day_name}, {day_formatted}"

    # Add title with the formatted date
    plt.title(title_date)

    # Tight layout for better spacing
    fig.tight_layout()

    # Save plot as PNG file with the filename including parameters and date
    output_path = f'C:/Users/aryad/Downloads/SCD41/Penulisan/CO2_{csv_filename}_{day.strftime("%Y-%m-%d")}.png'
    fig.savefig(output_path, dpi=300)

    # Show plot
    plt.show()
