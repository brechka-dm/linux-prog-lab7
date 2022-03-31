#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

// Recurrent procedure for dir remove.
void dir_walker(string dir)
{
  
  // Trying to open dir.
  DIR* did_descriptor = opendir(dir.c_str());
  if(!did_descriptor){
    cout<<"Incorrect dir name"<<endl;
    return;
  }
  
  // Getting first dir record.
  dirent* dir_record = readdir(did_descriptor);
  
  // Add '/' to base name if necessarry.
  if(dir[dir.size()-1]!='/')
    dir = dir+"/";
  
  // In-depth search.
  while(dir_record!=NULL)
  {
    
    // Make full name for record.
    const string r_name=dir+dir_record->d_name;
    
    struct stat buf;
    
    // Ignoring '.' and '..' dirs.
    if(string(dir_record->d_name)!="." && 
      string(dir_record->d_name)!="..")
    {
      
      // Getting dir record stat.
      lstat(r_name.c_str(), &buf);
      
      // If record is dir call dir_walker for this record than remove dir.
      if(S_ISDIR(buf.st_mode)){
        dir_walker(r_name);
        rmdir(r_name.c_str());
      }
      else{
      
        // If record is not dir remove it.
        unlink(r_name.c_str());
      }
    }
    
    // Getting next dir record.
    dir_record = readdir(did_descriptor);
  }
  
  // Remove top level dir.
  rmdir(dir.c_str());
}

int main(int argc, char* argv[])
{
  if(argc<2)
  {
    cout<<"Input dir to remove"<<endl;
    return -1;
  }
  dir_walker(string(argv[1]));
  return 0;
}