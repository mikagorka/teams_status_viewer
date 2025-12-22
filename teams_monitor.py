#!/usr/bin/env python3

import asyncio
import time
import os
import re
from datetime import datetime
from bleak import BleakClient, BleakScanner

# Configuration
DEVICE_NAME = "Teams Status"
CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

current_status = None
ble_device = None

async def find_device():
    """Searches for the BLE device"""
    global ble_device
    print("🔍 Searching for Teams Status device...")
    ble_device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)
    if ble_device:
        print(f"✅ Device found: {ble_device.address}")
    else:
        print("❌ Device not found!")
    return ble_device

async def send_status(status):
    """Sends status to ESP32"""
    global current_status, ble_device
    
    if status == current_status:
        return
    
    if not ble_device:
        await find_device()
        if not ble_device:
            return
    
    try:
        async with BleakClient(ble_device) as client:
            await client.write_gatt_char(CHAR_UUID, status.encode())
            current_status = status
            emoji = "📗" if status == "available" else "📞"
            print(f"{emoji} Status sent: {status} [{datetime.now().strftime('%H:%M:%S')}]")
    except Exception as e:
        print(f"❌ Error: {e}")
        ble_device = None

def get_teams_log_path():
    """Finds current Teams log file"""
    log_dir = os.path.expanduser("~/Library/Group Containers/UBF8T346G9.com.microsoft.teams/Library/Application Support/Logs/")
    
    # Find newest MSTeams log
    today = datetime.now().strftime("%Y%m%d")
    log_file = f"MSTeams_{today}.log"
    log_path = os.path.join(log_dir, log_file)
    
    if os.path.exists(log_path):
        return log_path
    
    # If today's log doesn't exist, take newest
    try:
        logs = [f for f in os.listdir(log_dir) if f.startswith("MSTeams_")]
        if logs:
            latest = sorted(logs)[-1]
            return os.path.join(log_dir, latest)
    except:
        pass
    
    return None

def parse_teams_status(log_path):
    """Reads Teams status from log"""
    try:
        with open(log_path, 'r', encoding='utf-8', errors='ignore') as f:
            # Read only last 200 lines (performance)
            lines = f.readlines()[-200:]
            
            # Search for last Availability Update
            for line in reversed(lines):
                if "OnAvailabilityUpdate" in line or "availability update" in line.lower():
                    print(f"📋 Found: {line.strip()}")
                    
                    # Parse status from line
                    # InCall, Busy, DoNotDisturb → incall (minus)
                    if any(keyword in line for keyword in [
                        "Busy",
                        "InACall",
                        "DoNotDisturb",
                        "Do not disturb"
                    ]):
                        return "incall"
                    elif "Available" in line or "PresenceUnknown" in line:
                        return "available"
            
            # Fallback: Check for Call-Keywords
            recent = ''.join(lines)
            if "call_state\": \"Connected" in recent or "meetingUpdate" in recent:
                return "incall"
                
    except Exception as e:
        print(f"⚠️ Log error: {e}")
    
    return "available"

async def monitor_loop():
    """Main loop"""
    print("🚀 Teams Status Monitor started")
    
    await find_device()
    
    last_check = ""
    
    while True:
        log_path = get_teams_log_path()
        
        if not log_path:
            print("❌ Teams log not found!")
            await asyncio.sleep(5)
            continue
        
        # Only output when log has changed
        if log_path != last_check:
            print(f"📂 Monitoring: {os.path.basename(log_path)}")
            last_check = log_path
        
        status = parse_teams_status(log_path)
        await send_status(status)
        
        await asyncio.sleep(5)

if __name__ == "__main__":
    try:
        asyncio.run(monitor_loop())
    except KeyboardInterrupt:
        print("\n👋 Monitor stopped")
