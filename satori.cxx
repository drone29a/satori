/*
 * satori.cxx - Executable Image Flow Analysis Application
 * (c) 2008 Michael Sullivan and Matt Revelle
 *
 * Last Revised: 05/04/08
 *
 * Code snippets for original "simple image flow example" taken from:
 * (1) lkdemo.c (available at: http://www.csie.ntu.edu.tw/~r94082/samples/c/lkdemo.c)
 * (2) Gady Agam's Introduction to programming with OpenCV (available at: http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00025000000000000000)
 * (3) ymazari@yahoo.fr (available at: http://groups.google.ws/group/OpenCV/msg/ec125224a1123fc2)
 * (4) cegparamesh@gmail.com (available at: http://opencvlibrary.sourceforge.net/DisplayManyImages)
 *
 * Code for Boost filesystem code taken from:
 * (1) simple_ls.cpp (available at http://www.boost.org/doc/libs/1_35_0/libs/filesystem/example/simple_ls.cpp)
 *
 * Code snippets for motion templates from:
 * (1) motempl.c (available at: http://www.csie.ntu.edu.tw/~r94082/samples/c/motempl.c)
 * (2) camshiftdemo.c (available at: http://www.csie.ntu.edu.tw/~r94082/samples/c/camshiftdemo.c)
 * All code snippets have been heavily modified for the most recent version of the program.
 *
 * This program uses getopt.h for option parsing, the Boost filesystem 
 * library, and the Open Computer Vision Library (OpenCV)
 *
 */

#include "satori.h"	// program header
#include "satori_app.h"

int main(int argc, char *argv[]){

  int optchar;							// for option input

  // handle input flags
  while((optchar = getopt(argc, argv, "i:f:s?o:w:")) != -1){	// read in arguments
    switch(optchar){
      case 'i':			// input directory
        input_directory = new string(optarg);
        break;
      case 'f':                 // file format
        file_format = new string(optarg);
        if(file_format->at(0) != '.') *file_format = "." + *file_format;
        break;
      case 's':                 // verbosity
        verbose = false;
        break;
      case 'o':
        output_directory = new string(optarg);
        break;
      case 'w':
        webcam = true;
        break;
      default:                  // display syntax help
      case '?':
        return display_program_syntax();
      }
    }
  
  // output title block, if applicable
  if(verbose) display_program_header();

  // resolve output path name and find directory (for later)
  fs::path out_path(fs::initial_path<fs::path>());
  out_path = fs::system_complete(fs::path(output_directory->c_str(), fs::native));
  if(!fs::exists(out_path) || !fs::is_directory(out_path)){
    cout << "[ERROR] Invalid output directory (" << out_path.native_directory_string() << ")!" << endl;
    return INVALID_OUTPUT_DIRECTORY;
  }

  // to store calculated flow information and intermediary data
  SatoriApp* app = new SatoriApp();

  // resolve input path name and find directory
  if(!webcam){
    fs::path full_path(fs::initial_path<fs::path>());
    full_path = fs::system_complete(fs::path(input_directory->c_str(), fs::native));
    if(!fs::exists(full_path) || !fs::is_directory(full_path)){
      cout << "[ERROR] Invalid input directory (" << full_path.native_directory_string() << ")!" << endl;
      return INVALID_INPUT_DIRECTORY;
    }

    if(verbose){
      cout << "  * " << "Finding *" << *file_format << " in " << full_path.native_directory_string() << endl;
    }

    // recurse through directory and handle all valid files
    fs::directory_iterator end_iter;
    for(fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr){

      // make sure is regular (i.e. non-directory) file
      if(fs::is_regular(dir_itr->status())){

        // make sure file extension is correct
        string ext = fs::extension(dir_itr->leaf());
        if(file_format->compare(ext) == 0){
          if(verbose){
            cout << "    * " << "Processing " << dir_itr->leaf() << "...";
          }

          // parse file (for later processing)
	  fs::path target_file(full_path);
          target_file /= dir_itr->leaf();
          bool added = app->add(target_file.native_directory_string());
	
          if(verbose){
            cout << "\t\t\t\t";
            if(added)
              cout << "[OK]" << endl;
            else
              cout << "[FAIL]" << endl;
          }
        }
      }
    }

    // find optical flow for each pair of images
    app->run();
  }
  else{	// using webcam
    display_program_commands();
    app->run_webcam(true);
  }
  
  // output a video to the proper folder
  if (save_output){
      
      app->animate(out_path.native_directory_string());
  }
         
  delete app;

  return 0;
}

void display_program_header(){
  cout << endl;
  cout << " " << PROGRAM_NAME << " v." << PROGRAM_VERSION << endl;
  cout << " " << "Written by " << AUTHOR_INFO << endl;
  cout << " " << VERSION_DATE << endl;
  cout << " --------------------------------------------" << endl;
  cout << endl;
}

int display_program_syntax(){
  cout << endl;
  cout << PROGRAM_NAME << ": Visually track a moving object without user input using optical flow and color-based motion segmentation." << endl;
  cout << endl;

  cout << "Syntax: " << PROGRAM_NAME << " -w (device) OR -i (directory) [-f (file format) -s -v]" << endl;
  cout << "  " << "-w" << ": Process input from an attached webcam (located at /dev/video0)" << endl;
  cout << "  " << "-i (directory)" << ": Process all image files from the given directory (default \"" << DEFAULT_INPUT_DIRECTORY << "/\")" << endl;
  cout << "  " << "-f (file format)" << ": Read any images with the given file extension (default *" << DEFAULT_FILE_FORMAT << ")" << endl;
  cout << "  " << "-s" << ": Disable program output" << endl;
  cout << "  " << "-?" << ": Display this screen" << endl;
  cout << endl;

  return 0;
}

void display_program_commands(){
  cout << endl;
  cout << "Commands:" << endl;
  cout << "  " << "f" << ": Toggle flow processing and feature tracking" << endl;
  cout << "  " << "t" << ": Toggle motion segmentation and object identificaion" << endl;
  cout << "  " << "r" << ": Find and track a moving object" << endl;
  cout << endl;
}
