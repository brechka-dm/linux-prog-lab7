#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <optional>
#include <ctime>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

// Struct for getting user and group names from system files.
struct NameId{
  NameId(string const& _name, unsigned _id)
    : name(_name), id(_id) {}
  const string name;
  const unsigned id;
};

// Function for parsing system files.
vector<NameId> extract_name_id(string const& file_name)
{
  vector<NameId> result;
  string file_content;
  char c;
  int fd=open(file_name.c_str(),O_RDONLY);
  
  // Read file symbol by sybmol.
  while(read(fd,&c,1)>0) file_content+=c;
  close(fd);
  
  // Split file content to separate strings.
  vector<string> tokens;
  boost::split(tokens, file_content, boost::is_any_of("\n"));
  
  // Split every string to fields.
  for(const string& token:tokens)
  {
    vector<string> fields;
    boost::split(fields, token, boost::is_any_of(":"));
    if(fields.size()>=3)
      
      // Field 0 contains name, field 2 contains id.
      result.emplace_back(fields[0],atoi(fields[2].c_str()));
  }
  return result;
}

// Function for getting user name by uid.
vector<NameId> user_list()
{
  return extract_name_id("/etc/passwd");
}

// Function for getting group name by gid.
vector<NameId> group_list()
{
  return extract_name_id("/etc/group");
}

// Function for getting name from id.
// Function returns name or nullopt if id was not found in struct.
optional<string> name_from_id(unsigned id, vector<NameId> const& records)
{
  auto uname = find_if(records.begin(), records.end(), [id](NameId const& item)
  {
    return id==item.id;
  });
  if(uname!=records.end()) return uname->name;
  return nullopt;
}

// Function for sorting dir content.
// Sorting based on last field in string.
bool sorting_function(string const& st1, string const& st2)
{
  vector<string> tokens1;
  vector<string> tokens2;
  
  // Spliting strings to fields.
  boost::split(tokens1, st1, boost::is_any_of("\t"));
  boost::split(tokens2, st2, boost::is_any_of("\t"));
  int l = tokens1.size();
  
  // Getting last fields and converting them to lower case.
  const string lower_str1 = boost::algorithm::to_lower_copy(tokens1[l-1]);
  const string lower_str2 = boost::algorithm::to_lower_copy(tokens2[l-1]);
  
  // Comparing strings.
  return lower_str1<lower_str2;
}

vector<string> dir_walker(string dir, vector<NameId> const& users, vector<NameId> const& groups)
{
  
  // Trying to open dir.
  DIR* did_descriptor = opendir(dir.c_str());
  if(!did_descriptor){
    cout<<"Incorrect dir name"<<endl;
    return {};
  }
  
  // Getting first dir record.
  dirent* dir_record = readdir(did_descriptor);
  
  // Add '/' to base name if necessarry.
  if(dir[dir.size()-1]!='/')
    dir = dir+"/";
  
  // Vector for results.
  vector<string> list;
  
  while(dir_record!=NULL)
  {
    
    // Make full name for record.
    const string r_name=dir+dir_record->d_name;
    
    struct stat buf;
    
    lstat(r_name.c_str(), &buf);
    
    // String for one record.
    string record;
    
    // Determining the file type.
    switch (buf.st_mode & S_IFMT)
    {
      case S_IFDIR: record+="d"; break;
      case S_IFIFO: record+="p"; break;
      case S_IFLNK: record+="s"; break;
      case S_IFREG: record+="-"; break;
    }
    
    // Determining the file permissions.
    // 1.Permissions for owner.
    if(buf.st_mode&S_IRUSR) record+="r";
    else record+="-";
    if(buf.st_mode&S_IWUSR) record+="w";
    else record+="-";
    if(buf.st_mode&S_IXUSR) record+="x";
    else record+="-";
    
    // 2.Permissions for group.
    if(buf.st_mode&S_IRGRP) record+="r";
    else record+="-";
    if(buf.st_mode&S_IWGRP) record+="w";
    else record+="-";
    if(buf.st_mode&S_IXGRP) record+="x";
    else record+="-";
    
    // 3.Permissions for others.
    if(buf.st_mode&S_IROTH) record+="r";
    else record+="-";
    if(buf.st_mode&S_IWOTH) record+="w";
    else record+="-";
    if(buf.st_mode&S_IXOTH) record+="x";
    else record+="-";
    record+="\t";
    
    // Link count.
    record+=to_string(buf.st_nlink);
    record+="\t";
    
    // File user name.
    auto name=name_from_id(buf.st_uid,users);
    if(name)
      record+=*name;
    else record+=to_string(buf.st_uid);
    record+="\t";
    
    // File group name.
    name=name_from_id(buf.st_gid,groups);
    if(name)
      record+=*name;
    else record+=to_string(buf.st_gid);
    record+="\t";
    
    // File size.
    record+=to_string(buf.st_size);
    record+="\t";
    
    // File timestamp.
    char str_time[100];
    struct tm * timeinfo;
    timeinfo = localtime(&buf.st_ctime);
    
    // Converting timestamp to string format.
    // Month day hours:minutes
    strftime(str_time, 32, "%b %d %H:%M", timeinfo); 
    record+=str_time;
    record+="\t";
    
    record+=dir_record->d_name;
    list.push_back(record);
    
    // Getting next dir record.
    dir_record = readdir(did_descriptor);
  }
  
  // Sorting all records.
  std::sort(list.begin(), list.end(), sorting_function);
  
  return list;
}

int main(int argc, char* argv[])
{
  string path=".";
  if(argc>1)
  {
    path=argv[1];
  }
  
  vector<string> dir_list=dir_walker(path, user_list(), group_list());
  
  for(const string& record:dir_list)
  {
    cout<<record<<endl;
  }
  
  return 0;
}