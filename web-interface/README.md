# Puzzle Box Web Interface

A comprehensive web interface for managing multiple ESP32 puzzle boxes, creating custom puzzles, and monitoring real-time status across your puzzle box network.

## Features

### Multi-Box Management
- **Real-time Discovery**: Automatically detects connected puzzle boxes on the network
- **Live Status Monitoring**: See button presses, LED states, and puzzle progress in real-time
- **Health Monitoring**: Track online/offline status with automatic reconnection
- **Batch Operations**: Deploy puzzles or reset multiple boxes simultaneously

### Puzzle Creation & Management
- **Visual Puzzle Designer**: Create custom 3x3 button layouts with intuitive grid interface
- **Color Customization**: Choose from 10 button colors (Pink, Red, Orange, Blue, Violet, Yellow, White, Black, Brown, Green)
- **Puzzle Library**: Save, organize, and reuse puzzle configurations
- **Win Condition Setup**: Define which corner color indicates puzzle completion

### Real-Time Dashboard
- **Live Grid Visualization**: See actual button states and colors for each connected box
- **Corner Indicators**: Monitor win condition progress with visual corner status
- **WebSocket Updates**: Sub-second status updates for immediate feedback
- **Box Analytics**: Track puzzle completion times and interaction patterns

## Setup Instructions

### 1. ESP32 Configuration

First, update your ESP32 code with WiFi credentials:

```cpp
// In src/main.cpp, update these lines:
initWiFi("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
```

Build and upload to your ESP32:
```bash
pio run -t upload
```

### 2. Web Server Setup

Install Node.js dependencies:
```bash
cd web-interface
npm install
```

Start the server:
```bash
npm start
# Or for development with auto-restart:
npm run dev
```

The web interface will be available at `http://localhost:3000`

### 3. Box Registration

When ESP32 boxes connect to WiFi, they automatically:
1. Start a web server on port 80
2. Broadcast their status every second via serial output
3. Become discoverable by the management interface

## API Endpoints

### Box Management
- `GET /api/boxes` - List all connected boxes
- `GET /api/boxes/:boxId/status` - Get detailed status for specific box
- `POST /api/boxes/register` - Register a new box (called automatically by ESP32)

### Puzzle Management  
- `GET /api/puzzles` - List all saved puzzles
- `POST /api/puzzles` - Create new puzzle
- `PUT /api/puzzles/:puzzleId` - Update existing puzzle
- `DELETE /api/puzzles/:puzzleId` - Delete puzzle

### Operations
- `POST /api/deploy` - Deploy puzzle to selected boxes
- `POST /api/reset` - Reset selected boxes

## ESP32 API Endpoints

Each ESP32 box exposes these endpoints:

- `GET /api/status` - Current puzzle state, button positions, win condition
- `POST /api/puzzle` - Deploy new puzzle configuration
- `POST /api/reset` - Reset puzzle to initial state

## Usage Guide

### Creating a Custom Puzzle

1. Click the ➕ button next to "Puzzles" in the sidebar
2. Enter puzzle name and description
3. Select the target "Solve Color" for corner LEDs
4. Configure each of the 9 buttons using the dropdown selectors:
   - **Brown buttons**: Cycle all colors in their row to the right
   - **Green buttons**: Basic interaction (implementation varies)
   - **Other colors**: Puzzle-specific behaviors
5. Click "Save Puzzle"

### Deploying Puzzles

1. Click "🚀 Deploy" next to any puzzle in the list
2. Select which boxes should receive the puzzle
3. Click "Deploy" to send the configuration
4. Boxes will automatically reset and load the new puzzle

### Monitoring Progress

The dashboard shows real-time status for all connected boxes:
- **Grid Display**: Current button colors and pressed states
- **Corner Indicators**: Green dots show active corners (puzzle progress)
- **Connection Status**: Online/offline status with last update time

## Network Architecture

```
[ESP32 Boxes] ←→ [WiFi Network] ←→ [Node.js Server] ←→ [Web Interface]
```

- ESP32 boxes connect to local WiFi and expose REST APIs
- Node.js server manages multiple boxes and provides central coordination
- Web interface connects via WebSocket for real-time updates
- All communication uses JSON for data exchange

## Troubleshooting

### Box Not Appearing
1. Verify ESP32 is connected to same WiFi network
2. Check serial monitor for connection status and IP address
3. Ensure firewall allows HTTP traffic on port 80

### Web Interface Not Loading
1. Verify Node.js server is running on port 3000
2. Check browser console for JavaScript errors
3. Ensure all npm dependencies are installed

### Real-time Updates Not Working
1. Check WebSocket connection status in browser developer tools
2. Verify server is running and accessible
3. Refresh the web page to reestablish connection

## Development

The codebase is organized as follows:

```
web-interface/
├── server.js           # Node.js backend server
├── package.json        # Dependencies and scripts
├── public/
│   ├── index.html      # Main web interface
│   ├── app.js          # Frontend JavaScript
│   └── style.css       # Styling and animations
└── README.md          # This file
```

Key files in the ESP32 codebase:
- `include/webServer.h` - Web server declarations
- `src/webServer.cpp` - WiFi and HTTP server implementation
- `src/main.cpp` - Updated main loop with web server integration