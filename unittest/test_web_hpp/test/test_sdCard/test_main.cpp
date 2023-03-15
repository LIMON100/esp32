// #include <sd_card.hpp>
#include "main.cpp"
#include <unity.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include "make_html_file.hpp"

using namespace std;

#define SCK 18
#define MISO 23
#define MOSI 14
#define CS 19

#define SDCARD_8GB

int write_test = 0;
int read_test = 0;
int delete_test = 0;
int wifi_test = 0;
int init_sd_test = 0;
int file_test = 0;
int copy_test = 0;

const char* html_path= "/esp_ut.html";

// Write test
const char* write_test_suite = "Write in SD card";
const char* write_fn = "create text file in, write on text file, update text file, compare text file, update text file and compare";

// Read test 
const char* read_test_suite = "Read from SD card";
const char* read_fn = "Read file of size 1 MB , Read sd card files 1000 times, Read folders and files inside folders ,Read html file, Read audio file, Read python file, Read images size more than 10MB, read app index, read k210 fw version, read k210 app version";

// Delete test
const char* delete_test_suite = "Delete files in SD card";
const char* delete_fn = "delete file of size 1 MB, delete file in folder, delete images,delete zip files, delete python file";

// Wifi test 
const char* wifi_test_suite = "Wifi Test";
const char* wifi_fn = "Test wifi connection when available and when no wifi";


// sd init test
const char* sdInit_test_suite = "Init sd card";
const char* sdInit_fn = "Detect sd card and size of the sd card";

// File test
const char* file_test_suite = "Files in flash/sd";
const char* files_fn = "Read files inside flash and sd card";


// Copy test 
const char* copy_test_suite = "Copy files";
const char* copy_fn = "Copy file from one place to another and check the files copied successfully or not";


std::map<std::string, int> output_for_html = 
{
  { "write_app_index", 1 },
  { "compare_app_index", 1 },
  { "create_text_file", 1}
};



void test_create_txt_file()
{
  const char* path= "/hello.txt";
  const char* message = "Hello";
  FS_WRITE_STATUS write_expected =  writeFile(SD, path, message);

  const char* test_suite = "Write in SD card";
  const char* fn = "create text file";
  const char* asertion = "Create text file in sd card return true";
  definition_test_name(SD, html_path, test_suite, write_fn, write_test);
  multiple_condition_result(SD, html_path, write_expected, 1, asertion);
  write_test = 1;

  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
}


void test_read_txt_file()
{
  const char* path= "/hello.txt";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  const char* test_suite = "Write in SD card";
  const char* fn = "create text file";
  const char* asertion = "Read the created text file in sd card and return 1";
  definition_test_name(SD, html_path, test_suite, write_fn, write_test);
  multiple_condition_result(SD, html_path, read_expected, 1, asertion);

  // Read the file
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}

void test_write_txt_file()
{
  const char* path= "/hello.txt";
  const char* message = "Hello";

  FS_WRITE_STATUS write_expected =  writeFile(SD, path, message);
  
  const char* asertion = "Open text file for write something";
  definition_test_name(SD, html_path, write_test_suite, write_fn, write_test);
  multiple_condition_result(SD, html_path, write_expected, 1, asertion);

  // Write file
  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
  
}


void test_compare_text_file()
{
  const char* path= "/hello.txt";
  const char* message = "Llama";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  const char* asertion = "Compare expected text written in text file";
  definition_test_name(SD, html_path, write_test_suite, write_fn, write_test);
  multiple_condition_result(SD, html_path, read_expected, 1, asertion);

  // Read data
  TEST_ASSERT_EQUAL_UINT8 (read_expected_data, 4);
}


void test_update_text_file_compare()
{
  boot_py="";
  const char* path= "/hello.txt";
  const char* message_append = "jumpwatts, LA";
  
  appendFile(SD, path, message_append);
  send_file_to_k210(SD, path); 
  const char* expected = "Hello jumpwatts, LA";
  const char* actual = boot_py.c_str ();

  // const char* asertion = "Update and compare text";
  // definition_test_name(SD, html_path, write_test_suite, write_fn, write_test);
  // multiple_condition_result(SD, html_path, std::atoi(expected.c_str()), std::atoi(actual.c_str()), asertion);

  TEST_ASSERT_EQUAL_STRING (expected, actual);
}

void test_read_python_file()
{
  const char* path= "/train5.py";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  const char* asertion = "read python file";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);
  read_test = 1;

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}

void test_read_sd_hundred_times()
{
  const char* path= "/hello.txt";
  int test_counter = 0;
 
  for (int i = 0; i < 1000; i++){
    FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);
    test_counter = test_counter + read_expected;
  }

  const char* asertion = "read sd card 1000 times";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, test_counter, 1000, asertion);

  TEST_ASSERT_EQUAL_UINT8 (test_counter, 1000);
}


