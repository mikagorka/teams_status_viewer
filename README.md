# Teams Status Viewer 📱

Show your Microsoft Teams status on an ESP32 OLED display at your door - so your housemates know when you're available or in a call.

## Features ✨

- **Real-time Status**: Shows Teams status on OLED display
- **Two States**: ✅ Available (checkmark) | ⊖ Busy/In Call/DND (blinking minus)
- **Bluetooth LE**: Wireless updates from your Mac
- **Auto-Monitor**: Python script monitors Teams logs

## Hardware 🛠️

**ESP32 with OLED**: [Heltec WiFi LoRa 32 V3](https://www.amazon.de/dp/B0FNMJ34V2?ref=ppx_yo2ov_dt_b_fed_asin_title)
- ESP32-S3 chip
- 0.96" OLED display (128x64)
- Bluetooth LE
- USB-C

## Setup 🚀

### 1. ESP32

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add board support: `https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.7/package_heltec_esp32_index.json`
3. Install library: "Heltec ESP32 Dev-Boards"
4. Upload `teams_status_display.ino`
5. Select board: **Heltec WiFi LoRa 32(V3)**

### 2. macOS Monitor

```bash
# Install dependencies
pip3 install bleak

# Clone repo
git clone https://github.com/mikagorka/teams_status_viewer.git
cd teams_status_viewer

# Test it
python3 teams_monitor.py
```

### 3. Auto-Start (Pick One)

**Option A: Cron (Simple)**
```bash
# Edit crontab
crontab -e

# Add this line (replace YOUR_USERNAME):
@reboot sleep 30 && /Users/YOUR_USERNAME/teams_venv/bin/python3 /Users/YOUR_USERNAME/teams_status_viewer/teams_monitor.py >> /Users/YOUR_USERNAME/teams_status_viewer/logs/monitor.log 2>&1 &

# Create log folder
mkdir -p ~/teams_status_viewer/logs
```

**Option B: LaunchAgent (Recommended)**
```bash
# Edit username in plist file
nano com.teamsstatus.monitor.plist  # Replace YOUR_USERNAME

# Install
cp com.teamsstatus.monitor.plist ~/Library/LaunchAgents/
launchctl load ~/Library/LaunchAgents/com.teamsstatus.monitor.plist
```

**Option C: Manual Start**
```bash
~/teams_venv/bin/python3 ~/teams_status_viewer/teams_monitor.py
```

## How It Works 🔧

```
Teams App → Log File → teams_monitor.py → Bluetooth LE → ESP32 Display
```

Script monitors: `~/Library/Group Containers/UBF8T346G9.com.microsoft.teams/Library/Application Support/Logs/`

## Troubleshooting 🔍

**ESP32 blank screen**: Check USB power, verify board selection, check Serial Monitor (115200 baud)

**Device not found**: Ensure ESP32 is on, Bluetooth enabled, try `python3 teams_monitor.py`

**Status not updating**: Check Teams is running, verify log path exists, restart monitor script

**Check logs**:
```bash
tail -f ~/teams_status_viewer/logs/monitor.log
```

## Customization 🎨

**Change blink speed**: Edit `teams_status_display.ino` line 108
```cpp
if (millis() - lastBlinkTime >= 500) {  // Change 500ms
```

**Change device name**: Update both files
- `teams_status_display.ino` line 73: `BLEDevice::init("Teams Status");`
- `teams_monitor.py` line 10: `DEVICE_NAME = "Teams Status"`

## Files 📁

- `teams_status_display.ino` - ESP32 code
- `teams_monitor.py` - macOS monitor script
- `com.teamsstatus.monitor.plist` - Auto-start service

## License 📄

MIT License

---

**Give it a ⭐ if you found this useful!**