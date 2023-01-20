#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <unity.h>
#include <Arduino.h>

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

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}



void test_sdCard_size(void){

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);

  int expected = 7580;
  TEST_ASSERT_EQUAL(expected, cardSize);

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
        dir1["detect_files_more_than_40MB"] = 1;
      }
    }

    file = root.openNextFile();
  }


  return dir1;
}



void test_listDir(void)
{
  std::map<std::string, int> dir1;

	dir1 = listDir(SD, "/", 0);

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }

}


std::map<std::string, int> createDir(fs::FS &fs, const char * path){

  std::map<std::string, int> dir1;
  Serial.printf("Creating Dir: %s\n", path);

  if(fs.mkdir(path)){
    dir1["create-directory"] = 1;
    Serial.println("Dir created");
  } 
  else {
    Serial.println("mkdir failed");
    dir1["create-directory"] = 0;
  }

  return dir1;

}


void test_createDir(void)
{
  std::map<std::string, int> dir1;
	dir1 = createDir(SD, "/mydir");

  std::map<std::string, int>::iterator it1 = dir1.begin();

  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }

}


std::map<std::string, int> removeDir(fs::FS &fs, const char * path){

  std::map<std::string, int> dir1;
  Serial.printf("Removing Dir: %s\n", path);

  if(fs.rmdir(path)){
    Serial.println("Dir removed");
    dir1["remove-directory"] = 1;
  } else {
    Serial.println("rmdir failed");
    dir1["remove-directory"] = 0;
  }
  return dir1;
}

void test_removeDir(void)
{
  std::map<std::string, int> dir1;
	dir1 = removeDir(SD, "/mydir");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }
}


std::map<std::string, int> readFile(fs::FS &fs, const char * path){

  std::map<std::string, int> dir1;
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    dir1["read-file"] = 0;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
    dir1["read-file"] = 1;
  }
  file.close();
  return dir1;
}

void test_readFile(void)
{
  std::map<std::string, int> dir1;
	dir1 = readFile(SD, "/hello.txt");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(0, it1->second);
    ++it1;
    
  }
}


std::map<std::string, int> writeFile(fs::FS &fs, const char * path, const char * message){

  std::map<std::string, int> dir1;
  Serial.printf("Writing file: %s\n", path);
  dir1["detect-file"] = 1;

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    dir1["detect-file"] = 0;
  }

  if(file.print(message)){
    Serial.println("File written");
    dir1["file-write"] = 1;

  } 
  else {

    Serial.println("Write failed");
    dir1["file-write"] = 0;
  }
  file.close();
  return dir1;
}


void test_writeFile(void)
{
  std::map<std::string, int> dir1;
	dir1 = writeFile(SD, "/hello.txt", "Hello ");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }
}


std::map<std::string, int> appendFile(fs::FS &fs, const char * path, const char * message){

  std::map<std::string, int> dir1;
  Serial.printf("Appending to file: %s\n", path);
  dir1["detect-file"] = 1;

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    dir1["detect-file"] = 0;
  }

  if(file.print(message)){
      Serial.println("Message appended");
      dir1["append-file"] = 1;
  } else {
    Serial.println("Append failed");
    dir1["append-file"] = 0;
  }
  file.close();
  return dir1;
}


void test_appendFile(void)
{
  std::map<std::string, int> dir1;
	dir1 =appendFile(SD, "/hello.txt", "World!\n");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }
}



std::map<std::string, int> renameFile(fs::FS &fs, const char * path1, const char * path2){

  std::map<std::string, int> dir1;
  Serial.printf("Renaming file %s to %s\n", path1, path2);

  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
    dir1["rename-file"] = 1;
  } 
  else {
    Serial.println("Rename failed");
    dir1["rename-file"] = 0;
  }

  return dir1;
}


void test_renameFile(void)
{
  std::map<std::string, int> dir1;
	dir1 = renameFile(SD, "/hello.txt", "/foo.txt");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }
}


std::map<std::string, int> deleteFile(fs::FS &fs, const char * path){

  std::map<std::string, int> dir1;
  Serial.printf("Deleting file: %s\n", path);

  if(fs.remove(path)){
    Serial.println("File deleted");
    dir1["delete-file"] = 1;
  } 
  else {
    Serial.println("Delete failed");
    dir1["delete-file"] = 0;
  }
  return dir1;
}


void test_deleteFile(void)
{
  std::map<std::string, int> dir1;
	dir1 = deleteFile(SD, "/foo.txt");

  std::map<std::string, int>::iterator it1 = dir1.begin();
  
  while (it1 != dir1.end())
  {
    std::cout << it1->first << ", return: " << it1->second << std::endl;
    TEST_ASSERT_EQUAL(1, it1->second);
    ++it1;
    
  }
}


// std::map<std::string, int> testFileIO(fs::FS &fs, const char * path){

//   File file = fs.open(path);
//   static uint8_t buf[512];
//   size_t len = 0;
//   uint32_t start = millis();
//   uint32_t end = start;

//   if(file){
//     len = file.size();
//     size_t flen = len;
//     start = millis();

//     while(len){
//       size_t toRead = len;
//       if(toRead > 512){
//         toRead = 512;
//       }
//       file.read(buf, toRead);
//       len -= toRead;
//     }

//     end = millis() - start;
//     Serial.printf("%u bytes read for %u ms\n", flen, end);
//     file.close();
//   } 
//   else {
//     Serial.println("Failed to open file for reading");
//   }


//   file = fs.open(path, FILE_WRITE);
//   if(!file){
//     Serial.println("Failed to open file for writing");
//     return;
//   }

//   size_t i;
//   start = millis();
//   for(i=0; i<2048; i++){
//     file.write(buf, 512);
//   }

//   end = millis() - start;
//   Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
//   file.close();
// }


// void test_testFileIO(void)
// {
//   testFileIO(SD, "/test.txt");
// }


void setup(){

  pinMode(RESET_K210, OUTPUT);
  pinMode(START_CAP_K210, OUTPUT);
	Serial.begin(115200);

  spi.begin(SCK, MISO, MOSI, CS);

  Serial.println("starting");

	if(!SD.begin(CS, spi, 80000000)){
		Serial.println("Card Mount Failed");
	}

  delay(2000);

  UNITY_BEGIN();
  
  RUN_TEST(test_sdCard_size);
  RUN_TEST(test_listDir);
  RUN_TEST(test_createDir);
  RUN_TEST(test_removeDir);
  RUN_TEST(test_readFile);
  RUN_TEST(test_writeFile);
  RUN_TEST(test_appendFile);
  RUN_TEST(test_renameFile);
  RUN_TEST(test_deleteFile);

  UNITY_END();

}

void loop(){

}

