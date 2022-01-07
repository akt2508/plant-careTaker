//including header files
#include <ESP8266WiFi.h>
#include <ESP_Notify.h>

String inputs;

#define relay1 2		//Connect relay1 to pin 9
#define relay2 3		//COnnect relay2 to pin 8

#define WIFI_SSID "CONFEDENTIAL DATA"
#define WIFI_PASSWORD "CONFEDENTIAL DATA"

#define Device_ID "DEVICE_ID"

ESP_Notify notifier;

void
initialize ()
{
  Serial.begin (9600);		//Set rate for communicating with moisture sensor
  Serial.begin (115200);	// connection port with WiFi
  Serial.begin (9601);		// connecting with bluetooth to interact with mobile device

  pinMode (relay1, OUTPUT);	//Set relay1 as an output
  pinMode (relay2, OUTPUT);	//Set rlay2 as an Output
  digitalWrite (relay1, LOW);	//Switch relay1 off

  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);

  Serial.print ("\nConnecting");

  while (WiFi.status () != WL_CONNECTED)
    {
      Serial.print ('.');
      delay (500);
    }

  Serial.print ("\nWiFi Connected");
}

void
setup ()
{
  while (WiFi.status () != WL_CONNECTED)
    {
      initialize ();
      Serial.print ("\nFailed to connect, trying again...");
      delay (8 * 1000);
    }
}

int timeout = 0;

void
FireNotification (String body)
{
  notifier.sendNotification (Device_ID, "Alert", body);
}

void
stopWaterFlow ()
{
  digitalWrite (relay1, LOW);
  FireNotification ("Enough moisture in the soil, stopping the water flow");
}

void
startWaterFlow ()
{
  digitalWrite (relay1, HIGH);
  timeout = 5;
  FireNotification ("Plant is low on moisture, starting the water flow");
}

void
WaterController ()
{
  int sensorValue = analogRead (A0);
  if (sensorValue < 200 && !digitalRead (relay1))
    {
      startWaterFlow ();
    }
  else
    {
      if (timeout)
	{
	  timeout--;
	  delay (1000);
	}
      else
	{
	  stopWaterFlow ();
	}
    }
}

void
loop ()
{
  WaterController ();
  while (Serial.available ())	// check if device is available
    {
      char c = Serial.read ();
      if (c == '#')
	{
	  break;
	}
      inputs += c;
    }
  if (inputs == "A")		// user is requesting to stop water flow
    {
      stopWaterFlow ();
      FireNotification ("Stopping water flow");
      inputs = "";
      delay (2 * 60 * 1000);
    }
  else if (inputs == "B")	// user is requesting to start water flow
    {
      startWaterFlow ();
      FireNotification ("Extra water flowing for 5 seconds");
      inputs = "";
    }
  else if (inputs == "C")
    {
      digitalWrite (relay2, HIGH);
      FireNotification ("Artificial sunlight started");
    }
  else if (inputs == "c")
    {
      digitalWrite (relay2, LOW);
      FireNotification ("Artificial sunlight stopped");
    }
}
