#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>

#include <string>
#include <vector>

// sudo apt-get install libboost-all-dev
// for split function.
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;

// One file record.
struct Record {
  size_t key;
  char name[20];
  char last_name[20];
  char middle_name[20];
  unsigned short year; 
};

void print_record(struct Record* record)
{
  cout<<record->key<<" "<<record->name<<" "<<record->middle_name<<" "<<record->last_name<<" "<<record->year<<endl;
}

// Query example:
// select key <value>
void select_key(string value, struct Record* db, int dbSize)
{
  Record* tmpptr = db;
  for(int i=0;i<dbSize;i++)
  {
    if(tmpptr->key==stoi(value))
    {
      print_record(tmpptr);
    }
    
    // Next record.
    tmpptr++;
  }
}

// Query example:
// select name <value>
void select_name(string value, struct Record* db, int dbSize)
{
  Record* tmpptr = db;
  for(int i=0;i<dbSize;i++)
  {
    if(tmpptr->name==value)
    {
      print_record(tmpptr);
    }
    
    // Next record.
    tmpptr++;
  }
}

// Query example:
// select middle_name <value>
void select_middle_name(string value, struct Record* db, int dbSize)
{
  Record* tmpptr = db;
  for(int i=0;i<dbSize;i++)
  {
    if(tmpptr->middle_name==value)
    {
      print_record(tmpptr);
    }
    
    // Next record.
    tmpptr++;
  }
}

// Query example:
// select last_name <value>
void select_last_name(string value, struct Record* db, int dbSize)
{
  Record* tmpptr = db;
  for(int i=0;i<dbSize;i++)
  {
    if(tmpptr->last_name==value)
    {
      print_record(tmpptr);
    }
    
    // Next record.
    tmpptr++;
  }
}

// Common select query procesor.
void select_query(string field, string value, struct Record* db, int dbSize)
{
  if(field=="key") select_key(value,db, dbSize);
  else if(field=="name") select_name(value,db, dbSize);
  else if(field=="middle_name") select_middle_name(value,db, dbSize);
  else if(field=="last_name") select_last_name(value,db, dbSize);
}


// Query example:
// find year <from> <to>
void find_query(int from, int to, struct Record* db, int dbSize)
{
  if(from<1990||from>2022||to<1990||to>2022||from<to)
    cout<<"incorrect range"<<endl;
  
  Record* tmpptr = db;
  for(int i=0;i<dbSize;i++)
  {
    if(tmpptr->year>=from && tmpptr->year<=to)
    {
      print_record(tmpptr);
    }
    
    // Next record.
    tmpptr++;
  }
}

int main()
{
  int fd; 
  size_t length;
  int i;
  
  // Record pointers.  
  Record *ptr, *tmpptr;
  
  // Open file.
  fd = open("mapped.dat", O_RDWR, 0666);
  if(fd == -1) {
    cout<<"File open failed!"<<endl;
    return 1;
  }
  
  // Set file size.
  length = 1000*sizeof(struct Record);
  ftruncate(fd,length);
  
  // Count records number.
  const int db_records_count = int(length/sizeof(struct Record));
  
  // Map file for read.
  ptr = (struct Record*) mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
  
  close(fd);
  if ( ptr == MAP_FAILED ){
    cout<<"Mapping failed!"<<endl;
    return 2;
  }
  
  tmpptr = ptr;
  string tmpString="0";
  
  // Vector of fields.
  vector<string> validFields({
    "key", 
    "name", 
    "second_name", 
    "middle_name", 
    "year"
  });
  
  while(1)
  {
    
    // Get user input.
    getline(cin,tmpString);
    
    // "exit" user command breaks while.
    if (tmpString == "exit") break;
    
    // Vector for spliting user input.
    vector<string> tokens;
    
    // Split input by sapce. Store result to tokens.
    boost::split(tokens, tmpString, boost::is_any_of(" "));
    
    if(tokens.empty() || (tokens[0]!="select" && tokens[0]!="find"))
    {
      cout<<"Unknown comand"<<endl;
      continue;
    }
    if((tokens.size()<2) || 
      (find(validFields.begin(), validFields.end(), tokens[1])==
      validFields.end()))
    {
      cout<<"Incorrect field"<<endl;
      continue;
    }
    if(tokens.size()<2)
    {
      cout<<"Incorrect value"<<endl;
      continue;
    }
    if(tokens[0]=="select")
      select_query(tokens[1], tokens[2], ptr, db_records_count);
    else if(tokens[0]=="find")
    {
      if(tokens.size()<4)
        cout<<"Incorrect query"<<endl;
      try{
        find_query(stoi(tokens[2]), stoi(tokens[3]),ptr, db_records_count);
      }
      catch(...){ //stoi error for example.
        cout<<"Incorrect range"<<endl;
      }
    }
  }
  
  // Unmap shared memory.
  munmap((void *)ptr, length);
  return 0;
}