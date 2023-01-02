/*====================================================================================
  This sketch contains support functions for the ESP6266 SPIFFS filing system

  Created by Bodmer 15th Jan 2017
  Updated by Bodmer to support ESP32 with SPIFFS Jan 2018
  ==================================================================================*/

//====================================================================================
//                 Print a SPIFFS directory list (root directory)
//====================================================================================
#ifdef ESP8266
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);
  uint8_t jpeg_count=0;
  while (dir.next()) {
    String fileName = dir.fileName();
    if(jpeg_count<10-1&&fileName.endsWith(".jpg")&&fileName.substring(0,9).equals("/data/img"))
    {
        FILE_JPEG[jpeg_count]=fileName;
        jpeg_count++;
    }
    Serial.println(fileName);
    int spaces = 40 - fileName.length(); // Tabulate nicely
    while (spaces--) Serial.print(" ");
    //Serial.println(fileName.length());
    /*
    fs::File f = dir.openFile("r");
    String fileSize = (String) f.size();
    //spaces = 15 - fileSize.length(); // Tabulate nicely
    //while (spaces--) Serial.print(" ");
    //Serial.println(fileName.length());
    Serial.println(fileSize + " bytes");
    */
  }

  FILE_JPEG[jpeg_count]="end";
  Serial.println(line);
  Serial.println();
  //delay(1000);
}
#endif

//====================================================================================

#ifdef ESP32

void listFiles(void) {
  listDir(SPIFFS, "/", 0);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {

  Serial.println();
  Serial.println("SPIFFS files found:");

  Serial.printf("Listing directory: %s\n", "/");
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      int spaces = 20 - fileName.length(); // Tabulate nicely
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(line);
  Serial.println();
  delay(1000);
}
#endif