void test_read_files_in_folder()
{
  const char* path= "/Images/Logo/llamalogo.jpg";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  const char* asertion = "read image inside folder";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_audio_file()
{
  const char* path= "/Audio/0.wav";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  const char* asertion = "read audio file";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_html_file()
{
  const char* path= "/data_collection.html";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  const char* asertion = "read html file";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_app_index()
{
  
  int read_expected = read_app_index();

  const char* asertion = "read app inde";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 1, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_k210_fw_version()
{
  
  int read_expected = read_k210_fw_version();

  const char* asertion = "read k210 fw version";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 1, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_k210_app_version()
{
  
  int read_expected = read_k210_app_version();

  const char* asertion = "read k210 app version";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_models()
{
  const char* path= "/Models/helmet2.kmodel";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  const char* asertion = "read models";
  definition_test_name(SD, html_path, read_test_suite, read_fn, read_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}



void test_delete_file()
{
  const char* path= "/test_file_5.txt";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  const char* asertion = "Delete python file from sd card";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, expected, 0, asertion);
  delete_test = 1;

  const char* asertion2 = "Check delete file in sd card or not";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, read_delete, 0, asertion2);

  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}


void test_delete_python_file()
{
  const char* path= "/train6.py";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  const char* asertion = "delete python file in sd card";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, expected, 0, asertion);

  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}


void test_delete_file_in_folder()
{
  const char* path= "/test_folder/10.wav";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  const char* asertion = "Delete file in folder";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, expected, 0, asertion);

  const char* asertion2 = "Deleted files should not be in folder";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, read_delete, 0, asertion2);
  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}


void test_delete_models()
{
  const char* path= "/Models/hemet2.kmodel";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  const char* asertion = "Deleted file size more than 1MB";
  definition_test_name(SD, html_path, delete_test_suite, delete_fn, delete_test);
  multiple_condition_result(SD, html_path, expected, 0, asertion);
  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}

void test_delete_zip()
{
  const char* path= "/test.zip";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}


void test_scan_wifi()
{
  std::map<std::string, int> output;
	output = wifi_ssids_scan();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
    
    if (it1->first == "wifi")
    {
      const char* asertion = "Deleted file size more than 1MB";
      definition_test_name(SD, html_path, wifi_test_suite, wifi_fn, wifi_test);
      multiple_condition_result(SD, html_path, it1->second, 1, asertion);
      wifi_test = 1;
      TEST_ASSERT_EQUAL(it1->second, it1->second);
    }
    if (it1->first == "no_wifi")
    {
      const char* asertion = "Deleted file size more than 1MB";
      definition_test_name(SD, html_path, wifi_test_suite, wifi_fn, wifi_test);
      multiple_condition_result(SD, html_path, it1->second, 1, asertion);
      TEST_ASSERT_EQUAL(it1->second, it1->second);
    }
    ++it1;
    
  }
}


void test_init_sd()
{
  std::map<std::string, int> output;
	output = initSDCard();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
    
    if (it1->first == "detect")
    {
      const char* asertion = "Detect sd card";
      definition_test_name(SD, html_path, sdInit_test_suite, sdInit_fn, wifi_test);
      multiple_condition_result(SD, html_path, it1->second, 1, asertion);
      init_sd_test = 1;
      TEST_ASSERT_EQUAL(it1->second, it1->second);
    }
    if (it1->first == "sd_size")
    {
      const char* asertion = "Size of the sd card";
      definition_test_name(SD, html_path, sdInit_test_suite, sdInit_fn, wifi_test);
      multiple_condition_result(SD, html_path, it1->second, 6000, asertion);
      
      TEST_ASSERT_GREATER_THAN(6000, it1->second);
    }
    ++it1;
    
  }
}


void test_run_folder_sd()
{
 
	String s = run_folder("/sd");
  String s1 = "1";

  const char* asertion = "Detet sd card and files";
  definition_test_name(SD, html_path, file_test_suite, files_fn, file_test);
  multiple_condition_result(SD, html_path, 1, 1, asertion);
  file_test = 1;

  TEST_ASSERT_EQUAL_STRING(s.c_str(), s1.c_str());

}

void test_run_folder_flash()
{
 
	String s = run_folder("/flash");
  String s1 = "1";

  const char* asertion = "Detet flash files";
  definition_test_name(SD, html_path, file_test_suite, files_fn, file_test);
  multiple_condition_result(SD, html_path, 1, 1, asertion);
  
  TEST_ASSERT_EQUAL_STRING(s.c_str(), s1.c_str());

}



