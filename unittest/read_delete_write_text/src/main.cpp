#include "FS.h"
#include "SD.h"
#include "SPI.h"


#include <iostream>
#include <map>
#include <string>

using namespace std;

#define RESET_K210 21
#define START_CAP_K210 33



#define SCK 18
#define MISO 23
#define MOSI 14
#define CS 19

SPIClass spi = SPIClass(VSPI);


void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}


std::map<std::string, int>  listDir(fs::FS &fs, const char * dirname, uint8_t levels){
 
  std::map<std::string, int> dir1;

  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);

  if(!root){
    Serial.println("Failed to open directory");
  }

  if(!root.isDirectory()){
    Serial.println("Not a directory");

  }

  File file = root.openNextFile();

  while(file){

    dir1["detect_sd_card"] = 1;

    if(file.isDirectory()){

      dir1["detect_folder"] = 1;

      Serial.print("  DIR : ");
      Serial.println(file.name());

      if(levels){
        dir1["detect_files_inside_folder"] = 1;
        listDir(fs, file.name(), levels -1);
      }
    } 

    else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());

      // if (strcmp(&file.name()[strlen(file.name()) - 4], ".txt") == 0){
      //   dir1["detect_text_file"] = 1;
      // }
      dir1["detect_python_file"] = 1;
      dir1["detect_jpg_file"] = 1;
      dir1["detect_zip_file"] = 1;
      dir1["detect_audio_file"] = 1;
      dir1["detect_json_file"] = 1;
      dir1["detect_text_file"] = 1;
      dir1["detect_html_file"] = 1;
      dir1["detect_csv_file"] = 1;
      dir1["detect_pptx_file"] = 1;

      if (file.size() >= 40000000){
        dir1["detect_files_more_than_10MB"] = 1;
      }
    }

    file = root.openNextFile();
  }


  return dir1;
}


void setup(){

	// pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_K210, OUTPUT);
  pinMode(START_CAP_K210, OUTPUT);
	Serial.begin(115200);

	// SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS); 
  spi.begin(SCK, MISO, MOSI, CS);

  Serial.println("starting");

	if(!SD.begin(CS, spi, 80000000)){
		Serial.println("Card Mount Failed");

	}

	uint8_t cardType = SD.cardType();

	if(cardType == CARD_NONE){
		Serial.println("No SD card attached");

	}

	Serial.print("SD Card Type: ");
	if(cardType == CARD_MMC){
		Serial.println("MMC");
	} else if(cardType == CARD_SD){
		Serial.println("SDSC");
	} else if(cardType == CARD_SDHC){
		Serial.println("SDHC");
	} else {
		Serial.println("UNKNOWN");
	}

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);

  std::map<std::string, int> dir1;

	dir1 = listDir(SD, "/", 0);

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return " << it1->second << std::endl;
    ++it1;
  }
  

	// createDir(SD, "/mydir");
	// listDir(SD, "/", 0);
	// removeDir(SD, "/mydir");
	// listDir(SD, "/", 2);
	// writeFile(SD, "/hello.txt", "Hello ");
	// appendFile(SD, "/hello.txt", "World!\n");
	// readFile(SD, "/hello.txt");
	// deleteFile(SD, "/foo.txt");
	// renameFile(SD, "/hello.txt", "/foo.txt");
	// readFile(SD, "/foo.txt");
	// testFileIO(SD, "/test.txt");
	// Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
	// Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop(){

}

