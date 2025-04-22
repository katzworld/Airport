# iNav Radar ATC Formation Tracker
![image](https://github.com/user-attachments/assets/779b278d-7155-45ac-9ec4-5fcba05a9d46)

This project creates a complete solution for tracking multiple aircraft equipped with iNav flight controllers on a locally hosted interactive map. It consists of an ESP8266-based WiFi bridge that connects to iNav Radar networks and forwards telemetry data to a web application displaying real-time positions on custom map tiles.

## Features

- 🛩️ Multi-aircraft tracking with real-time position updates
- 📡 Automatic connection to nearby iNav Radar networks
- 🗺️ Custom map tilesets (.mbtiles format) with offline support
- 📱 Mobile-friendly responsive web interface
- 🚨 Lost aircraft tracking and alerts with last known position
- 📊 Formation flight data visualization for multiple aircraft
- 📈 Flight path trails showing historical routes
- 🔄 Automatic map centering on active or lost aircraft
- 🔌 Web Serial API for direct browser-to-hardware connection

## System Architecture

The system consists of two main components:

### 1. ESP8266 WiFi Bridge (PlatformIO)

- Scans for and connects to iNav Radar WiFi networks automatically
- Polls the iNav Radar API for aircraft position data
- Formats and forwards telemetry via serial connection
- Handles reconnection and error recovery
- Compatible with most ESP8266-based boards (NodeMCU, Wemos D1, etc.)

### 2. Web Interface (Python Flask)

- Provides a web server for accessing the map interface
- Connects directly to the ESP8266 via browser's Web Serial API
- Displays aircraft positions on interactive map
- Supports custom offline map tiles (.mbtiles format)
- Provides detailed telemetry information for each aircraft

## Hardware Requirements

- ESP8266-based development board (NodeMCU v0.9/v2, Wemos D1, etc.)
- USB connection to host computer
- iNav-equipped aircraft with Radar feature enabled

## Software Dependencies

### ESP8266 Bridge

- Arduino framework
- ESP8266WiFi library
- ESP8266HTTPClient library  
- ArduinoJson (v6.21.3+)

### Web Interface

- Python 3.6+ with Flask
- Modern web browser with Web Serial API support (Chrome, Edge, etc.)
- OpenLayers for map rendering
- SQL.js for processing MBTiles map files

## Installation

### ESP8266 Bridge Setup

1. Install PlatformIO (VS Code extension or CLI)
2. Connect your ESP8266 board via USB
3. Open the `platformIO` directory
4. Build and upload the firmware:

```bash
cd platformIO
pio run -t upload
```

5. Monitor the serial output (optional):

```bash
pio device monitor -b 115200
```

![image](https://github.com/user-attachments/assets/3775d1e4-5878-4ab2-b263-2c7dbe037de3)

### Web Interface Setup

1. Install Python dependencies:

```bash
python -m venv .venv
source .venv/bin/activate  # Linux/Mac
# or
.venv\Scripts\activate     # Windows

pip install -r requirements.txt
```

2. Start the web server:

```bash
python app.py
```

3. Open your browser and navigate to:

```
http://localhost:5000
```

## Usage Instructions

### Connecting to Aircraft

1. Power up your iNav-equipped aircraft with Radar feature enabled
2. Connect the ESP8266 bridge to your computer via USB
3. In the web interface, click "Connect Serial" in the Radar Status panel
4. Select the appropriate serial port from the browser dialog
5. The ESP8266 will automatically scan for and connect to the iNav Radar WiFi network
6. Aircraft locations will appear on the map as data is received

### Map Navigation

- Pan: Click and drag the map
- Zoom: Use mouse wheel or pinch gestures on mobile
- Select Aircraft: Click on an aircraft icon or name in the list
- Center on Aircraft: Click any aircraft in the side panel
- View Details: Selected aircraft shows detailed telemetry data

### Custom Maps

The application supports custom offline maps in MBTiles format:

1. Click on "Map Options" in the top-left corner of the interface
2. Click "Select OSM Map File" and choose a .mbtiles file
3. The map will reload with your custom tiles
4. Your map selection is remembered between sessions

### Lost Aircraft Tracking

If an aircraft loses connection:

1. Its marker turns red and begins to flash
2. A warning banner appears with the aircraft name and time since last contact
3. The map automatically centers on the last known position
4. The flight path remains visible to help with recovery

### Mobile Usage

The interface is fully responsive for mobile devices:

1. Collapsible info panels maximize map viewing area
2. Touch-optimized controls for map navigation
3. Responsive layout adapts to any screen size
4. Aircraft selection and tracking works with touch input

## ESP8266 Bridge Operation

The ESP8266 bridge performs these operations:

1. Scans for WiFi networks starting with "iNav Radar"
2. Connects using the standard password ("inavradar")
3. Verifies connection to the iNav Radar API (192.168.4.1)
4. Polls two endpoints:
   - `/system/status` - Verifies system is online
   - `/peermanager/status` - Gets aircraft telemetry data
5. Parses the JSON response and formats it for the web interface
6. Sends formatted data via serial connection at 115200 baud
7. Handles reconnection if the WiFi signal is lost

## Configuration Options

### ESP8266 Bridge

Edit `platformIO/src/main.cpp` to change:

- WiFi network prefix and password
- API endpoints and polling interval
- Serial connection speed

### Web Interface

Edit `templates/index.html` to adjust:

- Default map center coordinates and zoom level
- Aircraft icon appearance and size
- Aircraft trail length and color
- Lost aircraft detection thresholds

## Troubleshooting

### ESP8266 Bridge Issues

- Clean 5volt and GND connection (OTA wifi, not uart) for the Inav Radar device on the Airport  
- Ensure iNav Radar is enabled on your flight controller
- Verify the ESP8266 is receiving power (LED indicators)
- Check serial monitor output for connection status
- Verify WiFi settings match your iNav configuration

### Web Interface Issues

- Ensure you're using a Web Serial API compatible browser
- Check browser console for JavaScript errors
- Verify the Flask server is running without errors
- Try selecting a different serial port or reconnecting

## Related Projects

This project is linked to the [FormationFlight](https://github.com/FormationFlight/FormationFlight/tree/master) project and extends its capabilities with local map visualization.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- [iNav](https://github.com/iNavFlight/inav) team for the radar protocol
- [OpenMapTiles](https://openmaptiles.org/) for map tile support
- [OpenLayers](https://openlayers.org/) for the mapping interface
- [FormationFlight](https://github.com/FormationFlight/FormationFlight) project for inspiration
