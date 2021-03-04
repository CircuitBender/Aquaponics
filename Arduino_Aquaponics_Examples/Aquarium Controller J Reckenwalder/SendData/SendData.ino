

 void sendData(String thisData) {
    if (client.connect(server, 80)) {
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    client.println(thisData);
    digitalWrite(SS_ETHERNET, HIGH);  

  } 
  else {
    client.stop();
    digitalWrite(SS_ETHERNET, HIGH);    } 
    
} //Send Data Loop end
