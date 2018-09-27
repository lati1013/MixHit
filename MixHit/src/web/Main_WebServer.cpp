#include "Main_WebServer.h"

String USED_SSID;
String MY_IP;
String MY_MAC;
WiFiServer server(80,200);
vector<UserIpOrder> User_Order_List;
String HTML_StartPage = "";
String HTML_VIPAuth = "";
String HTML_ADMINAuth = "";
String HTML_AlcoholicList = "";
String HTML_NonAlcoholicList = "";
String HTML_OrderOK = "";

String HTML_InformationMixHit = "";
String HTML_InformationUser = "";

String HTML_MixHitConfigChange = "";
String HTML_WiFiConfigChange = "";
String HTML_MixHitConfig = "";
String HTML_WiFiConfig = "";
String fontSizeLabels = "h2";
String fontSizeText = "font size=\"5\"";

void setup_WebServer() 
{
	#ifndef REAGON_CheckbootstrapFile
	File lFile = SPIFFS.open("/bootstrap.min.css");
	Serial.println("Bootstrap.size() = " + String(lFile.size()));
	if (!lFile) // Pruefen ob das File vorhanden ist
	{
		lFile.close();
		WriteBootstrap();
	}
	else
	{
		lFile.close();
	}
	#endif
	#ifndef LoadFile
	String lMode = "";
	String lSSID = "";
	String lPassword = "";
	String lConfigFileName = "";
	readFile(SPIFFS, "/Config_Select.wtxt", &lConfigFileName); // Liest ein, welche Config-Datei geladen werden soll.
	Serial.println("Content: " + lConfigFileName);
	if (lConfigFileName == "")
	{ // Falls die Datei nicht vorhanden oder leer ist.
		lConfigFileName = "/Default_Config.wtxt"; // Default_Config soll geladen werden.
	}
	writeFile(SPIFFS, "/Config_Select.wtxt", "/Default_Config.wtxt");	// Zur�cksetzen auf Default (dass falls die aktuelle Config beschaedigt ist, nach einem Reset wieder die Default geladen wird - Damit der Controler nicht staendeig die fehlerhafte Datei laed und resetet).
	loadWiFiConfig(lConfigFileName, &lMode, &lSSID, &lPassword);
	if (lSSID == "")
	{
		lSSID = "MixHit_WiFi";
	}
	writeFile(SPIFFS, "/Config_Select.wtxt", lConfigFileName); // Falls beim Laden alles in Ordnung war (und das Programm nicht abstuerzt) wird die Config_Select wieder auf den Urspruenglichen Wert gesetzt.
	#endif
	if (lMode == "Connect_Mode")
	{
		#ifndef REGION_CONNECT_MODE
		if (lPassword == "")
		{
			WiFi.begin(lSSID.c_str());
		}
		else
		{
			WiFi.begin(lSSID.c_str(), lPassword.c_str());
		}
		unsigned long lStartTime = millis();
		unsigned long lMaxTimeToConnect = 10000; // 10sec
		while (WiFi.status() != WL_CONNECTED && millis() - lStartTime <= lMaxTimeToConnect)
		{
			delay(500);
			Serial.println("Connecting to WiFi..");
		}
		if (WiFi.status() == WL_CONNECTED)
		{
			Serial.println("Connected to the WiFi network");
			Serial.println("local IP: " + WiFi.localIP().toString());
			
			USED_SSID = lSSID;
			MY_IP = WiFi.localIP().toString();

			server.begin();
		}
		else
		{
			Serial.println("Connection Failed - Start SoftAP now");
		}

		
		#endif
	}
	
	if (lMode != "Connect_Mode" || WiFi.status() != WL_CONNECTED)
	{
		if (lMode == "Connect_Mode")
		{
			Serial.println("Changed SSID from \"" + lSSID + "\" to \"esp32_" + lSSID + "\"");
			lSSID = "esp32_" + lSSID;
			
		}
		#ifndef REGION_AP_MODE
		// WLAN Access Point aktivieren
		WiFi.softAP(lSSID.c_str());
		// Webserver auf Port 80 starten

		USED_SSID = lSSID;
		MY_IP = WiFi.softAPIP().toString();

		server.begin();
		// WLAN-Stromsparmodus aktivieren
		esp_wifi_set_ps((wifi_ps_type_t)1);

		
		#endif
		wifi_config_t lwifi_config;
		esp_wifi_get_config(WIFI_IF_AP, &lwifi_config);
		Serial.println("############### MaxCon:" + lwifi_config.ap.max_connection);
		lwifi_config.ap.max_connection = 10;
		esp_wifi_set_config(WIFI_IF_AP, &lwifi_config);
	}
	

	MY_MAC = WiFi.macAddress();
	Serial.println("SSID: " + USED_SSID);
	Serial.println("IP-Adress: " + MY_IP);
	Serial.println("MAC-Adressc" + MY_MAC);
}

