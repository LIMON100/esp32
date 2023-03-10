#include <iostream>
#include <fstream>
#include <string>
#include "FS.h"

using namespace std;

int make_html_upperbody(fs::FS &fs, const char *path) {
  int test_html = 0;

  const char *message2 = R"(
    <html>
    <head>
        <style type="text/css">
        .gallery li {
        display: inline;
        list-style: none;
        width: 150px;
        min-height: 175px;
        float: left;
        margin: 0 10px 10px 0;
        text-align: center;
        }
        </style>
    </head>
    <body>
        <table>
            <tr>
              <th>Test Suite &nbsp &nbsp</th>
              <th>Test Description &nbsp &nbsp </th>
              <th>Expected result  &nbsp &nbsp</th>
              <th>Actual result &nbsp &nbsp</th>
              <th>Assertion  &nbsp &nbsp</th>
              <th>Result  &nbsp &nbsp</th>
            </tr>

    )";

  const char *message1 = R"(
    <html>
    <head>
    <style>
        table {
            font-family: arial, sans-serif;
            border-collapse: collapse;
            width: 100%;
            }

            td, th {
            border: 1px solid #dddddd;
            text-align: left;
            padding: 8px;
            }

            tr:nth-child(even) {
            background-color: #D6EEEE;

            }
        </style>
    </head>
    <body>
        <br>
        <center>
        <span style='font-size:150px;'>&#129433;</span>
        <br>
        <h1>ESP32 TEST RESULTS </h1>
        </center>
        <br><br>
        <h2> Test Information <h2><br>
        <table>

    )";

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return test_html;
  }
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return test_html;
  }
  if (file.print(message1))
  {
    Serial.println("- file written");
  }
  file.close();

  // Append next html
  File file2 = fs.open(path, FILE_APPEND);
  const char *message4 = R"(<tr><th> Date </th>)";
  file2.print(message4);
  const char *message5 = R"(<th>  3/10/2023  </th></tr>)";
  file2.print(message5);

  const char *message6 = R"(<tr><th> Tester Name </th>)";
  file2.print(message6);
  const char *message7 = R"(<th> LIMON </th></tr>)";
  file2.print(message7);

  const char *message8 = R"(<tr><th> Machine Name </th>)";
  file2.print(message8);
  const char *message9 = R"(<th> esp32doit-devkit-v1 </th></tr>)";
  file2.print(message9);

  const char *message10 = R"(<tr><th> Test type </th>)";
  file2.print(message10);
  const char *message11 = R"(<th> Unittest </th></tr>)";
  file2.print(message11);

  const char *message12 = R"(</table>)";
  file2.print(message12);
  const char *message13 = R"(<br><br>)";
  file2.print(message13);
  const char *message14 = R"(<center><h2> TEST CASES </h2></center><br><br>)";
  file2.print(message14);

  if (!file2)
  {
    Serial.println("- failed to open file for appending");
  }

  if (file2.print(message2))
  {
    test_html = 1;
    Serial.println("- message appended");
    return test_html;
  }
  file2.close();

  return test_html;
}
