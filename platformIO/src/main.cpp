#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// WiFi configuration
const char *TARGET_SSID_PREFIX = "iNav Radar";
const char *PSK_PASSWORD = "inavradar";
const IPAddress TARGET_IP(192, 168, 4, 1);
const IPAddress TARGET_SUBNET(255, 255, 255, 0);

// Radar endpoints
const char *STATUS_ENDPOINT = "http://192.168.4.1/system/status";
const char *PLANES_ENDPOINT = "http://192.168.4.1/peermanager/status";

// Function declarations
void scanNetworks();
bool connectToNetwork(const char *ssid);
bool verifyConnection();
bool checkRadarStatus();
void getPlaneData();

bool isConnected = false;
bool radarActive = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup complete");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    // WiFi connection lost, reset state
    Serial.println("WiFi connection lost, reconnecting...");
    isConnected = false;
    radarActive = false;
  }

  if (!isConnected)
  {
    // Scan for networks
    scanNetworks();
  }
  else if (!radarActive)
  {
    // Check radar status once
    radarActive = checkRadarStatus();
    if (radarActive)
    {
      Serial.println("Radar is active, starting plane data collection");
    }
    else
    {
      // If status check failed, wait a bit before trying again
      delay(2000);
    }
  }
  else
  {
    // Get plane data
    getPlaneData();
    delay(1000); // Wait 1 second between updates
  }
}

void scanNetworks()
{
  Serial.println("Scanning for networks...");

  // Start WiFi scan
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete");

  if (n == 0)
  {
    Serial.println("No networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");

    for (int i = 0; i < n; ++i)
    {
      String ssid = WiFi.SSID(i);

      // Check if SSID starts with "iNav Radar"
      if (ssid.startsWith(TARGET_SSID_PREFIX))
      {
        Serial.print("Found iNav Radar network: ");
        Serial.println(ssid);
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm");

        // Try to connect to this network
        if (connectToNetwork(ssid.c_str()))
        {
          Serial.println("Successfully connected!");
          isConnected = true;
          return;
        }
      }
    }
  }
}

bool connectToNetwork(const char *ssid)
{
  Serial.print("Attempting to connect to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, PSK_PASSWORD);

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connection failed");
    return false;
  }

  // Verify we're on the correct subnet
  IPAddress localIP = WiFi.localIP();
  if (localIP[0] == 192 && localIP[1] == 168 && localIP[2] == 4)
  {
    Serial.print("Connected! IP address: ");
    Serial.println(localIP);
    return true;
  }
  else
  {
    Serial.println("Connected but wrong subnet");
    WiFi.disconnect();
    return false;
  }
}

bool checkRadarStatus()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
    return false;
  }

  WiFiClient client;
  HTTPClient http;
  http.begin(client, STATUS_ENDPOINT);
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    String payload = http.getString();
    Serial.print("Status response: ");
    Serial.println(payload);
    http.end();

    // If we get any valid response, consider the radar active
    // No need to check for specific text as just getting a response means the device is online
    return true;
  }
  else
  {
    Serial.print("Error checking status: ");
    Serial.println(httpCode);
    http.end();
    return false;
  }
}

void getPlaneData()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
    return;
  }

  WiFiClient client;
  HTTPClient http;
  http.begin(client, PLANES_ENDPOINT);
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    String payload = http.getString();

    // Parse JSON response
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      http.end();
      return;
    }

    // Print basic info
    Serial.println("\n=== Radar Status ===");
    Serial.printf("My ID: %s\n", doc["myID"].as<const char *>());
    Serial.printf("Total Peers: %d\n", doc["count"].as<int>());
    Serial.printf("Active Peers: %d\n", doc["countActive"].as<int>());

    // Process each peer
    JsonArray peers = doc["peers"];
    for (JsonObject peer : peers)
    {
      Serial.println("\n=== Peer Info ===");
      Serial.printf("ID: %s\n", peer["id"].as<const char *>());
      Serial.printf("Name: %s\n", peer["name"].as<const char *>());
      Serial.printf("Last Update: %d ms ago\n", peer["age"].as<int>());
      Serial.printf("Location: %.6f, %.6f\n", peer["lat"].as<float>(), peer["lon"].as<float>());
      Serial.printf("Altitude: %d m\n", peer["alt"].as<int>());
      Serial.printf("Speed: %d km/h\n", peer["groundSpeed"].as<int>());
      Serial.printf("Course: %d°\n", peer["groundCourse"].as<int>());
      Serial.printf("Distance: %.2f m\n", peer["distance"].as<float>());
      Serial.printf("Course to: %d°\n", peer["courseTo"].as<int>());
      Serial.printf("Relative Altitude: %d m\n", peer["relativeAltitude"].as<int>());
      Serial.printf("Packets Received: %d\n", peer["packetsReceived"].as<int>());
    }
  }
  else
  {
    Serial.print("Error getting plane data: ");
    Serial.println(httpCode);
  }
  http.end();
}