void loop_WebServer()
{
	WiFiClient client = server.available(); // Warten auf Anfragen von Clients
	if (client)
	{
		String lURL;
		String lUserIP;
		int lUserIPIndex;
		String c;
		if (client.connected())
		{
			int client_Index = client.available();
			if (client_Index)
			{
				#ifndef REGION_ReadClientInfo	
				c = client.readString(); // Gesamte Information welche vom client empfangen wird
				lURL = getURL(c);
				lUserIP = client.remoteIP().toString();
				lUserIPIndex = getUserAgentIndex(lUserIP);

				Serial.println("Anzahl Clients: " + String(User_Order_List.size()));
				Serial.println("[Request from] " + lUserIP);
				Serial.println("[Request to] " + lURL);
				Serial.println("");
				#endif
				#ifndef REGION_SelectPage
				#ifndef REGION_Startpage
				if (lURL == "/")
				{
					Print_StartPage(client);
				}
				#endif
				#ifndef REGION_VIP_User_Freischalten
				else if (lURL == "/EnableVipStatus")
				{
					if (c.indexOf("Authorization: Basic VmlwVXNlcjpNaXhIaXRWaXBQVw") > 0) // VipUser;MixHitVipPW - �berpr�fung der Credentials, base64 encode
					{
						Serial.println("###################### Passwort richtig");
						User_Order_List[lUserIPIndex].mVIP_Status = true;
						Print_VIP_Page_AuthOK(client);
						
					}
					else
					{
						Serial.println("###################### Passwortabfrage");
						Print_VIP_Page_Auth(client);
					}

				}
				#endif
				#ifndef REGION_ADMIN_User_Freischalten
				else if (lURL == "/EnableAdminStatus")
				{
					if (c.indexOf("Authorization: Basic QWRtaW5Vc2VyOk1peEhpdEFkbWluTW9wZQ") > 0) // AdminUser;MixHitAdminMope - �berpr�fung der Credentials, base64 encode
					{ //           Authorization: Basic QWRtaW5Vc2VyOk1peEhpdEFkbWluTW9wZQ==
						Serial.println("###################### Passwort richtig");
						User_Order_List[lUserIPIndex].mAdmin_Status = true;
						Print_ADMIN_Page_AuthOK(client);
					}
					else
					{
						Serial.println("###################### Passwortabfrage");
						Print_ADMIN_Page_Auth(client);
					}
				}
				#endif
				#ifndef REGION_Cocktailkarte
				#ifndef REGION_Cocktailkarte_Alkoholfrei
				else if (lURL == "/Cocktailkarte_Alkoholfrei")
				{
					Print_NonAlcoholicCocktailsPage(client);
				}
				#endif
				#ifndef REGION_Cocktailkarte_Alkoholisch
				else if (lURL == "/Cocktailkarte_Alkoholisch")
				{
					Print_AlcoholicCocktailsPage(client);
				}
				#endif
				#ifndef REGION_Bestellung_Alkoholisch_Klein
				else if (lURL.indexOf("/Bestellung_Alkoholisch_Klein/") == 0)
				{
					int lPrio = 0;
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						lPrio = 2;
					}
					else if(User_Order_List[lUserIPIndex].mVIP_Status)
					{
						lPrio = 1;
					}
					Print_Order(client, lUserIP, lURL, 1 /*Alkoholisch*/, ORDER_SMALL, lPrio);
				}
				#endif
				#ifndef REGION_Bestellung_Alkoholisch_Gross
				else if (lURL.indexOf("/Bestellung_Alkoholisch_Gross/") == 0)
				{
					int lPrio = 0;
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						lPrio = 2;
					}
					else if (User_Order_List[lUserIPIndex].mVIP_Status)
					{
						lPrio = 1;
					}
					Print_Order(client, lUserIP, lURL, 1 /*Alkoholisch*/, ORDER_BIG, lPrio);
				}
				#endif
				#ifndef REGION_Bestellung_Alkoholfrei_Klein
				else if (lURL.indexOf("/Bestellung_Alkoholfrei_Klein/") == 0)
				{
					int lPrio = 0;
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						lPrio = 2;
					}
					else if (User_Order_List[lUserIPIndex].mVIP_Status)
					{
						lPrio = 1;
					}
					Print_Order(client, lUserIP, lURL, 0 /*Alkoholfrei*/, ORDER_SMALL, lPrio);
				}
				#endif
				#ifndef REGION_Bestellung_Alkoholfrei_Gross
				else if (lURL.indexOf("/Bestellung_Alkoholfrei_Gross/") == 0)
				{
					int lPrio = 0;
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						lPrio = 2;
					}
					else if (User_Order_List[lUserIPIndex].mVIP_Status)
					{
						lPrio = 1;
					}
					Print_Order(client, lUserIP, lURL, 0 /*Alkoholfrei*/, ORDER_BIG, lPrio);
				}
				#endif
				#endif
				#ifndef REGION_Informationen_MixHit
				else if (lURL == "/Informationen_MixHit")
				{
					int lPrio = 0;
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						lPrio = 2;
					}
					else if (User_Order_List[lUserIPIndex].mVIP_Status)
					{
						lPrio = 1;
					}
					Print_InformationMixHit(client, lPrio);
				}
				#endif
				#ifndef REGION_Informationen_User
				else if (lURL == "/Informationen_User")
				{
					Print_InformationUser(client, lUserIPIndex);
				}
				#endif
				#ifndef REGION_Einstellungen
				#ifndef REGION_MixHit_Konfiguration
				else if (lURL == "/Einstellungen_MixHit")
				{
					Print_MixHitConfig(client, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_MixHit/bearbeiten/") == 0)
				{ 
					String lFileName = lURL;
					lFileName.replace("/Einstellungen_MixHit/bearbeiten/", "");
					Print_ConfigChange(client, lFileName, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_MixHit/entfernen/") == 0)
				{
					String lFileName = String(lURL);
					lFileName.replace("/Einstellungen_MixHit/entfernen/", "");
					if (lFileName != "Default_Config")
						deleteFile(SPIFFS, "/" + lFileName + ".txt");
					Print_MixHitConfig(client, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_MixHit/anwenden/") == 0)
				{
					String lFileName = String(lURL);
					lFileName.replace("/Einstellungen_MixHit/anwenden/", "");
					writeFile(SPIFFS, "/Config_Select.txt", "/" + lFileName + ".txt");
					ESP.restart();
				}
				else if (lURL.indexOf("/Einstellungen_MixHit/speichern/") == 0)
				{
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						String lFileName = getStringBetween(lURL, "&name=", "&endname");
						String lFileContent = getStringBetween(lURL, "&content=", "&endcontent=end");
						if (lFileName != "" && lFileContent != "")
						{
							lFileName = String("/" + lFileName + ".txt");
							if (lFileName == "/Default_Config.txt")
							{
								lFileName = "/User_Config.txt";
							}
							lFileContent.replace("%3Cbr/%3E", "\n");
							lFileContent.replace("%20", " ");

							vector<String> lFileLines;
							split(&lFileContent, '\n', &lFileLines);
							writeFile(SPIFFS, lFileName, lFileLines[0] + "\n\r");
							for (int i = 1; i < lFileLines.size(); i++)
							{
								appendFile(SPIFFS, lFileName, lFileLines[i] + "\n\r");
							}
						}
					}
				}
				#endif
				#ifndef REGION_WiFi_Konfiguration
				else if (lURL == "/Einstellungen_WiFi")
				{
					Print_WiFiConfig(client, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_WiFi/bearbeiten/") == 0)
				{
					String lFileName = lURL;
					lFileName.replace("/Einstellungen_WiFi/bearbeiten/", "");
					Print_WiFiConfigChange(client, lFileName, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_WiFi/entfernen/") == 0)
				{
					String lFileName = String(lURL);
					lFileName.replace("/Einstellungen_WiFi/entfernen/", "");
					if (lFileName != "Default_Config")
						deleteFile(SPIFFS, "/" + lFileName + ".wtxt");
					Print_WiFiConfig(client, User_Order_List[lUserIPIndex].mAdmin_Status);
				}
				else if (lURL.indexOf("/Einstellungen_WiFi/anwenden/") == 0)
				{
					String lFileName = String(lURL);
					lFileName.replace("/Einstellungen_WiFi/anwenden/", "");
					writeFile(SPIFFS, "/Config_Select.wtxt", "/" + lFileName + ".wtxt");
					ESP.restart();
				}
				else if (lURL.indexOf("/Einstellungen_WiFi/speichern/") == 0)
				{
					if (User_Order_List[lUserIPIndex].mAdmin_Status)
					{
						String lFileName = getStringBetween(lURL, "&name=", "&endname");
						String lFileContent = getStringBetween(lURL, "&content=", "&endcontent=end");
						if (lFileName != "" && lFileContent != "")
						{
							lFileName = String("/" + lFileName + ".wtxt");
							if (lFileName == "/Default_Config.wtxt")
							{
								lFileName = "/User_Config.wtxt";
							}
							lFileContent.replace("%3Cbr/%3E", "\n");
							lFileContent.replace("%20", " ");

							vector<String> lFileLines;
							split(&lFileContent, '\n', &lFileLines);
							writeFile(SPIFFS, lFileName, lFileLines[0] + "\n\r");
							for (int i = 1; i < lFileLines.size(); i++)
							{
								appendFile(SPIFFS, lFileName, lFileLines[i] + "\n\r");
							}
						}
					}
				}
				#endif
				#endif
				else if(lURL == "/bootstrap.min.css")
				{
					// this method STREAMS a file to the client
					// without the need to load the whole file in a buffer

					Serial.println("################   bootstrap   #############");
					
					// file was uploaded with arduino spiffs tool
					// open file from SPIFFS
					File file = SPIFFS.open("/bootstrap.min.css");
					if(!file) {
						Serial.println("Can not open bootstrap file");
						return;
					}
					Serial.println("Load bootstap");
					// HTTP header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-type: text/css");
					client.println("Expires: Sun, 17-Jan-2038 19:14:07 GMT");	// for caching in browser, file is loaded only once
					Serial.println("Bootstrap.size() = " + String(file.size()));
					client.println("Content-Length: 120296");
					client.println();

					// reserve buffer varialbe on stack with size 100
					char buf[100] = "";

					// read as long as there is content in file
					while(file.available()) {

						// read 99 bytes
						file.readBytes(buf, 99);

						// set 100th (array is 0-indexed!) to \0
						// \0 marks end of a string
						buf[99] = '\0';

						//send buf to client
						client.print(buf);
						//memset(buf, 0, sizeof(buf));
					}
					
					client.println();
					client.stop();
					Serial.println("################    bootstrap ende   #############");
					file.close();
				}
				#ifndef REGION_404
				else
				{
					send404(client);
				}
				#endif
				#endif
			}
		}

	}
	vTaskDelay(10); // Unterbricht den Task f�r 10ms, um Strom zu sparen
}

void loadWiFiConfig(String pFileName, String *pMode, String *pSSID, String *pPassword)
{
	String lFileContent = "";
	readFile(SPIFFS, pFileName, &lFileContent);
	if (lFileContent == "")
	{
		WriteDefaultWiFiConfig();
		readFile(SPIFFS, "/Default_WiFi_Config.wtxt", &lFileContent);
	}
	lFileContent.replace("\r", "");
	vector<String> Lines;
	split(&lFileContent, '\n', &Lines);
	vector<String>Line0;
	vector<String>Line1;
	vector<String>Line2;
	split(&Lines[0], ';', &Line0);
	split(&Lines[1], ';', &Line1);
	split(&Lines[2], ';', &Line2);
	*pMode = Line0[1];
	*pSSID = Line1[1];
	*pPassword = Line2[1];
}
void WriteDefaultWiFiConfig()
{
	String pFileName = "/Default_Config.wtxt";
	writeFile(SPIFFS, pFileName, "Modus;AP_Mode;AP_Mode (Eigenes WiFi aufspannen) oder Connect_Mode (mit vorhandenem WiFi verbinden)\n\r");
	appendFile(SPIFFS, pFileName, "SSID;MixHit_WiFi;Name des Netzwerkes\n\r");
	appendFile(SPIFFS, pFileName, "Password;;Leer = kein Passwort\n\r");
}
void WriteBootstrap()
{
	writeFile(SPIFFS, "/bootstrap.min.css", "/*!\n * Bootstrap v3.3.7 (http://getbootstrap.com)\n * Copyright 2011-2016 Twitter, Inc.\n * Licensed under MIT (https://github.com/twbs/bootstrap/blob/master/LICENSE)\n *//*! normalize.css v3.0.3 | MIT License | github.com/necolas/normalize.css */html{font-family:sans-serif;-webkit-text-size-adjust:100");
	appendFile(SPIFFS, "/bootstrap.min.css", "%;-ms-text-size-adjust:100%}body{margin:0}article,aside,details,figcaption,figure,footer,header,hgroup,main,menu,nav,section,summary{display:block}audio,canvas,progress,video{display:inline-block;vertical-align:baseline}audio:not([controls]){display:none;height:0}[hidden],template{display:none}a{bac");
	appendFile(SPIFFS, "/bootstrap.min.css", "kground-color:transparent}a:active,a:hover{outline:0}abbr[title]{border-bottom:1px dotted}b,strong{font-weight:700}dfn{font-style:italic}h1{margin:.67em 0;font-size:2em}mark{color:#000;background:#ff0}small{font-size:80%}sub,sup{position:relative;font-size:75%;line-height:0;vertical-align:baseline}s");
	appendFile(SPIFFS, "/bootstrap.min.css", "up{top:-.5em}sub{bottom:-.25em}img{border:0}svg:not(:root){overflow:hidden}figure{margin:1em 40px}hr{height:0;-webkit-box-sizing:content-box;-moz-box-sizing:content-box;box-sizing:content-box}pre{overflow:auto}code,kbd,pre,samp{font-family:monospace,monospace;font-size:1em}button,input,optgroup,sele");
	appendFile(SPIFFS, "/bootstrap.min.css", "ct,textarea{margin:0;font:inherit;color:inherit}button{overflow:visible}button,select{text-transform:none}button,html input[type=button],input[type=reset],input[type=submit]{-webkit-appearance:button;cursor:pointer}button[disabled],html input[disabled]{cursor:default}button::-moz-focus-inner,input::");
	appendFile(SPIFFS, "/bootstrap.min.css", "-moz-focus-inner{padding:0;border:0}input{line-height:normal}input[type=checkbox],input[type=radio]{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box;padding:0}input[type=number]::-webkit-inner-spin-button,input[type=number]::-webkit-outer-spin-button{height:auto}input[t");
	appendFile(SPIFFS, "/bootstrap.min.css", "ype=search]{-webkit-box-sizing:content-box;-moz-box-sizing:content-box;box-sizing:content-box;-webkit-appearance:textfield}input[type=search]::-webkit-search-cancel-button,input[type=search]::-webkit-search-decoration{-webkit-appearance:none}fieldset{padding:.35em .625em .75em;margin:0 2px;border:1p");
	appendFile(SPIFFS, "/bootstrap.min.css", "x solid silver}legend{padding:0;border:0}textarea{overflow:auto}optgroup{font-weight:700}table{border-spacing:0;border-collapse:collapse}td,th{padding:0}/*! Source: https://github.com/h5bp/html5-boilerplate/blob/master/src/css/main.css */@media print{*,:after,:before{color:#000!important;text-shadow");
	appendFile(SPIFFS, "/bootstrap.min.css", ":none!important;background:0 0!important;-webkit-box-shadow:none!important;box-shadow:none!important}a,a:visited{text-decoration:underline}a[href]:after{content:\" (\" attr(href) \")\"}abbr[title]:after{content:\" (\" attr(title) \")\"}a[href^=\"javascript:\"]:after,a[href^=\"#\"]:after{content:\"\"}blockquote,pr");
	appendFile(SPIFFS, "/bootstrap.min.css", "e{border:1px solid #999;page-break-inside:avoid}thead{display:table-header-group}img,tr{page-break-inside:avoid}img{max-width:100%!important}h2,h3,p{orphans:3;widows:3}h2,h3{page-break-after:avoid}.navbar{display:none}.btn>.caret,.dropup>.btn>.caret{border-top-color:#000!important}.label{border:1px ");
	appendFile(SPIFFS, "/bootstrap.min.css", "solid #000}.table{border-collapse:collapse!important}.table td,.table th{background-color:#fff!important}.table-bordered td,.table-bordered th{border:1px solid #ddd!important}}@font-face{font-family:'Glyphicons Halflings';src:url(../fonts/glyphicons-halflings-regular.eot);src:url(../fonts/glyphicons");
	appendFile(SPIFFS, "/bootstrap.min.css", "-halflings-regular.eot?#iefix) format('embedded-opentype'),url(../fonts/glyphicons-halflings-regular.woff2) format('woff2'),url(../fonts/glyphicons-halflings-regular.woff) format('woff'),url(../fonts/glyphicons-halflings-regular.ttf) format('truetype'),url(../fonts/glyphicons-halflings-regular.svg#g");
	appendFile(SPIFFS, "/bootstrap.min.css", "lyphicons_halflingsregular) format('svg')}.glyphicon{position:relative;top:1px;display:inline-block;font-family:'Glyphicons Halflings';font-style:normal;font-weight:400;line-height:1;-webkit-font-smoothing:antialiased;-moz-osx-font-smoothing:grayscale}.glyphicon-asterisk:before{content:\"\\002a\"}.glyp");
	appendFile(SPIFFS, "/bootstrap.min.css", "hicon-plus:before{content:\"\\002b\"}.glyphicon-eur:before,.glyphicon-euro:before{content:\"\\20ac\"}.glyphicon-minus:before{content:\"\\2212\"}.glyphicon-cloud:before{content:\"\\2601\"}.glyphicon-envelope:before{content:\"\\2709\"}.glyphicon-pencil:before{content:\"\\270f\"}.glyphicon-glass:before{content:\"\\e001\"}.");
	appendFile(SPIFFS, "/bootstrap.min.css", "glyphicon-music:before{content:\"\\e002\"}.glyphicon-search:before{content:\"\\e003\"}.glyphicon-heart:before{content:\"\\e005\"}.glyphicon-star:before{content:\"\\e006\"}.glyphicon-star-empty:before{content:\"\\e007\"}.glyphicon-user:before{content:\"\\e008\"}.glyphicon-film:before{content:\"\\e009\"}.glyphicon-th-larg");
	appendFile(SPIFFS, "/bootstrap.min.css", "e:before{content:\"\\e010\"}.glyphicon-th:before{content:\"\\e011\"}.glyphicon-th-list:before{content:\"\\e012\"}.glyphicon-ok:before{content:\"\\e013\"}.glyphicon-remove:before{content:\"\\e014\"}.glyphicon-zoom-in:before{content:\"\\e015\"}.glyphicon-zoom-out:before{content:\"\\e016\"}.glyphicon-off:before{content:\"\\e");
	appendFile(SPIFFS, "/bootstrap.min.css", "017\"}.glyphicon-signal:before{content:\"\\e018\"}.glyphicon-cog:before{content:\"\\e019\"}.glyphicon-trash:before{content:\"\\e020\"}.glyphicon-home:before{content:\"\\e021\"}.glyphicon-file:before{content:\"\\e022\"}.glyphicon-time:before{content:\"\\e023\"}.glyphicon-road:before{content:\"\\e024\"}.glyphicon-download-");
	appendFile(SPIFFS, "/bootstrap.min.css", "alt:before{content:\"\\e025\"}.glyphicon-download:before{content:\"\\e026\"}.glyphicon-upload:before{content:\"\\e027\"}.glyphicon-inbox:before{content:\"\\e028\"}.glyphicon-play-circle:before{content:\"\\e029\"}.glyphicon-repeat:before{content:\"\\e030\"}.glyphicon-refresh:before{content:\"\\e031\"}.glyphicon-list-alt:");
	appendFile(SPIFFS, "/bootstrap.min.css", "before{content:\"\\e032\"}.glyphicon-lock:before{content:\"\\e033\"}.glyphicon-flag:before{content:\"\\e034\"}.glyphicon-headphones:before{content:\"\\e035\"}.glyphicon-volume-off:before{content:\"\\e036\"}.glyphicon-volume-down:before{content:\"\\e037\"}.glyphicon-volume-up:before{content:\"\\e038\"}.glyphicon-qrcode:b");
	appendFile(SPIFFS, "/bootstrap.min.css", "efore{content:\"\\e039\"}.glyphicon-barcode:before{content:\"\\e040\"}.glyphicon-tag:before{content:\"\\e041\"}.glyphicon-tags:before{content:\"\\e042\"}.glyphicon-book:before{content:\"\\e043\"}.glyphicon-bookmark:before{content:\"\\e044\"}.glyphicon-print:before{content:\"\\e045\"}.glyphicon-camera:before{content:\"\\e0");
	appendFile(SPIFFS, "/bootstrap.min.css", "46\"}.glyphicon-font:before{content:\"\\e047\"}.glyphicon-bold:before{content:\"\\e048\"}.glyphicon-italic:before{content:\"\\e049\"}.glyphicon-text-height:before{content:\"\\e050\"}.glyphicon-text-width:before{content:\"\\e051\"}.glyphicon-align-left:before{content:\"\\e052\"}.glyphicon-align-center:before{content:\"\\");
	appendFile(SPIFFS, "/bootstrap.min.css", "e053\"}.glyphicon-align-right:before{content:\"\\e054\"}.glyphicon-align-justify:before{content:\"\\e055\"}.glyphicon-list:before{content:\"\\e056\"}.glyphicon-indent-left:before{content:\"\\e057\"}.glyphicon-indent-right:before{content:\"\\e058\"}.glyphicon-facetime-video:before{content:\"\\e059\"}.glyphicon-picture:");
	appendFile(SPIFFS, "/bootstrap.min.css", "before{content:\"\\e060\"}.glyphicon-map-marker:before{content:\"\\e062\"}.glyphicon-adjust:before{content:\"\\e063\"}.glyphicon-tint:before{content:\"\\e064\"}.glyphicon-edit:before{content:\"\\e065\"}.glyphicon-share:before{content:\"\\e066\"}.glyphicon-check:before{content:\"\\e067\"}.glyphicon-move:before{content:\"\\");
	appendFile(SPIFFS, "/bootstrap.min.css", "e068\"}.glyphicon-step-backward:before{content:\"\\e069\"}.glyphicon-fast-backward:before{content:\"\\e070\"}.glyphicon-backward:before{content:\"\\e071\"}.glyphicon-play:before{content:\"\\e072\"}.glyphicon-pause:before{content:\"\\e073\"}.glyphicon-stop:before{content:\"\\e074\"}.glyphicon-forward:before{content:\"\\e");
	appendFile(SPIFFS, "/bootstrap.min.css", "075\"}.glyphicon-fast-forward:before{content:\"\\e076\"}.glyphicon-step-forward:before{content:\"\\e077\"}.glyphicon-eject:before{content:\"\\e078\"}.glyphicon-chevron-left:before{content:\"\\e079\"}.glyphicon-chevron-right:before{content:\"\\e080\"}.glyphicon-plus-sign:before{content:\"\\e081\"}.glyphicon-minus-sign:");
	appendFile(SPIFFS, "/bootstrap.min.css", "before{content:\"\\e082\"}.glyphicon-remove-sign:before{content:\"\\e083\"}.glyphicon-ok-sign:before{content:\"\\e084\"}.glyphicon-question-sign:before{content:\"\\e085\"}.glyphicon-info-sign:before{content:\"\\e086\"}.glyphicon-screenshot:before{content:\"\\e087\"}.glyphicon-remove-circle:before{content:\"\\e088\"}.gly");
	appendFile(SPIFFS, "/bootstrap.min.css", "phicon-ok-circle:before{content:\"\\e089\"}.glyphicon-ban-circle:before{content:\"\\e090\"}.glyphicon-arrow-left:before{content:\"\\e091\"}.glyphicon-arrow-right:before{content:\"\\e092\"}.glyphicon-arrow-up:before{content:\"\\e093\"}.glyphicon-arrow-down:before{content:\"\\e094\"}.glyphicon-share-alt:before{content:");
	appendFile(SPIFFS, "/bootstrap.min.css", "\"\\e095\"}.glyphicon-resize-full:before{content:\"\\e096\"}.glyphicon-resize-small:before{content:\"\\e097\"}.glyphicon-exclamation-sign:before{content:\"\\e101\"}.glyphicon-gift:before{content:\"\\e102\"}.glyphicon-leaf:before{content:\"\\e103\"}.glyphicon-fire:before{content:\"\\e104\"}.glyphicon-eye-open:before{cont");
	appendFile(SPIFFS, "/bootstrap.min.css", "ent:\"\\e105\"}.glyphicon-eye-close:before{content:\"\\e106\"}.glyphicon-warning-sign:before{content:\"\\e107\"}.glyphicon-plane:before{content:\"\\e108\"}.glyphicon-calendar:before{content:\"\\e109\"}.glyphicon-random:before{content:\"\\e110\"}.glyphicon-comment:before{content:\"\\e111\"}.glyphicon-magnet:before{conten");
	appendFile(SPIFFS, "/bootstrap.min.css", "t:\"\\e112\"}.glyphicon-chevron-up:before{content:\"\\e113\"}.glyphicon-chevron-down:before{content:\"\\e114\"}.glyphicon-retweet:before{content:\"\\e115\"}.glyphicon-shopping-cart:before{content:\"\\e116\"}.glyphicon-folder-close:before{content:\"\\e117\"}.glyphicon-folder-open:before{content:\"\\e118\"}.glyphicon-resi");
	appendFile(SPIFFS, "/bootstrap.min.css", "ze-vertical:before{content:\"\\e119\"}.glyphicon-resize-horizontal:before{content:\"\\e120\"}.glyphicon-hdd:before{content:\"\\e121\"}.glyphicon-bullhorn:before{content:\"\\e122\"}.glyphicon-bell:before{content:\"\\e123\"}.glyphicon-certificate:before{content:\"\\e124\"}.glyphicon-thumbs-up:before{content:\"\\e125\"}.gl");
	appendFile(SPIFFS, "/bootstrap.min.css", "yphicon-thumbs-down:before{content:\"\\e126\"}.glyphicon-hand-right:before{content:\"\\e127\"}.glyphicon-hand-left:before{content:\"\\e128\"}.glyphicon-hand-up:before{content:\"\\e129\"}.glyphicon-hand-down:before{content:\"\\e130\"}.glyphicon-circle-arrow-right:before{content:\"\\e131\"}.glyphicon-circle-arrow-left:");
	appendFile(SPIFFS, "/bootstrap.min.css", "before{content:\"\\e132\"}.glyphicon-circle-arrow-up:before{content:\"\\e133\"}.glyphicon-circle-arrow-down:before{content:\"\\e134\"}.glyphicon-globe:before{content:\"\\e135\"}.glyphicon-wrench:before{content:\"\\e136\"}.glyphicon-tasks:before{content:\"\\e137\"}.glyphicon-filter:before{content:\"\\e138\"}.glyphicon-br");
	appendFile(SPIFFS, "/bootstrap.min.css", "iefcase:before{content:\"\\e139\"}.glyphicon-fullscreen:before{content:\"\\e140\"}.glyphicon-dashboard:before{content:\"\\e141\"}.glyphicon-paperclip:before{content:\"\\e142\"}.glyphicon-heart-empty:before{content:\"\\e143\"}.glyphicon-link:before{content:\"\\e144\"}.glyphicon-phone:before{content:\"\\e145\"}.glyphicon-");
	appendFile(SPIFFS, "/bootstrap.min.css", "pushpin:before{content:\"\\e146\"}.glyphicon-usd:before{content:\"\\e148\"}.glyphicon-gbp:before{content:\"\\e149\"}.glyphicon-sort:before{content:\"\\e150\"}.glyphicon-sort-by-alphabet:before{content:\"\\e151\"}.glyphicon-sort-by-alphabet-alt:before{content:\"\\e152\"}.glyphicon-sort-by-order:before{content:\"\\e153\"}");
	appendFile(SPIFFS, "/bootstrap.min.css", ".glyphicon-sort-by-order-alt:before{content:\"\\e154\"}.glyphicon-sort-by-attributes:before{content:\"\\e155\"}.glyphicon-sort-by-attributes-alt:before{content:\"\\e156\"}.glyphicon-unchecked:before{content:\"\\e157\"}.glyphicon-expand:before{content:\"\\e158\"}.glyphicon-collapse-down:before{content:\"\\e159\"}.glyp");
	appendFile(SPIFFS, "/bootstrap.min.css", "hicon-collapse-up:before{content:\"\\e160\"}.glyphicon-log-in:before{content:\"\\e161\"}.glyphicon-flash:before{content:\"\\e162\"}.glyphicon-log-out:before{content:\"\\e163\"}.glyphicon-new-window:before{content:\"\\e164\"}.glyphicon-record:before{content:\"\\e165\"}.glyphicon-save:before{content:\"\\e166\"}.glyphicon-");
	appendFile(SPIFFS, "/bootstrap.min.css", "open:before{content:\"\\e167\"}.glyphicon-saved:before{content:\"\\e168\"}.glyphicon-import:before{content:\"\\e169\"}.glyphicon-export:before{content:\"\\e170\"}.glyphicon-send:before{content:\"\\e171\"}.glyphicon-floppy-disk:before{content:\"\\e172\"}.glyphicon-floppy-saved:before{content:\"\\e173\"}.glyphicon-floppy-");
	appendFile(SPIFFS, "/bootstrap.min.css", "remove:before{content:\"\\e174\"}.glyphicon-floppy-save:before{content:\"\\e175\"}.glyphicon-floppy-open:before{content:\"\\e176\"}.glyphicon-credit-card:before{content:\"\\e177\"}.glyphicon-transfer:before{content:\"\\e178\"}.glyphicon-cutlery:before{content:\"\\e179\"}.glyphicon-header:before{content:\"\\e180\"}.glyph");
	appendFile(SPIFFS, "/bootstrap.min.css", "icon-compressed:before{content:\"\\e181\"}.glyphicon-earphone:before{content:\"\\e182\"}.glyphicon-phone-alt:before{content:\"\\e183\"}.glyphicon-tower:before{content:\"\\e184\"}.glyphicon-stats:before{content:\"\\e185\"}.glyphicon-sd-video:before{content:\"\\e186\"}.glyphicon-hd-video:before{content:\"\\e187\"}.glyphic");
	appendFile(SPIFFS, "/bootstrap.min.css", "on-subtitles:before{content:\"\\e188\"}.glyphicon-sound-stereo:before{content:\"\\e189\"}.glyphicon-sound-dolby:before{content:\"\\e190\"}.glyphicon-sound-5-1:before{content:\"\\e191\"}.glyphicon-sound-6-1:before{content:\"\\e192\"}.glyphicon-sound-7-1:before{content:\"\\e193\"}.glyphicon-copyright-mark:before{conten");
	appendFile(SPIFFS, "/bootstrap.min.css", "t:\"\\e194\"}.glyphicon-registration-mark:before{content:\"\\e195\"}.glyphicon-cloud-download:before{content:\"\\e197\"}.glyphicon-cloud-upload:before{content:\"\\e198\"}.glyphicon-tree-conifer:before{content:\"\\e199\"}.glyphicon-tree-deciduous:before{content:\"\\e200\"}.glyphicon-cd:before{content:\"\\e201\"}.glyphico");
	appendFile(SPIFFS, "/bootstrap.min.css", "n-save-file:before{content:\"\\e202\"}.glyphicon-open-file:before{content:\"\\e203\"}.glyphicon-level-up:before{content:\"\\e204\"}.glyphicon-copy:before{content:\"\\e205\"}.glyphicon-paste:before{content:\"\\e206\"}.glyphicon-alert:before{content:\"\\e209\"}.glyphicon-equalizer:before{content:\"\\e210\"}.glyphicon-king");
	appendFile(SPIFFS, "/bootstrap.min.css", ":before{content:\"\\e211\"}.glyphicon-queen:before{content:\"\\e212\"}.glyphicon-pawn:before{content:\"\\e213\"}.glyphicon-bishop:before{content:\"\\e214\"}.glyphicon-knight:before{content:\"\\e215\"}.glyphicon-baby-formula:before{content:\"\\e216\"}.glyphicon-tent:before{content:\"\\26fa\"}.glyphicon-blackboard:before{");
	appendFile(SPIFFS, "/bootstrap.min.css", "content:\"\\e218\"}.glyphicon-bed:before{content:\"\\e219\"}.glyphicon-apple:before{content:\"\\f8ff\"}.glyphicon-erase:before{content:\"\\e221\"}.glyphicon-hourglass:before{content:\"\\231b\"}.glyphicon-lamp:before{content:\"\\e223\"}.glyphicon-duplicate:before{content:\"\\e224\"}.glyphicon-piggy-bank:before{content:\"\\");
	appendFile(SPIFFS, "/bootstrap.min.css", "e225\"}.glyphicon-scissors:before{content:\"\\e226\"}.glyphicon-bitcoin:before{content:\"\\e227\"}.glyphicon-btc:before{content:\"\\e227\"}.glyphicon-xbt:before{content:\"\\e227\"}.glyphicon-yen:before{content:\"\\00a5\"}.glyphicon-jpy:before{content:\"\\00a5\"}.glyphicon-ruble:before{content:\"\\20bd\"}.glyphicon-rub:be");
	appendFile(SPIFFS, "/bootstrap.min.css", "fore{content:\"\\20bd\"}.glyphicon-scale:before{content:\"\\e230\"}.glyphicon-ice-lolly:before{content:\"\\e231\"}.glyphicon-ice-lolly-tasted:before{content:\"\\e232\"}.glyphicon-education:before{content:\"\\e233\"}.glyphicon-option-horizontal:before{content:\"\\e234\"}.glyphicon-option-vertical:before{content:\"\\e235");
	appendFile(SPIFFS, "/bootstrap.min.css", "\"}.glyphicon-menu-hamburger:before{content:\"\\e236\"}.glyphicon-modal-window:before{content:\"\\e237\"}.glyphicon-oil:before{content:\"\\e238\"}.glyphicon-grain:before{content:\"\\e239\"}.glyphicon-sunglasses:before{content:\"\\e240\"}.glyphicon-text-size:before{content:\"\\e241\"}.glyphicon-text-color:before{conten");
	appendFile(SPIFFS, "/bootstrap.min.css", "t:\"\\e242\"}.glyphicon-text-background:before{content:\"\\e243\"}.glyphicon-object-align-top:before{content:\"\\e244\"}.glyphicon-object-align-bottom:before{content:\"\\e245\"}.glyphicon-object-align-horizontal:before{content:\"\\e246\"}.glyphicon-object-align-left:before{content:\"\\e247\"}.glyphicon-object-align-v");
	appendFile(SPIFFS, "/bootstrap.min.css", "ertical:before{content:\"\\e248\"}.glyphicon-object-align-right:before{content:\"\\e249\"}.glyphicon-triangle-right:before{content:\"\\e250\"}.glyphicon-triangle-left:before{content:\"\\e251\"}.glyphicon-triangle-bottom:before{content:\"\\e252\"}.glyphicon-triangle-top:before{content:\"\\e253\"}.glyphicon-console:bef");
	appendFile(SPIFFS, "/bootstrap.min.css", "ore{content:\"\\e254\"}.glyphicon-superscript:before{content:\"\\e255\"}.glyphicon-subscript:before{content:\"\\e256\"}.glyphicon-menu-left:before{content:\"\\e257\"}.glyphicon-menu-right:before{content:\"\\e258\"}.glyphicon-menu-down:before{content:\"\\e259\"}.glyphicon-menu-up:before{content:\"\\e260\"}*{-webkit-box-s");
	appendFile(SPIFFS, "/bootstrap.min.css", "izing:border-box;-moz-box-sizing:border-box;box-sizing:border-box}:after,:before{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box}html{font-size:10px;-webkit-tap-highlight-color:rgba(0,0,0,0)}body{font-family:\"Helvetica Neue\",Helvetica,Arial,sans-serif;font-size:14px;li");
	appendFile(SPIFFS, "/bootstrap.min.css", "ne-height:1.42857143;color:#333;background-color:#fff}button,input,select,textarea{font-family:inherit;font-size:inherit;line-height:inherit}a{color:#337ab7;text-decoration:none}a:focus,a:hover{color:#23527c;text-decoration:underline}a:focus{outline:5px auto -webkit-focus-ring-color;outline-offset:-");
	appendFile(SPIFFS, "/bootstrap.min.css", "2px}figure{margin:0}img{vertical-align:middle}.carousel-inner>.item>a>img,.carousel-inner>.item>img,.img-responsive,.thumbnail a>img,.thumbnail>img{display:block;max-width:100%;height:auto}.img-rounded{border-radius:6px}.img-thumbnail{display:inline-block;max-width:100%;height:auto;padding:4px;line-");
	appendFile(SPIFFS, "/bootstrap.min.css", "height:1.42857143;background-color:#fff;border:1px solid #ddd;border-radius:4px;-webkit-transition:all .2s ease-in-out;-o-transition:all .2s ease-in-out;transition:all .2s ease-in-out}.img-circle{border-radius:50%}hr{margin-top:20px;margin-bottom:20px;border:0;border-top:1px solid #eee}.sr-only{posi");
	appendFile(SPIFFS, "/bootstrap.min.css", "tion:absolute;width:1px;height:1px;padding:0;margin:-1px;overflow:hidden;clip:rect(0,0,0,0);border:0}.sr-only-focusable:active,.sr-only-focusable:focus{position:static;width:auto;height:auto;margin:0;overflow:visible;clip:auto}[role=button]{cursor:pointer}.h1,.h2,.h3,.h4,.h5,.h6,h1,h2,h3,h4,h5,h6{fo");
	appendFile(SPIFFS, "/bootstrap.min.css", "nt-family:inherit;font-weight:500;line-height:1.1;color:inherit}.h1 .small,.h1 small,.h2 .small,.h2 small,.h3 .small,.h3 small,.h4 .small,.h4 small,.h5 .small,.h5 small,.h6 .small,.h6 small,h1 .small,h1 small,h2 .small,h2 small,h3 .small,h3 small,h4 .small,h4 small,h5 .small,h5 small,h6 .small,h6 sm");
	appendFile(SPIFFS, "/bootstrap.min.css", "all{font-weight:400;line-height:1;color:#777}.h1,.h2,.h3,h1,h2,h3{margin-top:20px;margin-bottom:10px}.h1 .small,.h1 small,.h2 .small,.h2 small,.h3 .small,.h3 small,h1 .small,h1 small,h2 .small,h2 small,h3 .small,h3 small{font-size:65%}.h4,.h5,.h6,h4,h5,h6{margin-top:10px;margin-bottom:10px}.h4 .smal");
	appendFile(SPIFFS, "/bootstrap.min.css", "l,.h4 small,.h5 .small,.h5 small,.h6 .small,.h6 small,h4 .small,h4 small,h5 .small,h5 small,h6 .small,h6 small{font-size:75%}.h1,h1{font-size:36px}.h2,h2{font-size:30px}.h3,h3{font-size:24px}.h4,h4{font-size:18px}.h5,h5{font-size:14px}.h6,h6{font-size:12px}p{margin:0 0 10px}.lead{margin-bottom:20px;");
	appendFile(SPIFFS, "/bootstrap.min.css", "font-size:16px;font-weight:300;line-height:1.4}@media (min-width:768px){.lead{font-size:21px}}.small,small{font-size:85%}.mark,mark{padding:.2em;background-color:#fcf8e3}.text-left{text-align:left}.text-right{text-align:right}.text-center{text-align:center}.text-justify{text-align:justify}.text-nowr");
	appendFile(SPIFFS, "/bootstrap.min.css", "ap{white-space:nowrap}.text-lowercase{text-transform:lowercase}.text-uppercase{text-transform:uppercase}.text-capitalize{text-transform:capitalize}.text-muted{color:#777}.text-primary{color:#337ab7}a.text-primary:focus,a.text-primary:hover{color:#286090}.text-success{color:#3c763d}a.text-success:foc");
	appendFile(SPIFFS, "/bootstrap.min.css", "us,a.text-success:hover{color:#2b542c}.text-info{color:#31708f}a.text-info:focus,a.text-info:hover{color:#245269}.text-warning{color:#8a6d3b}a.text-warning:focus,a.text-warning:hover{color:#66512c}.text-danger{color:#a94442}a.text-danger:focus,a.text-danger:hover{color:#843534}.bg-primary{color:#fff");
	appendFile(SPIFFS, "/bootstrap.min.css", ";background-color:#337ab7}a.bg-primary:focus,a.bg-primary:hover{background-color:#286090}.bg-success{background-color:#dff0d8}a.bg-success:focus,a.bg-success:hover{background-color:#c1e2b3}.bg-info{background-color:#d9edf7}a.bg-info:focus,a.bg-info:hover{background-color:#afd9ee}.bg-warning{backgrou");
	appendFile(SPIFFS, "/bootstrap.min.css", "nd-color:#fcf8e3}a.bg-warning:focus,a.bg-warning:hover{background-color:#f7ecb5}.bg-danger{background-color:#f2dede}a.bg-danger:focus,a.bg-danger:hover{background-color:#e4b9b9}.page-header{padding-bottom:9px;margin:40px 0 20px;border-bottom:1px solid #eee}ol,ul{margin-top:0;margin-bottom:10px}ol ol");
	appendFile(SPIFFS, "/bootstrap.min.css", ",ol ul,ul ol,ul ul{margin-bottom:0}.list-unstyled{padding-left:0;list-style:none}.list-inline{padding-left:0;margin-left:-5px;list-style:none}.list-inline>li{display:inline-block;padding-right:5px;padding-left:5px}dl{margin-top:0;margin-bottom:20px}dd,dt{line-height:1.42857143}dt{font-weight:700}dd{");
	appendFile(SPIFFS, "/bootstrap.min.css", "margin-left:0}@media (min-width:768px){.dl-horizontal dt{float:left;width:160px;overflow:hidden;clear:left;text-align:right;text-overflow:ellipsis;white-space:nowrap}.dl-horizontal dd{margin-left:180px}}abbr[data-original-title],abbr[title]{cursor:help;border-bottom:1px dotted #777}.initialism{font-");
	appendFile(SPIFFS, "/bootstrap.min.css", "size:90%;text-transform:uppercase}blockquote{padding:10px 20px;margin:0 0 20px;font-size:17.5px;border-left:5px solid #eee}blockquote ol:last-child,blockquote p:last-child,blockquote ul:last-child{margin-bottom:0}blockquote .small,blockquote footer,blockquote small{display:block;font-size:80%;line-h");
	appendFile(SPIFFS, "/bootstrap.min.css", "eight:1.42857143;color:#777}blockquote .small:before,blockquote footer:before,blockquote small:before{content:'\\2014 \\00A0'}.blockquote-reverse,blockquote.pull-right{padding-right:15px;padding-left:0;text-align:right;border-right:5px solid #eee;border-left:0}.blockquote-reverse .small:before,.blockq");
	appendFile(SPIFFS, "/bootstrap.min.css", "uote-reverse footer:before,.blockquote-reverse small:before,blockquote.pull-right .small:before,blockquote.pull-right footer:before,blockquote.pull-right small:before{content:''}.blockquote-reverse .small:after,.blockquote-reverse footer:after,.blockquote-reverse small:after,blockquote.pull-right .s");
	appendFile(SPIFFS, "/bootstrap.min.css", "mall:after,blockquote.pull-right footer:after,blockquote.pull-right small:after{content:'\\00A0 \\2014'}address{margin-bottom:20px;font-style:normal;line-height:1.42857143}code,kbd,pre,samp{font-family:Menlo,Monaco,Consolas,\"Courier New\",monospace}code{padding:2px 4px;font-size:90%;color:#c7254e;backg");
	appendFile(SPIFFS, "/bootstrap.min.css", "round-color:#f9f2f4;border-radius:4px}kbd{padding:2px 4px;font-size:90%;color:#fff;background-color:#333;border-radius:3px;-webkit-box-shadow:inset 0 -1px 0 rgba(0,0,0,.25);box-shadow:inset 0 -1px 0 rgba(0,0,0,.25)}kbd kbd{padding:0;font-size:100%;font-weight:700;-webkit-box-shadow:none;box-shadow:n");
	appendFile(SPIFFS, "/bootstrap.min.css", "one}pre{display:block;padding:9.5px;margin:0 0 10px;font-size:13px;line-height:1.42857143;color:#333;word-break:break-all;word-wrap:break-word;background-color:#f5f5f5;border:1px solid #ccc;border-radius:4px}pre code{padding:0;font-size:inherit;color:inherit;white-space:pre-wrap;background-color:tra");
	appendFile(SPIFFS, "/bootstrap.min.css", "nsparent;border-radius:0}.pre-scrollable{max-height:340px;overflow-y:scroll}.container{padding-right:15px;padding-left:15px;margin-right:auto;margin-left:auto}@media (min-width:768px){.container{width:750px}}@media (min-width:992px){.container{width:970px}}@media (min-width:1200px){.container{width:");
	appendFile(SPIFFS, "/bootstrap.min.css", "1170px}}.container-fluid{padding-right:15px;padding-left:15px;margin-right:auto;margin-left:auto}.row{margin-right:-15px;margin-left:-15px}.col-lg-1,.col-lg-10,.col-lg-11,.col-lg-12,.col-lg-2,.col-lg-3,.col-lg-4,.col-lg-5,.col-lg-6,.col-lg-7,.col-lg-8,.col-lg-9,.col-md-1,.col-md-10,.col-md-11,.col-m");
	appendFile(SPIFFS, "/bootstrap.min.css", "d-12,.col-md-2,.col-md-3,.col-md-4,.col-md-5,.col-md-6,.col-md-7,.col-md-8,.col-md-9,.col-sm-1,.col-sm-10,.col-sm-11,.col-sm-12,.col-sm-2,.col-sm-3,.col-sm-4,.col-sm-5,.col-sm-6,.col-sm-7,.col-sm-8,.col-sm-9,.col-xs-1,.col-xs-10,.col-xs-11,.col-xs-12,.col-xs-2,.col-xs-3,.col-xs-4,.col-xs-5,.col-xs-6");
	appendFile(SPIFFS, "/bootstrap.min.css", ",.col-xs-7,.col-xs-8,.col-xs-9{position:relative;min-height:1px;padding-right:15px;padding-left:15px}.col-xs-1,.col-xs-10,.col-xs-11,.col-xs-12,.col-xs-2,.col-xs-3,.col-xs-4,.col-xs-5,.col-xs-6,.col-xs-7,.col-xs-8,.col-xs-9{float:left}.col-xs-12{width:100%}.col-xs-11{width:91.66666667%}.col-xs-10{wi");
	appendFile(SPIFFS, "/bootstrap.min.css", "dth:83.33333333%}.col-xs-9{width:75%}.col-xs-8{width:66.66666667%}.col-xs-7{width:58.33333333%}.col-xs-6{width:50%}.col-xs-5{width:41.66666667%}.col-xs-4{width:33.33333333%}.col-xs-3{width:25%}.col-xs-2{width:16.66666667%}.col-xs-1{width:8.33333333%}.col-xs-pull-12{right:100%}.col-xs-pull-11{right:9");
	appendFile(SPIFFS, "/bootstrap.min.css", "1.66666667%}.col-xs-pull-10{right:83.33333333%}.col-xs-pull-9{right:75%}.col-xs-pull-8{right:66.66666667%}.col-xs-pull-7{right:58.33333333%}.col-xs-pull-6{right:50%}.col-xs-pull-5{right:41.66666667%}.col-xs-pull-4{right:33.33333333%}.col-xs-pull-3{right:25%}.col-xs-pull-2{right:16.66666667%}.col-xs-");
	appendFile(SPIFFS, "/bootstrap.min.css", "pull-1{right:8.33333333%}.col-xs-pull-0{right:auto}.col-xs-push-12{left:100%}.col-xs-push-11{left:91.66666667%}.col-xs-push-10{left:83.33333333%}.col-xs-push-9{left:75%}.col-xs-push-8{left:66.66666667%}.col-xs-push-7{left:58.33333333%}.col-xs-push-6{left:50%}.col-xs-push-5{left:41.66666667%}.col-xs-");
	appendFile(SPIFFS, "/bootstrap.min.css", "push-4{left:33.33333333%}.col-xs-push-3{left:25%}.col-xs-push-2{left:16.66666667%}.col-xs-push-1{left:8.33333333%}.col-xs-push-0{left:auto}.col-xs-offset-12{margin-left:100%}.col-xs-offset-11{margin-left:91.66666667%}.col-xs-offset-10{margin-left:83.33333333%}.col-xs-offset-9{margin-left:75%}.col-xs");
	appendFile(SPIFFS, "/bootstrap.min.css", "-offset-8{margin-left:66.66666667%}.col-xs-offset-7{margin-left:58.33333333%}.col-xs-offset-6{margin-left:50%}.col-xs-offset-5{margin-left:41.66666667%}.col-xs-offset-4{margin-left:33.33333333%}.col-xs-offset-3{margin-left:25%}.col-xs-offset-2{margin-left:16.66666667%}.col-xs-offset-1{margin-left:8.");
	appendFile(SPIFFS, "/bootstrap.min.css", "33333333%}.col-xs-offset-0{margin-left:0}@media (min-width:768px){.col-sm-1,.col-sm-10,.col-sm-11,.col-sm-12,.col-sm-2,.col-sm-3,.col-sm-4,.col-sm-5,.col-sm-6,.col-sm-7,.col-sm-8,.col-sm-9{float:left}.col-sm-12{width:100%}.col-sm-11{width:91.66666667%}.col-sm-10{width:83.33333333%}.col-sm-9{width:75");
	appendFile(SPIFFS, "/bootstrap.min.css", "%}.col-sm-8{width:66.66666667%}.col-sm-7{width:58.33333333%}.col-sm-6{width:50%}.col-sm-5{width:41.66666667%}.col-sm-4{width:33.33333333%}.col-sm-3{width:25%}.col-sm-2{width:16.66666667%}.col-sm-1{width:8.33333333%}.col-sm-pull-12{right:100%}.col-sm-pull-11{right:91.66666667%}.col-sm-pull-10{right:8");
	appendFile(SPIFFS, "/bootstrap.min.css", "3.33333333%}.col-sm-pull-9{right:75%}.col-sm-pull-8{right:66.66666667%}.col-sm-pull-7{right:58.33333333%}.col-sm-pull-6{right:50%}.col-sm-pull-5{right:41.66666667%}.col-sm-pull-4{right:33.33333333%}.col-sm-pull-3{right:25%}.col-sm-pull-2{right:16.66666667%}.col-sm-pull-1{right:8.33333333%}.col-sm-pu");
	appendFile(SPIFFS, "/bootstrap.min.css", "ll-0{right:auto}.col-sm-push-12{left:100%}.col-sm-push-11{left:91.66666667%}.col-sm-push-10{left:83.33333333%}.col-sm-push-9{left:75%}.col-sm-push-8{left:66.66666667%}.col-sm-push-7{left:58.33333333%}.col-sm-push-6{left:50%}.col-sm-push-5{left:41.66666667%}.col-sm-push-4{left:33.33333333%}.col-sm-pu");
	appendFile(SPIFFS, "/bootstrap.min.css", "sh-3{left:25%}.col-sm-push-2{left:16.66666667%}.col-sm-push-1{left:8.33333333%}.col-sm-push-0{left:auto}.col-sm-offset-12{margin-left:100%}.col-sm-offset-11{margin-left:91.66666667%}.col-sm-offset-10{margin-left:83.33333333%}.col-sm-offset-9{margin-left:75%}.col-sm-offset-8{margin-left:66.66666667%}");
	appendFile(SPIFFS, "/bootstrap.min.css", ".col-sm-offset-7{margin-left:58.33333333%}.col-sm-offset-6{margin-left:50%}.col-sm-offset-5{margin-left:41.66666667%}.col-sm-offset-4{margin-left:33.33333333%}.col-sm-offset-3{margin-left:25%}.col-sm-offset-2{margin-left:16.66666667%}.col-sm-offset-1{margin-left:8.33333333%}.col-sm-offset-0{margin-l");
	appendFile(SPIFFS, "/bootstrap.min.css", "eft:0}}@media (min-width:992px){.col-md-1,.col-md-10,.col-md-11,.col-md-12,.col-md-2,.col-md-3,.col-md-4,.col-md-5,.col-md-6,.col-md-7,.col-md-8,.col-md-9{float:left}.col-md-12{width:100%}.col-md-11{width:91.66666667%}.col-md-10{width:83.33333333%}.col-md-9{width:75%}.col-md-8{width:66.66666667%}.co");
	appendFile(SPIFFS, "/bootstrap.min.css", "l-md-7{width:58.33333333%}.col-md-6{width:50%}.col-md-5{width:41.66666667%}.col-md-4{width:33.33333333%}.col-md-3{width:25%}.col-md-2{width:16.66666667%}.col-md-1{width:8.33333333%}.col-md-pull-12{right:100%}.col-md-pull-11{right:91.66666667%}.col-md-pull-10{right:83.33333333%}.col-md-pull-9{right:7");
	appendFile(SPIFFS, "/bootstrap.min.css", "5%}.col-md-pull-8{right:66.66666667%}.col-md-pull-7{right:58.33333333%}.col-md-pull-6{right:50%}.col-md-pull-5{right:41.66666667%}.col-md-pull-4{right:33.33333333%}.col-md-pull-3{right:25%}.col-md-pull-2{right:16.66666667%}.col-md-pull-1{right:8.33333333%}.col-md-pull-0{right:auto}.col-md-push-12{le");
	appendFile(SPIFFS, "/bootstrap.min.css", "ft:100%}.col-md-push-11{left:91.66666667%}.col-md-push-10{left:83.33333333%}.col-md-push-9{left:75%}.col-md-push-8{left:66.66666667%}.col-md-push-7{left:58.33333333%}.col-md-push-6{left:50%}.col-md-push-5{left:41.66666667%}.col-md-push-4{left:33.33333333%}.col-md-push-3{left:25%}.col-md-push-2{left:");
	appendFile(SPIFFS, "/bootstrap.min.css", "16.66666667%}.col-md-push-1{left:8.33333333%}.col-md-push-0{left:auto}.col-md-offset-12{margin-left:100%}.col-md-offset-11{margin-left:91.66666667%}.col-md-offset-10{margin-left:83.33333333%}.col-md-offset-9{margin-left:75%}.col-md-offset-8{margin-left:66.66666667%}.col-md-offset-7{margin-left:58.33");
	appendFile(SPIFFS, "/bootstrap.min.css", "333333%}.col-md-offset-6{margin-left:50%}.col-md-offset-5{margin-left:41.66666667%}.col-md-offset-4{margin-left:33.33333333%}.col-md-offset-3{margin-left:25%}.col-md-offset-2{margin-left:16.66666667%}.col-md-offset-1{margin-left:8.33333333%}.col-md-offset-0{margin-left:0}}@media (min-width:1200px){.");
	appendFile(SPIFFS, "/bootstrap.min.css", "col-lg-1,.col-lg-10,.col-lg-11,.col-lg-12,.col-lg-2,.col-lg-3,.col-lg-4,.col-lg-5,.col-lg-6,.col-lg-7,.col-lg-8,.col-lg-9{float:left}.col-lg-12{width:100%}.col-lg-11{width:91.66666667%}.col-lg-10{width:83.33333333%}.col-lg-9{width:75%}.col-lg-8{width:66.66666667%}.col-lg-7{width:58.33333333%}.col-lg");
	appendFile(SPIFFS, "/bootstrap.min.css", "-6{width:50%}.col-lg-5{width:41.66666667%}.col-lg-4{width:33.33333333%}.col-lg-3{width:25%}.col-lg-2{width:16.66666667%}.col-lg-1{width:8.33333333%}.col-lg-pull-12{right:100%}.col-lg-pull-11{right:91.66666667%}.col-lg-pull-10{right:83.33333333%}.col-lg-pull-9{right:75%}.col-lg-pull-8{right:66.666666");
	appendFile(SPIFFS, "/bootstrap.min.css", "67%}.col-lg-pull-7{right:58.33333333%}.col-lg-pull-6{right:50%}.col-lg-pull-5{right:41.66666667%}.col-lg-pull-4{right:33.33333333%}.col-lg-pull-3{right:25%}.col-lg-pull-2{right:16.66666667%}.col-lg-pull-1{right:8.33333333%}.col-lg-pull-0{right:auto}.col-lg-push-12{left:100%}.col-lg-push-11{left:91.6");
	appendFile(SPIFFS, "/bootstrap.min.css", "6666667%}.col-lg-push-10{left:83.33333333%}.col-lg-push-9{left:75%}.col-lg-push-8{left:66.66666667%}.col-lg-push-7{left:58.33333333%}.col-lg-push-6{left:50%}.col-lg-push-5{left:41.66666667%}.col-lg-push-4{left:33.33333333%}.col-lg-push-3{left:25%}.col-lg-push-2{left:16.66666667%}.col-lg-push-1{left:");
	appendFile(SPIFFS, "/bootstrap.min.css", "8.33333333%}.col-lg-push-0{left:auto}.col-lg-offset-12{margin-left:100%}.col-lg-offset-11{margin-left:91.66666667%}.col-lg-offset-10{margin-left:83.33333333%}.col-lg-offset-9{margin-left:75%}.col-lg-offset-8{margin-left:66.66666667%}.col-lg-offset-7{margin-left:58.33333333%}.col-lg-offset-6{margin-l");
	appendFile(SPIFFS, "/bootstrap.min.css", "eft:50%}.col-lg-offset-5{margin-left:41.66666667%}.col-lg-offset-4{margin-left:33.33333333%}.col-lg-offset-3{margin-left:25%}.col-lg-offset-2{margin-left:16.66666667%}.col-lg-offset-1{margin-left:8.33333333%}.col-lg-offset-0{margin-left:0}}table{background-color:transparent}caption{padding-top:8px;p");
	appendFile(SPIFFS, "/bootstrap.min.css", "adding-bottom:8px;color:#777;text-align:left}th{text-align:left}.table{width:100%;max-width:100%;margin-bottom:20px}.table>tbody>tr>td,.table>tbody>tr>th,.table>tfoot>tr>td,.table>tfoot>tr>th,.table>thead>tr>td,.table>thead>tr>th{padding:8px;line-height:1.42857143;vertical-align:top;border-top:1px s");
	appendFile(SPIFFS, "/bootstrap.min.css", "olid #ddd}.table>thead>tr>th{vertical-align:bottom;border-bottom:2px solid #ddd}.table>caption+thead>tr:first-child>td,.table>caption+thead>tr:first-child>th,.table>colgroup+thead>tr:first-child>td,.table>colgroup+thead>tr:first-child>th,.table>thead:first-child>tr:first-child>td,.table>thead:first-");
	appendFile(SPIFFS, "/bootstrap.min.css", "child>tr:first-child>th{border-top:0}.table>tbody+tbody{border-top:2px solid #ddd}.table .table{background-color:#fff}.table-condensed>tbody>tr>td,.table-condensed>tbody>tr>th,.table-condensed>tfoot>tr>td,.table-condensed>tfoot>tr>th,.table-condensed>thead>tr>td,.table-condensed>thead>tr>th{padding:");
	appendFile(SPIFFS, "/bootstrap.min.css", "5px}.table-bordered{border:1px solid #ddd}.table-bordered>tbody>tr>td,.table-bordered>tbody>tr>th,.table-bordered>tfoot>tr>td,.table-bordered>tfoot>tr>th,.table-bordered>thead>tr>td,.table-bordered>thead>tr>th{border:1px solid #ddd}.table-bordered>thead>tr>td,.table-bordered>thead>tr>th{border-botto");
	appendFile(SPIFFS, "/bootstrap.min.css", "m-width:2px}.table-striped>tbody>tr:nth-of-type(odd){background-color:#f9f9f9}.table-hover>tbody>tr:hover{background-color:#f5f5f5}table col[class*=col-]{position:static;display:table-column;float:none}table td[class*=col-],table th[class*=col-]{position:static;display:table-cell;float:none}.table>t");
	appendFile(SPIFFS, "/bootstrap.min.css", "body>tr.active>td,.table>tbody>tr.active>th,.table>tbody>tr>td.active,.table>tbody>tr>th.active,.table>tfoot>tr.active>td,.table>tfoot>tr.active>th,.table>tfoot>tr>td.active,.table>tfoot>tr>th.active,.table>thead>tr.active>td,.table>thead>tr.active>th,.table>thead>tr>td.active,.table>thead>tr>th.act");
	appendFile(SPIFFS, "/bootstrap.min.css", "ive{background-color:#f5f5f5}.table-hover>tbody>tr.active:hover>td,.table-hover>tbody>tr.active:hover>th,.table-hover>tbody>tr:hover>.active,.table-hover>tbody>tr>td.active:hover,.table-hover>tbody>tr>th.active:hover{background-color:#e8e8e8}.table>tbody>tr.success>td,.table>tbody>tr.success>th,.tab");
	appendFile(SPIFFS, "/bootstrap.min.css", "le>tbody>tr>td.success,.table>tbody>tr>th.success,.table>tfoot>tr.success>td,.table>tfoot>tr.success>th,.table>tfoot>tr>td.success,.table>tfoot>tr>th.success,.table>thead>tr.success>td,.table>thead>tr.success>th,.table>thead>tr>td.success,.table>thead>tr>th.success{background-color:#dff0d8}.table-ho");
	appendFile(SPIFFS, "/bootstrap.min.css", "ver>tbody>tr.success:hover>td,.table-hover>tbody>tr.success:hover>th,.table-hover>tbody>tr:hover>.success,.table-hover>tbody>tr>td.success:hover,.table-hover>tbody>tr>th.success:hover{background-color:#d0e9c6}.table>tbody>tr.info>td,.table>tbody>tr.info>th,.table>tbody>tr>td.info,.table>tbody>tr>th.");
	appendFile(SPIFFS, "/bootstrap.min.css", "info,.table>tfoot>tr.info>td,.table>tfoot>tr.info>th,.table>tfoot>tr>td.info,.table>tfoot>tr>th.info,.table>thead>tr.info>td,.table>thead>tr.info>th,.table>thead>tr>td.info,.table>thead>tr>th.info{background-color:#d9edf7}.table-hover>tbody>tr.info:hover>td,.table-hover>tbody>tr.info:hover>th,.table");
	appendFile(SPIFFS, "/bootstrap.min.css", "-hover>tbody>tr:hover>.info,.table-hover>tbody>tr>td.info:hover,.table-hover>tbody>tr>th.info:hover{background-color:#c4e3f3}.table>tbody>tr.warning>td,.table>tbody>tr.warning>th,.table>tbody>tr>td.warning,.table>tbody>tr>th.warning,.table>tfoot>tr.warning>td,.table>tfoot>tr.warning>th,.table>tfoot>");
	appendFile(SPIFFS, "/bootstrap.min.css", "tr>td.warning,.table>tfoot>tr>th.warning,.table>thead>tr.warning>td,.table>thead>tr.warning>th,.table>thead>tr>td.warning,.table>thead>tr>th.warning{background-color:#fcf8e3}.table-hover>tbody>tr.warning:hover>td,.table-hover>tbody>tr.warning:hover>th,.table-hover>tbody>tr:hover>.warning,.table-hove");
	appendFile(SPIFFS, "/bootstrap.min.css", "r>tbody>tr>td.warning:hover,.table-hover>tbody>tr>th.warning:hover{background-color:#faf2cc}.table>tbody>tr.danger>td,.table>tbody>tr.danger>th,.table>tbody>tr>td.danger,.table>tbody>tr>th.danger,.table>tfoot>tr.danger>td,.table>tfoot>tr.danger>th,.table>tfoot>tr>td.danger,.table>tfoot>tr>th.danger,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".table>thead>tr.danger>td,.table>thead>tr.danger>th,.table>thead>tr>td.danger,.table>thead>tr>th.danger{background-color:#f2dede}.table-hover>tbody>tr.danger:hover>td,.table-hover>tbody>tr.danger:hover>th,.table-hover>tbody>tr:hover>.danger,.table-hover>tbody>tr>td.danger:hover,.table-hover>tbody>tr");
	appendFile(SPIFFS, "/bootstrap.min.css", ">th.danger:hover{background-color:#ebcccc}.table-responsive{min-height:.01%;overflow-x:auto}@media screen and (max-width:767px){.table-responsive{width:100%;margin-bottom:15px;overflow-y:hidden;-ms-overflow-style:-ms-autohiding-scrollbar;border:1px solid #ddd}.table-responsive>.table{margin-bottom:0");
	appendFile(SPIFFS, "/bootstrap.min.css", "}.table-responsive>.table>tbody>tr>td,.table-responsive>.table>tbody>tr>th,.table-responsive>.table>tfoot>tr>td,.table-responsive>.table>tfoot>tr>th,.table-responsive>.table>thead>tr>td,.table-responsive>.table>thead>tr>th{white-space:nowrap}.table-responsive>.table-bordered{border:0}.table-responsi");
	appendFile(SPIFFS, "/bootstrap.min.css", "ve>.table-bordered>tbody>tr>td:first-child,.table-responsive>.table-bordered>tbody>tr>th:first-child,.table-responsive>.table-bordered>tfoot>tr>td:first-child,.table-responsive>.table-bordered>tfoot>tr>th:first-child,.table-responsive>.table-bordered>thead>tr>td:first-child,.table-responsive>.table-");
	appendFile(SPIFFS, "/bootstrap.min.css", "bordered>thead>tr>th:first-child{border-left:0}.table-responsive>.table-bordered>tbody>tr>td:last-child,.table-responsive>.table-bordered>tbody>tr>th:last-child,.table-responsive>.table-bordered>tfoot>tr>td:last-child,.table-responsive>.table-bordered>tfoot>tr>th:last-child,.table-responsive>.table-");
	appendFile(SPIFFS, "/bootstrap.min.css", "bordered>thead>tr>td:last-child,.table-responsive>.table-bordered>thead>tr>th:last-child{border-right:0}.table-responsive>.table-bordered>tbody>tr:last-child>td,.table-responsive>.table-bordered>tbody>tr:last-child>th,.table-responsive>.table-bordered>tfoot>tr:last-child>td,.table-responsive>.table-");
	appendFile(SPIFFS, "/bootstrap.min.css", "bordered>tfoot>tr:last-child>th{border-bottom:0}}fieldset{min-width:0;padding:0;margin:0;border:0}legend{display:block;width:100%;padding:0;margin-bottom:20px;font-size:21px;line-height:inherit;color:#333;border:0;border-bottom:1px solid #e5e5e5}label{display:inline-block;max-width:100%;margin-botto");
	appendFile(SPIFFS, "/bootstrap.min.css", "m:5px;font-weight:700}input[type=search]{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box}input[type=checkbox],input[type=radio]{margin:4px 0 0;margin-top:1px\\9;line-height:normal}input[type=file]{display:block}input[type=range]{display:block;width:100%}select[multiple]");
	appendFile(SPIFFS, "/bootstrap.min.css", ",select[size]{height:auto}input[type=file]:focus,input[type=checkbox]:focus,input[type=radio]:focus{outline:5px auto -webkit-focus-ring-color;outline-offset:-2px}output{display:block;padding-top:7px;font-size:14px;line-height:1.42857143;color:#555}.form-control{display:block;width:100%;height:34px;p");
	appendFile(SPIFFS, "/bootstrap.min.css", "adding:6px 12px;font-size:14px;line-height:1.42857143;color:#555;background-color:#fff;background-image:none;border:1px solid #ccc;border-radius:4px;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(0,0,0,.075);-webkit-transition:border-color ease-in-out .15s,-webki");
	appendFile(SPIFFS, "/bootstrap.min.css", "t-box-shadow ease-in-out .15s;-o-transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s;transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s}.form-control:focus{border-color:#66afe9;outline:0;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.");
	appendFile(SPIFFS, "/bootstrap.min.css", "6);box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.6)}.form-control::-moz-placeholder{color:#999;opacity:1}.form-control:-ms-input-placeholder{color:#999}.form-control::-webkit-input-placeholder{color:#999}.form-control::-ms-expand{background-color:transparent;border:0}.form-co");
	appendFile(SPIFFS, "/bootstrap.min.css", "ntrol[disabled],.form-control[readonly],fieldset[disabled] .form-control{background-color:#eee;opacity:1}.form-control[disabled],fieldset[disabled] .form-control{cursor:not-allowed}textarea.form-control{height:auto}input[type=search]{-webkit-appearance:none}@media screen and (-webkit-min-device-pixe");
	appendFile(SPIFFS, "/bootstrap.min.css", "l-ratio:0){input[type=date].form-control,input[type=time].form-control,input[type=datetime-local].form-control,input[type=month].form-control{line-height:34px}.input-group-sm input[type=date],.input-group-sm input[type=time],.input-group-sm input[type=datetime-local],.input-group-sm input[type=month");
	appendFile(SPIFFS, "/bootstrap.min.css", "],input[type=date].input-sm,input[type=time].input-sm,input[type=datetime-local].input-sm,input[type=month].input-sm{line-height:30px}.input-group-lg input[type=date],.input-group-lg input[type=time],.input-group-lg input[type=datetime-local],.input-group-lg input[type=month],input[type=date].input-");
	appendFile(SPIFFS, "/bootstrap.min.css", "lg,input[type=time].input-lg,input[type=datetime-local].input-lg,input[type=month].input-lg{line-height:46px}}.form-group{margin-bottom:15px}.checkbox,.radio{position:relative;display:block;margin-top:10px;margin-bottom:10px}.checkbox label,.radio label{min-height:20px;padding-left:20px;margin-botto");
	appendFile(SPIFFS, "/bootstrap.min.css", "m:0;font-weight:400;cursor:pointer}.checkbox input[type=checkbox],.checkbox-inline input[type=checkbox],.radio input[type=radio],.radio-inline input[type=radio]{position:absolute;margin-top:4px\\9;margin-left:-20px}.checkbox+.checkbox,.radio+.radio{margin-top:-5px}.checkbox-inline,.radio-inline{posit");
	appendFile(SPIFFS, "/bootstrap.min.css", "ion:relative;display:inline-block;padding-left:20px;margin-bottom:0;font-weight:400;vertical-align:middle;cursor:pointer}.checkbox-inline+.checkbox-inline,.radio-inline+.radio-inline{margin-top:0;margin-left:10px}fieldset[disabled] input[type=checkbox],fieldset[disabled] input[type=radio],input[type");
	appendFile(SPIFFS, "/bootstrap.min.css", "=checkbox].disabled,input[type=checkbox][disabled],input[type=radio].disabled,input[type=radio][disabled]{cursor:not-allowed}.checkbox-inline.disabled,.radio-inline.disabled,fieldset[disabled] .checkbox-inline,fieldset[disabled] .radio-inline{cursor:not-allowed}.checkbox.disabled label,.radio.disabl");
	appendFile(SPIFFS, "/bootstrap.min.css", "ed label,fieldset[disabled] .checkbox label,fieldset[disabled] .radio label{cursor:not-allowed}.form-control-static{min-height:34px;padding-top:7px;padding-bottom:7px;margin-bottom:0}.form-control-static.input-lg,.form-control-static.input-sm{padding-right:0;padding-left:0}.input-sm{height:30px;padd");
	appendFile(SPIFFS, "/bootstrap.min.css", "ing:5px 10px;font-size:12px;line-height:1.5;border-radius:3px}select.input-sm{height:30px;line-height:30px}select[multiple].input-sm,textarea.input-sm{height:auto}.form-group-sm .form-control{height:30px;padding:5px 10px;font-size:12px;line-height:1.5;border-radius:3px}.form-group-sm select.form-con");
	appendFile(SPIFFS, "/bootstrap.min.css", "trol{height:30px;line-height:30px}.form-group-sm select[multiple].form-control,.form-group-sm textarea.form-control{height:auto}.form-group-sm .form-control-static{height:30px;min-height:32px;padding:6px 10px;font-size:12px;line-height:1.5}.input-lg{height:46px;padding:10px 16px;font-size:18px;line-");
	appendFile(SPIFFS, "/bootstrap.min.css", "height:1.3333333;border-radius:6px}select.input-lg{height:46px;line-height:46px}select[multiple].input-lg,textarea.input-lg{height:auto}.form-group-lg .form-control{height:46px;padding:10px 16px;font-size:18px;line-height:1.3333333;border-radius:6px}.form-group-lg select.form-control{height:46px;lin");
	appendFile(SPIFFS, "/bootstrap.min.css", "e-height:46px}.form-group-lg select[multiple].form-control,.form-group-lg textarea.form-control{height:auto}.form-group-lg .form-control-static{height:46px;min-height:38px;padding:11px 16px;font-size:18px;line-height:1.3333333}.has-feedback{position:relative}.has-feedback .form-control{padding-right");
	appendFile(SPIFFS, "/bootstrap.min.css", ":42.5px}.form-control-feedback{position:absolute;top:0;right:0;z-index:2;display:block;width:34px;height:34px;line-height:34px;text-align:center;pointer-events:none}.form-group-lg .form-control+.form-control-feedback,.input-group-lg+.form-control-feedback,.input-lg+.form-control-feedback{width:46px;");
	appendFile(SPIFFS, "/bootstrap.min.css", "height:46px;line-height:46px}.form-group-sm .form-control+.form-control-feedback,.input-group-sm+.form-control-feedback,.input-sm+.form-control-feedback{width:30px;height:30px;line-height:30px}.has-success .checkbox,.has-success .checkbox-inline,.has-success .control-label,.has-success .help-block,.");
	appendFile(SPIFFS, "/bootstrap.min.css", "has-success .radio,.has-success .radio-inline,.has-success.checkbox label,.has-success.checkbox-inline label,.has-success.radio label,.has-success.radio-inline label{color:#3c763d}.has-success .form-control{border-color:#3c763d;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1");
	appendFile(SPIFFS, "/bootstrap.min.css", "px 1px rgba(0,0,0,.075)}.has-success .form-control:focus{border-color:#2b542c;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #67b168;box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #67b168}.has-success .input-group-addon{color:#3c763d;background-color:#dff0d8;border-color:#3c763d}.h");
	appendFile(SPIFFS, "/bootstrap.min.css", "as-success .form-control-feedback{color:#3c763d}.has-warning .checkbox,.has-warning .checkbox-inline,.has-warning .control-label,.has-warning .help-block,.has-warning .radio,.has-warning .radio-inline,.has-warning.checkbox label,.has-warning.checkbox-inline label,.has-warning.radio label,.has-warnin");
	appendFile(SPIFFS, "/bootstrap.min.css", "g.radio-inline label{color:#8a6d3b}.has-warning .form-control{border-color:#8a6d3b;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(0,0,0,.075)}.has-warning .form-control:focus{border-color:#66512c;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #c0a16b");
	appendFile(SPIFFS, "/bootstrap.min.css", ";box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #c0a16b}.has-warning .input-group-addon{color:#8a6d3b;background-color:#fcf8e3;border-color:#8a6d3b}.has-warning .form-control-feedback{color:#8a6d3b}.has-error .checkbox,.has-error .checkbox-inline,.has-error .control-label,.has-error .help-block");
	appendFile(SPIFFS, "/bootstrap.min.css", ",.has-error .radio,.has-error .radio-inline,.has-error.checkbox label,.has-error.checkbox-inline label,.has-error.radio label,.has-error.radio-inline label{color:#a94442}.has-error .form-control{border-color:#a94442;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(");
	appendFile(SPIFFS, "/bootstrap.min.css", "0,0,0,.075)}.has-error .form-control:focus{border-color:#843534;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #ce8483;box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 6px #ce8483}.has-error .input-group-addon{color:#a94442;background-color:#f2dede;border-color:#a94442}.has-error .form-c");
	appendFile(SPIFFS, "/bootstrap.min.css", "ontrol-feedback{color:#a94442}.has-feedback label~.form-control-feedback{top:25px}.has-feedback label.sr-only~.form-control-feedback{top:0}.help-block{display:block;margin-top:5px;margin-bottom:10px;color:#737373}@media (min-width:768px){.form-inline .form-group{display:inline-block;margin-bottom:0;");
	appendFile(SPIFFS, "/bootstrap.min.css", "vertical-align:middle}.form-inline .form-control{display:inline-block;width:auto;vertical-align:middle}.form-inline .form-control-static{display:inline-block}.form-inline .input-group{display:inline-table;vertical-align:middle}.form-inline .input-group .form-control,.form-inline .input-group .input-");
	appendFile(SPIFFS, "/bootstrap.min.css", "group-addon,.form-inline .input-group .input-group-btn{width:auto}.form-inline .input-group>.form-control{width:100%}.form-inline .control-label{margin-bottom:0;vertical-align:middle}.form-inline .checkbox,.form-inline .radio{display:inline-block;margin-top:0;margin-bottom:0;vertical-align:middle}.f");
	appendFile(SPIFFS, "/bootstrap.min.css", "orm-inline .checkbox label,.form-inline .radio label{padding-left:0}.form-inline .checkbox input[type=checkbox],.form-inline .radio input[type=radio]{position:relative;margin-left:0}.form-inline .has-feedback .form-control-feedback{top:0}}.form-horizontal .checkbox,.form-horizontal .checkbox-inline,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".form-horizontal .radio,.form-horizontal .radio-inline{padding-top:7px;margin-top:0;margin-bottom:0}.form-horizontal .checkbox,.form-horizontal .radio{min-height:27px}.form-horizontal .form-group{margin-right:-15px;margin-left:-15px}@media (min-width:768px){.form-horizontal .control-label{padding-to");
	appendFile(SPIFFS, "/bootstrap.min.css", "p:7px;margin-bottom:0;text-align:right}}.form-horizontal .has-feedback .form-control-feedback{right:15px}@media (min-width:768px){.form-horizontal .form-group-lg .control-label{padding-top:11px;font-size:18px}}@media (min-width:768px){.form-horizontal .form-group-sm .control-label{padding-top:6px;fo");
	appendFile(SPIFFS, "/bootstrap.min.css", "nt-size:12px}}.btn{display:inline-block;padding:6px 12px;margin-bottom:0;font-size:14px;font-weight:400;line-height:1.42857143;text-align:center;white-space:nowrap;vertical-align:middle;-ms-touch-action:manipulation;touch-action:manipulation;cursor:pointer;-webkit-user-select:none;-moz-user-select:n");
	appendFile(SPIFFS, "/bootstrap.min.css", "one;-ms-user-select:none;user-select:none;background-image:none;border:1px solid transparent;border-radius:4px}.btn.active.focus,.btn.active:focus,.btn.focus,.btn:active.focus,.btn:active:focus,.btn:focus{outline:5px auto -webkit-focus-ring-color;outline-offset:-2px}.btn.focus,.btn:focus,.btn:hover{");
	appendFile(SPIFFS, "/bootstrap.min.css", "color:#333;text-decoration:none}.btn.active,.btn:active{background-image:none;outline:0;-webkit-box-shadow:inset 0 3px 5px rgba(0,0,0,.125);box-shadow:inset 0 3px 5px rgba(0,0,0,.125)}.btn.disabled,.btn[disabled],fieldset[disabled] .btn{cursor:not-allowed;filter:alpha(opacity=65);-webkit-box-shadow:");
	appendFile(SPIFFS, "/bootstrap.min.css", "none;box-shadow:none;opacity:.65}a.btn.disabled,fieldset[disabled] a.btn{pointer-events:none}.btn-default{color:#333;background-color:#fff;border-color:#ccc}.btn-default.focus,.btn-default:focus{color:#333;background-color:#e6e6e6;border-color:#8c8c8c}.btn-default:hover{color:#333;background-color:#");
	appendFile(SPIFFS, "/bootstrap.min.css", "e6e6e6;border-color:#adadad}.btn-default.active,.btn-default:active,.open>.dropdown-toggle.btn-default{color:#333;background-color:#e6e6e6;border-color:#adadad}.btn-default.active.focus,.btn-default.active:focus,.btn-default.active:hover,.btn-default:active.focus,.btn-default:active:focus,.btn-defau");
	appendFile(SPIFFS, "/bootstrap.min.css", "lt:active:hover,.open>.dropdown-toggle.btn-default.focus,.open>.dropdown-toggle.btn-default:focus,.open>.dropdown-toggle.btn-default:hover{color:#333;background-color:#d4d4d4;border-color:#8c8c8c}.btn-default.active,.btn-default:active,.open>.dropdown-toggle.btn-default{background-image:none}.btn-de");
	appendFile(SPIFFS, "/bootstrap.min.css", "fault.disabled.focus,.btn-default.disabled:focus,.btn-default.disabled:hover,.btn-default[disabled].focus,.btn-default[disabled]:focus,.btn-default[disabled]:hover,fieldset[disabled] .btn-default.focus,fieldset[disabled] .btn-default:focus,fieldset[disabled] .btn-default:hover{background-color:#fff;");
	appendFile(SPIFFS, "/bootstrap.min.css", "border-color:#ccc}.btn-default .badge{color:#fff;background-color:#333}.btn-primary{color:#fff;background-color:#337ab7;border-color:#2e6da4}.btn-primary.focus,.btn-primary:focus{color:#fff;background-color:#286090;border-color:#122b40}.btn-primary:hover{color:#fff;background-color:#286090;border-co");
	appendFile(SPIFFS, "/bootstrap.min.css", "lor:#204d74}.btn-primary.active,.btn-primary:active,.open>.dropdown-toggle.btn-primary{color:#fff;background-color:#286090;border-color:#204d74}.btn-primary.active.focus,.btn-primary.active:focus,.btn-primary.active:hover,.btn-primary:active.focus,.btn-primary:active:focus,.btn-primary:active:hover,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".open>.dropdown-toggle.btn-primary.focus,.open>.dropdown-toggle.btn-primary:focus,.open>.dropdown-toggle.btn-primary:hover{color:#fff;background-color:#204d74;border-color:#122b40}.btn-primary.active,.btn-primary:active,.open>.dropdown-toggle.btn-primary{background-image:none}.btn-primary.disabled.f");
	appendFile(SPIFFS, "/bootstrap.min.css", "ocus,.btn-primary.disabled:focus,.btn-primary.disabled:hover,.btn-primary[disabled].focus,.btn-primary[disabled]:focus,.btn-primary[disabled]:hover,fieldset[disabled] .btn-primary.focus,fieldset[disabled] .btn-primary:focus,fieldset[disabled] .btn-primary:hover{background-color:#337ab7;border-color:");
	appendFile(SPIFFS, "/bootstrap.min.css", "#2e6da4}.btn-primary .badge{color:#337ab7;background-color:#fff}.btn-success{color:#fff;background-color:#5cb85c;border-color:#4cae4c}.btn-success.focus,.btn-success:focus{color:#fff;background-color:#449d44;border-color:#255625}.btn-success:hover{color:#fff;background-color:#449d44;border-color:#39");
	appendFile(SPIFFS, "/bootstrap.min.css", "8439}.btn-success.active,.btn-success:active,.open>.dropdown-toggle.btn-success{color:#fff;background-color:#449d44;border-color:#398439}.btn-success.active.focus,.btn-success.active:focus,.btn-success.active:hover,.btn-success:active.focus,.btn-success:active:focus,.btn-success:active:hover,.open>.");
	appendFile(SPIFFS, "/bootstrap.min.css", "dropdown-toggle.btn-success.focus,.open>.dropdown-toggle.btn-success:focus,.open>.dropdown-toggle.btn-success:hover{color:#fff;background-color:#398439;border-color:#255625}.btn-success.active,.btn-success:active,.open>.dropdown-toggle.btn-success{background-image:none}.btn-success.disabled.focus,.b");
	appendFile(SPIFFS, "/bootstrap.min.css", "tn-success.disabled:focus,.btn-success.disabled:hover,.btn-success[disabled].focus,.btn-success[disabled]:focus,.btn-success[disabled]:hover,fieldset[disabled] .btn-success.focus,fieldset[disabled] .btn-success:focus,fieldset[disabled] .btn-success:hover{background-color:#5cb85c;border-color:#4cae4c");
	appendFile(SPIFFS, "/bootstrap.min.css", "}.btn-success .badge{color:#5cb85c;background-color:#fff}.btn-info{color:#fff;background-color:#5bc0de;border-color:#46b8da}.btn-info.focus,.btn-info:focus{color:#fff;background-color:#31b0d5;border-color:#1b6d85}.btn-info:hover{color:#fff;background-color:#31b0d5;border-color:#269abc}.btn-info.acti");
	appendFile(SPIFFS, "/bootstrap.min.css", "ve,.btn-info:active,.open>.dropdown-toggle.btn-info{color:#fff;background-color:#31b0d5;border-color:#269abc}.btn-info.active.focus,.btn-info.active:focus,.btn-info.active:hover,.btn-info:active.focus,.btn-info:active:focus,.btn-info:active:hover,.open>.dropdown-toggle.btn-info.focus,.open>.dropdown");
	appendFile(SPIFFS, "/bootstrap.min.css", "-toggle.btn-info:focus,.open>.dropdown-toggle.btn-info:hover{color:#fff;background-color:#269abc;border-color:#1b6d85}.btn-info.active,.btn-info:active,.open>.dropdown-toggle.btn-info{background-image:none}.btn-info.disabled.focus,.btn-info.disabled:focus,.btn-info.disabled:hover,.btn-info[disabled]");
	appendFile(SPIFFS, "/bootstrap.min.css", ".focus,.btn-info[disabled]:focus,.btn-info[disabled]:hover,fieldset[disabled] .btn-info.focus,fieldset[disabled] .btn-info:focus,fieldset[disabled] .btn-info:hover{background-color:#5bc0de;border-color:#46b8da}.btn-info .badge{color:#5bc0de;background-color:#fff}.btn-warning{color:#fff;background-co");
	appendFile(SPIFFS, "/bootstrap.min.css", "lor:#f0ad4e;border-color:#eea236}.btn-warning.focus,.btn-warning:focus{color:#fff;background-color:#ec971f;border-color:#985f0d}.btn-warning:hover{color:#fff;background-color:#ec971f;border-color:#d58512}.btn-warning.active,.btn-warning:active,.open>.dropdown-toggle.btn-warning{color:#fff;background");
	appendFile(SPIFFS, "/bootstrap.min.css", "-color:#ec971f;border-color:#d58512}.btn-warning.active.focus,.btn-warning.active:focus,.btn-warning.active:hover,.btn-warning:active.focus,.btn-warning:active:focus,.btn-warning:active:hover,.open>.dropdown-toggle.btn-warning.focus,.open>.dropdown-toggle.btn-warning:focus,.open>.dropdown-toggle.btn");
	appendFile(SPIFFS, "/bootstrap.min.css", "-warning:hover{color:#fff;background-color:#d58512;border-color:#985f0d}.btn-warning.active,.btn-warning:active,.open>.dropdown-toggle.btn-warning{background-image:none}.btn-warning.disabled.focus,.btn-warning.disabled:focus,.btn-warning.disabled:hover,.btn-warning[disabled].focus,.btn-warning[disab");
	appendFile(SPIFFS, "/bootstrap.min.css", "led]:focus,.btn-warning[disabled]:hover,fieldset[disabled] .btn-warning.focus,fieldset[disabled] .btn-warning:focus,fieldset[disabled] .btn-warning:hover{background-color:#f0ad4e;border-color:#eea236}.btn-warning .badge{color:#f0ad4e;background-color:#fff}.btn-danger{color:#fff;background-color:#d95");
	appendFile(SPIFFS, "/bootstrap.min.css", "34f;border-color:#d43f3a}.btn-danger.focus,.btn-danger:focus{color:#fff;background-color:#c9302c;border-color:#761c19}.btn-danger:hover{color:#fff;background-color:#c9302c;border-color:#ac2925}.btn-danger.active,.btn-danger:active,.open>.dropdown-toggle.btn-danger{color:#fff;background-color:#c9302c");
	appendFile(SPIFFS, "/bootstrap.min.css", ";border-color:#ac2925}.btn-danger.active.focus,.btn-danger.active:focus,.btn-danger.active:hover,.btn-danger:active.focus,.btn-danger:active:focus,.btn-danger:active:hover,.open>.dropdown-toggle.btn-danger.focus,.open>.dropdown-toggle.btn-danger:focus,.open>.dropdown-toggle.btn-danger:hover{color:#f");
	appendFile(SPIFFS, "/bootstrap.min.css", "ff;background-color:#ac2925;border-color:#761c19}.btn-danger.active,.btn-danger:active,.open>.dropdown-toggle.btn-danger{background-image:none}.btn-danger.disabled.focus,.btn-danger.disabled:focus,.btn-danger.disabled:hover,.btn-danger[disabled].focus,.btn-danger[disabled]:focus,.btn-danger[disabled");
	appendFile(SPIFFS, "/bootstrap.min.css", "]:hover,fieldset[disabled] .btn-danger.focus,fieldset[disabled] .btn-danger:focus,fieldset[disabled] .btn-danger:hover{background-color:#d9534f;border-color:#d43f3a}.btn-danger .badge{color:#d9534f;background-color:#fff}.btn-link{font-weight:400;color:#337ab7;border-radius:0}.btn-link,.btn-link.acti");
	appendFile(SPIFFS, "/bootstrap.min.css", "ve,.btn-link:active,.btn-link[disabled],fieldset[disabled] .btn-link{background-color:transparent;-webkit-box-shadow:none;box-shadow:none}.btn-link,.btn-link:active,.btn-link:focus,.btn-link:hover{border-color:transparent}.btn-link:focus,.btn-link:hover{color:#23527c;text-decoration:underline;backgr");
	appendFile(SPIFFS, "/bootstrap.min.css", "ound-color:transparent}.btn-link[disabled]:focus,.btn-link[disabled]:hover,fieldset[disabled] .btn-link:focus,fieldset[disabled] .btn-link:hover{color:#777;text-decoration:none}.btn-group-lg>.btn,.btn-lg{padding:10px 16px;font-size:18px;line-height:1.3333333;border-radius:6px}.btn-group-sm>.btn,.btn");
	appendFile(SPIFFS, "/bootstrap.min.css", "-sm{padding:5px 10px;font-size:12px;line-height:1.5;border-radius:3px}.btn-group-xs>.btn,.btn-xs{padding:1px 5px;font-size:12px;line-height:1.5;border-radius:3px}.btn-block{display:block;width:100%}.btn-block+.btn-block{margin-top:5px}input[type=button].btn-block,input[type=reset].btn-block,input[ty");
	appendFile(SPIFFS, "/bootstrap.min.css", "pe=submit].btn-block{width:100%}.fade{opacity:0;-webkit-transition:opacity .15s linear;-o-transition:opacity .15s linear;transition:opacity .15s linear}.fade.in{opacity:1}.collapse{display:none}.collapse.in{display:block}tr.collapse.in{display:table-row}tbody.collapse.in{display:table-row-group}.col");
	appendFile(SPIFFS, "/bootstrap.min.css", "lapsing{position:relative;height:0;overflow:hidden;-webkit-transition-timing-function:ease;-o-transition-timing-function:ease;transition-timing-function:ease;-webkit-transition-duration:.35s;-o-transition-duration:.35s;transition-duration:.35s;-webkit-transition-property:height,visibility;-o-transit");
	appendFile(SPIFFS, "/bootstrap.min.css", "ion-property:height,visibility;transition-property:height,visibility}.caret{display:inline-block;width:0;height:0;margin-left:2px;vertical-align:middle;border-top:4px dashed;border-top:4px solid\\9;border-right:4px solid transparent;border-left:4px solid transparent}.dropdown,.dropup{position:relativ");
	appendFile(SPIFFS, "/bootstrap.min.css", "e}.dropdown-toggle:focus{outline:0}.dropdown-menu{position:absolute;top:100%;left:0;z-index:1000;display:none;float:left;min-width:160px;padding:5px 0;margin:2px 0 0;font-size:14px;text-align:left;list-style:none;background-color:#fff;-webkit-background-clip:padding-box;background-clip:padding-box;b");
	appendFile(SPIFFS, "/bootstrap.min.css", "order:1px solid #ccc;border:1px solid rgba(0,0,0,.15);border-radius:4px;-webkit-box-shadow:0 6px 12px rgba(0,0,0,.175);box-shadow:0 6px 12px rgba(0,0,0,.175)}.dropdown-menu.pull-right{right:0;left:auto}.dropdown-menu .divider{height:1px;margin:9px 0;overflow:hidden;background-color:#e5e5e5}.dropdown");
	appendFile(SPIFFS, "/bootstrap.min.css", "-menu>li>a{display:block;padding:3px 20px;clear:both;font-weight:400;line-height:1.42857143;color:#333;white-space:nowrap}.dropdown-menu>li>a:focus,.dropdown-menu>li>a:hover{color:#262626;text-decoration:none;background-color:#f5f5f5}.dropdown-menu>.active>a,.dropdown-menu>.active>a:focus,.dropdown-");
	appendFile(SPIFFS, "/bootstrap.min.css", "menu>.active>a:hover{color:#fff;text-decoration:none;background-color:#337ab7;outline:0}.dropdown-menu>.disabled>a,.dropdown-menu>.disabled>a:focus,.dropdown-menu>.disabled>a:hover{color:#777}.dropdown-menu>.disabled>a:focus,.dropdown-menu>.disabled>a:hover{text-decoration:none;cursor:not-allowed;ba");
	appendFile(SPIFFS, "/bootstrap.min.css", "ckground-color:transparent;background-image:none;filter:progid:DXImageTransform.Microsoft.gradient(enabled=false)}.open>.dropdown-menu{display:block}.open>a{outline:0}.dropdown-menu-right{right:0;left:auto}.dropdown-menu-left{right:auto;left:0}.dropdown-header{display:block;padding:3px 20px;font-siz");
	appendFile(SPIFFS, "/bootstrap.min.css", "e:12px;line-height:1.42857143;color:#777;white-space:nowrap}.dropdown-backdrop{position:fixed;top:0;right:0;bottom:0;left:0;z-index:990}.pull-right>.dropdown-menu{right:0;left:auto}.dropup .caret,.navbar-fixed-bottom .dropdown .caret{content:\"\";border-top:0;border-bottom:4px dashed;border-bottom:4px");
	appendFile(SPIFFS, "/bootstrap.min.css", " solid\\9}.dropup .dropdown-menu,.navbar-fixed-bottom .dropdown .dropdown-menu{top:auto;bottom:100%;margin-bottom:2px}@media (min-width:768px){.navbar-right .dropdown-menu{right:0;left:auto}.navbar-right .dropdown-menu-left{right:auto;left:0}}.btn-group,.btn-group-vertical{position:relative;display:i");
	appendFile(SPIFFS, "/bootstrap.min.css", "nline-block;vertical-align:middle}.btn-group-vertical>.btn,.btn-group>.btn{position:relative;float:left}.btn-group-vertical>.btn.active,.btn-group-vertical>.btn:active,.btn-group-vertical>.btn:focus,.btn-group-vertical>.btn:hover,.btn-group>.btn.active,.btn-group>.btn:active,.btn-group>.btn:focus,.b");
	appendFile(SPIFFS, "/bootstrap.min.css", "tn-group>.btn:hover{z-index:2}.btn-group .btn+.btn,.btn-group .btn+.btn-group,.btn-group .btn-group+.btn,.btn-group .btn-group+.btn-group{margin-left:-1px}.btn-toolbar{margin-left:-5px}.btn-toolbar .btn,.btn-toolbar .btn-group,.btn-toolbar .input-group{float:left}.btn-toolbar>.btn,.btn-toolbar>.btn-");
	appendFile(SPIFFS, "/bootstrap.min.css", "group,.btn-toolbar>.input-group{margin-left:5px}.btn-group>.btn:not(:first-child):not(:last-child):not(.dropdown-toggle){border-radius:0}.btn-group>.btn:first-child{margin-left:0}.btn-group>.btn:first-child:not(:last-child):not(.dropdown-toggle){border-top-right-radius:0;border-bottom-right-radius:0");
	appendFile(SPIFFS, "/bootstrap.min.css", "}.btn-group>.btn:last-child:not(:first-child),.btn-group>.dropdown-toggle:not(:first-child){border-top-left-radius:0;border-bottom-left-radius:0}.btn-group>.btn-group{float:left}.btn-group>.btn-group:not(:first-child):not(:last-child)>.btn{border-radius:0}.btn-group>.btn-group:first-child:not(:last-");
	appendFile(SPIFFS, "/bootstrap.min.css", "child)>.btn:last-child,.btn-group>.btn-group:first-child:not(:last-child)>.dropdown-toggle{border-top-right-radius:0;border-bottom-right-radius:0}.btn-group>.btn-group:last-child:not(:first-child)>.btn:first-child{border-top-left-radius:0;border-bottom-left-radius:0}.btn-group .dropdown-toggle:activ");
	appendFile(SPIFFS, "/bootstrap.min.css", "e,.btn-group.open .dropdown-toggle{outline:0}.btn-group>.btn+.dropdown-toggle{padding-right:8px;padding-left:8px}.btn-group>.btn-lg+.dropdown-toggle{padding-right:12px;padding-left:12px}.btn-group.open .dropdown-toggle{-webkit-box-shadow:inset 0 3px 5px rgba(0,0,0,.125);box-shadow:inset 0 3px 5px rg");
	appendFile(SPIFFS, "/bootstrap.min.css", "ba(0,0,0,.125)}.btn-group.open .dropdown-toggle.btn-link{-webkit-box-shadow:none;box-shadow:none}.btn .caret{margin-left:0}.btn-lg .caret{border-width:5px 5px 0;border-bottom-width:0}.dropup .btn-lg .caret{border-width:0 5px 5px}.btn-group-vertical>.btn,.btn-group-vertical>.btn-group,.btn-group-vert");
	appendFile(SPIFFS, "/bootstrap.min.css", "ical>.btn-group>.btn{display:block;float:none;width:100%;max-width:100%}.btn-group-vertical>.btn-group>.btn{float:none}.btn-group-vertical>.btn+.btn,.btn-group-vertical>.btn+.btn-group,.btn-group-vertical>.btn-group+.btn,.btn-group-vertical>.btn-group+.btn-group{margin-top:-1px;margin-left:0}.btn-gr");
	appendFile(SPIFFS, "/bootstrap.min.css", "oup-vertical>.btn:not(:first-child):not(:last-child){border-radius:0}.btn-group-vertical>.btn:first-child:not(:last-child){border-top-left-radius:4px;border-top-right-radius:4px;border-bottom-right-radius:0;border-bottom-left-radius:0}.btn-group-vertical>.btn:last-child:not(:first-child){border-top-");
	appendFile(SPIFFS, "/bootstrap.min.css", "left-radius:0;border-top-right-radius:0;border-bottom-right-radius:4px;border-bottom-left-radius:4px}.btn-group-vertical>.btn-group:not(:first-child):not(:last-child)>.btn{border-radius:0}.btn-group-vertical>.btn-group:first-child:not(:last-child)>.btn:last-child,.btn-group-vertical>.btn-group:first");
	appendFile(SPIFFS, "/bootstrap.min.css", "-child:not(:last-child)>.dropdown-toggle{border-bottom-right-radius:0;border-bottom-left-radius:0}.btn-group-vertical>.btn-group:last-child:not(:first-child)>.btn:first-child{border-top-left-radius:0;border-top-right-radius:0}.btn-group-justified{display:table;width:100%;table-layout:fixed;border-co");
	appendFile(SPIFFS, "/bootstrap.min.css", "llapse:separate}.btn-group-justified>.btn,.btn-group-justified>.btn-group{display:table-cell;float:none;width:1%}.btn-group-justified>.btn-group .btn{width:100%}.btn-group-justified>.btn-group .dropdown-menu{left:auto}[data-toggle=buttons]>.btn input[type=checkbox],[data-toggle=buttons]>.btn input[t");
	appendFile(SPIFFS, "/bootstrap.min.css", "ype=radio],[data-toggle=buttons]>.btn-group>.btn input[type=checkbox],[data-toggle=buttons]>.btn-group>.btn input[type=radio]{position:absolute;clip:rect(0,0,0,0);pointer-events:none}.input-group{position:relative;display:table;border-collapse:separate}.input-group[class*=col-]{float:none;padding-ri");
	appendFile(SPIFFS, "/bootstrap.min.css", "ght:0;padding-left:0}.input-group .form-control{position:relative;z-index:2;float:left;width:100%;margin-bottom:0}.input-group .form-control:focus{z-index:3}.input-group-lg>.form-control,.input-group-lg>.input-group-addon,.input-group-lg>.input-group-btn>.btn{height:46px;padding:10px 16px;font-size:");
	appendFile(SPIFFS, "/bootstrap.min.css", "18px;line-height:1.3333333;border-radius:6px}select.input-group-lg>.form-control,select.input-group-lg>.input-group-addon,select.input-group-lg>.input-group-btn>.btn{height:46px;line-height:46px}select[multiple].input-group-lg>.form-control,select[multiple].input-group-lg>.input-group-addon,select[m");
	appendFile(SPIFFS, "/bootstrap.min.css", "ultiple].input-group-lg>.input-group-btn>.btn,textarea.input-group-lg>.form-control,textarea.input-group-lg>.input-group-addon,textarea.input-group-lg>.input-group-btn>.btn{height:auto}.input-group-sm>.form-control,.input-group-sm>.input-group-addon,.input-group-sm>.input-group-btn>.btn{height:30px;");
	appendFile(SPIFFS, "/bootstrap.min.css", "padding:5px 10px;font-size:12px;line-height:1.5;border-radius:3px}select.input-group-sm>.form-control,select.input-group-sm>.input-group-addon,select.input-group-sm>.input-group-btn>.btn{height:30px;line-height:30px}select[multiple].input-group-sm>.form-control,select[multiple].input-group-sm>.input");
	appendFile(SPIFFS, "/bootstrap.min.css", "-group-addon,select[multiple].input-group-sm>.input-group-btn>.btn,textarea.input-group-sm>.form-control,textarea.input-group-sm>.input-group-addon,textarea.input-group-sm>.input-group-btn>.btn{height:auto}.input-group .form-control,.input-group-addon,.input-group-btn{display:table-cell}.input-group");
	appendFile(SPIFFS, "/bootstrap.min.css", " .form-control:not(:first-child):not(:last-child),.input-group-addon:not(:first-child):not(:last-child),.input-group-btn:not(:first-child):not(:last-child){border-radius:0}.input-group-addon,.input-group-btn{width:1%;white-space:nowrap;vertical-align:middle}.input-group-addon{padding:6px 12px;font-s");
	appendFile(SPIFFS, "/bootstrap.min.css", "ize:14px;font-weight:400;line-height:1;color:#555;text-align:center;background-color:#eee;border:1px solid #ccc;border-radius:4px}.input-group-addon.input-sm{padding:5px 10px;font-size:12px;border-radius:3px}.input-group-addon.input-lg{padding:10px 16px;font-size:18px;border-radius:6px}.input-group-");
	appendFile(SPIFFS, "/bootstrap.min.css", "addon input[type=checkbox],.input-group-addon input[type=radio]{margin-top:0}.input-group .form-control:first-child,.input-group-addon:first-child,.input-group-btn:first-child>.btn,.input-group-btn:first-child>.btn-group>.btn,.input-group-btn:first-child>.dropdown-toggle,.input-group-btn:last-child>");
	appendFile(SPIFFS, "/bootstrap.min.css", ".btn-group:not(:last-child)>.btn,.input-group-btn:last-child>.btn:not(:last-child):not(.dropdown-toggle){border-top-right-radius:0;border-bottom-right-radius:0}.input-group-addon:first-child{border-right:0}.input-group .form-control:last-child,.input-group-addon:last-child,.input-group-btn:first-chi");
	appendFile(SPIFFS, "/bootstrap.min.css", "ld>.btn-group:not(:first-child)>.btn,.input-group-btn:first-child>.btn:not(:first-child),.input-group-btn:last-child>.btn,.input-group-btn:last-child>.btn-group>.btn,.input-group-btn:last-child>.dropdown-toggle{border-top-left-radius:0;border-bottom-left-radius:0}.input-group-addon:last-child{border");
	appendFile(SPIFFS, "/bootstrap.min.css", "-left:0}.input-group-btn{position:relative;font-size:0;white-space:nowrap}.input-group-btn>.btn{position:relative}.input-group-btn>.btn+.btn{margin-left:-1px}.input-group-btn>.btn:active,.input-group-btn>.btn:focus,.input-group-btn>.btn:hover{z-index:2}.input-group-btn:first-child>.btn,.input-group-");
	appendFile(SPIFFS, "/bootstrap.min.css", "btn:first-child>.btn-group{margin-right:-1px}.input-group-btn:last-child>.btn,.input-group-btn:last-child>.btn-group{z-index:2;margin-left:-1px}.nav{padding-left:0;margin-bottom:0;list-style:none}.nav>li{position:relative;display:block}.nav>li>a{position:relative;display:block;padding:10px 15px}.nav");
	appendFile(SPIFFS, "/bootstrap.min.css", ">li>a:focus,.nav>li>a:hover{text-decoration:none;background-color:#eee}.nav>li.disabled>a{color:#777}.nav>li.disabled>a:focus,.nav>li.disabled>a:hover{color:#777;text-decoration:none;cursor:not-allowed;background-color:transparent}.nav .open>a,.nav .open>a:focus,.nav .open>a:hover{background-color:#");
	appendFile(SPIFFS, "/bootstrap.min.css", "eee;border-color:#337ab7}.nav .nav-divider{height:1px;margin:9px 0;overflow:hidden;background-color:#e5e5e5}.nav>li>a>img{max-width:none}.nav-tabs{border-bottom:1px solid #ddd}.nav-tabs>li{float:left;margin-bottom:-1px}.nav-tabs>li>a{margin-right:2px;line-height:1.42857143;border:1px solid transpare");
	appendFile(SPIFFS, "/bootstrap.min.css", "nt;border-radius:4px 4px 0 0}.nav-tabs>li>a:hover{border-color:#eee #eee #ddd}.nav-tabs>li.active>a,.nav-tabs>li.active>a:focus,.nav-tabs>li.active>a:hover{color:#555;cursor:default;background-color:#fff;border:1px solid #ddd;border-bottom-color:transparent}.nav-tabs.nav-justified{width:100%;border-");
	appendFile(SPIFFS, "/bootstrap.min.css", "bottom:0}.nav-tabs.nav-justified>li{float:none}.nav-tabs.nav-justified>li>a{margin-bottom:5px;text-align:center}.nav-tabs.nav-justified>.dropdown .dropdown-menu{top:auto;left:auto}@media (min-width:768px){.nav-tabs.nav-justified>li{display:table-cell;width:1%}.nav-tabs.nav-justified>li>a{margin-bott");
	appendFile(SPIFFS, "/bootstrap.min.css", "om:0}}.nav-tabs.nav-justified>li>a{margin-right:0;border-radius:4px}.nav-tabs.nav-justified>.active>a,.nav-tabs.nav-justified>.active>a:focus,.nav-tabs.nav-justified>.active>a:hover{border:1px solid #ddd}@media (min-width:768px){.nav-tabs.nav-justified>li>a{border-bottom:1px solid #ddd;border-radius");
	appendFile(SPIFFS, "/bootstrap.min.css", ":4px 4px 0 0}.nav-tabs.nav-justified>.active>a,.nav-tabs.nav-justified>.active>a:focus,.nav-tabs.nav-justified>.active>a:hover{border-bottom-color:#fff}}.nav-pills>li{float:left}.nav-pills>li>a{border-radius:4px}.nav-pills>li+li{margin-left:2px}.nav-pills>li.active>a,.nav-pills>li.active>a:focus,.na");
	appendFile(SPIFFS, "/bootstrap.min.css", "v-pills>li.active>a:hover{color:#fff;background-color:#337ab7}.nav-stacked>li{float:none}.nav-stacked>li+li{margin-top:2px;margin-left:0}.nav-justified{width:100%}.nav-justified>li{float:none}.nav-justified>li>a{margin-bottom:5px;text-align:center}.nav-justified>.dropdown .dropdown-menu{top:auto;lef");
	appendFile(SPIFFS, "/bootstrap.min.css", "t:auto}@media (min-width:768px){.nav-justified>li{display:table-cell;width:1%}.nav-justified>li>a{margin-bottom:0}}.nav-tabs-justified{border-bottom:0}.nav-tabs-justified>li>a{margin-right:0;border-radius:4px}.nav-tabs-justified>.active>a,.nav-tabs-justified>.active>a:focus,.nav-tabs-justified>.acti");
	appendFile(SPIFFS, "/bootstrap.min.css", "ve>a:hover{border:1px solid #ddd}@media (min-width:768px){.nav-tabs-justified>li>a{border-bottom:1px solid #ddd;border-radius:4px 4px 0 0}.nav-tabs-justified>.active>a,.nav-tabs-justified>.active>a:focus,.nav-tabs-justified>.active>a:hover{border-bottom-color:#fff}}.tab-content>.tab-pane{display:non");
	appendFile(SPIFFS, "/bootstrap.min.css", "e}.tab-content>.active{display:block}.nav-tabs .dropdown-menu{margin-top:-1px;border-top-left-radius:0;border-top-right-radius:0}.navbar{position:relative;min-height:50px;margin-bottom:20px;border:1px solid transparent}@media (min-width:768px){.navbar{border-radius:4px}}@media (min-width:768px){.nav");
	appendFile(SPIFFS, "/bootstrap.min.css", "bar-header{float:left}}.navbar-collapse{padding-right:15px;padding-left:15px;overflow-x:visible;-webkit-overflow-scrolling:touch;border-top:1px solid transparent;-webkit-box-shadow:inset 0 1px 0 rgba(255,255,255,.1);box-shadow:inset 0 1px 0 rgba(255,255,255,.1)}.navbar-collapse.in{overflow-y:auto}@m");
	appendFile(SPIFFS, "/bootstrap.min.css", "edia (min-width:768px){.navbar-collapse{width:auto;border-top:0;-webkit-box-shadow:none;box-shadow:none}.navbar-collapse.collapse{display:block!important;height:auto!important;padding-bottom:0;overflow:visible!important}.navbar-collapse.in{overflow-y:visible}.navbar-fixed-bottom .navbar-collapse,.na");
	appendFile(SPIFFS, "/bootstrap.min.css", "vbar-fixed-top .navbar-collapse,.navbar-static-top .navbar-collapse{padding-right:0;padding-left:0}}.navbar-fixed-bottom .navbar-collapse,.navbar-fixed-top .navbar-collapse{max-height:340px}@media (max-device-width:480px) and (orientation:landscape){.navbar-fixed-bottom .navbar-collapse,.navbar-fixe");
	appendFile(SPIFFS, "/bootstrap.min.css", "d-top .navbar-collapse{max-height:200px}}.container-fluid>.navbar-collapse,.container-fluid>.navbar-header,.container>.navbar-collapse,.container>.navbar-header{margin-right:-15px;margin-left:-15px}@media (min-width:768px){.container-fluid>.navbar-collapse,.container-fluid>.navbar-header,.container>");
	appendFile(SPIFFS, "/bootstrap.min.css", ".navbar-collapse,.container>.navbar-header{margin-right:0;margin-left:0}}.navbar-static-top{z-index:1000;border-width:0 0 1px}@media (min-width:768px){.navbar-static-top{border-radius:0}}.navbar-fixed-bottom,.navbar-fixed-top{position:fixed;right:0;left:0;z-index:1030}@media (min-width:768px){.navba");
	appendFile(SPIFFS, "/bootstrap.min.css", "r-fixed-bottom,.navbar-fixed-top{border-radius:0}}.navbar-fixed-top{top:0;border-width:0 0 1px}.navbar-fixed-bottom{bottom:0;margin-bottom:0;border-width:1px 0 0}.navbar-brand{float:left;height:50px;padding:15px 15px;font-size:18px;line-height:20px}.navbar-brand:focus,.navbar-brand:hover{text-decora");
	appendFile(SPIFFS, "/bootstrap.min.css", "tion:none}.navbar-brand>img{display:block}@media (min-width:768px){.navbar>.container .navbar-brand,.navbar>.container-fluid .navbar-brand{margin-left:-15px}}.navbar-toggle{position:relative;float:right;padding:9px 10px;margin-top:8px;margin-right:15px;margin-bottom:8px;background-color:transparent;");
	appendFile(SPIFFS, "/bootstrap.min.css", "background-image:none;border:1px solid transparent;border-radius:4px}.navbar-toggle:focus{outline:0}.navbar-toggle .icon-bar{display:block;width:22px;height:2px;border-radius:1px}.navbar-toggle .icon-bar+.icon-bar{margin-top:4px}@media (min-width:768px){.navbar-toggle{display:none}}.navbar-nav{margi");
	appendFile(SPIFFS, "/bootstrap.min.css", "n:7.5px -15px}.navbar-nav>li>a{padding-top:10px;padding-bottom:10px;line-height:20px}@media (max-width:767px){.navbar-nav .open .dropdown-menu{position:static;float:none;width:auto;margin-top:0;background-color:transparent;border:0;-webkit-box-shadow:none;box-shadow:none}.navbar-nav .open .dropdown-");
	appendFile(SPIFFS, "/bootstrap.min.css", "menu .dropdown-header,.navbar-nav .open .dropdown-menu>li>a{padding:5px 15px 5px 25px}.navbar-nav .open .dropdown-menu>li>a{line-height:20px}.navbar-nav .open .dropdown-menu>li>a:focus,.navbar-nav .open .dropdown-menu>li>a:hover{background-image:none}}@media (min-width:768px){.navbar-nav{float:left;");
	appendFile(SPIFFS, "/bootstrap.min.css", "margin:0}.navbar-nav>li{float:left}.navbar-nav>li>a{padding-top:15px;padding-bottom:15px}}.navbar-form{padding:10px 15px;margin-top:8px;margin-right:-15px;margin-bottom:8px;margin-left:-15px;border-top:1px solid transparent;border-bottom:1px solid transparent;-webkit-box-shadow:inset 0 1px 0 rgba(25");
	appendFile(SPIFFS, "/bootstrap.min.css", "5,255,255,.1),0 1px 0 rgba(255,255,255,.1);box-shadow:inset 0 1px 0 rgba(255,255,255,.1),0 1px 0 rgba(255,255,255,.1)}@media (min-width:768px){.navbar-form .form-group{display:inline-block;margin-bottom:0;vertical-align:middle}.navbar-form .form-control{display:inline-block;width:auto;vertical-align");
	appendFile(SPIFFS, "/bootstrap.min.css", ":middle}.navbar-form .form-control-static{display:inline-block}.navbar-form .input-group{display:inline-table;vertical-align:middle}.navbar-form .input-group .form-control,.navbar-form .input-group .input-group-addon,.navbar-form .input-group .input-group-btn{width:auto}.navbar-form .input-group>.fo");
	appendFile(SPIFFS, "/bootstrap.min.css", "rm-control{width:100%}.navbar-form .control-label{margin-bottom:0;vertical-align:middle}.navbar-form .checkbox,.navbar-form .radio{display:inline-block;margin-top:0;margin-bottom:0;vertical-align:middle}.navbar-form .checkbox label,.navbar-form .radio label{padding-left:0}.navbar-form .checkbox inpu");
	appendFile(SPIFFS, "/bootstrap.min.css", "t[type=checkbox],.navbar-form .radio input[type=radio]{position:relative;margin-left:0}.navbar-form .has-feedback .form-control-feedback{top:0}}@media (max-width:767px){.navbar-form .form-group{margin-bottom:5px}.navbar-form .form-group:last-child{margin-bottom:0}}@media (min-width:768px){.navbar-fo");
	appendFile(SPIFFS, "/bootstrap.min.css", "rm{width:auto;padding-top:0;padding-bottom:0;margin-right:0;margin-left:0;border:0;-webkit-box-shadow:none;box-shadow:none}}.navbar-nav>li>.dropdown-menu{margin-top:0;border-top-left-radius:0;border-top-right-radius:0}.navbar-fixed-bottom .navbar-nav>li>.dropdown-menu{margin-bottom:0;border-top-left");
	appendFile(SPIFFS, "/bootstrap.min.css", "-radius:4px;border-top-right-radius:4px;border-bottom-right-radius:0;border-bottom-left-radius:0}.navbar-btn{margin-top:8px;margin-bottom:8px}.navbar-btn.btn-sm{margin-top:10px;margin-bottom:10px}.navbar-btn.btn-xs{margin-top:14px;margin-bottom:14px}.navbar-text{margin-top:15px;margin-bottom:15px}@m");
	appendFile(SPIFFS, "/bootstrap.min.css", "edia (min-width:768px){.navbar-text{float:left;margin-right:15px;margin-left:15px}}@media (min-width:768px){.navbar-left{float:left!important}.navbar-right{float:right!important;margin-right:-15px}.navbar-right~.navbar-right{margin-right:0}}.navbar-default{background-color:#f8f8f8;border-color:#e7e7");
	appendFile(SPIFFS, "/bootstrap.min.css", "e7}.navbar-default .navbar-brand{color:#777}.navbar-default .navbar-brand:focus,.navbar-default .navbar-brand:hover{color:#5e5e5e;background-color:transparent}.navbar-default .navbar-text{color:#777}.navbar-default .navbar-nav>li>a{color:#777}.navbar-default .navbar-nav>li>a:focus,.navbar-default .n");
	appendFile(SPIFFS, "/bootstrap.min.css", "avbar-nav>li>a:hover{color:#333;background-color:transparent}.navbar-default .navbar-nav>.active>a,.navbar-default .navbar-nav>.active>a:focus,.navbar-default .navbar-nav>.active>a:hover{color:#555;background-color:#e7e7e7}.navbar-default .navbar-nav>.disabled>a,.navbar-default .navbar-nav>.disabled");
	appendFile(SPIFFS, "/bootstrap.min.css", ">a:focus,.navbar-default .navbar-nav>.disabled>a:hover{color:#ccc;background-color:transparent}.navbar-default .navbar-toggle{border-color:#ddd}.navbar-default .navbar-toggle:focus,.navbar-default .navbar-toggle:hover{background-color:#ddd}.navbar-default .navbar-toggle .icon-bar{background-color:#8");
	appendFile(SPIFFS, "/bootstrap.min.css", "88}.navbar-default .navbar-collapse,.navbar-default .navbar-form{border-color:#e7e7e7}.navbar-default .navbar-nav>.open>a,.navbar-default .navbar-nav>.open>a:focus,.navbar-default .navbar-nav>.open>a:hover{color:#555;background-color:#e7e7e7}@media (max-width:767px){.navbar-default .navbar-nav .open");
	appendFile(SPIFFS, "/bootstrap.min.css", " .dropdown-menu>li>a{color:#777}.navbar-default .navbar-nav .open .dropdown-menu>li>a:focus,.navbar-default .navbar-nav .open .dropdown-menu>li>a:hover{color:#333;background-color:transparent}.navbar-default .navbar-nav .open .dropdown-menu>.active>a,.navbar-default .navbar-nav .open .dropdown-menu>");
	appendFile(SPIFFS, "/bootstrap.min.css", ".active>a:focus,.navbar-default .navbar-nav .open .dropdown-menu>.active>a:hover{color:#555;background-color:#e7e7e7}.navbar-default .navbar-nav .open .dropdown-menu>.disabled>a,.navbar-default .navbar-nav .open .dropdown-menu>.disabled>a:focus,.navbar-default .navbar-nav .open .dropdown-menu>.disab");
	appendFile(SPIFFS, "/bootstrap.min.css", "led>a:hover{color:#ccc;background-color:transparent}}.navbar-default .navbar-link{color:#777}.navbar-default .navbar-link:hover{color:#333}.navbar-default .btn-link{color:#777}.navbar-default .btn-link:focus,.navbar-default .btn-link:hover{color:#333}.navbar-default .btn-link[disabled]:focus,.navbar");
	appendFile(SPIFFS, "/bootstrap.min.css", "-default .btn-link[disabled]:hover,fieldset[disabled] .navbar-default .btn-link:focus,fieldset[disabled] .navbar-default .btn-link:hover{color:#ccc}.navbar-inverse{background-color:#222;border-color:#080808}.navbar-inverse .navbar-brand{color:#9d9d9d}.navbar-inverse .navbar-brand:focus,.navbar-inver");
	appendFile(SPIFFS, "/bootstrap.min.css", "se .navbar-brand:hover{color:#fff;background-color:transparent}.navbar-inverse .navbar-text{color:#9d9d9d}.navbar-inverse .navbar-nav>li>a{color:#9d9d9d}.navbar-inverse .navbar-nav>li>a:focus,.navbar-inverse .navbar-nav>li>a:hover{color:#fff;background-color:transparent}.navbar-inverse .navbar-nav>.");
	appendFile(SPIFFS, "/bootstrap.min.css", "active>a,.navbar-inverse .navbar-nav>.active>a:focus,.navbar-inverse .navbar-nav>.active>a:hover{color:#fff;background-color:#080808}.navbar-inverse .navbar-nav>.disabled>a,.navbar-inverse .navbar-nav>.disabled>a:focus,.navbar-inverse .navbar-nav>.disabled>a:hover{color:#444;background-color:transpa");
	appendFile(SPIFFS, "/bootstrap.min.css", "rent}.navbar-inverse .navbar-toggle{border-color:#333}.navbar-inverse .navbar-toggle:focus,.navbar-inverse .navbar-toggle:hover{background-color:#333}.navbar-inverse .navbar-toggle .icon-bar{background-color:#fff}.navbar-inverse .navbar-collapse,.navbar-inverse .navbar-form{border-color:#101010}.nav");
	appendFile(SPIFFS, "/bootstrap.min.css", "bar-inverse .navbar-nav>.open>a,.navbar-inverse .navbar-nav>.open>a:focus,.navbar-inverse .navbar-nav>.open>a:hover{color:#fff;background-color:#080808}@media (max-width:767px){.navbar-inverse .navbar-nav .open .dropdown-menu>.dropdown-header{border-color:#080808}.navbar-inverse .navbar-nav .open .d");
	appendFile(SPIFFS, "/bootstrap.min.css", "ropdown-menu .divider{background-color:#080808}.navbar-inverse .navbar-nav .open .dropdown-menu>li>a{color:#9d9d9d}.navbar-inverse .navbar-nav .open .dropdown-menu>li>a:focus,.navbar-inverse .navbar-nav .open .dropdown-menu>li>a:hover{color:#fff;background-color:transparent}.navbar-inverse .navbar-n");
	appendFile(SPIFFS, "/bootstrap.min.css", "av .open .dropdown-menu>.active>a,.navbar-inverse .navbar-nav .open .dropdown-menu>.active>a:focus,.navbar-inverse .navbar-nav .open .dropdown-menu>.active>a:hover{color:#fff;background-color:#080808}.navbar-inverse .navbar-nav .open .dropdown-menu>.disabled>a,.navbar-inverse .navbar-nav .open .drop");
	appendFile(SPIFFS, "/bootstrap.min.css", "down-menu>.disabled>a:focus,.navbar-inverse .navbar-nav .open .dropdown-menu>.disabled>a:hover{color:#444;background-color:transparent}}.navbar-inverse .navbar-link{color:#9d9d9d}.navbar-inverse .navbar-link:hover{color:#fff}.navbar-inverse .btn-link{color:#9d9d9d}.navbar-inverse .btn-link:focus,.na");
	appendFile(SPIFFS, "/bootstrap.min.css", "vbar-inverse .btn-link:hover{color:#fff}.navbar-inverse .btn-link[disabled]:focus,.navbar-inverse .btn-link[disabled]:hover,fieldset[disabled] .navbar-inverse .btn-link:focus,fieldset[disabled] .navbar-inverse .btn-link:hover{color:#444}.breadcrumb{padding:8px 15px;margin-bottom:20px;list-style:none");
	appendFile(SPIFFS, "/bootstrap.min.css", ";background-color:#f5f5f5;border-radius:4px}.breadcrumb>li{display:inline-block}.breadcrumb>li+li:before{padding:0 5px;color:#ccc;content:\"/\\00a0\"}.breadcrumb>.active{color:#777}.pagination{display:inline-block;padding-left:0;margin:20px 0;border-radius:4px}.pagination>li{display:inline}.pagination>");
	appendFile(SPIFFS, "/bootstrap.min.css", "li>a,.pagination>li>span{position:relative;float:left;padding:6px 12px;margin-left:-1px;line-height:1.42857143;color:#337ab7;text-decoration:none;background-color:#fff;border:1px solid #ddd}.pagination>li:first-child>a,.pagination>li:first-child>span{margin-left:0;border-top-left-radius:4px;border-b");
	appendFile(SPIFFS, "/bootstrap.min.css", "ottom-left-radius:4px}.pagination>li:last-child>a,.pagination>li:last-child>span{border-top-right-radius:4px;border-bottom-right-radius:4px}.pagination>li>a:focus,.pagination>li>a:hover,.pagination>li>span:focus,.pagination>li>span:hover{z-index:2;color:#23527c;background-color:#eee;border-color:#dd");
	appendFile(SPIFFS, "/bootstrap.min.css", "d}.pagination>.active>a,.pagination>.active>a:focus,.pagination>.active>a:hover,.pagination>.active>span,.pagination>.active>span:focus,.pagination>.active>span:hover{z-index:3;color:#fff;cursor:default;background-color:#337ab7;border-color:#337ab7}.pagination>.disabled>a,.pagination>.disabled>a:foc");
	appendFile(SPIFFS, "/bootstrap.min.css", "us,.pagination>.disabled>a:hover,.pagination>.disabled>span,.pagination>.disabled>span:focus,.pagination>.disabled>span:hover{color:#777;cursor:not-allowed;background-color:#fff;border-color:#ddd}.pagination-lg>li>a,.pagination-lg>li>span{padding:10px 16px;font-size:18px;line-height:1.3333333}.pagin");
	appendFile(SPIFFS, "/bootstrap.min.css", "ation-lg>li:first-child>a,.pagination-lg>li:first-child>span{border-top-left-radius:6px;border-bottom-left-radius:6px}.pagination-lg>li:last-child>a,.pagination-lg>li:last-child>span{border-top-right-radius:6px;border-bottom-right-radius:6px}.pagination-sm>li>a,.pagination-sm>li>span{padding:5px 10p");
	appendFile(SPIFFS, "/bootstrap.min.css", "x;font-size:12px;line-height:1.5}.pagination-sm>li:first-child>a,.pagination-sm>li:first-child>span{border-top-left-radius:3px;border-bottom-left-radius:3px}.pagination-sm>li:last-child>a,.pagination-sm>li:last-child>span{border-top-right-radius:3px;border-bottom-right-radius:3px}.pager{padding-left");
	appendFile(SPIFFS, "/bootstrap.min.css", ":0;margin:20px 0;text-align:center;list-style:none}.pager li{display:inline}.pager li>a,.pager li>span{display:inline-block;padding:5px 14px;background-color:#fff;border:1px solid #ddd;border-radius:15px}.pager li>a:focus,.pager li>a:hover{text-decoration:none;background-color:#eee}.pager .next>a,.p");
	appendFile(SPIFFS, "/bootstrap.min.css", "ager .next>span{float:right}.pager .previous>a,.pager .previous>span{float:left}.pager .disabled>a,.pager .disabled>a:focus,.pager .disabled>a:hover,.pager .disabled>span{color:#777;cursor:not-allowed;background-color:#fff}.label{display:inline;padding:.2em .6em .3em;font-size:75%;font-weight:700;li");
	appendFile(SPIFFS, "/bootstrap.min.css", "ne-height:1;color:#fff;text-align:center;white-space:nowrap;vertical-align:baseline;border-radius:.25em}a.label:focus,a.label:hover{color:#fff;text-decoration:none;cursor:pointer}.label:empty{display:none}.btn .label{position:relative;top:-1px}.label-default{background-color:#777}.label-default[href");
	appendFile(SPIFFS, "/bootstrap.min.css", "]:focus,.label-default[href]:hover{background-color:#5e5e5e}.label-primary{background-color:#337ab7}.label-primary[href]:focus,.label-primary[href]:hover{background-color:#286090}.label-success{background-color:#5cb85c}.label-success[href]:focus,.label-success[href]:hover{background-color:#449d44}.l");
	appendFile(SPIFFS, "/bootstrap.min.css", "abel-info{background-color:#5bc0de}.label-info[href]:focus,.label-info[href]:hover{background-color:#31b0d5}.label-warning{background-color:#f0ad4e}.label-warning[href]:focus,.label-warning[href]:hover{background-color:#ec971f}.label-danger{background-color:#d9534f}.label-danger[href]:focus,.label-d");
	appendFile(SPIFFS, "/bootstrap.min.css", "anger[href]:hover{background-color:#c9302c}.badge{display:inline-block;min-width:10px;padding:3px 7px;font-size:12px;font-weight:700;line-height:1;color:#fff;text-align:center;white-space:nowrap;vertical-align:middle;background-color:#777;border-radius:10px}.badge:empty{display:none}.btn .badge{posi");
	appendFile(SPIFFS, "/bootstrap.min.css", "tion:relative;top:-1px}.btn-group-xs>.btn .badge,.btn-xs .badge{top:0;padding:1px 5px}a.badge:focus,a.badge:hover{color:#fff;text-decoration:none;cursor:pointer}.list-group-item.active>.badge,.nav-pills>.active>a>.badge{color:#337ab7;background-color:#fff}.list-group-item>.badge{float:right}.list-gr");
	appendFile(SPIFFS, "/bootstrap.min.css", "oup-item>.badge+.badge{margin-right:5px}.nav-pills>li>a>.badge{margin-left:3px}.jumbotron{padding-top:30px;padding-bottom:30px;margin-bottom:30px;color:inherit;background-color:#eee}.jumbotron .h1,.jumbotron h1{color:inherit}.jumbotron p{margin-bottom:15px;font-size:21px;font-weight:200}.jumbotron>h");
	appendFile(SPIFFS, "/bootstrap.min.css", "r{border-top-color:#d5d5d5}.container .jumbotron,.container-fluid .jumbotron{padding-right:15px;padding-left:15px;border-radius:6px}.jumbotron .container{max-width:100%}@media screen and (min-width:768px){.jumbotron{padding-top:48px;padding-bottom:48px}.container .jumbotron,.container-fluid .jumbotr");
	appendFile(SPIFFS, "/bootstrap.min.css", "on{padding-right:60px;padding-left:60px}.jumbotron .h1,.jumbotron h1{font-size:63px}}.thumbnail{display:block;padding:4px;margin-bottom:20px;line-height:1.42857143;background-color:#fff;border:1px solid #ddd;border-radius:4px;-webkit-transition:border .2s ease-in-out;-o-transition:border .2s ease-in");
	appendFile(SPIFFS, "/bootstrap.min.css", "-out;transition:border .2s ease-in-out}.thumbnail a>img,.thumbnail>img{margin-right:auto;margin-left:auto}a.thumbnail.active,a.thumbnail:focus,a.thumbnail:hover{border-color:#337ab7}.thumbnail .caption{padding:9px;color:#333}.alert{padding:15px;margin-bottom:20px;border:1px solid transparent;border-");
	appendFile(SPIFFS, "/bootstrap.min.css", "radius:4px}.alert h4{margin-top:0;color:inherit}.alert .alert-link{font-weight:700}.alert>p,.alert>ul{margin-bottom:0}.alert>p+p{margin-top:5px}.alert-dismissable,.alert-dismissible{padding-right:35px}.alert-dismissable .close,.alert-dismissible .close{position:relative;top:-2px;right:-21px;color:in");
	appendFile(SPIFFS, "/bootstrap.min.css", "herit}.alert-success{color:#3c763d;background-color:#dff0d8;border-color:#d6e9c6}.alert-success hr{border-top-color:#c9e2b3}.alert-success .alert-link{color:#2b542c}.alert-info{color:#31708f;background-color:#d9edf7;border-color:#bce8f1}.alert-info hr{border-top-color:#a6e1ec}.alert-info .alert-link");
	appendFile(SPIFFS, "/bootstrap.min.css", "{color:#245269}.alert-warning{color:#8a6d3b;background-color:#fcf8e3;border-color:#faebcc}.alert-warning hr{border-top-color:#f7e1b5}.alert-warning .alert-link{color:#66512c}.alert-danger{color:#a94442;background-color:#f2dede;border-color:#ebccd1}.alert-danger hr{border-top-color:#e4b9c0}.alert-dan");
	appendFile(SPIFFS, "/bootstrap.min.css", "ger .alert-link{color:#843534}@-webkit-keyframes progress-bar-stripes{from{background-position:40px 0}to{background-position:0 0}}@-o-keyframes progress-bar-stripes{from{background-position:40px 0}to{background-position:0 0}}@keyframes progress-bar-stripes{from{background-position:40px 0}to{backgrou");
	appendFile(SPIFFS, "/bootstrap.min.css", "nd-position:0 0}}.progress{height:20px;margin-bottom:20px;overflow:hidden;background-color:#f5f5f5;border-radius:4px;-webkit-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);box-shadow:inset 0 1px 2px rgba(0,0,0,.1)}.progress-bar{float:left;width:0;height:100%;font-size:12px;line-height:20px;color:#fff;tex");
	appendFile(SPIFFS, "/bootstrap.min.css", "t-align:center;background-color:#337ab7;-webkit-box-shadow:inset 0 -1px 0 rgba(0,0,0,.15);box-shadow:inset 0 -1px 0 rgba(0,0,0,.15);-webkit-transition:width .6s ease;-o-transition:width .6s ease;transition:width .6s ease}.progress-bar-striped,.progress-striped .progress-bar{background-image:-webkit-");
	appendFile(SPIFFS, "/bootstrap.min.css", "linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:-o-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,");
	appendFile(SPIFFS, "/bootstrap.min.css", "255,.15) 75%,transparent 75%,transparent);background-image:linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);-webkit-background-size:40px 40px;background-size:40px 40px}.progress-bar.active");
	appendFile(SPIFFS, "/bootstrap.min.css", ",.progress.active .progress-bar{-webkit-animation:progress-bar-stripes 2s linear infinite;-o-animation:progress-bar-stripes 2s linear infinite;animation:progress-bar-stripes 2s linear infinite}.progress-bar-success{background-color:#5cb85c}.progress-striped .progress-bar-success{background-image:-we");
	appendFile(SPIFFS, "/bootstrap.min.css", "bkit-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:-o-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255");
	appendFile(SPIFFS, "/bootstrap.min.css", ",255,255,.15) 75%,transparent 75%,transparent);background-image:linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent)}.progress-bar-info{background-color:#5bc0de}.progress-striped .progress-bar");
	appendFile(SPIFFS, "/bootstrap.min.css", "-info{background-image:-webkit-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:-o-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255");
	appendFile(SPIFFS, "/bootstrap.min.css", ",255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent)}.progress-bar-warning{background-color:#f0ad4e}.p");
	appendFile(SPIFFS, "/bootstrap.min.css", "rogress-striped .progress-bar-warning{background-image:-webkit-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:-o-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transpar");
	appendFile(SPIFFS, "/bootstrap.min.css", "ent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent)}.progress-bar-dan");
	appendFile(SPIFFS, "/bootstrap.min.css", "ger{background-color:#d9534f}.progress-striped .progress-bar-danger{background-image:-webkit-linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:-o-linear-gradient(45deg,rgba");
	appendFile(SPIFFS, "/bootstrap.min.css", "(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,transparent);background-image:linear-gradient(45deg,rgba(255,255,255,.15) 25%,transparent 25%,transparent 50%,rgba(255,255,255,.15) 50%,rgba(255,255,255,.15) 75%,transparent 75%,");
	appendFile(SPIFFS, "/bootstrap.min.css", "transparent)}.media{margin-top:15px}.media:first-child{margin-top:0}.media,.media-body{overflow:hidden;zoom:1}.media-body{width:10000px}.media-object{display:block}.media-object.img-thumbnail{max-width:none}.media-right,.media>.pull-right{padding-left:10px}.media-left,.media>.pull-left{padding-right");
	appendFile(SPIFFS, "/bootstrap.min.css", ":10px}.media-body,.media-left,.media-right{display:table-cell;vertical-align:top}.media-middle{vertical-align:middle}.media-bottom{vertical-align:bottom}.media-heading{margin-top:0;margin-bottom:5px}.media-list{padding-left:0;list-style:none}.list-group{padding-left:0;margin-bottom:20px}.list-group-");
	appendFile(SPIFFS, "/bootstrap.min.css", "item{position:relative;display:block;padding:10px 15px;margin-bottom:-1px;background-color:#fff;border:1px solid #ddd}.list-group-item:first-child{border-top-left-radius:4px;border-top-right-radius:4px}.list-group-item:last-child{margin-bottom:0;border-bottom-right-radius:4px;border-bottom-left-radi");
	appendFile(SPIFFS, "/bootstrap.min.css", "us:4px}a.list-group-item,button.list-group-item{color:#555}a.list-group-item .list-group-item-heading,button.list-group-item .list-group-item-heading{color:#333}a.list-group-item:focus,a.list-group-item:hover,button.list-group-item:focus,button.list-group-item:hover{color:#555;text-decoration:none;b");
	appendFile(SPIFFS, "/bootstrap.min.css", "ackground-color:#f5f5f5}button.list-group-item{width:100%;text-align:left}.list-group-item.disabled,.list-group-item.disabled:focus,.list-group-item.disabled:hover{color:#777;cursor:not-allowed;background-color:#eee}.list-group-item.disabled .list-group-item-heading,.list-group-item.disabled:focus .");
	appendFile(SPIFFS, "/bootstrap.min.css", "list-group-item-heading,.list-group-item.disabled:hover .list-group-item-heading{color:inherit}.list-group-item.disabled .list-group-item-text,.list-group-item.disabled:focus .list-group-item-text,.list-group-item.disabled:hover .list-group-item-text{color:#777}.list-group-item.active,.list-group-it");
	appendFile(SPIFFS, "/bootstrap.min.css", "em.active:focus,.list-group-item.active:hover{z-index:2;color:#fff;background-color:#337ab7;border-color:#337ab7}.list-group-item.active .list-group-item-heading,.list-group-item.active .list-group-item-heading>.small,.list-group-item.active .list-group-item-heading>small,.list-group-item.active:foc");
	appendFile(SPIFFS, "/bootstrap.min.css", "us .list-group-item-heading,.list-group-item.active:focus .list-group-item-heading>.small,.list-group-item.active:focus .list-group-item-heading>small,.list-group-item.active:hover .list-group-item-heading,.list-group-item.active:hover .list-group-item-heading>.small,.list-group-item.active:hover .l");
	appendFile(SPIFFS, "/bootstrap.min.css", "ist-group-item-heading>small{color:inherit}.list-group-item.active .list-group-item-text,.list-group-item.active:focus .list-group-item-text,.list-group-item.active:hover .list-group-item-text{color:#c7ddef}.list-group-item-success{color:#3c763d;background-color:#dff0d8}a.list-group-item-success,but");
	appendFile(SPIFFS, "/bootstrap.min.css", "ton.list-group-item-success{color:#3c763d}a.list-group-item-success .list-group-item-heading,button.list-group-item-success .list-group-item-heading{color:inherit}a.list-group-item-success:focus,a.list-group-item-success:hover,button.list-group-item-success:focus,button.list-group-item-success:hover");
	appendFile(SPIFFS, "/bootstrap.min.css", "{color:#3c763d;background-color:#d0e9c6}a.list-group-item-success.active,a.list-group-item-success.active:focus,a.list-group-item-success.active:hover,button.list-group-item-success.active,button.list-group-item-success.active:focus,button.list-group-item-success.active:hover{color:#fff;background-c");
	appendFile(SPIFFS, "/bootstrap.min.css", "olor:#3c763d;border-color:#3c763d}.list-group-item-info{color:#31708f;background-color:#d9edf7}a.list-group-item-info,button.list-group-item-info{color:#31708f}a.list-group-item-info .list-group-item-heading,button.list-group-item-info .list-group-item-heading{color:inherit}a.list-group-item-info:fo");
	appendFile(SPIFFS, "/bootstrap.min.css", "cus,a.list-group-item-info:hover,button.list-group-item-info:focus,button.list-group-item-info:hover{color:#31708f;background-color:#c4e3f3}a.list-group-item-info.active,a.list-group-item-info.active:focus,a.list-group-item-info.active:hover,button.list-group-item-info.active,button.list-group-item-");
	appendFile(SPIFFS, "/bootstrap.min.css", "info.active:focus,button.list-group-item-info.active:hover{color:#fff;background-color:#31708f;border-color:#31708f}.list-group-item-warning{color:#8a6d3b;background-color:#fcf8e3}a.list-group-item-warning,button.list-group-item-warning{color:#8a6d3b}a.list-group-item-warning .list-group-item-headin");
	appendFile(SPIFFS, "/bootstrap.min.css", "g,button.list-group-item-warning .list-group-item-heading{color:inherit}a.list-group-item-warning:focus,a.list-group-item-warning:hover,button.list-group-item-warning:focus,button.list-group-item-warning:hover{color:#8a6d3b;background-color:#faf2cc}a.list-group-item-warning.active,a.list-group-item-");
	appendFile(SPIFFS, "/bootstrap.min.css", "warning.active:focus,a.list-group-item-warning.active:hover,button.list-group-item-warning.active,button.list-group-item-warning.active:focus,button.list-group-item-warning.active:hover{color:#fff;background-color:#8a6d3b;border-color:#8a6d3b}.list-group-item-danger{color:#a94442;background-color:#f");
	appendFile(SPIFFS, "/bootstrap.min.css", "2dede}a.list-group-item-danger,button.list-group-item-danger{color:#a94442}a.list-group-item-danger .list-group-item-heading,button.list-group-item-danger .list-group-item-heading{color:inherit}a.list-group-item-danger:focus,a.list-group-item-danger:hover,button.list-group-item-danger:focus,button.l");
	appendFile(SPIFFS, "/bootstrap.min.css", "ist-group-item-danger:hover{color:#a94442;background-color:#ebcccc}a.list-group-item-danger.active,a.list-group-item-danger.active:focus,a.list-group-item-danger.active:hover,button.list-group-item-danger.active,button.list-group-item-danger.active:focus,button.list-group-item-danger.active:hover{co");
	appendFile(SPIFFS, "/bootstrap.min.css", "lor:#fff;background-color:#a94442;border-color:#a94442}.list-group-item-heading{margin-top:0;margin-bottom:5px}.list-group-item-text{margin-bottom:0;line-height:1.3}.panel{margin-bottom:20px;background-color:#fff;border:1px solid transparent;border-radius:4px;-webkit-box-shadow:0 1px 1px rgba(0,0,0,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".05);box-shadow:0 1px 1px rgba(0,0,0,.05)}.panel-body{padding:15px}.panel-heading{padding:10px 15px;border-bottom:1px solid transparent;border-top-left-radius:3px;border-top-right-radius:3px}.panel-heading>.dropdown .dropdown-toggle{color:inherit}.panel-title{margin-top:0;margin-bottom:0;font-size:1");
	appendFile(SPIFFS, "/bootstrap.min.css", "6px;color:inherit}.panel-title>.small,.panel-title>.small>a,.panel-title>a,.panel-title>small,.panel-title>small>a{color:inherit}.panel-footer{padding:10px 15px;background-color:#f5f5f5;border-top:1px solid #ddd;border-bottom-right-radius:3px;border-bottom-left-radius:3px}.panel>.list-group,.panel>.");
	appendFile(SPIFFS, "/bootstrap.min.css", "panel-collapse>.list-group{margin-bottom:0}.panel>.list-group .list-group-item,.panel>.panel-collapse>.list-group .list-group-item{border-width:1px 0;border-radius:0}.panel>.list-group:first-child .list-group-item:first-child,.panel>.panel-collapse>.list-group:first-child .list-group-item:first-chil");
	appendFile(SPIFFS, "/bootstrap.min.css", "d{border-top:0;border-top-left-radius:3px;border-top-right-radius:3px}.panel>.list-group:last-child .list-group-item:last-child,.panel>.panel-collapse>.list-group:last-child .list-group-item:last-child{border-bottom:0;border-bottom-right-radius:3px;border-bottom-left-radius:3px}.panel>.panel-heading");
	appendFile(SPIFFS, "/bootstrap.min.css", "+.panel-collapse>.list-group .list-group-item:first-child{border-top-left-radius:0;border-top-right-radius:0}.panel-heading+.list-group .list-group-item:first-child{border-top-width:0}.list-group+.panel-footer{border-top-width:0}.panel>.panel-collapse>.table,.panel>.table,.panel>.table-responsive>.t");
	appendFile(SPIFFS, "/bootstrap.min.css", "able{margin-bottom:0}.panel>.panel-collapse>.table caption,.panel>.table caption,.panel>.table-responsive>.table caption{padding-right:15px;padding-left:15px}.panel>.table-responsive:first-child>.table:first-child,.panel>.table:first-child{border-top-left-radius:3px;border-top-right-radius:3px}.pane");
	appendFile(SPIFFS, "/bootstrap.min.css", "l>.table-responsive:first-child>.table:first-child>tbody:first-child>tr:first-child,.panel>.table-responsive:first-child>.table:first-child>thead:first-child>tr:first-child,.panel>.table:first-child>tbody:first-child>tr:first-child,.panel>.table:first-child>thead:first-child>tr:first-child{border-to");
	appendFile(SPIFFS, "/bootstrap.min.css", "p-left-radius:3px;border-top-right-radius:3px}.panel>.table-responsive:first-child>.table:first-child>tbody:first-child>tr:first-child td:first-child,.panel>.table-responsive:first-child>.table:first-child>tbody:first-child>tr:first-child th:first-child,.panel>.table-responsive:first-child>.table:fi");
	appendFile(SPIFFS, "/bootstrap.min.css", "rst-child>thead:first-child>tr:first-child td:first-child,.panel>.table-responsive:first-child>.table:first-child>thead:first-child>tr:first-child th:first-child,.panel>.table:first-child>tbody:first-child>tr:first-child td:first-child,.panel>.table:first-child>tbody:first-child>tr:first-child th:fi");
	appendFile(SPIFFS, "/bootstrap.min.css", "rst-child,.panel>.table:first-child>thead:first-child>tr:first-child td:first-child,.panel>.table:first-child>thead:first-child>tr:first-child th:first-child{border-top-left-radius:3px}.panel>.table-responsive:first-child>.table:first-child>tbody:first-child>tr:first-child td:last-child,.panel>.tabl");
	appendFile(SPIFFS, "/bootstrap.min.css", "e-responsive:first-child>.table:first-child>tbody:first-child>tr:first-child th:last-child,.panel>.table-responsive:first-child>.table:first-child>thead:first-child>tr:first-child td:last-child,.panel>.table-responsive:first-child>.table:first-child>thead:first-child>tr:first-child th:last-child,.pa");
	appendFile(SPIFFS, "/bootstrap.min.css", "nel>.table:first-child>tbody:first-child>tr:first-child td:last-child,.panel>.table:first-child>tbody:first-child>tr:first-child th:last-child,.panel>.table:first-child>thead:first-child>tr:first-child td:last-child,.panel>.table:first-child>thead:first-child>tr:first-child th:last-child{border-top-");
	appendFile(SPIFFS, "/bootstrap.min.css", "right-radius:3px}.panel>.table-responsive:last-child>.table:last-child,.panel>.table:last-child{border-bottom-right-radius:3px;border-bottom-left-radius:3px}.panel>.table-responsive:last-child>.table:last-child>tbody:last-child>tr:last-child,.panel>.table-responsive:last-child>.table:last-child>tfoo");
	appendFile(SPIFFS, "/bootstrap.min.css", "t:last-child>tr:last-child,.panel>.table:last-child>tbody:last-child>tr:last-child,.panel>.table:last-child>tfoot:last-child>tr:last-child{border-bottom-right-radius:3px;border-bottom-left-radius:3px}.panel>.table-responsive:last-child>.table:last-child>tbody:last-child>tr:last-child td:first-child,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".panel>.table-responsive:last-child>.table:last-child>tbody:last-child>tr:last-child th:first-child,.panel>.table-responsive:last-child>.table:last-child>tfoot:last-child>tr:last-child td:first-child,.panel>.table-responsive:last-child>.table:last-child>tfoot:last-child>tr:last-child th:first-child,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".panel>.table:last-child>tbody:last-child>tr:last-child td:first-child,.panel>.table:last-child>tbody:last-child>tr:last-child th:first-child,.panel>.table:last-child>tfoot:last-child>tr:last-child td:first-child,.panel>.table:last-child>tfoot:last-child>tr:last-child th:first-child{border-bottom-le");
	appendFile(SPIFFS, "/bootstrap.min.css", "ft-radius:3px}.panel>.table-responsive:last-child>.table:last-child>tbody:last-child>tr:last-child td:last-child,.panel>.table-responsive:last-child>.table:last-child>tbody:last-child>tr:last-child th:last-child,.panel>.table-responsive:last-child>.table:last-child>tfoot:last-child>tr:last-child td:");
	appendFile(SPIFFS, "/bootstrap.min.css", "last-child,.panel>.table-responsive:last-child>.table:last-child>tfoot:last-child>tr:last-child th:last-child,.panel>.table:last-child>tbody:last-child>tr:last-child td:last-child,.panel>.table:last-child>tbody:last-child>tr:last-child th:last-child,.panel>.table:last-child>tfoot:last-child>tr:last-");
	appendFile(SPIFFS, "/bootstrap.min.css", "child td:last-child,.panel>.table:last-child>tfoot:last-child>tr:last-child th:last-child{border-bottom-right-radius:3px}.panel>.panel-body+.table,.panel>.panel-body+.table-responsive,.panel>.table+.panel-body,.panel>.table-responsive+.panel-body{border-top:1px solid #ddd}.panel>.table>tbody:first-c");
	appendFile(SPIFFS, "/bootstrap.min.css", "hild>tr:first-child td,.panel>.table>tbody:first-child>tr:first-child th{border-top:0}.panel>.table-bordered,.panel>.table-responsive>.table-bordered{border:0}.panel>.table-bordered>tbody>tr>td:first-child,.panel>.table-bordered>tbody>tr>th:first-child,.panel>.table-bordered>tfoot>tr>td:first-child,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".panel>.table-bordered>tfoot>tr>th:first-child,.panel>.table-bordered>thead>tr>td:first-child,.panel>.table-bordered>thead>tr>th:first-child,.panel>.table-responsive>.table-bordered>tbody>tr>td:first-child,.panel>.table-responsive>.table-bordered>tbody>tr>th:first-child,.panel>.table-responsive>.tab");
	appendFile(SPIFFS, "/bootstrap.min.css", "le-bordered>tfoot>tr>td:first-child,.panel>.table-responsive>.table-bordered>tfoot>tr>th:first-child,.panel>.table-responsive>.table-bordered>thead>tr>td:first-child,.panel>.table-responsive>.table-bordered>thead>tr>th:first-child{border-left:0}.panel>.table-bordered>tbody>tr>td:last-child,.panel>.t");
	appendFile(SPIFFS, "/bootstrap.min.css", "able-bordered>tbody>tr>th:last-child,.panel>.table-bordered>tfoot>tr>td:last-child,.panel>.table-bordered>tfoot>tr>th:last-child,.panel>.table-bordered>thead>tr>td:last-child,.panel>.table-bordered>thead>tr>th:last-child,.panel>.table-responsive>.table-bordered>tbody>tr>td:last-child,.panel>.table-r");
	appendFile(SPIFFS, "/bootstrap.min.css", "esponsive>.table-bordered>tbody>tr>th:last-child,.panel>.table-responsive>.table-bordered>tfoot>tr>td:last-child,.panel>.table-responsive>.table-bordered>tfoot>tr>th:last-child,.panel>.table-responsive>.table-bordered>thead>tr>td:last-child,.panel>.table-responsive>.table-bordered>thead>tr>th:last-c");
	appendFile(SPIFFS, "/bootstrap.min.css", "hild{border-right:0}.panel>.table-bordered>tbody>tr:first-child>td,.panel>.table-bordered>tbody>tr:first-child>th,.panel>.table-bordered>thead>tr:first-child>td,.panel>.table-bordered>thead>tr:first-child>th,.panel>.table-responsive>.table-bordered>tbody>tr:first-child>td,.panel>.table-responsive>.t");
	appendFile(SPIFFS, "/bootstrap.min.css", "able-bordered>tbody>tr:first-child>th,.panel>.table-responsive>.table-bordered>thead>tr:first-child>td,.panel>.table-responsive>.table-bordered>thead>tr:first-child>th{border-bottom:0}.panel>.table-bordered>tbody>tr:last-child>td,.panel>.table-bordered>tbody>tr:last-child>th,.panel>.table-bordered>t");
	appendFile(SPIFFS, "/bootstrap.min.css", "foot>tr:last-child>td,.panel>.table-bordered>tfoot>tr:last-child>th,.panel>.table-responsive>.table-bordered>tbody>tr:last-child>td,.panel>.table-responsive>.table-bordered>tbody>tr:last-child>th,.panel>.table-responsive>.table-bordered>tfoot>tr:last-child>td,.panel>.table-responsive>.table-bordered");
	appendFile(SPIFFS, "/bootstrap.min.css", ">tfoot>tr:last-child>th{border-bottom:0}.panel>.table-responsive{margin-bottom:0;border:0}.panel-group{margin-bottom:20px}.panel-group .panel{margin-bottom:0;border-radius:4px}.panel-group .panel+.panel{margin-top:5px}.panel-group .panel-heading{border-bottom:0}.panel-group .panel-heading+.panel-col");
	appendFile(SPIFFS, "/bootstrap.min.css", "lapse>.list-group,.panel-group .panel-heading+.panel-collapse>.panel-body{border-top:1px solid #ddd}.panel-group .panel-footer{border-top:0}.panel-group .panel-footer+.panel-collapse .panel-body{border-bottom:1px solid #ddd}.panel-default{border-color:#ddd}.panel-default>.panel-heading{color:#333;ba");
	appendFile(SPIFFS, "/bootstrap.min.css", "ckground-color:#f5f5f5;border-color:#ddd}.panel-default>.panel-heading+.panel-collapse>.panel-body{border-top-color:#ddd}.panel-default>.panel-heading .badge{color:#f5f5f5;background-color:#333}.panel-default>.panel-footer+.panel-collapse>.panel-body{border-bottom-color:#ddd}.panel-primary{border-co");
	appendFile(SPIFFS, "/bootstrap.min.css", "lor:#337ab7}.panel-primary>.panel-heading{color:#fff;background-color:#337ab7;border-color:#337ab7}.panel-primary>.panel-heading+.panel-collapse>.panel-body{border-top-color:#337ab7}.panel-primary>.panel-heading .badge{color:#337ab7;background-color:#fff}.panel-primary>.panel-footer+.panel-collapse>");
	appendFile(SPIFFS, "/bootstrap.min.css", ".panel-body{border-bottom-color:#337ab7}.panel-success{border-color:#d6e9c6}.panel-success>.panel-heading{color:#3c763d;background-color:#dff0d8;border-color:#d6e9c6}.panel-success>.panel-heading+.panel-collapse>.panel-body{border-top-color:#d6e9c6}.panel-success>.panel-heading .badge{color:#dff0d8;");
	appendFile(SPIFFS, "/bootstrap.min.css", "background-color:#3c763d}.panel-success>.panel-footer+.panel-collapse>.panel-body{border-bottom-color:#d6e9c6}.panel-info{border-color:#bce8f1}.panel-info>.panel-heading{color:#31708f;background-color:#d9edf7;border-color:#bce8f1}.panel-info>.panel-heading+.panel-collapse>.panel-body{border-top-colo");
	appendFile(SPIFFS, "/bootstrap.min.css", "r:#bce8f1}.panel-info>.panel-heading .badge{color:#d9edf7;background-color:#31708f}.panel-info>.panel-footer+.panel-collapse>.panel-body{border-bottom-color:#bce8f1}.panel-warning{border-color:#faebcc}.panel-warning>.panel-heading{color:#8a6d3b;background-color:#fcf8e3;border-color:#faebcc}.panel-wa");
	appendFile(SPIFFS, "/bootstrap.min.css", "rning>.panel-heading+.panel-collapse>.panel-body{border-top-color:#faebcc}.panel-warning>.panel-heading .badge{color:#fcf8e3;background-color:#8a6d3b}.panel-warning>.panel-footer+.panel-collapse>.panel-body{border-bottom-color:#faebcc}.panel-danger{border-color:#ebccd1}.panel-danger>.panel-heading{c");
	appendFile(SPIFFS, "/bootstrap.min.css", "olor:#a94442;background-color:#f2dede;border-color:#ebccd1}.panel-danger>.panel-heading+.panel-collapse>.panel-body{border-top-color:#ebccd1}.panel-danger>.panel-heading .badge{color:#f2dede;background-color:#a94442}.panel-danger>.panel-footer+.panel-collapse>.panel-body{border-bottom-color:#ebccd1}");
	appendFile(SPIFFS, "/bootstrap.min.css", ".embed-responsive{position:relative;display:block;height:0;padding:0;overflow:hidden}.embed-responsive .embed-responsive-item,.embed-responsive embed,.embed-responsive iframe,.embed-responsive object,.embed-responsive video{position:absolute;top:0;bottom:0;left:0;width:100%;height:100%;border:0}.emb");
	appendFile(SPIFFS, "/bootstrap.min.css", "ed-responsive-16by9{padding-bottom:56.25%}.embed-responsive-4by3{padding-bottom:75%}.well{min-height:20px;padding:19px;margin-bottom:20px;background-color:#f5f5f5;border:1px solid #e3e3e3;border-radius:4px;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.05);box-shadow:inset 0 1px 1px rgba(0,0,0,.05)}");
	appendFile(SPIFFS, "/bootstrap.min.css", ".well blockquote{border-color:#ddd;border-color:rgba(0,0,0,.15)}.well-lg{padding:24px;border-radius:6px}.well-sm{padding:9px;border-radius:3px}.close{float:right;font-size:21px;font-weight:700;line-height:1;color:#000;text-shadow:0 1px 0 #fff;filter:alpha(opacity=20);opacity:.2}.close:focus,.close:h");
	appendFile(SPIFFS, "/bootstrap.min.css", "over{color:#000;text-decoration:none;cursor:pointer;filter:alpha(opacity=50);opacity:.5}button.close{-webkit-appearance:none;padding:0;cursor:pointer;background:0 0;border:0}.modal-open{overflow:hidden}.modal{position:fixed;top:0;right:0;bottom:0;left:0;z-index:1050;display:none;overflow:hidden;-web");
	appendFile(SPIFFS, "/bootstrap.min.css", "kit-overflow-scrolling:touch;outline:0}.modal.fade .modal-dialog{-webkit-transition:-webkit-transform .3s ease-out;-o-transition:-o-transform .3s ease-out;transition:transform .3s ease-out;-webkit-transform:translate(0,-25%);-ms-transform:translate(0,-25%);-o-transform:translate(0,-25%);transform:tr");
	appendFile(SPIFFS, "/bootstrap.min.css", "anslate(0,-25%)}.modal.in .modal-dialog{-webkit-transform:translate(0,0);-ms-transform:translate(0,0);-o-transform:translate(0,0);transform:translate(0,0)}.modal-open .modal{overflow-x:hidden;overflow-y:auto}.modal-dialog{position:relative;width:auto;margin:10px}.modal-content{position:relative;back");
	appendFile(SPIFFS, "/bootstrap.min.css", "ground-color:#fff;-webkit-background-clip:padding-box;background-clip:padding-box;border:1px solid #999;border:1px solid rgba(0,0,0,.2);border-radius:6px;outline:0;-webkit-box-shadow:0 3px 9px rgba(0,0,0,.5);box-shadow:0 3px 9px rgba(0,0,0,.5)}.modal-backdrop{position:fixed;top:0;right:0;bottom:0;le");
	appendFile(SPIFFS, "/bootstrap.min.css", "ft:0;z-index:1040;background-color:#000}.modal-backdrop.fade{filter:alpha(opacity=0);opacity:0}.modal-backdrop.in{filter:alpha(opacity=50);opacity:.5}.modal-header{padding:15px;border-bottom:1px solid #e5e5e5}.modal-header .close{margin-top:-2px}.modal-title{margin:0;line-height:1.42857143}.modal-bo");
	appendFile(SPIFFS, "/bootstrap.min.css", "dy{position:relative;padding:15px}.modal-footer{padding:15px;text-align:right;border-top:1px solid #e5e5e5}.modal-footer .btn+.btn{margin-bottom:0;margin-left:5px}.modal-footer .btn-group .btn+.btn{margin-left:-1px}.modal-footer .btn-block+.btn-block{margin-left:0}.modal-scrollbar-measure{position:a");
	appendFile(SPIFFS, "/bootstrap.min.css", "bsolute;top:-9999px;width:50px;height:50px;overflow:scroll}@media (min-width:768px){.modal-dialog{width:600px;margin:30px auto}.modal-content{-webkit-box-shadow:0 5px 15px rgba(0,0,0,.5);box-shadow:0 5px 15px rgba(0,0,0,.5)}.modal-sm{width:300px}}@media (min-width:992px){.modal-lg{width:900px}}.tool");
	appendFile(SPIFFS, "/bootstrap.min.css", "tip{position:absolute;z-index:1070;display:block;font-family:\"Helvetica Neue\",Helvetica,Arial,sans-serif;font-size:12px;font-style:normal;font-weight:400;line-height:1.42857143;text-align:left;text-align:start;text-decoration:none;text-shadow:none;text-transform:none;letter-spacing:normal;word-break");
	appendFile(SPIFFS, "/bootstrap.min.css", ":normal;word-spacing:normal;word-wrap:normal;white-space:normal;filter:alpha(opacity=0);opacity:0;line-break:auto}.tooltip.in{filter:alpha(opacity=90);opacity:.9}.tooltip.top{padding:5px 0;margin-top:-3px}.tooltip.right{padding:0 5px;margin-left:3px}.tooltip.bottom{padding:5px 0;margin-top:3px}.tool");
	appendFile(SPIFFS, "/bootstrap.min.css", "tip.left{padding:0 5px;margin-left:-3px}.tooltip-inner{max-width:200px;padding:3px 8px;color:#fff;text-align:center;background-color:#000;border-radius:4px}.tooltip-arrow{position:absolute;width:0;height:0;border-color:transparent;border-style:solid}.tooltip.top .tooltip-arrow{bottom:0;left:50%;marg");
	appendFile(SPIFFS, "/bootstrap.min.css", "in-left:-5px;border-width:5px 5px 0;border-top-color:#000}.tooltip.top-left .tooltip-arrow{right:5px;bottom:0;margin-bottom:-5px;border-width:5px 5px 0;border-top-color:#000}.tooltip.top-right .tooltip-arrow{bottom:0;left:5px;margin-bottom:-5px;border-width:5px 5px 0;border-top-color:#000}.tooltip.r");
	appendFile(SPIFFS, "/bootstrap.min.css", "ight .tooltip-arrow{top:50%;left:0;margin-top:-5px;border-width:5px 5px 5px 0;border-right-color:#000}.tooltip.left .tooltip-arrow{top:50%;right:0;margin-top:-5px;border-width:5px 0 5px 5px;border-left-color:#000}.tooltip.bottom .tooltip-arrow{top:0;left:50%;margin-left:-5px;border-width:0 5px 5px;b");
	appendFile(SPIFFS, "/bootstrap.min.css", "order-bottom-color:#000}.tooltip.bottom-left .tooltip-arrow{top:0;right:5px;margin-top:-5px;border-width:0 5px 5px;border-bottom-color:#000}.tooltip.bottom-right .tooltip-arrow{top:0;left:5px;margin-top:-5px;border-width:0 5px 5px;border-bottom-color:#000}.popover{position:absolute;top:0;left:0;z-in");
	appendFile(SPIFFS, "/bootstrap.min.css", "dex:1060;display:none;max-width:276px;padding:1px;font-family:\"Helvetica Neue\",Helvetica,Arial,sans-serif;font-size:14px;font-style:normal;font-weight:400;line-height:1.42857143;text-align:left;text-align:start;text-decoration:none;text-shadow:none;text-transform:none;letter-spacing:normal;word-brea");
	appendFile(SPIFFS, "/bootstrap.min.css", "k:normal;word-spacing:normal;word-wrap:normal;white-space:normal;background-color:#fff;-webkit-background-clip:padding-box;background-clip:padding-box;border:1px solid #ccc;border:1px solid rgba(0,0,0,.2);border-radius:6px;-webkit-box-shadow:0 5px 10px rgba(0,0,0,.2);box-shadow:0 5px 10px rgba(0,0,0");
	appendFile(SPIFFS, "/bootstrap.min.css", ",.2);line-break:auto}.popover.top{margin-top:-10px}.popover.right{margin-left:10px}.popover.bottom{margin-top:10px}.popover.left{margin-left:-10px}.popover-title{padding:8px 14px;margin:0;font-size:14px;background-color:#f7f7f7;border-bottom:1px solid #ebebeb;border-radius:5px 5px 0 0}.popover-conte");
	appendFile(SPIFFS, "/bootstrap.min.css", "nt{padding:9px 14px}.popover>.arrow,.popover>.arrow:after{position:absolute;display:block;width:0;height:0;border-color:transparent;border-style:solid}.popover>.arrow{border-width:11px}.popover>.arrow:after{content:\"\";border-width:10px}.popover.top>.arrow{bottom:-11px;left:50%;margin-left:-11px;bord");
	appendFile(SPIFFS, "/bootstrap.min.css", "er-top-color:#999;border-top-color:rgba(0,0,0,.25);border-bottom-width:0}.popover.top>.arrow:after{bottom:1px;margin-left:-10px;content:\" \";border-top-color:#fff;border-bottom-width:0}.popover.right>.arrow{top:50%;left:-11px;margin-top:-11px;border-right-color:#999;border-right-color:rgba(0,0,0,.25)");
	appendFile(SPIFFS, "/bootstrap.min.css", ";border-left-width:0}.popover.right>.arrow:after{bottom:-10px;left:1px;content:\" \";border-right-color:#fff;border-left-width:0}.popover.bottom>.arrow{top:-11px;left:50%;margin-left:-11px;border-top-width:0;border-bottom-color:#999;border-bottom-color:rgba(0,0,0,.25)}.popover.bottom>.arrow:after{top:");
	appendFile(SPIFFS, "/bootstrap.min.css", "1px;margin-left:-10px;content:\" \";border-top-width:0;border-bottom-color:#fff}.popover.left>.arrow{top:50%;right:-11px;margin-top:-11px;border-right-width:0;border-left-color:#999;border-left-color:rgba(0,0,0,.25)}.popover.left>.arrow:after{right:1px;bottom:-10px;content:\" \";border-right-width:0;bor");
	appendFile(SPIFFS, "/bootstrap.min.css", "der-left-color:#fff}.carousel{position:relative}.carousel-inner{position:relative;width:100%;overflow:hidden}.carousel-inner>.item{position:relative;display:none;-webkit-transition:.6s ease-in-out left;-o-transition:.6s ease-in-out left;transition:.6s ease-in-out left}.carousel-inner>.item>a>img,.ca");
	appendFile(SPIFFS, "/bootstrap.min.css", "rousel-inner>.item>img{line-height:1}@media all and (transform-3d),(-webkit-transform-3d){.carousel-inner>.item{-webkit-transition:-webkit-transform .6s ease-in-out;-o-transition:-o-transform .6s ease-in-out;transition:transform .6s ease-in-out;-webkit-backface-visibility:hidden;backface-visibility:");
	appendFile(SPIFFS, "/bootstrap.min.css", "hidden;-webkit-perspective:1000px;perspective:1000px}.carousel-inner>.item.active.right,.carousel-inner>.item.next{left:0;-webkit-transform:translate3d(100%,0,0);transform:translate3d(100%,0,0)}.carousel-inner>.item.active.left,.carousel-inner>.item.prev{left:0;-webkit-transform:translate3d(-100%,0,");
	appendFile(SPIFFS, "/bootstrap.min.css", "0);transform:translate3d(-100%,0,0)}.carousel-inner>.item.active,.carousel-inner>.item.next.left,.carousel-inner>.item.prev.right{left:0;-webkit-transform:translate3d(0,0,0);transform:translate3d(0,0,0)}}.carousel-inner>.active,.carousel-inner>.next,.carousel-inner>.prev{display:block}.carousel-inne");
	appendFile(SPIFFS, "/bootstrap.min.css", "r>.active{left:0}.carousel-inner>.next,.carousel-inner>.prev{position:absolute;top:0;width:100%}.carousel-inner>.next{left:100%}.carousel-inner>.prev{left:-100%}.carousel-inner>.next.left,.carousel-inner>.prev.right{left:0}.carousel-inner>.active.left{left:-100%}.carousel-inner>.active.right{left:10");
	appendFile(SPIFFS, "/bootstrap.min.css", "0%}.carousel-control{position:absolute;top:0;bottom:0;left:0;width:15%;font-size:20px;color:#fff;text-align:center;text-shadow:0 1px 2px rgba(0,0,0,.6);background-color:rgba(0,0,0,0);filter:alpha(opacity=50);opacity:.5}.carousel-control.left{background-image:-webkit-linear-gradient(left,rgba(0,0,0,.");
	appendFile(SPIFFS, "/bootstrap.min.css", "5) 0,rgba(0,0,0,.0001) 100%);background-image:-o-linear-gradient(left,rgba(0,0,0,.5) 0,rgba(0,0,0,.0001) 100%);background-image:-webkit-gradient(linear,left top,right top,from(rgba(0,0,0,.5)),to(rgba(0,0,0,.0001)));background-image:linear-gradient(to right,rgba(0,0,0,.5) 0,rgba(0,0,0,.0001) 100%);fi");
	appendFile(SPIFFS, "/bootstrap.min.css", "lter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#80000000', endColorstr='#00000000', GradientType=1);background-repeat:repeat-x}.carousel-control.right{right:0;left:auto;background-image:-webkit-linear-gradient(left,rgba(0,0,0,.0001) 0,rgba(0,0,0,.5) 100%);background-image:-o-linear-g");
	appendFile(SPIFFS, "/bootstrap.min.css", "radient(left,rgba(0,0,0,.0001) 0,rgba(0,0,0,.5) 100%);background-image:-webkit-gradient(linear,left top,right top,from(rgba(0,0,0,.0001)),to(rgba(0,0,0,.5)));background-image:linear-gradient(to right,rgba(0,0,0,.0001) 0,rgba(0,0,0,.5) 100%);filter:progid:DXImageTransform.Microsoft.gradient(startColo");
	appendFile(SPIFFS, "/bootstrap.min.css", "rstr='#00000000', endColorstr='#80000000', GradientType=1);background-repeat:repeat-x}.carousel-control:focus,.carousel-control:hover{color:#fff;text-decoration:none;filter:alpha(opacity=90);outline:0;opacity:.9}.carousel-control .glyphicon-chevron-left,.carousel-control .glyphicon-chevron-right,.ca");
	appendFile(SPIFFS, "/bootstrap.min.css", "rousel-control .icon-next,.carousel-control .icon-prev{position:absolute;top:50%;z-index:5;display:inline-block;margin-top:-10px}.carousel-control .glyphicon-chevron-left,.carousel-control .icon-prev{left:50%;margin-left:-10px}.carousel-control .glyphicon-chevron-right,.carousel-control .icon-next{r");
	appendFile(SPIFFS, "/bootstrap.min.css", "ight:50%;margin-right:-10px}.carousel-control .icon-next,.carousel-control .icon-prev{width:20px;height:20px;font-family:serif;line-height:1}.carousel-control .icon-prev:before{content:'\\2039'}.carousel-control .icon-next:before{content:'\\203a'}.carousel-indicators{position:absolute;bottom:10px;left");
	appendFile(SPIFFS, "/bootstrap.min.css", ":50%;z-index:15;width:60%;padding-left:0;margin-left:-30%;text-align:center;list-style:none}.carousel-indicators li{display:inline-block;width:10px;height:10px;margin:1px;text-indent:-999px;cursor:pointer;background-color:#000\\9;background-color:rgba(0,0,0,0);border:1px solid #fff;border-radius:10px");
	appendFile(SPIFFS, "/bootstrap.min.css", "}.carousel-indicators .active{width:12px;height:12px;margin:0;background-color:#fff}.carousel-caption{position:absolute;right:15%;bottom:20px;left:15%;z-index:10;padding-top:20px;padding-bottom:20px;color:#fff;text-align:center;text-shadow:0 1px 2px rgba(0,0,0,.6)}.carousel-caption .btn{text-shadow:");
	appendFile(SPIFFS, "/bootstrap.min.css", "none}@media screen and (min-width:768px){.carousel-control .glyphicon-chevron-left,.carousel-control .glyphicon-chevron-right,.carousel-control .icon-next,.carousel-control .icon-prev{width:30px;height:30px;margin-top:-10px;font-size:30px}.carousel-control .glyphicon-chevron-left,.carousel-control .");
	appendFile(SPIFFS, "/bootstrap.min.css", "icon-prev{margin-left:-10px}.carousel-control .glyphicon-chevron-right,.carousel-control .icon-next{margin-right:-10px}.carousel-caption{right:20%;left:20%;padding-bottom:30px}.carousel-indicators{bottom:20px}}.btn-group-vertical>.btn-group:after,.btn-group-vertical>.btn-group:before,.btn-toolbar:af");
	appendFile(SPIFFS, "/bootstrap.min.css", "ter,.btn-toolbar:before,.clearfix:after,.clearfix:before,.container-fluid:after,.container-fluid:before,.container:after,.container:before,.dl-horizontal dd:after,.dl-horizontal dd:before,.form-horizontal .form-group:after,.form-horizontal .form-group:before,.modal-footer:after,.modal-footer:before,");
	appendFile(SPIFFS, "/bootstrap.min.css", ".modal-header:after,.modal-header:before,.nav:after,.nav:before,.navbar-collapse:after,.navbar-collapse:before,.navbar-header:after,.navbar-header:before,.navbar:after,.navbar:before,.pager:after,.pager:before,.panel-body:after,.panel-body:before,.row:after,.row:before{display:table;content:\" \"}.btn");
	appendFile(SPIFFS, "/bootstrap.min.css", "-group-vertical>.btn-group:after,.btn-toolbar:after,.clearfix:after,.container-fluid:after,.container:after,.dl-horizontal dd:after,.form-horizontal .form-group:after,.modal-footer:after,.modal-header:after,.nav:after,.navbar-collapse:after,.navbar-header:after,.navbar:after,.pager:after,.panel-body");
	appendFile(SPIFFS, "/bootstrap.min.css", ":after,.row:after{clear:both}.center-block{display:block;margin-right:auto;margin-left:auto}.pull-right{float:right!important}.pull-left{float:left!important}.hide{display:none!important}.show{display:block!important}.invisible{visibility:hidden}.text-hide{font:0/0 a;color:transparent;text-shadow:no");
	appendFile(SPIFFS, "/bootstrap.min.css", "ne;background-color:transparent;border:0}.hidden{display:none!important}.affix{position:fixed}@-ms-viewport{width:device-width}.visible-lg,.visible-md,.visible-sm,.visible-xs{display:none!important}.visible-lg-block,.visible-lg-inline,.visible-lg-inline-block,.visible-md-block,.visible-md-inline,.vi");
	appendFile(SPIFFS, "/bootstrap.min.css", "sible-md-inline-block,.visible-sm-block,.visible-sm-inline,.visible-sm-inline-block,.visible-xs-block,.visible-xs-inline,.visible-xs-inline-block{display:none!important}@media (max-width:767px){.visible-xs{display:block!important}table.visible-xs{display:table!important}tr.visible-xs{display:table-r");
	appendFile(SPIFFS, "/bootstrap.min.css", "ow!important}td.visible-xs,th.visible-xs{display:table-cell!important}}@media (max-width:767px){.visible-xs-block{display:block!important}}@media (max-width:767px){.visible-xs-inline{display:inline!important}}@media (max-width:767px){.visible-xs-inline-block{display:inline-block!important}}@media (m");
	appendFile(SPIFFS, "/bootstrap.min.css", "in-width:768px) and (max-width:991px){.visible-sm{display:block!important}table.visible-sm{display:table!important}tr.visible-sm{display:table-row!important}td.visible-sm,th.visible-sm{display:table-cell!important}}@media (min-width:768px) and (max-width:991px){.visible-sm-block{display:block!import");
	appendFile(SPIFFS, "/bootstrap.min.css", "ant}}@media (min-width:768px) and (max-width:991px){.visible-sm-inline{display:inline!important}}@media (min-width:768px) and (max-width:991px){.visible-sm-inline-block{display:inline-block!important}}@media (min-width:992px) and (max-width:1199px){.visible-md{display:block!important}table.visible-m");
	appendFile(SPIFFS, "/bootstrap.min.css", "d{display:table!important}tr.visible-md{display:table-row!important}td.visible-md,th.visible-md{display:table-cell!important}}@media (min-width:992px) and (max-width:1199px){.visible-md-block{display:block!important}}@media (min-width:992px) and (max-width:1199px){.visible-md-inline{display:inline!i");
	appendFile(SPIFFS, "/bootstrap.min.css", "mportant}}@media (min-width:992px) and (max-width:1199px){.visible-md-inline-block{display:inline-block!important}}@media (min-width:1200px){.visible-lg{display:block!important}table.visible-lg{display:table!important}tr.visible-lg{display:table-row!important}td.visible-lg,th.visible-lg{display:tabl");
	appendFile(SPIFFS, "/bootstrap.min.css", "e-cell!important}}@media (min-width:1200px){.visible-lg-block{display:block!important}}@media (min-width:1200px){.visible-lg-inline{display:inline!important}}@media (min-width:1200px){.visible-lg-inline-block{display:inline-block!important}}@media (max-width:767px){.hidden-xs{display:none!important}");
	appendFile(SPIFFS, "/bootstrap.min.css", "}@media (min-width:768px) and (max-width:991px){.hidden-sm{display:none!important}}@media (min-width:992px) and (max-width:1199px){.hidden-md{display:none!important}}@media (min-width:1200px){.hidden-lg{display:none!important}}.visible-print{display:none!important}@media print{.visible-print{display");
	appendFile(SPIFFS, "/bootstrap.min.css", ":block!important}table.visible-print{display:table!important}tr.visible-print{display:table-row!important}td.visible-print,th.visible-print{display:table-cell!important}}.visible-print-block{display:none!important}@media print{.visible-print-block{display:block!important}}.visible-print-inline{displ");
	appendFile(SPIFFS, "/bootstrap.min.css", "ay:none!important}@media print{.visible-print-inline{display:inline!important}}.visible-print-inline-block{display:none!important}@media print{.visible-print-inline-block{display:inline-block!important}}@media print{.hidden-print{display:none!important}}\n/*# sourceMappingURL=bootstrap.min.css.map */");
	appendFile(SPIFFS, "/bootstrap.min.css", "\n");

	File lFile = SPIFFS.open("/bootstrap.min.css");
	Serial.println("Bootstrap.size() = " + String(lFile.size()));
	lFile.close();
	Serial.println("END");
}
String getURL(String pString)
{
	// Position von 'GET /' suchen
	int addr_start = pString.indexOf("GET /");
	int lOffset = 4;
	if (addr_start < 4)
	{
		addr_start = pString.indexOf("POST /");
		lOffset = 5;
	}
	// Position von ' HTTP' suchen
	int addr_end = pString.indexOf(" HTTP", addr_start + 1);

	// Text dazwischen zur�ckgeben
	if (addr_start > -1 || addr_end != -1)
	{
		return pString.substring(addr_start + lOffset, addr_end);
	}
	return "";
}
int getUserAgentIndex(String pUserIP)
{
	for (int i = 0; i < User_Order_List.size(); i++)
	{
		if (User_Order_List[i].mUserIP == pUserIP)
		{
			return i;
		}
	}
	UserIpOrder NewUserIpOrder;
	NewUserIpOrder.mUserIP = pUserIP;
	User_Order_List.push_back(NewUserIpOrder);
	return User_Order_List.size() - 1;
}

void send404(WiFiClient c)
{
	c.println("HTTP/1.1 404 Not Found");
	c.println("Content-type: text/html");
	c.println();
	c.print("This page was not found");
	c.println();
	c.stop();
}
String getHeader() 
{
	String header = "";

	header += "<!DOCTYPE html><html>\n\r";
	header += "<head>\n\r";
	header += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	header += "<title>MixHit - ESP32 - Webserver</title>\n\r";
	header += "<link rel=\"stylesheet\" href=\"/bootstrap.min.css\">";
	header += "<style>";
	
	// own styles
	header += ".spacer-bottom{margin-bottom:10px}\n";
	header += ".spacer-left{margin-left:20px}";
	header += ".spacer-left2{margin-left:74px}";
	header += ".mybadge,.mytag{background-color:#6495ED;color:#fff;display:inline-block;padding-left:8px;padding-right:8px;text-align:center}.mybadge{border-radius:10%}";
	header += ".margin-right{margin-right:15px!important}";
	header += ".myright{float:right!important}";


	header += "</style>\n\r";
	header += "</head>\n\r";

	return header;
}
void Print_StartPage(WiFiClient pWifiClient)
{
	String lUsedConfigFile = "";
	readFile(SPIFFS, "/Config_Select.txt", &lUsedConfigFile);
	String lUsedWiFiConfigFile = "";
	readFile(SPIFFS, "/Config_Select.wtxt", &lUsedWiFiConfigFile);

	HTML_StartPage = "HTTP/1.1 200 OK\n\r";
	HTML_StartPage += "Content-type: text/html\n\r\n\r";
	HTML_StartPage += getHeader();
	HTML_StartPage += "<body>";

	HTML_StartPage += "<div class=container>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-12 text-center'>";
	HTML_StartPage += "   <h1>Herzlich Willkommen zur MixHit-Cocktailmaschine!</h1>\n\r";
	HTML_StartPage += "   <h1>Hochschule Karlsruhe - Technik und Wirtschaft</h1>\n\r";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-12 text-center'>";
	HTML_StartPage += "	  <h2>Cocktailkarte</h2>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12 spacer-bottom'>";
	HTML_StartPage += "	  <a href='/Cocktailkarte_Alkoholfrei' class='btn btn-info btn-block'><"+fontSizeLabels+">Alkoholfrei</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12'>";
	HTML_StartPage += "	  <a href='/Cocktailkarte_Alkoholisch' class='btn btn-info btn-block'><" + fontSizeLabels + ">Alkoholisch</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-12 text-center'>";
	HTML_StartPage += "	  <h2>Informationen</h2>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12 spacer-bottom'>";
	HTML_StartPage += "	  <a href='/Informationen_MixHit' class='btn btn-info btn-block'><" + fontSizeLabels + ">MixHit</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12'>";
	HTML_StartPage += "	  <a href='/Informationen_User' class='btn btn-info btn-block'><" + fontSizeLabels + ">Meine Bestellungen</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-12 text-center'>";
	HTML_StartPage += "	  <h2>Einstellungen</h2>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12 spacer-bottom'>";
	HTML_StartPage += "	  <a href='/Einstellungen_MixHit' class='btn btn-info btn-block'><" + fontSizeLabels + ">MixHit</" + fontSizeLabels + "></a>";
	HTML_StartPage += "	  <span class=\"label label-primary\">" + lUsedConfigFile + "</span>";
	HTML_StartPage += " </div>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12'>";
	HTML_StartPage += "	  <a href='/Einstellungen_WiFi' class='btn btn-info btn-block'><" + fontSizeLabels + ">Wifi</" + fontSizeLabels + "></a>";
	HTML_StartPage += "	  <span class=\"label label-primary\">" + lUsedWiFiConfigFile + "</span>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-12 text-center'>";
	HTML_StartPage += "	  <h2>User-Status</h2>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "<div class=row>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12 spacer-bottom'>";
	HTML_StartPage += "	  <a href='/EnableVipStatus' class='btn btn-info btn-block'><" + fontSizeLabels + ">VipUser</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += " <div class='col-lg-6 col-xs-12'>";
	HTML_StartPage += "	  <a href='/EnableAdminStatus' class='btn btn-info btn-block'><" + fontSizeLabels + ">Admin</" + fontSizeLabels + "></a>";
	HTML_StartPage += " </div>";
	HTML_StartPage += "</div>";

	HTML_StartPage += "</div></body></html>";
	pWifiClient.println(HTML_StartPage);
	pWifiClient.stop();
}
void Print_VIP_Page_AuthOK(WiFiClient pWifiClient)
{
	HTML_VIPAuth += "HTTP/1.1 200 OK\n\r";
	HTML_VIPAuth += "Content-type: text/html\n\r\n\r";
	HTML_VIPAuth += getHeader();
	HTML_VIPAuth += "<body>";

	HTML_VIPAuth += "<div class=container>";
	
	HTML_VIPAuth += "<div class=row>";
	HTML_VIPAuth += " <div class='col-lg-12 text-center'>";
	HTML_VIPAuth += "   <h1>VIP-Status freigeschaltet</h1>\n\r";
	HTML_VIPAuth += " </div>";
	HTML_VIPAuth += "</div>";

	HTML_VIPAuth += "<div class=row>";
	HTML_VIPAuth += "<div class=\"col-md-12\">";
	HTML_VIPAuth += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
	HTML_VIPAuth += "</div>";
	HTML_VIPAuth += "</div>";

	HTML_VIPAuth += "</div></body></html>";
	pWifiClient.println(HTML_VIPAuth);
	pWifiClient.stop();
}
void Print_VIP_Page_Auth(WiFiClient pWifiClient)
{
	pWifiClient.println("HTTP/1.1 401 Unauthorized");
	pWifiClient.println("WWW-Authenticate: Basic realm=\"EnableVipStatus\", charset=\"UTF-8\"");
	pWifiClient.stop();
}
void Print_ADMIN_Page_AuthOK(WiFiClient pWifiClient)
{
	HTML_ADMINAuth += "HTTP/1.1 200 OK\n\r";
	HTML_ADMINAuth += "Content-type: text/html\n\r\n\r";
	HTML_ADMINAuth += getHeader();
	HTML_ADMINAuth += "<body>";

	HTML_ADMINAuth += "<div class=container>";
	
	HTML_ADMINAuth += "<div class=row>";
	HTML_ADMINAuth += " <div class='col-lg-12 text-center'>";
	HTML_ADMINAuth += "   <h1>Admin-Status freigeschaltet</h1>\n\r";
	HTML_ADMINAuth += " </div>";
	HTML_ADMINAuth += "</div>";

	HTML_ADMINAuth += "<div class=row>";
	HTML_ADMINAuth += "<div class=\"col-md-12\">";
	HTML_ADMINAuth += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
	HTML_ADMINAuth += "</div>";
	HTML_ADMINAuth += "</div>";

	HTML_ADMINAuth += "</div></body></html>";
	pWifiClient.println(HTML_ADMINAuth);
	pWifiClient.stop();
}
void Print_ADMIN_Page_Auth(WiFiClient pWifiClient)
{
	pWifiClient.println("HTTP/1.1 401 Unauthorized");
	pWifiClient.println("WWW-Authenticate: Basic realm=\"EnableAdminStatus\", charset=\"UTF-8\"");
	pWifiClient.stop();
}
void Print_NonAlcoholicCocktailsPage(WiFiClient pWifiClient)
{
	HTML_NonAlcoholicList = "HTTP/1.1 200 OK\n\r";
	HTML_NonAlcoholicList += "Content-type: text/html\n\r\n\r";

	HTML_NonAlcoholicList += getHeader();

	HTML_NonAlcoholicList += "<body>";

	HTML_NonAlcoholicList += "<div class=container>";

	HTML_NonAlcoholicList += "<div class=row>";
	HTML_NonAlcoholicList += " <div class='col-lg-12 text-center'>";
	HTML_NonAlcoholicList += "   <h1>Cocktailkarte</h1>\n\r";
	HTML_NonAlcoholicList += " </div>";
	HTML_NonAlcoholicList += "</div>";

	HTML_NonAlcoholicList += "<div class=row>";
	HTML_NonAlcoholicList += " <div class='col-lg-12 text-center'>";
	HTML_NonAlcoholicList += "   <h2>Alkoholfrei</h2>\n\r";
	HTML_NonAlcoholicList += " </div>";
	HTML_NonAlcoholicList += "</div>";

	HTML_NonAlcoholicList += "<div class=row>";
	HTML_NonAlcoholicList += "<table class=\"table\">";
	HTML_NonAlcoholicList += "<thead>";
	HTML_NonAlcoholicList += "<tr>";
	HTML_NonAlcoholicList += "<th scope=\"colgroup\" colspan=\"2\" style=\"text-align:center;\">Cocktail</th>";
	HTML_NonAlcoholicList += "<th scope=\"colgroup\" colspan=\"2\" style=\"text-align:center;\">Gr&ouml&szlige</th>";
	HTML_NonAlcoholicList += "</tr>";
	HTML_NonAlcoholicList += "</thead>";
	HTML_NonAlcoholicList += "<tbody>";

	for (int i = 0; i < gNonAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		
		HTML_NonAlcoholicList += "<tr>";
		HTML_NonAlcoholicList += "<th scope=\"row\" style=\"vertical-align:middle\"><" + fontSizeText + ">" + String(i + 1) + "</font></th>";
		HTML_NonAlcoholicList += "<td><strong><" + fontSizeText + ">" + gNonAlcoholicList.getCocktail(i).getCocktailName() + "</font></strong></br>";
		String lIngredients = "";
		for (int j = 0; j < gNonAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1; j++)
		{
			lIngredients += (gNonAlcoholicList.getCocktail(i).getIngredient(j).getName() + "(" + String((int)(gNonAlcoholicList.getCocktail(i).getIngredient(j).getRelAmount() * gCocktailMixer.mGlasses.getBigAmount())) + "ml), ");
		}
		lIngredients += gNonAlcoholicList.getCocktail(i).getIngredient(gNonAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1).getName() + "(" + String((int)(gNonAlcoholicList.getCocktail(i).getIngredient(gNonAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1).getRelAmount() * gCocktailMixer.mGlasses.getBigAmount())) + "ml)";
		HTML_NonAlcoholicList += "<em><" + fontSizeText + ">" + lIngredients + "</font></em></td>";
		HTML_NonAlcoholicList += "<td><a href='/Bestellung_Alkoholfrei_Klein/" + String(i) + "'class='btn btn-primary btn-block'><" + fontSizeText + ">" + String(gCocktailMixer.mGlasses.getSmallAmount()) + "ml</font></a></td>";
		HTML_NonAlcoholicList += "<td><a href='/Bestellung_Alkoholfrei_Gross/" + String(i) + "'class='btn btn-primary btn-block'><" + fontSizeText + ">" + String(gCocktailMixer.mGlasses.getBigAmount()) + "ml</font></a></td>";
		HTML_NonAlcoholicList += "</tr>";
		
	}

	HTML_NonAlcoholicList += "</tbody>";
	HTML_NonAlcoholicList += "</table>";
	HTML_NonAlcoholicList += "</div>";
	HTML_NonAlcoholicList += "</div></body></html>";
	pWifiClient.print(HTML_NonAlcoholicList);
	pWifiClient.stop();
}
void Print_AlcoholicCocktailsPage(WiFiClient pWifiClient)
{

	HTML_AlcoholicList = "HTTP/1.1 200 OK\n\r";
	HTML_AlcoholicList += "Content-type: text/html\n\r\n\r";

	HTML_AlcoholicList += getHeader();

	HTML_AlcoholicList += "<body>";

	HTML_AlcoholicList += "<div class=container>";

	HTML_AlcoholicList += "<div class=row>";
	HTML_AlcoholicList += " <div class='col-lg-12 text-center'>";
	HTML_AlcoholicList += "   <h1>Cocktailkarte</h1>\n\r";
	HTML_AlcoholicList += " </div>";
	HTML_AlcoholicList += "</div>";

	HTML_AlcoholicList += "<div class=row>";
	HTML_AlcoholicList += " <div class='col-lg-12 text-center'>";
	HTML_AlcoholicList += "   <h2>Alkoholisch</h2>\n\r";
	HTML_AlcoholicList += " </div>";
	HTML_AlcoholicList += "</div>";

	HTML_AlcoholicList += "<div class=row>";
	HTML_AlcoholicList += "<table class=\"table\">";
	HTML_AlcoholicList += "<thead>";
	HTML_AlcoholicList += "<tr>";
	HTML_AlcoholicList += "<th scope=\"colgroup\" colspan=\"2\" style=\"text-align:center;\">Cocktail</th>";
	HTML_AlcoholicList += "<th scope=\"colgroup\" colspan=\"2\" style=\"text-align:center;\">Gr&ouml&szlige</th>";
	HTML_AlcoholicList += "</tr>";
	HTML_AlcoholicList += "</thead>";
	HTML_AlcoholicList += "<tbody>";

	for (int i = 0; i < gAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		HTML_AlcoholicList += "<tr>";
		HTML_AlcoholicList += "<th scope=\"row\" style=\"vertical-align:middle\"><" + fontSizeText + ">" + String(i + 1) + "</font></th>";
		HTML_AlcoholicList += "<td><strong><" + fontSizeText + ">" + gAlcoholicList.getCocktail(i).getCocktailName() + "</font></strong></br>";
		String lIngredients = "";
		for (int j = 0; j < gAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1; j++)
		{
			lIngredients += (gAlcoholicList.getCocktail(i).getIngredient(j).getName() + "(" + String((int)(gAlcoholicList.getCocktail(i).getIngredient(j).getRelAmount() * gCocktailMixer.mGlasses.getBigAmount())) + "ml), ");
		}
		lIngredients += gAlcoholicList.getCocktail(i).getIngredient(gAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1).getName() + "(" + String((int)(gAlcoholicList.getCocktail(i).getIngredient(gAlcoholicList.getCocktail(i).getNumberOfIngredients() - 1).getRelAmount() * gCocktailMixer.mGlasses.getBigAmount())) + "ml)";
		HTML_AlcoholicList += "<em><" + fontSizeText + ">" + lIngredients + "</font></em></td>";
		HTML_AlcoholicList += "<td><a href='/Bestellung_Alkoholisch_Klein/" + String(i) + "' class='btn btn-primary btn-block'><" + fontSizeText + ">" + String(gCocktailMixer.mGlasses.getSmallAmount()) + "ml</font></a></td>";
		HTML_AlcoholicList += "<td><a href='/Bestellung_Alkoholisch_Gross/" + String(i) + "' class='btn btn-primary btn-block'><" + fontSizeText + ">" + String(gCocktailMixer.mGlasses.getBigAmount()) + "ml</font></a></td>";
		HTML_AlcoholicList += "</tr>";
	}

	HTML_AlcoholicList += "</tbody>";
	HTML_AlcoholicList += "</table>";
	HTML_AlcoholicList += "</div>";
	HTML_AlcoholicList += "</div></body></html>";
	pWifiClient.print(HTML_AlcoholicList);
	pWifiClient.stop();

}
void Print_Order(WiFiClient pWiFiClient, String pUserIP, String pURL, int pListSelect, int pSize, int pPrio)
{
	String cocktailId = String(pURL);
	cocktailId.remove(0, 30);
	int id = cocktailId.toInt();
	cOrder lOrder;
	if (pListSelect == 0) // Alkoholfrei
	{
		lOrder = cOrder(gNonAlcoholicList.getCocktail(id), pSize, pPrio);
	}
	else
	{
		lOrder = cOrder(gAlcoholicList.getCocktail(id), pSize, pPrio);
	}
	String lSize = pSize == ORDER_SMALL ? "klein" : "gross";
	int lOrderNumber = gCocktailMixer.addOrderToQueue(lOrder);
	Serial.println("Bestellnummer: " + String(lOrderNumber));
	if (lOrderNumber >= 0)
	{
		for (int i = 0; i < User_Order_List.size(); i++)
		{
			if (User_Order_List[i].mUserIP == pUserIP)
			{
				OrderInfo lOrderInfo;
				lOrderInfo.mOrderNumber = lOrderNumber;
				lOrderInfo.mCocltailName = lOrder.getCocktail().getCocktailName() + " " + lSize;
				User_Order_List[i].Bestellnummer_Cocktailname.push_back(lOrderInfo);
			}
		}
	}
	HTML_OrderOK = "HTTP/1.1 200 OK\n\r";
	HTML_OrderOK += "Content-type: text/html\n\r\n\r";
	HTML_OrderOK += getHeader();

	HTML_OrderOK += "<body>";

	HTML_OrderOK += "<div class=container>";

	HTML_OrderOK += "<div class=row>";
	HTML_OrderOK += " <div class='col-lg-12 text-center'>";
	if (lOrderNumber >= 0)
	{
		HTML_OrderOK += "   <h2>Bestellung erfolgreich</h2>\n\r";
		HTML_OrderOK += " </div>";
		HTML_OrderOK += "</div>";
		HTML_OrderOK += "<div class=row>";
		HTML_OrderOK += "<div class=\"col-md-12\">";
		HTML_OrderOK += "<ul class=\"list-group\">";
		HTML_OrderOK += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong><" + fontSizeText + ">";
		HTML_OrderOK += "  Bestellinformationen";
		HTML_OrderOK += "</font></strong></li>";
		HTML_OrderOK += "<li class=\"list-group-item justify-content-between\"><" + fontSizeText + ">";
		HTML_OrderOK += "  Bestellnummer:";
		HTML_OrderOK += " <span class=\"mybadge myright margin-right\">" + String(lOrderNumber) + "</span>";
		HTML_OrderOK += "</font></li>";
		HTML_OrderOK += "<li class=\"list-group-item justify-content-between\"><" + fontSizeText + ">";
		HTML_OrderOK += "  Cocktailname:";
		HTML_OrderOK += "  <span class=\"mybadge myright margin-right\">" + lOrder.getCocktail().getCocktailName() + " - " + lSize + "</span>";
		HTML_OrderOK += "</font></li>";
		HTML_OrderOK += "</ul>";
		HTML_OrderOK += "</div>";
		HTML_OrderOK += "</div>";
	}
	else
	{
		HTML_OrderOK += "   <h2>Fehler - Warteschlange voll!</h2>\n\r";
		HTML_OrderOK += " </div>";
		HTML_OrderOK += "</div>";
		HTML_OrderOK += "<div class=row>";
		HTML_OrderOK += "<div class=\"col-md-12\">";
	}
	
	HTML_OrderOK += "<div class=row>";
	HTML_OrderOK += "<div class=\"col-md-12\">";
	HTML_OrderOK += "<a href='/' class='btn btn-primary btn-block'><" + fontSizeText + ">Zur&uumlck zur Startseite</font></a>";
	HTML_OrderOK += "</div>";
	HTML_OrderOK += "</div>";

	HTML_OrderOK += "</div>";
	HTML_OrderOK += "</div>";
	HTML_OrderOK += "</div></body></html>";

	pWiFiClient.println(HTML_OrderOK);
	pWiFiClient.stop();
}

void Print_InformationMixHit(WiFiClient pWiFiClient, int pPrio)
{
	HTML_InformationMixHit = "HTTP/1.1 200 OK\n\r";
	HTML_InformationMixHit += "Content-type: text/html\n\r\n\r";
	HTML_InformationMixHit += getHeader();

	HTML_InformationMixHit += "<body>";

	HTML_InformationMixHit += "<div class=container>";

	HTML_InformationMixHit += "<div class=row>";
	HTML_InformationMixHit += " <div class='col-lg-12 text-center'>";
	HTML_InformationMixHit += "   <h2>Informationen MixHit</h2>\n\r";
	HTML_InformationMixHit += " </div>";
	HTML_InformationMixHit += "</div>";

	HTML_InformationMixHit += "<div class=row>";
	HTML_InformationMixHit += "<div class=\"col-md-12\">";
	HTML_InformationMixHit += "<ul class=\"list-group\">";
	HTML_InformationMixHit += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
	HTML_InformationMixHit += "  Bestellinformationen";
	HTML_InformationMixHit += "</strong></li>";

	HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">";
	HTML_InformationMixHit += "  Angemeldete User";
	HTML_InformationMixHit += " <span class=\"mybadge myright margin-right\">" + String(User_Order_List.size()) + "</span>";
	HTML_InformationMixHit += "</li>";

	HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">";
	HTML_InformationMixHit += "  Warteschlange";
	HTML_InformationMixHit += " <span class=\"mybadge myright margin-right\">" + String(gCocktailMixer.getNumberOfOrdersInQueue(pPrio)) + "</span>";
	HTML_InformationMixHit += "</li>";
	HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">";
	HTML_InformationMixHit += "  Aktuelle Bestellnummer";
	HTML_InformationMixHit += "  <span class=\"mybadge myright margin-right\">" + String(gCocktailMixer.getCurrentOrderNumber()) + "</span>";
	HTML_InformationMixHit += "</li>";
	HTML_InformationMixHit += "</ul>";
	HTML_InformationMixHit += "</div>";
	HTML_InformationMixHit += "</div>";

	HTML_InformationMixHit += "<div class=\"row\">";
	HTML_InformationMixHit += "<div class=\"col-md-12\">";
	HTML_InformationMixHit += "<ul class=\"list-group\">";
	HTML_InformationMixHit += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
	HTML_InformationMixHit += "  Drehteller";
	HTML_InformationMixHit += "</strong></li>";

	for (int i = 0; i < NumberOfSlotsRotateTable; i++)
	{
		if (gCocktailMixer.getSlotBestellnummer(i + 1) != 0)
		{
			HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">" + String(i + 1) + ": " + gCocktailMixer.getSlotBestellnummer(i + 1) + " - " + gCocktailMixer.getSlotCocktailName(i + 1) + "</li>";
		}
		else
		{
			HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">" + String(i + 1) + ":</li>";
		}
	}
	HTML_InformationMixHit += "</ul>";
	HTML_InformationMixHit += "</div>";
	HTML_InformationMixHit += "</div>";


	HTML_InformationMixHit += "<div class=\"row\">";
	HTML_InformationMixHit += "<div class=\"col-md-12\">";
	HTML_InformationMixHit += "<ul class=\"list-group\">";
	HTML_InformationMixHit += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
	HTML_InformationMixHit += "  Verbrauch";
	HTML_InformationMixHit += "</strong></li>";

	for (int i = 0; i < gCocktailMixer.mReservoir.getNumberOfReservoir(); i++)
	{
		String lName = gCocktailMixer.mReservoir.getReservoirName(i);
		String lSum = String(gCocktailMixer.mReservoir.getSum(i));
		HTML_InformationMixHit += "<li class=\"list-group-item justify-content-between\">" + lName + "<span class=\"mybadge myright margin-right\">" + lSum + "L</span></li>";
	}

	HTML_InformationMixHit += "</ul>";
	HTML_InformationMixHit += "</div>";
	HTML_InformationMixHit += "</div>";

	HTML_InformationMixHit += "<div class=row>";
	HTML_InformationMixHit += "<div class=\"col-md-12\">";
	HTML_InformationMixHit += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
	HTML_InformationMixHit += "</div>";
	HTML_InformationMixHit += "</div>";
	
	HTML_InformationMixHit += "</div></body></html>";
	pWiFiClient.println(HTML_InformationMixHit);
	pWiFiClient.stop();
}
void Print_InformationUser(WiFiClient pWiFiClient, int pUserIndex)
{
	int lPrio = 0;
	if (User_Order_List[pUserIndex].mAdmin_Status)
	{
		lPrio = 2;
	}
	else if (User_Order_List[pUserIndex].mVIP_Status)
	{
		lPrio = 1;
	}
	HTML_InformationUser = "HTTP/1.1 200 OK\n\r";
	HTML_InformationUser += "Content-type: text/html\n\r\n\r";
	HTML_InformationUser += getHeader();

	HTML_InformationUser += "<body>";

	HTML_InformationUser += "<div class=container>";
	
	HTML_InformationUser += "<div class=row>";
	HTML_InformationUser += " <div class='col-lg-12 text-center'>";
	HTML_InformationUser += "   <h2>Informationen Anwender</h2>\n\r";
	HTML_InformationUser += " </div>";
	HTML_InformationUser += "</div>";

	HTML_InformationUser += "<div class=row>";
	HTML_InformationUser += "<div class=\"col-md-12\">";
	HTML_InformationUser += "<ul class=\"list-group\">";
	HTML_InformationUser += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
	HTML_InformationUser += "  Bestellinformationen MixHit";
	HTML_InformationUser += "</strong></li>";
	HTML_InformationUser += "<li class=\"list-group-item justify-content-between\">";
	HTML_InformationUser += "  Warteschlange";
	HTML_InformationUser += "  <span class=\"mybadge myright margin-right\">" + String(gCocktailMixer.getNumberOfOrdersInQueue(lPrio)) + "</span>";
	HTML_InformationUser += "</li>";
	HTML_InformationUser += "<li class=\"list-group-item justify-content-between\">";
	HTML_InformationUser += "  Aktuelle Bestellnummer";
	HTML_InformationUser += "  <span class=\"mybadge myright margin-right\">" + String(gCocktailMixer.getCurrentOrderNumber()) + "</span>";
	HTML_InformationUser += "</li>";
	HTML_InformationUser += "</ul>";
	HTML_InformationUser += "</div>";
	HTML_InformationUser += "</div>";
	

	HTML_InformationUser += "<div class=row>";
	HTML_InformationUser += "<div class=\"col-md-12\">";
	HTML_InformationUser += "<ul class=\"list-group\">";
	HTML_InformationUser += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
	HTML_InformationUser += "  Meine Bestellungen";
	HTML_InformationUser += "</strong></li>";

	for (int i = User_Order_List[pUserIndex].Bestellnummer_Cocktailname.size() - 1; i >= 0; i--)
	{
		int OrdersBefore = gCocktailMixer.getNumberOfOrdersBeforeOrderNumber(User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mOrderNumber); // Anzahl an BEstellungen ermitteln, welche vor der angegebenen Bestellnummer bedient werden.
		Serial.println("Index: " + String(i) + " OrdersBefore: " + String(OrdersBefore));
		if (OrdersBefore >= 0) 
		{	
			HTML_InformationUser += "<li class=\"list-group-item justify-content-between\">" + String(User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mOrderNumber) + " - Position Warteschlange : " + String(OrdersBefore) + " - " + User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mCocltailName + "</li>";
		}
		else
		{
			int lSlotNumber = gCocktailMixer.getSlotnummer(User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mOrderNumber);
			if (lSlotNumber >= 1)
			{
				HTML_InformationUser += "<li class=\"list-group-item justify-content-between\">" + String(User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mOrderNumber) + " - Fertig - Slot: " + String(lSlotNumber) + " - " + User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mCocltailName + "</li>";
			}
			else
			{
				HTML_InformationUser += "<li class=\"list-group-item justify-content-between\">" + String(User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mOrderNumber) + " - " + User_Order_List[pUserIndex].Bestellnummer_Cocktailname[i].mCocltailName + "</li>";
			}
		}
	}
	HTML_InformationUser += "</ul>";
	HTML_InformationUser += "</div>";
	HTML_InformationUser += "</div>";

	HTML_InformationUser += "<div class=row>";
	HTML_InformationUser += "<div class=\"col-md-12\">";
	HTML_InformationUser += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
	HTML_InformationUser += "</div>";
	HTML_InformationUser += "</div>";

	HTML_InformationUser += "</div></body></html>";
	pWiFiClient.println(HTML_InformationUser);
	pWiFiClient.stop();
}

void Print_MixHitConfig(WiFiClient pWiFiClient, bool pAdmin)
{
	int lMachintStage = getMachineState();
	if (pAdmin && (lMachintStage >= MachineState_Einstellungen && lMachintStage < MachineState_Betriebsmodus))
	{
		String lFiles = "";
		listDir(SPIFFS, "/", 0, &lFiles);
		vector<String> lFileNames;
		split(&lFiles, ';', &lFileNames);
	
		String lUsedConfigFile = "";
		readFile(SPIFFS, "/Config_Select.txt", &lUsedConfigFile);
		Serial.println("Config: " + lUsedConfigFile);
		lUsedConfigFile.replace("/", "");
		lUsedConfigFile.replace(".txt", "");
	
		
	
		HTML_MixHitConfig = "HTTP/1.1 200 OK\n\r";
		HTML_MixHitConfig += "Content-type: text/html\n\r\n\r";
		HTML_MixHitConfig += getHeader();
	
		HTML_MixHitConfig += "<body>";
		
		HTML_MixHitConfig += "<div class=container>";
		
		HTML_MixHitConfig += "<div class=row>";
		HTML_MixHitConfig += " <div class='col-lg-12 text-center'>";
		HTML_MixHitConfig += "   <h2>MixHit-Konfigurationen</h2>\n\r";
		HTML_MixHitConfig += " </div>";
		HTML_MixHitConfig += "</div>";
	
		HTML_MixHitConfig += "<div class=row>";
		HTML_MixHitConfig += "<div class=\"col-md-12\">";
		HTML_MixHitConfig += "<ul class=\"list-group\">";
		HTML_MixHitConfig += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
		HTML_MixHitConfig += "Aktuelle Konfiguration: <span class=\"label label-primary\">" + lUsedConfigFile + "</span>";
		HTML_MixHitConfig += "</strong></li>";
		HTML_MixHitConfig += "</ul>";
		HTML_MixHitConfig += "</div>";
		HTML_MixHitConfig += "</div>";
	
		HTML_MixHitConfig += "<div class=row>";
		HTML_MixHitConfig += "<table class=\"table\">";
		HTML_MixHitConfig += "<thead>";
		HTML_MixHitConfig += "<tr>";
		HTML_MixHitConfig += "<th scope=\"col\" style=\"text-align:center;\">Dateiname</th>";
		HTML_MixHitConfig += "</tr>";
		HTML_MixHitConfig += "</thead>";
		HTML_MixHitConfig += "<tbody>";
	
		for (int i = 0; i < lFileNames.size(); i++)
		{
			if (lFileNames[i].indexOf(".txt") > 0)
			{
				if (lFileNames[i] != "/Config_Select.txt")
				{
					lFileNames[i].replace(".txt", "");
					lFileNames[i].replace("/", "");
					HTML_MixHitConfig += "<tr>";
					HTML_MixHitConfig += "<td><strong>" + lFileNames[i] + "</strong></td>";
					HTML_MixHitConfig += "<td><a href='/Einstellungen_MixHit/bearbeiten/" + lFileNames[i] + "' class='btn btn-warning btn-block'>bearbeiten</a></td>";
					HTML_MixHitConfig += "<td><a href='/Einstellungen_MixHit/anwenden/" + lFileNames[i] + "' class='btn btn-success btn-block'>anwenden</a></td>";
					HTML_MixHitConfig += "<td><a href='/Einstellungen_MixHit/entfernen/" + lFileNames[i] + "' class='btn btn-danger btn-block'>entfernen</a></td>";
					HTML_MixHitConfig += "</tr>";
				}
			}
		}
	
	
		HTML_MixHitConfig += "</tbody>";
		HTML_MixHitConfig += "</table>";
		HTML_MixHitConfig += "</div>";
		HTML_MixHitConfig += "</div></body></html>";
		pWiFiClient.println(HTML_MixHitConfig);
		pWiFiClient.stop();

	}
	else
	{
		HTML_MixHitConfig = "HTTP/1.1 200 OK\n\r";
		HTML_MixHitConfig += "Content-type: text/html\n\r\n\r";
		HTML_MixHitConfig += getHeader();
	
		HTML_MixHitConfig += "<body>";
	
		HTML_MixHitConfig += "<div class=container>";
		
		HTML_MixHitConfig += "<div class=row>";
		HTML_MixHitConfig += " <div class='col-lg-12 text-center'>";
		HTML_MixHitConfig += "   <h2>Adminrechte und Men&uumlpunkt Einstellungen notwendig!</h2>\n\r";
		HTML_MixHitConfig += " </div>";
		HTML_MixHitConfig += "</div>";

		HTML_MixHitConfig += "<div class=row>";
		HTML_MixHitConfig += "<div class=\"col-md-12\">";
		HTML_MixHitConfig += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
		HTML_MixHitConfig += "</div>";
		HTML_MixHitConfig += "</div>";

		HTML_MixHitConfig += "</div></body></html>";
		pWiFiClient.println(HTML_MixHitConfig);
		pWiFiClient.stop();

	}

}
void Print_WiFiConfig(WiFiClient pWiFiClient, bool pAdmin)
{
	int lMachintStage = getMachineState();
	if (pAdmin && (lMachintStage >= MachineState_Einstellungen && lMachintStage < MachineState_Betriebsmodus))
	{
		String lFiles = "";
		listDir(SPIFFS, "/", 0, &lFiles);
	
		String lUsedWiFiConfigFile = "";
		readFile(SPIFFS, "/Config_Select.wtxt", &lUsedWiFiConfigFile);
		Serial.println("WiFi_Config: " + lUsedWiFiConfigFile);
		lUsedWiFiConfigFile.replace("/", "");
		lUsedWiFiConfigFile.replace(".wtxt", "");
	
		vector<String> lFileNames;
		split(&lFiles, ';', &lFileNames);
	
		HTML_WiFiConfig = "HTTP/1.1 200 OK\n\r";
		HTML_WiFiConfig += "Content-type: text/html\n\r\n\r";
		HTML_WiFiConfig += getHeader();
	
		HTML_WiFiConfig += "<body>";
	
		HTML_WiFiConfig += "<div class=container>";
		
		HTML_WiFiConfig += "<div class=row>";
		HTML_WiFiConfig += " <div class='col-lg-12 text-center'>";
		HTML_WiFiConfig += "   <h2>Wifi-Konfigurationen</h2>\n\r";
		HTML_WiFiConfig += " </div>";
		HTML_WiFiConfig += "</div>";
	
		HTML_WiFiConfig += "<div class=row>";
		HTML_WiFiConfig += "<div class=\"col-md-12\">";
		HTML_WiFiConfig += "<ul class=\"list-group\">";
		HTML_WiFiConfig += "<li class=\"list-group-item list-group-item-info justify-content-between\"><strong>";
		HTML_WiFiConfig += "Aktuelle Konfiguration: <span class=\"label label-primary\">" + lUsedWiFiConfigFile + "</span>";
		HTML_WiFiConfig += "</strong></li>";
		HTML_WiFiConfig += "</ul>";
		HTML_WiFiConfig += "</div>";
		HTML_WiFiConfig += "</div>";
	
		HTML_WiFiConfig += "<div class=row>";
		HTML_WiFiConfig += "<table class=\"table\">";
		HTML_WiFiConfig += "<thead>";
		HTML_WiFiConfig += "<tr>";
		HTML_WiFiConfig += "<th scope=\"col\" style=\"text-align:center;\">Dateiname</th>";
		HTML_WiFiConfig += "</tr>";
		HTML_WiFiConfig += "</thead>";
		HTML_WiFiConfig += "<tbody>";
	
		for (int i = 0; i < lFileNames.size(); i++)
		{
			if (lFileNames[i].indexOf(".wtxt") > 0)
			{
				if (lFileNames[i] != "/Config_Select.wtxt")
				{
					lFileNames[i].replace(".wtxt", "");
					lFileNames[i].replace("/", "");
					HTML_WiFiConfig += "<tr>";
					HTML_WiFiConfig += "<td><strong>" + lFileNames[i] + "</strong></td>";
					HTML_WiFiConfig += "<td><a href='/Einstellungen_WiFi/bearbeiten/" + lFileNames[i] + "' class='btn btn-warning btn-block'>bearbeiten</a></td>";
					HTML_WiFiConfig += "<td><a href='/Einstellungen_WiFi/anwenden/" + lFileNames[i] + "' class='btn btn-success btn-block'>anwenden</a></td>";
					HTML_WiFiConfig += "<td><a href='/Einstellungen_WiFi/entfernen/" + lFileNames[i] + "' class='btn btn-danger btn-block'>entfernen</a></td>";
					HTML_WiFiConfig += "</tr>";
				}
			}
		}
	
	
		HTML_WiFiConfig += "</tbody>";
		HTML_WiFiConfig += "</table>";
		HTML_WiFiConfig += "</div>";
		HTML_WiFiConfig += "</div></body></html>";
		pWiFiClient.println(HTML_WiFiConfig);
		pWiFiClient.stop();
	}
	else
	{
		HTML_WiFiConfig = "HTTP/1.1 200 OK\n\r";
		HTML_WiFiConfig += "Content-type: text/html\n\r\n\r";
		HTML_WiFiConfig += getHeader();
	
		HTML_WiFiConfig += "<body>";
	
		HTML_WiFiConfig += "<div class=container>";
		
		HTML_WiFiConfig += "<div class=row>";
		HTML_WiFiConfig += " <div class='col-lg-12 text-center'>";
		HTML_WiFiConfig += "   <h2>Adminrechte und Men&uumlpunkt Einstellungen notwendig!</h2>\n\r";
		HTML_WiFiConfig += " </div>";
		HTML_WiFiConfig += "</div>";

		HTML_WiFiConfig += "<div class=row>";
		HTML_WiFiConfig += "<div class=\"col-md-12\">";
		HTML_WiFiConfig += "<a href='/' class='btn btn-primary btn-block'>Zur&uumlck zur Startseite</a>";
		HTML_WiFiConfig += "</div>";
		HTML_WiFiConfig += "</div>";

		HTML_WiFiConfig += "</div></body></html>";
		pWiFiClient.println(HTML_WiFiConfig);
		pWiFiClient.stop();

	}
}
void Print_ConfigChange(WiFiClient pWiFiClient, String pLoadFileName, bool pAdmin)
{
	int lMachintStage = getMachineState();
	if (pAdmin && (lMachintStage >= MachineState_Einstellungen && lMachintStage < MachineState_Betriebsmodus))
	{
		String lLoadFileName = String("/" + pLoadFileName + ".txt");
		String lFileContent = "";
		readFile(SPIFFS, lLoadFileName, &lFileContent);
		lFileContent.replace("\r", "");

		HTML_MixHitConfigChange = "HTTP/1.1 200 OK\n\r";
		HTML_MixHitConfigChange += "Content-type: text/html\n\r\n\r";
		HTML_MixHitConfigChange += getHeader();
		HTML_MixHitConfigChange += "<body>\n\r";

		HTML_MixHitConfigChange += "<h1>Konfiguration</h1>\n\r";
		HTML_MixHitConfigChange += "<table>\n\r";
		HTML_MixHitConfigChange += "<tr>\n\r";
		HTML_MixHitConfigChange += "<td style = \"width: 75px;\">Dateiname:</td>\n\r";
		HTML_MixHitConfigChange += "<td style = \"width: 350px;\">\n\r";
		HTML_MixHitConfigChange += "<textarea id=\"ta_FileName\" name=\"ta_NameFileName\" style = \"width: 350px; height: 20px;\">" + pLoadFileName + "</textarea></td>\n\r";
		HTML_MixHitConfigChange += "<td style = \"width: 75px\"><input id = \"Submit1\" type = \"submit\" value = \"Speichern\"/ onclick = \"SendText()\"></td>\n\r";
		HTML_MixHitConfigChange += "</tr>\n\r";
		HTML_MixHitConfigChange += "</table>\n\r";
		HTML_MixHitConfigChange += "<textarea id = \"ta_FileContent\" name = \"ta_NameFileContent\" style = \"width: 1000px; height: 2000px; line-height: 14px;\">";
		HTML_MixHitConfigChange += lFileContent;
		HTML_MixHitConfigChange += "</textarea>\n\r";

		HTML_MixHitConfigChange += "<script>\n\r";
		HTML_MixHitConfigChange += "strText = \"\";\n\r";
		HTML_MixHitConfigChange += "function SendText()\n\r";
		HTML_MixHitConfigChange += "{\n\r";
		HTML_MixHitConfigChange += "nocache = \"&nocache = \" + Math.random() * 1000000;\n\r";
		HTML_MixHitConfigChange += "var request = new XMLHttpRequest();\n\r";
		HTML_MixHitConfigChange += "strText = \"&name=\" + document.getElementById(\"ta_FileName\").value +  \"&endname_&content=\" + document.getElementById(\"ta_FileContent\").value + \"&endcontent=end\";\n\r";
		HTML_MixHitConfigChange += "strText = nl2br_js(strText);\n\r";
		HTML_MixHitConfigChange += "request.open(\"POST\", \"/Einstellungen_MixHit/speichern/\" + strText + nocache, true);\n\r";
		HTML_MixHitConfigChange += "request.send(null);\n\r";
		HTML_MixHitConfigChange += "}\n\r";

		HTML_MixHitConfigChange += "function nl2br_js(myString)\n\r";
		HTML_MixHitConfigChange += "{\n\r";
		HTML_MixHitConfigChange += "s = new String(myString);\n\r";
		HTML_MixHitConfigChange += "s = s.replace(/\\n/g, \"<br/>\");\n\r";
		HTML_MixHitConfigChange += "return s;\n\r";
		HTML_MixHitConfigChange += "}\n\r";
		HTML_MixHitConfigChange += "</script>\n\r";

		HTML_MixHitConfigChange += "</body>\n\r";
		HTML_MixHitConfigChange += "</html>";
		pWiFiClient.print(HTML_MixHitConfigChange);
		pWiFiClient.stop();
	}
	else
	{
		HTML_MixHitConfigChange = "HTTP/1.1 200 OK\n\r";
		HTML_MixHitConfigChange += "Content-type: text/html\n\r\n\r";
		HTML_MixHitConfigChange += "<!DOCTYPE html><html>\n\r";
		HTML_MixHitConfigChange += "<head>\n\r";
		HTML_MixHitConfigChange += "<title>MixHit - ESP32 - Webserver</title>\n\r";
		HTML_MixHitConfigChange += "<style>input.MyButton {width: 150px;padding: 10px;cursor: pointer;font-weight: bold;font-size: 100%;background: #3366cc;color: #fff;border: 1px solid #3366cc;border-radius: 10px;}</style>\n\r";
		HTML_MixHitConfigChange += "</head>\n\r";
		HTML_MixHitConfigChange += "<body>\n\r";
		HTML_MixHitConfigChange += "<h2>Adminrechte und Men&uumlpunkt Einstellungen notwendig!</h2>\n\r";
		HTML_MixHitConfigChange += "<pre></br></pre>\n\r";
		HTML_MixHitConfigChange += "</body></html>";
		pWiFiClient.println(HTML_MixHitConfigChange);
		pWiFiClient.stop();
	}
}
void Print_WiFiConfigChange(WiFiClient pWiFiClient, String pLoadFileName, bool pAdmin)
{
	int lMachintStage = getMachineState();
	if (pAdmin && (lMachintStage >= MachineState_Einstellungen && lMachintStage < MachineState_Betriebsmodus))
	{
		String lLoadFileName = String("/" + pLoadFileName + ".wtxt");
		String lFileContent = "";
		readFile(SPIFFS, lLoadFileName, &lFileContent);
		lFileContent.replace("\r", "");

		HTML_WiFiConfigChange = "HTTP/1.1 200 OK\n\r";
		HTML_WiFiConfigChange += "Content-type: text/html\n\r\n\r";
		HTML_WiFiConfigChange += getHeader();
		HTML_WiFiConfigChange += "<body>\n\r";

		HTML_WiFiConfigChange += "<h1>Konfiguration</h1>\n\r";
		HTML_WiFiConfigChange += "<table style = \"width: 550px;\">\n\r";
		HTML_WiFiConfigChange += "<tr>\n\r";
		HTML_WiFiConfigChange += "<td style = \"width: 75px;\">Dateiname:</td>\n\r";
		HTML_WiFiConfigChange += "<td style = \"width: 350px;\">\n\r";
		HTML_WiFiConfigChange += "<textarea id=\"ta_FileName\" name=\"ta_NameFileName\" style = \"width: 350px; height: 20px;\">" + pLoadFileName + "</textarea></td>\n\r";
		HTML_WiFiConfigChange += "<td style = \"width: 75px\"><input id = \"Submit1\" type = \"submit\" value = \"Speichern\"/ onclick = \"SendText()\"></td>\n\r";
		HTML_WiFiConfigChange += "</tr>\n\r";
		HTML_WiFiConfigChange += "</table>\n\r";
		HTML_WiFiConfigChange += "<textarea id = \"ta_FileContent\" name = \"ta_NameFileContent\" style = \"width: 1000px; height: 2000px; line-height: 14px;\">";
		HTML_WiFiConfigChange += lFileContent;
		HTML_WiFiConfigChange += "</textarea>\n\r";

		HTML_WiFiConfigChange += "<script>\n\r";
		HTML_WiFiConfigChange += "strText = \"\";\n\r";
		HTML_WiFiConfigChange += "function SendText()\n\r";
		HTML_WiFiConfigChange += "{\n\r";
		HTML_WiFiConfigChange += "nocache = \"&nocache = \" + Math.random() * 1000000;\n\r";
		HTML_WiFiConfigChange += "var request = new XMLHttpRequest();\n\r";
		HTML_WiFiConfigChange += "strText = \"&name=\" + document.getElementById(\"ta_FileName\").value +  \"&endname_&content=\" + document.getElementById(\"ta_FileContent\").value + \"&endcontent=end\";\n\r";
		HTML_WiFiConfigChange += "strText = nl2br_js(strText);\n\r";
		HTML_WiFiConfigChange += "request.open(\"POST\", \"/Einstellungen_WiFi/speichern/\" + strText + nocache, true);\n\r";
		HTML_WiFiConfigChange += "request.send(null);\n\r";
		HTML_WiFiConfigChange += "}\n\r";

		HTML_WiFiConfigChange += "function nl2br_js(myString)\n\r";
		HTML_WiFiConfigChange += "{\n\r";
		HTML_WiFiConfigChange += "s = new String(myString);\n\r";
		HTML_WiFiConfigChange += "s = s.replace(/\\n/g, \"<br/>\");\n\r";
		HTML_WiFiConfigChange += "return s;\n\r";
		HTML_WiFiConfigChange += "}\n\r";
		HTML_WiFiConfigChange += "</script>\n\r";

		HTML_WiFiConfigChange += "</body>\n\r";
		HTML_WiFiConfigChange += "</html>\n\r";
		pWiFiClient.print(HTML_WiFiConfigChange);
		pWiFiClient.stop();
	}
	else
	{
		HTML_WiFiConfigChange = "HTTP/1.1 200 OK\n\r";
		HTML_WiFiConfigChange += "Content-type: text/html\n\r\n\r";
		HTML_WiFiConfigChange += "<!DOCTYPE html><html>\n\r";
		HTML_WiFiConfigChange += "<head>\n\r";
		HTML_WiFiConfigChange += "<title>MixHit - ESP32 - Webserver</title>\n\r";
		HTML_WiFiConfigChange += "<style>input.MyButton {width: 150px;padding: 10px;cursor: pointer;font-weight: bold;font-size: 100%;background: #3366cc;color: #fff;border: 1px solid #3366cc;border-radius: 10px;}</style>\n\r";
		HTML_WiFiConfigChange += "</head>\n\r";
		HTML_WiFiConfigChange += "<body>\n\r";
		HTML_WiFiConfigChange += "<h2>Adminrechte und Menuepunkt Einstellungen notwendig!</h2>\n\r";
		HTML_WiFiConfigChange += "<pre></br></pre>\n\r";
		HTML_WiFiConfigChange += "</body></html>";
		pWiFiClient.println(HTML_WiFiConfigChange);
		pWiFiClient.stop();
	}
}

