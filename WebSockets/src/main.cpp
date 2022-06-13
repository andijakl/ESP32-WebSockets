#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

const int pin_ldr = 32;

// TODO: Update SSID and password of your local Wifi
const char *ssid = "*******";
const char *password = "*******";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Handle incoming web socket messages
// Note: we need to define this function before we reference it in setup()!
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  Serial.println("Web socket data received");
  // We only take a look at receiving data. There would also be other
  // events for (dis)connected clients, errors, etc.
  if (type == WS_EVT_DATA)
  {
    // Complete code for handling multi-part incoming messages or binary messages:
    // https://github.com/me-no-dev/ESPAsyncWebServer
    String receivedData = (char *)data;
    // Serial.println((char *)data);
    Serial.println(receivedData);
    if (receivedData.compareTo(String("on")) == 0)
    {
      Serial.println("Turn LED on");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.println("Turn LED off");
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

void setup()
{
  // Configure ESP32 Serial port and PINs
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin_ldr, INPUT);

  // Establish WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  // Print info on IP address and how to connect to the web socket
  Serial.print("Connect to: ws://");
  Serial.print(WiFi.localIP());
  Serial.println("/ws");

  // Add web socket event listener for incoming messages
  ws.onEvent(onWsEvent);

  // Connect the web socket handler to the web server
  server.addHandler(&ws);

  // Start the web server
  server.begin();
}

void loop()
{
  // Read the analog input GPIO to get the measurement from the LDR
  int ldr_val = analogRead(pin_ldr);

  // Print the measurement to the serial port and send it to all connected web socket clients
  String message = "LDR: " + String(ldr_val);
  Serial.println(message);
  ws.textAll(message);

  // Cleanup closed web socket connections and wait for a second for the next measurement
  ws.cleanupClients();
  delay(1000);
}
