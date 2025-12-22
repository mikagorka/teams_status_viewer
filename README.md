# Teams Status Viewer 📱

A simple ESP32-based display that shows your Microsoft Teams status at your door, so your housemates know when you're available or in a call.

![Teams Status Display](https://img.shields.io/badge/Status-Working-brightgreen) ![Platform](https://img.shields.io/badge/Platform-ESP32-blue) ![License](https://img.shields.io/badge/License-MIT-yellow)

## Features ✨

- **Real-time Status Display**: Shows your Microsoft Teams status on an OLED display
- **Two States**:
  - ✅ **Available**: Green checkmark in white circle
  - ⊖ **Busy/In Call/Do Not Disturb**: Blinking minus sign in gray circle
- **Bluetooth LE Communication**: Wireless status updates from your Mac
- **Automatic Monitoring**: Python script monitors Teams logs and updates the display
- **Smooth Transitions**: Animated transitions between states

## Hardware Requirements 🛠️

### ESP32 with OLED Display
- **Recommended**: [Heltec WiFi LoRa 32 V3 with OLED Display](https://www.amazon.de/dp/B0FNMJ34V2?ref=ppx_yo2ov_dt_b_fed_asin_title)
- Features:
  - ESP32-S3 chip
  - Built-in 0.96" OLED display (128x64)
  - Bluetooth LE support
  - USB-C connection

### Additional Materials
- USB-C cable for programming and power
- Optional: 3D printed case or mounting solution

## Software Requirements 💻

### For ESP32
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.x or v2.x)
- ESP32 board support package
- Required libraries:
  - `Heltec ESP32 Dev-Boards` (for OLED display)
  - Built-in BLE libraries

### For macOS
- macOS with Microsoft Teams installed
- Python 3.7 or higher
- `bleak` Python library for Bluetooth LE

## Installation 🚀

### Step 1: Setup ESP32

1. **Install Arduino IDE** and add ESP32 board support:
   - Open Arduino IDE → Preferences
   - Add to "Additional Board Manager URLs": 
     ```
     https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.7/package_heltec_esp32_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "Heltec ESP32" and install

2. **Install Required Libraries**:
   - Go to Sketch → Include Library → Manage Libraries
   - Search and install: "Heltec ESP32 Dev-Boards"

3. **Upload the Code**:
   - Download `teams_status_display.ino`
   - Open it in Arduino IDE
   - Select your board: Tools → Board → Heltec WiFi LoRa 32(V3)
   - Select the correct port: Tools → Port
   - Click Upload ⬆️

4. **Verify**:
   - Open Serial Monitor (Tools → Serial Monitor, 115200 baud)
   - You should see: "Bluetooth started! Name: Teams Status"

### Step 2: Setup macOS Monitor

1. **Install Python dependencies**:
   ```bash
   pip3 install --user bleak
   ```

2. **Download the monitor script**:
   ```bash
   cd ~/
   git clone https://github.com/mikagorka/teams_status_viewer.git
   cd teams_status_viewer
   chmod +x teams_monitor.py
   ```

3. **Test the monitor**:
   ```bash
   python3 teams_monitor.py
   ```
   
   You should see:
   ```
   🚀 Teams Status Monitor started
   🔍 Searching for Teams Status device...
   ✅ Device found: XX:XX:XX:XX:XX:XX
   📂 Monitoring: MSTeams_20251222.log
   ```

### Step 3: Auto-Start on macOS (Optional)

To automatically start the monitor when you log in:

1. **Edit the LaunchAgent file**:
   ```bash
   nano ~/teams_status_viewer/com.teamsstatus.monitor.plist
   ```
   
   Replace `YOUR_USERNAME` with your actual macOS username (3 occurrences)

2. **Install the LaunchAgent**:
   ```bash
   cp ~/teams_status_viewer/com.teamsstatus.monitor.plist ~/Library/LaunchAgents/
   launchctl load ~/Library/LaunchAgents/com.teamsstatus.monitor.plist
   ```

3. **Check if it's running**:
   ```bash
   tail -f ~/teams_status_viewer/logs/monitor.log
   ```

## How It Works 🔧

### Architecture

```
Microsoft Teams (macOS)
         ↓
    Teams Log File
         ↓
  teams_monitor.py (Python)
         ↓
   Bluetooth LE
         ↓
    ESP32 Display
```

### Status Detection

The Python script monitors the Teams log file:
- **Location**: `~/Library/Group Containers/UBF8T346G9.com.microsoft.teams/Library/Application Support/Logs/MSTeams_YYYYMMDD.log`
- **Detection**: Searches for "OnAvailabilityUpdate" events
- **Triggers**:
  - `Available` → Shows checkmark ✅
  - `Busy`, `InACall`, `DoNotDisturb` → Shows blinking minus ⊖

### Display States

| Status | Display | Animation |
|--------|---------|-----------|
| Available | White circle with black checkmark | Static |
| In Call / Busy / DND | Gray circle with blinking minus | Blinks every 500ms |
| Status Change | Zoom out → Zoom in | Smooth transition |

## Troubleshooting 🔍

### ESP32 Issues

**Display stays blank**:
- Check if USB power is connected
- Verify correct board selection in Arduino IDE
- Open Serial Monitor to see debug output

**Bluetooth not visible**:
- Reset ESP32 (press RST button)
- Check Serial Monitor for "Bluetooth started!"
- Ensure no other device is connected to it

### macOS Monitor Issues

**"Device not found"**:
- Make sure ESP32 is powered on and nearby
- Check Bluetooth is enabled on Mac
- Try running: `python3 teams_monitor.py` and check output

**"Teams log not found"**:
- Verify Teams is installed and running
- Check if log path exists:
  ```bash
  ls ~/Library/Group\ Containers/UBF8T346G9.com.microsoft.teams/Library/Application\ Support/Logs/
  ```

**Status not updating**:
- Ensure Teams is actually changing status
- Check monitor logs: `tail -f ~/teams_status_viewer/logs/monitor.log`
- Restart the monitor script

## Customization 🎨

### Change Blink Speed

In `teams_status_display.ino`, line 108:
```cpp
if (millis() - lastBlinkTime >= 500) {  // Change 500 to desired milliseconds
```

### Change Device Name

In both files:
- `teams_status_display.ino`, line 73: `BLEDevice::init("Teams Status");`
- `teams_monitor.py`, line 10: `DEVICE_NAME = "Teams Status"`

### Add More Status States

You can extend the system to show more Teams states:
1. Add detection keywords in `teams_monitor.py` → `parse_teams_status()`
2. Create new status strings (e.g., "away", "offline")
3. Add corresponding display functions in `teams_status_display.ino`

## Files Overview 📁

- `teams_status_display.ino` - ESP32 Arduino sketch
- `teams_monitor.py` - macOS monitoring script
- `com.teamsstatus.monitor.plist` - LaunchAgent configuration for auto-start
- `README.md` - This file

## Contributing 🤝

Feel free to open issues or submit pull requests!

Ideas for improvements:
- Support for Windows/Linux
- Support for other communication platforms (Slack, Zoom)
- Battery-powered version with deep sleep
- Web interface for configuration
- Multiple status indicators

## License 📄

MIT License - Feel free to use and modify!

## Credits 👏

Created for showing Teams status to housemates. Built with ESP32 and Python.

---

**Enjoy your Teams Status Viewer!** 🎉

If you found this useful, give it a ⭐ on GitHub!