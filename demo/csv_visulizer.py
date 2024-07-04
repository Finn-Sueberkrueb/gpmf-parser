import pandas as pd
import plotly.graph_objects as go

# CSV-Datei einlesen
csv_filename = '/Users/hanna/Desktop/test/GX010388.MP4.csv'
df = pd.read_csv(csv_filename)

# Plotly-Figur erstellen
fig = go.Figure()

# Daten für Beschleunigung (ACCL) plotten
accl_df = df[df['Type'] == 'ACCL']
fig.add_trace(go.Scatter(x=accl_df['Time'], y=accl_df['X'],
              mode='lines', name='X-Achse Beschleunigung (ACCL)'))
fig.add_trace(go.Scatter(x=accl_df['Time'], y=accl_df['Y'],
              mode='lines', name='Y-Achse Beschleunigung (ACCL)'))
fig.add_trace(go.Scatter(x=accl_df['Time'], y=accl_df['Z'],
              mode='lines', name='Z-Achse Beschleunigung (ACCL)'))

# Daten für Gyroskop (GYRO) plotten
gyro_df = df[df['Type'] == 'GYRO']
fig.add_trace(go.Scatter(
    x=gyro_df['Time'], y=gyro_df['X'], mode='lines', name='X-Achse Gyroskop (GYRO)'))
fig.add_trace(go.Scatter(
    x=gyro_df['Time'], y=gyro_df['Y'], mode='lines', name='Y-Achse Gyroskop (GYRO)'))
fig.add_trace(go.Scatter(
    x=gyro_df['Time'], y=gyro_df['Z'], mode='lines', name='Z-Achse Gyroskop (GYRO)'))

# Daten für Gravitation (GRAV) plotten
grav_df = df[df['Type'] == 'GRAV']
fig.add_trace(go.Scatter(
    x=grav_df['Time'], y=grav_df['X'], mode='lines', name='X-Achse Gravitation (GRAV)'))
fig.add_trace(go.Scatter(
    x=grav_df['Time'], y=grav_df['Y'], mode='lines', name='Y-Achse Gravitation (GRAV)'))
fig.add_trace(go.Scatter(
    x=grav_df['Time'], y=grav_df['Z'], mode='lines', name='Z-Achse Gravitation (GRAV)'))

# Layout anpassen
fig.update_layout(
    title='Sensor Daten über die Zeit',
    xaxis_title='Zeit (s)',
    yaxis_title='Messwerte',
    legend_title='Sensortyp und Achse'
)

# Plot anzeigen
fig.show()
