# Teams Status Viewer

A macOS application that displays your Microsoft Teams status on an external display using Bluetooth Low Energy (BLE) communication.

## Overview

This application monitors your Microsoft Teams status and sends it to a connected BLE device for visual display. Perfect for letting others know when you're in a meeting, available, or busy without them having to check Teams.

## Features

- Real-time Microsoft Teams status monitoring
- Bluetooth Low Energy (BLE) communication with external display devices
- Easy-to-use macOS interface
- Automatic status updates
- Low power consumption

## macOS Installation Guide

### Prerequisites

- macOS 10.14 (Mojave) or later
- Python 3.8 or higher
- Bluetooth 4.0+ compatible Mac
- Admin/sudo access for some operations

### Step 1: Verify Python Installation

First, check if Python 3 is installed on your system:

```bash
python3 --version
```

If Python 3 is not installed, install it using Homebrew:

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Python 3
brew install python3
```

### Step 2: Clone the Repository

Clone this repository to your local machine:

```bash
git clone https://github.com/mikagorka/teams_status_viewer.git
cd teams_status_viewer
```

### Step 3: Create Python Virtual Environment

Creating a virtual environment keeps your project dependencies isolated:

```bash
# Create a virtual environment named 'venv'
python3 -m venv venv

# Activate the virtual environment
source venv/bin/activate
```

**Note:** You'll need to activate the virtual environment each time you open a new terminal session. You'll know it's activated when you see `(venv)` at the beginning of your terminal prompt.

### Step 4: Install Required Libraries

With the virtual environment activated, install all required dependencies:

```bash
# Upgrade pip to the latest version
pip install --upgrade pip

# Install bleak (Bluetooth Low Energy library)
pip install bleak

# Install other required libraries
pip install pyobjc-framework-Cocoa
pip install requests
```

Alternatively, if a `requirements.txt` file is present:

```bash
pip install -r requirements.txt
```

### Step 5: Configure Bluetooth Permissions

macOS requires explicit permission for Bluetooth access:

1. Open **System Preferences** (or **System Settings** on macOS 13+)
2. Navigate to **Security & Privacy** → **Privacy** → **Bluetooth**
3. Ensure Terminal (or your IDE) has permission to use Bluetooth
4. If prompted during first run, click **Allow** when asked for Bluetooth access

### Step 6: Set Up Microsoft Teams Integration

To monitor your Teams status, you'll need to configure authentication:

1. Create a configuration file (if not exists):
   ```bash
   touch config.json
   ```

2. Add your Teams credentials/configuration to `config.json`:
   ```json
   {
     "teams_log_path": "~/Library/Application Support/Microsoft/Teams/logs.db",
     "ble_device_name": "Your_BLE_Device_Name",
     "update_interval": 5
   }
   ```

**Note:** Adjust the configuration values according to your specific setup.

### Step 7: Pair Your BLE Device

Before running the application, pair your BLE display device:

1. Turn on your BLE device and set it to pairing mode
2. Open **System Preferences** → **Bluetooth**
3. Wait for your device to appear and click **Connect**
4. Note the device name for configuration

### Step 8: Run the Application

With everything set up, run the application:

```bash
# Make sure your virtual environment is activated
source venv/bin/activate

# Run the main application
python3 main.py
```

### Step 9: (Optional) Create a Launch Script

For easier launching, create a shell script:

```bash
# Create the launch script
cat > run_teams_viewer.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
source venv/bin/activate
python3 main.py
EOF

# Make it executable
chmod +x run_teams_viewer.sh
```

Now you can start the application by running:

```bash
./run_teams_viewer.sh
```

### Step 10: (Optional) Run at Startup

To automatically start the application when you log in:

1. Open **System Preferences** → **Users & Groups** → **Login Items**
2. Click the **+** button
3. Navigate to your `run_teams_viewer.sh` script and add it

Alternatively, create a LaunchAgent:

```bash
# Create LaunchAgents directory if it doesn't exist
mkdir -p ~/Library/LaunchAgents

# Create the plist file
cat > ~/Library/LaunchAgents/com.teams.statusviewer.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.teams.statusviewer</string>
    <key>ProgramArguments</key>
    <array>
        <string>/bin/bash</string>
        <string>$(pwd)/run_teams_viewer.sh</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
EOF

# Load the LaunchAgent
launchctl load ~/Library/LaunchAgents/com.teams.statusviewer.plist
```

## Troubleshooting

### Virtual Environment Issues

**Problem:** Cannot activate virtual environment
```bash
# Solution: Ensure you're in the correct directory
cd /path/to/teams_status_viewer
source venv/bin/activate
```

### Bluetooth Connection Issues

**Problem:** Cannot find BLE device
- Ensure the device is powered on and in pairing mode
- Check that Bluetooth is enabled on your Mac
- Try unpairing and re-pairing the device
- Run a BLE scan to verify device visibility:
  ```python
  python3 -c "import asyncio; from bleak import BleakScanner; asyncio.run(BleakScanner.discover())"
  ```

### Permission Errors

**Problem:** "Permission denied" for Bluetooth access
- Go to System Preferences → Security & Privacy → Privacy → Bluetooth
- Add Terminal or your Python IDE to the allowed apps
- Restart the application

### Teams Status Not Updating

**Problem:** Status doesn't reflect Teams changes
- Verify the Teams log path in `config.json` is correct
- Ensure Microsoft Teams is running
- Check that you're signed in to Teams
- Review application logs for errors

### Library Installation Issues

**Problem:** `pip install bleak` fails
```bash
# Solution: Ensure you have the latest pip and setuptools
pip install --upgrade pip setuptools wheel
pip install bleak
```

## Deactivating the Virtual Environment

When you're done working with the application:

```bash
deactivate
```

## Uninstallation

To completely remove the application:

```bash
# Stop and remove LaunchAgent (if configured)
launchctl unload ~/Library/LaunchAgents/com.teams.statusviewer.plist
rm ~/Library/LaunchAgents/com.teams.statusviewer.plist

# Remove the application directory
cd ..
rm -rf teams_status_viewer
```

## Requirements

- Python 3.8+
- bleak >= 0.20.0
- pyobjc-framework-Cocoa
- requests

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, questions, or contributions, please open an issue on GitHub.

## Acknowledgments

- Built with [Bleak](https://github.com/hbldh/bleak) - Python BLE library
- Microsoft Teams integration

---

**Last Updated:** 2026-01-02