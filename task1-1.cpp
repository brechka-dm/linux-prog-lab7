#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <string>
#include <vector>

//sudo apt-get install libboost-all-dev
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>


using namespace std;
int main()
{
  int fd; 
  size_t length;
  int i;
  
  // One file record.
  struct Record {
    size_t key;
    char name[20];
    char last_name[20];
    char middle_name[20];
    unsigned short year; 
  };
  
  Record *ptr, *tmpptr;
  
  // Open or create file.
  fd = open("mapped.dat", O_RDWR | O_CREAT, 0666);
  if(fd == -1) {
    cout<<"File open failed!"<<endl;
    return 1;
  }
  
  // Set file size.
  length = 1000*sizeof(struct Record);
  ftruncate(fd,length);
  
  // Map file for read and write.
  // ptr is a pointer to file begin.
  ptr = (struct Record*) mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  
  close(fd);
  
  if ( ptr == MAP_FAILED ){
    cout<<"Mapping failed!"<<endl;
    return -2;
  }
  
  // tmpptr is a pointer to one record (first it points to file begin).
  tmpptr = ptr;
  
  // k is a key value (starts from 1).
  size_t k = 1;
  
  string tmpString;
  
  while(1)
  {
    // Usrt tooltip.
    cout<<"Input <First name> <Middle name> <Last name> <Birth year> or \"END\" to exit"<<endl;
    
    // Get user input.
    getline(cin,tmpString);
    
    // "exit" user command breaks while.
    if (tmpString == "exit") break;
    
    // Vector for spliting user input.
    vector<string> tokens;
    
    // Split input by sapce. Store result to tokens.
    boost::split(tokens, tmpString, boost::is_any_of(" "));
    if(tokens.size()!=4)
    {
      cout<<"Incorrect fileds number"<<endl;
      continue;
    }
    for(int i=0;i<4;i++)
    {
      if(tokens[i].size()>20) 
      {
        cout<<"Incorrect field length ["<<i+1<< "] (mast be not more than 20)"<<endl;
        continue;
      }
    }
    unsigned short y; 
    try
    {
      y = stoi(tokens[3]);
    }
    catch(...)
    {
      cout<<"Incorrect year"<<endl;
      continue;
    }
    if(y<1900 || y>2022)
    {
      cout<<"Incorrect year"<<endl;
      continue;
    }
    
    // Store values to record.
    tmpptr->key=k++;
    strcpy(tmpptr->name, tokens[0].c_str());
    strcpy(tmpptr->middle_name, tokens[1].c_str());
    strcpy(tmpptr->last_name, tokens[2].c_str());
    tmpptr->year=y;
    
    // Next record.
    tmpptr++;
  }
  
  // Unmap file.
  munmap((void *)ptr, length);
  return 0;
}