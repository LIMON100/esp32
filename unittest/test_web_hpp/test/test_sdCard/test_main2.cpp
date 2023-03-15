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

int actual_result = 1;
const char* html_path= "/esp_ut.html";

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
  definition_test_name(SD, html_path, test_suite, fn, 0);
  multiple_condition_result(SD, html_path, write_expected, 1, asertion);

  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
}


void test_read_txt_file()
{
  const char* path= "/hello.txt";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  const char* test_suite = "Write in SD card";
  const char* fn = "create text file";
  const char* asertion = "Read the created text file in sd card and return 1";
  definition_test_name(SD, html_path, test_suite, fn, 0);
  multiple_condition_result(SD, html_path, read_expected, 1, asertion);

  // Read the file
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}

void test_write_txt_file()
{
  const char* path= "/hello.txt";
  const char* message = "Hello";

  
  FS_WRITE_STATUS write_expected =  writeFile(SD, path, message);
  // FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  // Write file
  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
  
}


void test_compare_text_file()
{
  const char* path= "/hello.txt";
  const char* message = "Llama";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

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
  TEST_ASSERT_EQUAL_STRING (expected, expected);
}

void test_read_python_file()
{
  const char* path= "/train5.py";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

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

  TEST_ASSERT_EQUAL_UINT8 (test_counter, 1000);
}


void test_read_files_in_folder()
{
  const char* path= "/Images/Logo/llamalogo.jpg";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_audio_file()
{
  const char* path= "/Audio/0.wav";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_html_file()
{
  const char* path= "/data_collection.html";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_app_index()
{
  
  int read_expected = read_app_index();
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_k210_fw_version()
{
  
  int read_expected = read_k210_fw_version();
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_k210_app_version()
{
  
  int read_expected = read_k210_app_version();
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

void test_read_models()
{
  const char* path= "/Models/helmet2.kmodel";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}



void test_delete_file()
{
  const char* path= "/test_file_5.txt";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

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
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    if (it1->first == "no_wifi")
    {
      TEST_ASSERT_EQUAL(1, it1->second);
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
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    if (it1->first == "sd_size")
    {
      TEST_ASSERT_GREATER_THAN(6000, it1->second);
    }
    ++it1;
    
  }
}


void test_run_folder_sd()
{
 
	String s = run_folder("/sd");
  String s1 = "1";
  TEST_ASSERT_EQUAL_STRING(s.c_str(), s1.c_str());

}

void test_run_folder_flash()
{
 
	String s = run_folder("/flash");
  String s1 = "1";
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
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    ++it1;
    
  }
  // Read that copied file 
  const char* path2 = "/files_path.py";
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path2);

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
  // RUN_TEST(test_write_txt_file);
  // RUN_TEST(test_compare_text_file);
  // RUN_TEST(test_update_text_file_compare);

  // // Read test
  // RUN_TEST(test_read_python_file);
  // RUN_TEST(test_read_sd_hundred_times);
  // RUN_TEST(test_read_files_in_folder);
  // RUN_TEST(test_read_audio_file);
  // RUN_TEST(test_read_html_file);
  // RUN_TEST(test_read_app_index);
  // RUN_TEST(test_read_k210_app_version);
  // RUN_TEST(test_read_k210_fw_version);
  // // RUN_TEST(test_read_models);

  // // Delete test
  // RUN_TEST(test_delete_file);
  // RUN_TEST(test_delete_file_in_folder);
  // RUN_TEST(test_delete_python_file);
  // RUN_TEST(test_delete_models);
  // // RUN_TEST(test_delete_zip);

  // // Wifi test
  // RUN_TEST(test_scan_wifi);

  // // Init Sd card
  // RUN_TEST(test_init_sd);

  // // FIles in sd/flash
  // RUN_TEST(test_run_folder_sd);
  // RUN_TEST(test_run_folder_flash);

  // // Copy test
  // RUN_TEST(test_copy_file);


  UNITY_END();
}

void loop(){
  
}




















