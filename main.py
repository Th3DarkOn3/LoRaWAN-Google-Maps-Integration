import paho.mqtt.client as mqtt
import json
from datetime import datetime
from geopy.geocoders import Nominatim


from websocket_server import WebsocketServer
import threading

APP_ID = "[ APP ID \ APP Name ]"
ACCESS_KEY = "[ API Key TTN MQTT server ]"

geolocator = Nominatim(user_agent="geoapi")

def on_connect(client, userdata, flags, rc):
    print(f"Connecté avec le code de résultat {rc}")
    client.subscribe(f'v3/{APP_ID}@ttn/devices/+/up')
    print(f"Abonné au topic : v3/{APP_ID}@ttn/devices/+/up")

def convert_to_timestamp(date_str):
    date_str_trimmed = date_str.split('.')[0] + 'Z'
    dt = datetime.strptime(date_str_trimmed, '%Y-%m-%dT%H:%M:%S%z')
    timestamp = dt.timestamp()
    return timestamp


def on_message(client, userdata, msg):
    string_payload = msg.payload.decode()
    json_payload = json.loads(string_payload)
    uplink_message = json_payload.get("uplink_message", {})

    if "decoded_payload" in uplink_message and "latitude" in uplink_message["decoded_payload"] and "longitude" in uplink_message["decoded_payload"]:
        latitude = uplink_message["decoded_payload"]["latitude"]
        longitude = uplink_message["decoded_payload"]["longitude"]
        location = geolocator.reverse((latitude, longitude), language='fr')
        city = None
        if location and 'address' in location.raw:
            address = location.raw['address']
            city = address.get('city') or address.get('town') or address.get('village')
            if city is not None:
                city = str(city)
        myCoordinates = {
            "lat": latitude,
            "lng": longitude,
            "label": city or "Inconnu"
        }
        print(f"myCoordinates : {myCoordinates}")
        save_json([myCoordinates])
    else:
        print(f"Pas de payload décodé...\nTrame : {uplink_message}")
        
def save_json(payload):
    # Sauvegarde le JSON complet à chaque nouveau message reçu
    with open("location.json", "w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2, ensure_ascii=False)
        
    try:
        ws_server.send_message_to_all("reload")
    except Exception as e:
        print("Erreur WebSocket :", e)
        
def start_ws_server():
    global ws_server
    ws_server = WebsocketServer(host='0.0.0.0', port=8765, loglevel=0)
    ws_server.run_forever()

ws_thread = threading.Thread(target=start_ws_server, daemon=True)
ws_thread.start()

client = mqtt.Client()
client.username_pw_set(APP_ID, ACCESS_KEY)
client.on_connect = on_connect
client.on_message = on_message

client.connect('eu1.cloud.thethings.network', 1883, 60)
client.loop_forever()
