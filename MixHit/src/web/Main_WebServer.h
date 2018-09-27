// ###############################################################################
// ##																			##
// ##		#ifndef REGION_...													##
// ##		...																	##
// ##		#endif																##
// ##																			##
// ##		Dies dient dazu, um Bereiche zu benamen und hat keine Bedeutung		##
// ##		für den Programmcode. Dadurch wird der Code übersichtlicher			##
// ##		und lesbarer gesteltet. In Visual-Studio können diese Bereiche		##
// ##		auch auf- und zugeklappt werden.									##
// ##																			##
// ###############################################################################

#ifndef _MAIN_WEBSERVER_H_
#define _MAIN_WEBSERVER_H_

#include "../libs/MyString.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include "../mixer/list/cCocktailList.h"
#include "../mixer/cCocktailMixer.h"
#include "../driver/FileOperations.h"


#define AP_SSID "esp32_MixHit_XXXX"

extern String USED_SSID;
extern String MY_IP;
extern String MY_MAC;

struct OrderInfo
{
	int mOrderNumber;
	String mCocltailName;
};

struct UserIpOrder
{
	String mUserIP;
	bool mVIP_Status = false;
	bool mAdmin_Status = false;
	vector<OrderInfo> Bestellnummer_Cocktailname;
};

void setup_WebServer();
void loop_WebServer();

void loadWiFiConfig(String pFileName, String *pMode, String *pSSID, String *pPassword);
void WriteDefaultWiFiConfig();				// Schreibt die Daten in die /Default_Config.wtxt
void WriteBootstrap();						// Schreibt die Datei /bootstrap.min.css
String getURL(String pString);
int getUserAgentIndex(String pUserIP);

void send404(WiFiClient c);

void Print_StartPage(WiFiClient pWifiClient);
void Print_NonAlcoholicCocktailsPage(WiFiClient pWifiClient);
void Print_AlcoholicCocktailsPage(WiFiClient pWifiClient);
void Print_Order(WiFiClient pWiFiClient, String pUserIP, String pURL, int pListSelect, int pSize, int pPrio);
void Print_InformationUser(WiFiClient pWiFiClient, int pUserIndex);
void Print_InformationMixHit(WiFiClient pWiFiClient, int pPrio);
void Print_VIP_Page_AuthOK(WiFiClient pWifiClient);
void Print_VIP_Page_Auth(WiFiClient pWifiClient);
void Print_ADMIN_Page_AuthOK(WiFiClient pWifiClient);
void Print_ADMIN_Page_Auth(WiFiClient pWifiClient);
void Print_MixHitConfig(WiFiClient pWiFiClient, bool pAdmin);
void Print_WiFiConfig(WiFiClient pWiFiClient, bool pAdmin);
void Print_ConfigChange(WiFiClient pWiFiClient, String pLoadFileName, bool pAdmin);
void Print_WiFiConfigChange(WiFiClient pWiFiClient, String pLoadFileName, bool pAdmin);
#endif