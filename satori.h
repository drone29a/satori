/*
 * satori.h - Header for Image Flow Analysis Application
 * (c) 2008 Michael Sullivan
 *
 * Version 1.0.1
 * Last Revised: 04/29/08
 *
 * Version History:
 *   1.0.1 - Made object oriented and incorporated getopt.h for option parsing (04/29/08)
 *   1.0.0 - Initial "simple image flow example" (04/22/08)
 *
 * Code snippets for imflow taken from:
 * (1) lkdemo.c (available at: http://www.csie.ntu.edu.tw/~r94082/samples/c/lkdemo.c)
 * (2) Gady Agam's Introduction to programming with OpenCV (available at: http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00025000000000000000)
 * (3) ymazari@yahoo.fr (available at: http://groups.google.ws/group/OpenCV/msg/ec125224a1123fc2)
 * (4) cegparamesh@gmail.com (available at: http://opencvlibrary.sourceforge.net/DisplayManyImages)
 *
 * This program utilizes the Boost.Filesystem library
 *
 */

#ifndef _SATORI_H_
#define _SATORI_H_

// includes
#include <iostream>
#include <string>
#include <unistd.h>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations

// namespace preparation
namespace fs = boost::filesystem;
using namespace std;

// constants
const string PROGRAM_NAME = "satori";
const string PROGRAM_VERSION = "1.0.1";
const string VERSION_DATE = "04/29/08";
const string AUTHOR_INFO = "Michael Sullivan & Matt Revelle";
const string DEFAULT_INPUT_DIRECTORY = "images";
const string DEFAULT_FILE_FORMAT = ".png";
const string DEFAULT_OUTPUT_DIRECTORY = "out/";

// global variables
string *input_directory = new string(DEFAULT_INPUT_DIRECTORY);  	// directory to read images from
string *file_format = new string(DEFAULT_FILE_FORMAT);          	// format of files to scan
bool verbose = true;							// print extra information?
string *output_directory = new string(DEFAULT_OUTPUT_DIRECTORY);	// directory to write images to
bool webcam = false;							// getting input from a webcam?
bool save_output = false;                                               // true when animation should be saved

// error codes
#define INVALID_INPUT_DIRECTORY 1
#define INVALID_OUTPUT_DIRECTORY 2

// prototypes
void display_program_header();				// display title block
int display_program_syntax();				// output syntax of program

#endif