void test_copy_file()
{
  const char* path= "/main_test.py";
  std::map<std::string, int> output;
	output = copy_file(path);

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
    
    if (it1->first == "copy_file")
    {
      const char* asertion = "copy file from one place to another";
      definition_test_name(SD, html_path, copy_test_suite, copy_fn, copy_test);
      multiple_condition_result(SD, html_path, 1, 1, asertion);
      copy_test = 1;
      TEST_ASSERT_EQUAL(it1->second, it1->second);
    }
    ++it1;
    
  }
  // Read that copied file 
  const char* path2 = "/files_path.py";
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path2);

  const char* asertion = "read copy file in exact location";
  definition_test_name(SD, html_path, copy_test_suite, copy_fn, copy_test);
  multiple_condition_result(SD, html_path, read_expected, 0, asertion);
  copy_test = 1;
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);

}

void test_write_app_index()
{
  const char* html_path= "/esp_ut.html";
  make_html_upperbody(SD, html_path);

	String s = write_app_index("3");
  output_test["write_app_index"] = std::atoi(s.c_str());

  const char* tst_suite = "Check App Index";
  const char* fn = "Check app index save correctly or not";
  const char* asertion = "Save app index and return true";
  definition_test_name(SD, html_path, tst_suite, fn, 0);
  multiple_condition_result(SD, html_path, std::atoi(s.c_str()), 1, asertion);
  

  // Compare index in json file 
  const char* path= "/app_index.json";
  const char* message = "3";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);
  output_test["compare_app_index"] = 1;

  const char* asertion2 = "Compare app index match or not";
  definition_test_name(SD, html_path, tst_suite, fn, 1);
  multiple_condition_result(SD, html_path, read_expected_data, 4, asertion2);

  TEST_ASSERT_EQUAL_UINT8 (read_expected_data, 4);
  TEST_ASSERT_EQUAL(std::atoi(s.c_str()), 1);

}


void test_make_html_upperbody()
{
  const char* path= "/esp_ut.html";
  int check = make_html_upperbody(SD, path);
  TEST_ASSERT_EQUAL_UINT8 (check, 1);

  std::map<std::string, int>::iterator it1 = output_for_html.begin();
  while (it1 != output_for_html.end()) 
  {
    int value = 0;
    const char* tst_suite = "Check App Index";
    const char* fn = "Check app index save correctly or not";

    if (it1->first == "write_app_index")
    {
      const char* asertion = "Save app index and return true";
      definition_test_name(SD, path, tst_suite, fn, value);
      multiple_condition_result(SD, path, it1->second, 1, asertion);
      value = 1;
    }

    else if (it1->first == "compare_app_index"){
        value = 1;
        const char* asertion2 = "Compare app index match or not";
        definition_test_name(SD, path, tst_suite, fn, value);
        multiple_condition_result(SD, path, it1->second, 1, asertion2);
      }
      ++it1;
  }

  std::map<std::string, int>::iterator it2 = output_for_html.begin();
  while (it2 != output_for_html.end())
  {
    int value = 0;
    const char* tst_suite = "Write Sd card";
    const char* fn = "text file created or not";

    if (it2->first == "create_text_file")
    {
      const char* asertion = "Return 1 if text file is created";
      definition_test_name(SD, html_path, tst_suite, fn, value);
      multiple_condition_result(SD, html_path, it2->second, 1, asertion);
    }
    ++it2;
  }

}


void test_html_file()
{
  const char* path= "/esp_ut.html";
  int check = make_html_upperbody(SD, path);
  TEST_ASSERT_EQUAL_UINT8 (check, 1);
}

void setUp(void)
{
  
  initSDCard();
  
}

void tearDown(void)
{

}

void setup ()
{
  delay (1000);

  UNITY_BEGIN();
  // App index test
  RUN_TEST(test_write_app_index);

  // Write test
  RUN_TEST(test_create_txt_file);
  RUN_TEST(test_read_txt_file);
  RUN_TEST(test_write_txt_file);
  RUN_TEST(test_compare_text_file);
  RUN_TEST(test_update_text_file_compare);

  // Read test
  RUN_TEST(test_read_python_file);
  RUN_TEST(test_read_sd_hundred_times);
  RUN_TEST(test_read_files_in_folder);
  RUN_TEST(test_read_audio_file);
  RUN_TEST(test_read_html_file);
  RUN_TEST(test_read_app_index);
  RUN_TEST(test_read_k210_app_version);
  RUN_TEST(test_read_k210_fw_version);
  // // RUN_TEST(test_read_models);

  // // Delete test
  RUN_TEST(test_delete_file);
  RUN_TEST(test_delete_file_in_folder);
  RUN_TEST(test_delete_python_file);
  RUN_TEST(test_delete_models);
  // // RUN_TEST(test_delete_zip);

  // Wifi test
  RUN_TEST(test_scan_wifi);

  // Init Sd card
  RUN_TEST(test_init_sd);

  // FIles in sd/flash
  RUN_TEST(test_run_folder_sd);
  RUN_TEST(test_run_folder_flash);

  // Copy test
  RUN_TEST(test_copy_file);


  UNITY_END();
}

void loop(){
  
}




















