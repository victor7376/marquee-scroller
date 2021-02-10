/** The MIT License (MIT)

Copyright (c) 2021 victor7376@github

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Chaturbate.h"

#define CHATURBATE_FINGERPRINT "16 a5 01 0d a1 67 f5 f1 4f 99 9c dd 00 16 18 c5 97 9b b9 44"

Chaturbate::Chaturbate(String tokenKey, String username)
{
  myTokenKey = tokenKey;
  myUsername = username;
}

void Chaturbate::updateDetails(String tokenKey, String username){
  myTokenKey = tokenKey;
  myUsername = username;
}

void Chaturbate::getDetails(){

      WiFiClientSecure modelClient;
      // Fingerprint renews Oct 2021
      modelClient.setFingerprint(CHATURBATE_FINGERPRINT);
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println(F("Connecting to Chaturbate..."));
      if (!modelClient.connect("chaturbate.com", 443)) {
        Serial.println(F("Failed to connect to Chaturbate"));
        return;
      }
      Serial.println(F("Connected to Chaturbate"));
    
      Serial.println(F("Sending request to Chaturbate..."));
      // Send HTTP request
      modelClient.println("GET /statsapi/?username=" + myUsername + "&token=" + myTokenKey + " HTTP/1.0");
      Serial.println("GET /statsapi/?username=" + myUsername + "&token=" + myTokenKey + " HTTP/1.1");
      modelClient.println(F("Host: chaturbate.com"));
      modelClient.println(F("User-Agent: Arduino"));
      modelClient.println(F("Connection: close"));
      modelClient.println();

  
      //Check the returning code                                                                  
        char status[32] = {0};
      modelClient.readBytesUntil('\r', status, sizeof(status));
      Serial.println("Response Header: " + String(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print("Unexpected HTTP status");
        Serial.println(status);
        return;
      }

      Serial.println(F("Receive response from chaturbate..."));

      // Skip response headers
      modelClient.find("\r\n\r\n");

      // CB v5
      const size_t bufferSize = JSON_OBJECT_SIZE(12) + 238;
      
      DynamicJsonBuffer jsonBuffer(bufferSize);
      JsonObject& root = jsonBuffer.parseObject(modelClient);
      // Parameters
      if (!root.success()) {
        Serial.println(F("Chaturbate Data Parsing failed!"));
        return;
      }

      const char* username = root["username"];
      int num_followers = root["num_followers"];
      int token_balance = root["token_balance"];

      chaturbateData.numfollowers = (const char*)root["num_followers"];
      chaturbateData.tokenbalance = (const char*)root["token_balance"];

      Serial.print("Model:");
      Serial.println(username);
      Serial.print("Followers:");
      Serial.println(num_followers);
      Serial.print("Balance:"); 
      Serial.println(token_balance);
      
    }
    modelClient.stop();
}

String Chaturbate::getNumFollowers(){
  return chaturbateData.numfollowers;
}

String Chaturbate::getTokenBalance(){
  return chaturbateData.tokenbalance;
}