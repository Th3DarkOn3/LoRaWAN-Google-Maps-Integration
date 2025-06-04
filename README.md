# LoRaWAN to Google Maps Integration

This project demonstrates how to transmit a **GPS position (latitude and longitude)** from a **LoRaWAN device** (real or simulated) to a **web-based Google Maps interface**. The position is passed through a JSON intermediary and visualized in near real-time using a **WebSocket-triggered map reload**.

> ⚠️ **Note**: The entire system is configured for use on a **single device** in a **local-only environment**, but it can be easily adapted for **global deployments**.

---

## 🚀 Quick Start

### 1. Clone the repository

```bash
git clone https://github.com/Th3DarkOn3/LoRaWAN-Google-Maps-Integration.git
cd LoRaWAN-Google-Maps-Integration
```

---

### 2. Create a virtual environment (recommended)

Using Python ≥ 3.7:

```bash
python -m venv .venv
.venv\Scripts\activate  # On Windows
```

> 💡 For best development experience in **VSCode**, refer to this setup guide:  
> [Form_pycharm_to_vscode](https://github.com/Th3DarkOn3/Form_pycharm_to_vscode)

---

### 3. Install dependencies

```bash
pip install -r requirements.txt
```

---

### 4. Configure your application

#### 🔑 Edit `main.py`

Replace the placeholders with:
- Your **TTN (The Things Network)** application name
- Your **TTN API key**

```python
# Example in main.py
APP_ID = "YOUR_TTN_APP_ID"
ACCESS_KEY = "YOUR_TTN_API_KEY"
```

---

### 5. Configure your Google Maps keys

#### 🗺️ In `./Frontend/index.html` (line 13)

Replace the placeholder with your **Google Maps API key**:

```html
<script src="https://maps.googleapis.com/maps/api/js?key=YOUR_GOOGLE_API_KEY&libraries=marker&callback=initMap" async defer></script>
```

#### 🆔 In `./Frontend/script.js` (line 9)

Replace the placeholder with your **Google Maps Map ID**:

```javascript
map = new google.maps.Map(document.getElementById("map"), {
  center: { lat: 46.603354, lng: 1.888334 },
  zoom: 6,
  mapId: "YOUR_GOOGLE_MAP_ID"
});
```

---

### 6. Run the backend server

```bash
python main.py
```

---

### 7. Open the map interface

Open `./Frontend/index.html` in your browser.

---

### 8. Start sending data from the LoRaWAN device

Your LoRaWAN device should now transmit position data to TTN. The server (`main.py`) captures this and updates `location.json`, which is used by the frontend to update the map through WebSocket.

---

### 9. Simulated LoRaWAN device (optional)

The file [`main.c`](./main.c) contains a sample firmware written for the **B-L072Z-LRWAN1 STM32** development board. It simulates GPS positions and sends them over LoRaWAN.

---

## 🧠 Project Overview

This project demonstrates:
- Position transmission from LoRaWAN to TTN
- JSON-based data handling
- WebSocket-based page refresh
- Google Maps integration with advanced markers
- Local deployment for single-device development

---

## 📦 Dependencies

- Python 3.13+
- WebSocket support
- Google Maps JavaScript API
- The Things Network (TTN) account and access

---

## 📌 Notes

- Designed for **local single-device** usage.
- Easily extendable for **multi-device or global use** by:
  - Serving HTML via a web server
  - Hosting WebSocket endpoint on a public IP or in the cloud
  - Exposing backend services securely

---

## 📜 License

This project has no license but if you find it interesting you can leave a star

---
