
// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "stdlib.h"

// Load SPI/I2S Libraries
#include <SPI.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>

// Define SD Card connections
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

// Define I2S connections
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;
File root;

//Music Info Struct
struct Music_info
{
    String name;
    int length;
    int runtime;
    int volume;
    int status;
    int mute_volume;
} music_info = {"", 0, 0, 0, 0, 0};

//Initialize File List
String file_list[400];
int file_num = 0;
int file_index = 0;
int songCounter = 0;

// Replace with your network credentials
const char *ssid = "7223";
const char *password = "AvenueDeL'Epee";
// Set static IP
IPAddress staticIP(192, 168, 1, 18);
//IPAddress gateway(192, 168, 1, 1); // Replace this with your gateway IP Addess
//IPAddress subnet(255, 255, 255, 0);  // Replace this with your Subnet Mask
//IPAddress dns(192, 168, 1, 1);     // Replace this with your DNS

// Set web server port number to 80
WebServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output2State = "off";
String soundOutput = "pause";

// Assign output variables to GPIO pins
const int output2 = 2;


// variable to Store SD status
#define SD_ON 1
#define SD_OFF 0
int SD_State = SD_ON; 


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


//========================    event handlers        ===============================
void handleListFiles() {

  /*String json;
  StaticJsonDocument<2048> doc;
  File root = SD.open("/");
  File file = root.openNextFile();
  while (file) {
    doc["files"].add(file.name());
    file = root.openNextFile();
  }
  serializeJson(doc, json);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
*/

//====================temp sending a static list of files
   Serial.println("listing files ...");
   StaticJsonDocument<2048> doc;
   String json;

    // Print the array of songs
    for (int i = 0; i < songCounter; i++) {
        doc["files"].add(file_list[i]);
    }
 
  serializeJson(doc, json);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
  
}

