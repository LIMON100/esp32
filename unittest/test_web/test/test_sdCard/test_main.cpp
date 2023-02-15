#include <sd_card.hpp>
// #include "main.cpp"
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
  TEST_ASSERT_EQUAL_UINT8 (write_expected, actual_result);
  
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


void test_update_text_file()
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


void test_read_images()
{
  const char* path= "/Images/Logo/llamalogo.jpg";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}

void test_read_files()
{
  const char* path= "/main_app.py";
 
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

void test_read_file_onemb()
{

  const char* path= "/firmware.bin";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}

void test_read_file_threemb()
{

  const char* path= "/models.kfpkg";
 
  FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  TEST_ASSERT_EQUAL_UINT8 (read_expected, 1);
}

// void test_web_server_setup()
// {
//   int a = web_server_setup();
//   TEST_ASSERT_EQUAL_UINT8 (a, 1);
  
// }


// void test_one_plus_one()
// {
//   int a = 1;
//   int b = 2;
//   TEST_ASSERT_EQUAL_UINT8 (a, a);
// }

// int main(void)
// {
//   UNITY_BEGIN();
//   RUN_TEST(test_one_plus_one);
//   UNITY_END();

// }

void setUp(void)
{
  initSDCard();
}

void tearDown(void)
{

}

void check_test_cases()
{
  delay (1000);
  UNITY_BEGIN();
  RUN_TEST(test_write_txt_file);
  // RUN_TEST(test_web_server_setup);
  // RUN_TEST(test_read_txt_file);
  // RUN_TEST(test_compare_text_file);
  // RUN_TEST(test_update_text_file);
  // RUN_TEST(test_read_images);
  // RUN_TEST(test_read_files);
  // RUN_TEST(test_delete_file);
  // RUN_TEST(test_read_file_onemb);
  // RUN_TEST(test_read_file_threemb);
  // RUN_TEST(test_web_server_setup);

  UNITY_END();
}


void setup ()
{
  // delay (1000);
  // UNITY_BEGIN();
  // RUN_TEST(test_write_txt_file);
  // // RUN_TEST(test_web_server_setup);
  // // RUN_TEST(test_read_txt_file);
  // // RUN_TEST(test_compare_text_file);
  // // RUN_TEST(test_update_text_file);
  // // RUN_TEST(test_read_images);
  // // RUN_TEST(test_read_files);
  // // RUN_TEST(test_delete_file);
  // // RUN_TEST(test_read_file_onemb);
  // // RUN_TEST(test_read_file_threemb);
  // // RUN_TEST(test_web_server_setup);

  // UNITY_END();
  check_test_cases();
}

void loop(){
  
}
