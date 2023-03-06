// #include <sd_card.hpp>
#include "main.cpp"
#include <unity.h>
#include <iostream>
using namespace std;

#define SCK 18
#define MISO 23
#define MOSI 14
#define CS 19

#define SDCARD_8GB

int actual_result = 1;


void test_write_txt_file()
{
  const char* path= "/hello.txt";
  const char* message = "Hello";

  
  FS_WRITE_STATUS write_expected =  writeFile(SD, path, message);
  // FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  // Write file
  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
  
}

void test_read_txt_file()
{
  const char* path= "/hello.txt";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  // Read the file
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
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
  TEST_ASSERT_EQUAL_STRING (expected, actual);
}

void test_read_files()
{
  const char* path= "/main_app.py";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_read_files_in_folder()
{
  const char* path= "/Images/Logo/llamalogo.jpg";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

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

void test_delete_file_in_folder()
{
  const char* path= "/test_folder/0.wav";

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
  const char* path= "/test_folder/test.zip";

  send_file_to_k210(SD, path);
  FS_DELETE_STATUS expected = deleteFile(SD, path);
  FS_READ_STATUS read_delete = send_file_to_k210(SD, path);

  // Delete file
  TEST_ASSERT_EQUAL_INT (expected, 0);

  // check file is deleted or not
  TEST_ASSERT_EQUAL_INT (read_delete, 0);
}

void test_read_models()
{

  const char* path= "/Models/helmet.kmodel";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
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

  String s1 = "1";
	String s = write_app_index("3");
  TEST_ASSERT_EQUAL(s.c_str(), s1.c_str());

  // Read that copied file 
  // const char* path2 = "/files_path.py";
  // FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path2);

  // TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);

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

  // RUN_TEST(test_read_txt_file);
  // RUN_TEST(test_write_txt_file);
  // RUN_TEST(test_compare_text_file);
  // RUN_TEST(test_update_text_file_compare);
  // RUN_TEST(test_read_files);
  // RUN_TEST(test_read_files_in_folder);
  // RUN_TEST(test_delete_file);
  // RUN_TEST(test_delete_file_in_folder);
  // RUN_TEST(test_delete_zip);
  // RUN_TEST(test_read_models);
  // RUN_TEST(test_scan_wifi);
  // RUN_TEST(test_init_sd);
  // RUN_TEST(test_run_folder_sd);
  // RUN_TEST(test_run_folder_flash);
  // RUN_TEST(test_copy_file);
  RUN_TEST(test_write_app_index);


  UNITY_END();
}

void loop(){
  
}