void handleRootRequest() {
  
 if(SD_State == SD_ON)
 {
    // read the HTML file from SD and send it
    File file = SD.open("/music player.html");
    if (!file) {
      Serial.println("Failed to open file");
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
 }
 else
 {
    // ------------ Rafik FILE -------------

  /*  
  String html = "<!DOCTYPE html>\n";
  html += "<html>\n";
  html += "<head>\n";
  html += "<title>Music Player</title>\n";
  html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n";
  html += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js\"></script>\n";
  html += "<script src=\"script.js\"></script>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<div class=\"container\">\n";
  html += "<h1>Smart Jukebox Player</h1>\n";
  html += "<div id=\"player\">\n";
  html += "<div id=\"song-info\" class=\"psong-info\">\n";
  html += "<marquee behavior=\"scroll\" direction=\"left\" scrollamount=\"5\">\n";
  html += "<div id=\"song-title\">Song Title- Song Title-Song Title</div>\n";
  html += "</marquee>\n";
  html += "<div id=\"song-artist\">Artist Name-Artist Name</div>\n";
  html += "</div>\n";
  html += "<button id=\"previous-button\" class=\"player-button\"></button>\n";
  html += "<button id=\"play-button\" class=\"player-button\"></button>\n";
  html += "<button id=\"pause-button\" class=\"player-button\"></button>\n";
  html += "<button id=\"next-button\" class=\"player-button\"></button>\n";
  html += "</div>\n";
  html += "</div>\n";
  html += "<div class=\"container\">\n";
  html += "<ul id=\"file-list\"></ul>\n";
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";

  server.send(200, "text/html", html);
  */
  
  /* 
   // --------------- ALEX FILE------------------
    String html = "<!DOCTYPE html>\n";

    // Display the HTML web page
    html += "<!DOCTYPE html><html>";
    html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    html += "<link rel=\"icon\" href=\"data:,\">";
                          // CSS to style the on/off buttons
                          // Feel free to change the background-color and font-size attributes to fit your preferences
    html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
    html += ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;";
    html += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
    html += ".button2 {background-color: #555555;}</style></head>";
  
                          // Web Page Heading
    html += "<body><h1>ESP32 Web Server and MP3 Player</h1>";
  
                          // Display current state, and ON/OFF buttons for GPIO 2
    html += "<p>GPIO 2 - State " + output2State + "</p>";
                          // If the output2State is off, it displays the ON button
    if (output2State == "off")
    {
      html += "<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>";
    }
    else
    {
      html += "<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>";
    }
    // Display current state, and Play/Pause
    html += "<p>Play / Pause " + soundOutput + "</p>";
    // If the playPause is off, it displays the PLAY button
    if (soundOutput == "pause")
    {
      html += "<p><a href=\"/sound/play\"><button class=\"button\">PLAY</button></a></p>";
    }
    else
    {
      html += "<p><a href=\"/sound/pause\"><button class=\"button button2\">PAUSE</button></a></p>";
    }
    // End of HTML File
    html += "</body></html>";
  
    // send response
    server.send(200, "text/html", html);
    */
 }
}
void handle_2_On() {
  Serial.println("GPIO 2 on");
  output2State = "on";
  digitalWrite(output2, HIGH);
  handleRootRequest();
}

void handle_2_Off() {
  Serial.println("GPIO 2 off");
  output2State = "off";
  digitalWrite(output2, LOW);
  handleRootRequest();

}

void handle_sound_play() {
  Serial.println("PLAY");
  soundOutput = "play";
  audio.pauseResume();
  handleRootRequest();
}

void handle_sound_pause() {
  Serial.println("Pause");
  soundOutput = "pause";
  audio.pauseResume();
  handleRootRequest();

}

void handle_sound_next(){
  Serial.println("Next");
  
  if (file_index < file_num - 1)
    file_index++;
  else
    file_index = 0;

  open_new_song(file_list[file_index]);
  handleRootRequest();
}

void handle_sound_back(){
  Serial.println("Back");
  if (file_index > 0)
    file_index--;
  else
    file_index = file_num - 1;

  open_new_song(file_list[file_index]);
  handleRootRequest();
}

void handle_style_css(){
 if(SD_State == SD_ON)
 {
    // Send the style.css file from the SD card
    File file = SD.open("/style.css");
    if (!file) {
      // If the file does not exist, return a 404 error
      server.send(404, "text/plain", "File not found");
      return;
    }
    
    // Send the JavaScript file to the client
    server.streamFile(file, "text/css");
    file.close();
 }
 else {
   String responce = "body {";
responce += "  background-color: #2c3e50;";
responce += " font-family: Arial, sans-serif;";
responce += "}";
responce += "\n";
responce += ".container {";
responce += " width: 400px;";
responce += " margin: 0 auto;";
responce += " padding: 20px;";
responce += " background-color: #34495e;";
responce += " color: #fff;";
responce += " border-radius: 10px;";
responce += " box-shadow: 0 0 10px rgba(0,0,0,0.5);";
responce += " text-align: center;";
responce += "}";
responce += "\n";
responce += "h1 {";
responce += " font-size: 36px;";
responce += " margin-bottom: 20px;";
responce += "}";
responce += "\n";
responce += "#player {";
responce += " width: 100%;";
responce += " height: 50px;";
responce += " margin: 20px 0;";
responce += " background-color: #2c3e50;";
responce += " border-radius: 5px;";
responce += " display: flex;";
responce += " align-items: center;";
responce += " justify-content: space-between;";
responce += " padding: 0 10px;";
responce += "}";
responce += ".song-info {";
responce += " margin-top: 20px;";
responce += " text-align: center;";
responce += " overflow: hidden;";
responce += " white-space: nowrap;";
responce += " text-overflow: ellipsis;";
responce += " height: 20px;";
responce += "}";
responce += "\n";
responce += ".song-info p {";
responce += " display: inline-block;";
responce += " margin: 0;";
responce += " padding-right: 100%;";
responce += " animation: marquee 10s linear infinite;";
responce += "}";
responce += "\n";
responce += "@keyframes marquee {";
responce += " 0% {";
responce += "   transform: translateX(0);";
responce += " }";
responce += " 100% {";
responce += "   transform: translateX(-100%);";
responce += " }";
responce += "}";
responce += "\n";
responce += "\n";
responce += "#song-title {";
responce += " font-size: 20px;";
responce += " margin-right: 10px;";
responce += "}";
responce += "\n";
responce += ".player-button {";
responce += " background-repeat: no-repeat;";
responce += " background-position: center;";
responce += " background-size: contain;";
responce += " padding: 20px;";
responce += " border-radius: 5px;";
responce += " border: none;";
responce += " font-size: 0;";
responce += " cursor: pointer;";
responce += " transition: all 0.2s ease-in-out;";
responce += "}";
responce += "\n";
responce += "#play-button {";
responce += " background-image: url('https://cdn3.iconfinder.com/data/icons/google-material-design-icons/48/ic_play_circle_outline_48px-512.png');";
responce += " background-color: #27ae60;";
responce += "}";
responce += "\n";
responce += "#play-button:hover {";
responce += " background-color: #1e8449;";
responce += "}";
responce += "\n";
responce += "#pause-button {";
responce += " background-image: url('https://cdn3.iconfinder.com/data/icons/google-material-design-icons/48/ic_pause_circle_outline_48px-512.png');";
responce += " background-color: #c0392b;";
responce += "}";
responce += "\n";
responce += "#pause-button:hover {";
responce += " background-color: #a93226;";
responce += "}";
responce += "\n";
responce += "#previous-button {";
responce += " background-image: url('https://cdn4.iconfinder.com/data/icons/ionicons/512/icon-ios7-arrow-back-512.png');";
responce += " background-color: #f1c40f;";
responce += "}";
responce += "\n";
responce += "#previous-button:hover {";
responce += " background-color: #f39c12;";
responce += "}";
responce += "\n";
responce += "#next-button {";
responce += " background-image: url('https://cdn4.iconfinder.com/data/icons/ionicons/512/icon-ios7-arrow-forward-512.png');";
responce += " background-color: #3498db;";
responce += "}";
responce += "\n";
responce += "#next-button:hover {";
responce += " background-color: #2980b9;";
responce += "}";
responce += "\n";
server.send(200, "text/css", responce);
 }
 
}


void handle_script_js(){
  if(SD_State == SD_ON)
 {
    // Send the script.js file from the SD card
    File file = SD.open("/script.js");
    if (!file) {
      // If the file does not exist, return a 404 error
      server.send(404, "text/plain", "File not found");
      return;
    }
  
    // Send the JavaScript file to the client
    server.streamFile(file, "application/javascript");
    file.close();
 }
 else {

  String responce ="$(document).ready(function() {\n";
responce += "  var taregt_url = 'http://192.168.1.18:80/list_files';\n";
responce += "  //var taregt_url = './list_files';\n";
responce += "  $.ajax({\n";
responce += "    url: taregt_url,\n";
responce += "    type: 'GET',\n";
responce += "    dataType: 'json', \n";
responce += "    success: function(data) {\n";
responce += "      console.log(data); // Log the response\n";
responce += "      // get the JSON response and update the HTML content\n";
responce += "      var files = data.files;\n";
responce += "      var fileList = document.getElementById('file-list');\n";
responce += "      for (var i = 0; i < files.length; i++) {\n";
responce += "        var li = document.createElement('li');\n";
responce += "        li.appendChild(document.createTextNode(files[i]));\n";
responce += "        fileList.appendChild(li);\n";
responce += "      }\n";
responce += "    },\n";
responce += "    error: function(xhr, status, error) {\n";
responce += "      console.log('Error: ' + error.message);\n";
responce += "    }\n";
responce += "  });\n";
responce += "});\n";

server.send(200, "text/css", responce);
 }
 
  
}
//========================    End of event handlers  ==============================




void setup()
{
   
    // Initialize the output variables as outputs
    pinMode(output2, OUTPUT);

    // Set outputs to LOW
    digitalWrite(output2, LOW);

    // Set SD Pins

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Serial.begin(115200);

    if (!SD.begin(SD_CS)) {
      Serial.println("Error initializing SD card !!!");
      SD_State = SD_OFF;
    }
    else
    {
      SD_State = SD_ON;
  //Read SD and Create File List - get_music+list(Library to use, starting folder, number of sub folders, size of array)
    file_num = get_music_list(SD, "/", 2, file_list);
    Serial.print("Music file count:");
    Serial.println(file_num);
    Serial.println("All music:");
    for (int i = 0; i < file_num; i++)
    {
        Serial.println(file_list[i]);
    }
    }

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
  /*  // Static IP Status
    if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false)
    {
        Serial.println("Configuration failed.");
    }
    */
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // -------------------------------- define server handlers -------------------------
    server.on("/list_files", handleListFiles);
    server.on("/", handleRootRequest);
    server.on("/2/on", handle_2_On);
    server.on("/2/off", handle_2_Off);
    server.on("/sound/play", handle_sound_play);
    server.on("/sound/pause", handle_sound_pause);
    server.on("/sound/next", handle_sound_next);
    server.on("/sound/back", handle_sound_back);
    server.on("/style.css", handle_style_css);
    server.on("/script.js", handle_script_js);



    // -------------------------------end define server handlers -------------------------
    Serial.println("starting server ... ");
    server.begin();    
    Serial.println("Server is up and listening on port 80");
    
    // Set idle timeout to 30 seconds
    // server.setIdleTimeout(30);
   

    // Connect to I2S Module + Internet Radio
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10);
    // audio.connecttoFS(SD, "/Rolling Stones, The/Beggars Banquet/01 Sympathy For The Devil.mp3");

    //Random song on open, to develop into function
    file_index = 57;

    open_new_song(file_list[file_index]);
}

void loop()
{
   server.handleClient();

   audio.loop();
      
}
// ID3 Data
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}

//Print Directory - Create Filename
int get_music_list(fs::FS &fs, const char *dirname, uint8_t levels, String wavlist[2000])
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return songCounter;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return songCounter;
    }
 
    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
          if (levels == 2){
            Serial.print("Artist : ");
            }          
          if (levels == 1){
            Serial.print("Album : ");
            }
            Serial.println(file.name());

          if(levels){
                get_music_list(SD, file.path(), levels - 1, file_list);
            }
          }
          else if (!levels){
            String temp = file.path();
            String songName = file.name();
            Serial.print("Song Name : ");
            Serial.println(songName);
            if (songName.endsWith("mp3"))
            {
                wavlist[songCounter] = temp;
                songCounter++;
            }
        }
        file = root.openNextFile();
    }
    return songCounter;
}

//New Song Function
void open_new_song(String filename)
{
    music_info.name = filename.substring(0, filename.indexOf("."));
    audio.connecttoFS(SD, filename.c_str());
    music_info.runtime = audio.getAudioCurrentTime();
    music_info.length = audio.getAudioFileDuration();
    music_info.volume = audio.getVolume();
    music_info.status = 1;
    Serial.println("**********Start a new song************");
}