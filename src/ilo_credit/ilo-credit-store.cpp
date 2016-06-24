/***************************************************************************/
/*(C) Copyright [2016] Hewlett Packard Enterprise Development Company, L.P.*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <tdb.h>

#include "base64.h"
#include "ilo-credit-store.h"

#define PROGRAM_NAME    "ilo-credit-store"
#define PROGRAM_VERSION 1.0
#define TDBDIR	 	"/etc/zabbix"
#define TDBFILE 	"/etc/zabbix/.zabbix_credential.tdb"

using namespace std;

TDB_CONTEXT *_tdb;             // Handle on tdb protected by lock

int storeString(const string &key, const string &value)
{
    TDB_DATA k, v;

    k.dptr = (unsigned char *)key.c_str();
    k.dsize = key.length();

    v.dptr = (unsigned char *)value.c_str();
    v.dsize = value.length();

    int result = tdb_store(_tdb, k, v, TDB_REPLACE);

    return result;
}

string fetchString(const string &key)
{
    TDB_DATA k;

    k.dptr = (unsigned char *)key.c_str();
    k.dsize = key.length();

    TDB_DATA v;

    v = tdb_fetch(_tdb, k);

    if (v.dptr == NULL)
        return string();

    string result((char *)v.dptr, v.dsize);

    free(v.dptr);

    return result;
}

bool remove(const string &key)
{
    TDB_DATA k;

    k.dptr = (unsigned char *)key.c_str();
    k.dsize = key.length();

    bool result = (tdb_delete(_tdb, k) == 0);

    return result;
}

bool exists(const string &key)
{
    TDB_DATA k;

    k.dptr = (unsigned char *)key.c_str();
    k.dsize = key.length();

    bool result = tdb_exists(_tdb, k);

    return result;
}

extern "C"
char *
get_cred(char * host)
{
  const char * encode_credential;
  string key, value;
  size_t test;
  int   option_instruction = OPTION_NOP;

  if (host)
    key=host;
  else
    return NULL;

  _tdb = tdb_open_ex(TDBFILE, 0, 0, O_RDWR, 0600, NULL, NULL);

  if (!_tdb) {
    cout<<"Could not open "<<TDBFILE<<endl;
    return NULL;
  }

  if (exists(key))
    value=fetchString(key);
  else
    value=fetchString("default");

//  cout<<key<<":"<<value<<endl;

//  base64_decode((char *)value.c_str(), (unsigned char **)&encode_credential, &test);
//  value = encode_credential;
//  cout<<value<<endl;

  tdb_close(_tdb); 
  base64_decode((char *)value.c_str(),(char **)&encode_credential);
//  encode_credential=value.c_str();
  return (char *)encode_credential;
}

extern "C"
int 
exist_cred(char * host) {
  string key;
  if (host)
    key=host;
  else
    return 1;

  _tdb = tdb_open_ex(TDBFILE, 0, 0, O_RDWR, 0600, NULL, NULL);

  if (!_tdb) 
    return 1;
  if (exists(key)) {
    tdb_close(_tdb);
    return 0;
  } else {
    tdb_close(_tdb);
    return 1;
  }
}

extern "C"
int 
set_cred(char * host, char * user, char * passwd) {
  char * encode_credential;
  string key = host, value = user, credential = passwd;
  FILE *_textf;
  int   option_instruction = OPTION_NOP, status;

  if((_textf = fopen(TDBFILE, "r+" )) == NULL) {
    DIR* dir = opendir(TDBDIR);
    if (dir) 
      /* Directory exists. */
      closedir(dir);
    else {
      status = mkdir(TDBDIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);        
      if (status != 0) {
        cout<<"Create dir "<<TDBDIR<<" failed!"<<endl;
        return -1;
      }
    }

    _tdb = tdb_open_ex(TDBFILE,
                       0,    // hash_size
                       0,    // tdb_flags
                       O_CREAT|O_RDWR, // open_flags
                       0600,          // mode
                       NULL,   // log_fn
                       NULL);         // hash_fn
  } else {
    fclose(_textf);
    _tdb = tdb_open_ex(TDBFILE, 0, 0, O_RDWR, 0600, NULL, NULL);
  }

  if (!_tdb) {
    cout<<"Could not create "<<TDBFILE<<endl;
    return -1;
  } else {
    if (exists(key)) {
      if (!remove(key)) {
        cout<<"remove error."<<endl;
        return -2;
      }

    }
    credential=value+":"+credential;
    value="";

    status = base64_encode((const unsigned char*)credential.c_str(), strlen(credential.c_str()), &encode_credential);
    if (status == 0) {
      value=encode_credential;
      if (storeString(key, value) == 0) {
         printf("Zabbix Credential succeeds to encrypt and save.\n");
//         value="";
//         value=fetchString(key);
//         cout<<key<<" "<<value<<endl;
      } else {
        printf("iLO Credential failed.\n");
        return -1;
      }
    }

    tdb_close(_tdb);
  }
}

/*
main(int argc, char **argv) {
  struct credential_datum cred_data;
  string file="/usr/local/nagios/var/.nagios_restful_credential.tdb",
         credential;
  char * encode_credential;
  string key, value;
  FILE *_textf;
  int   option_instruction = OPTION_NOP;
  cred_data.host=cred_data.user=cred_data.passwd="";

  if (process_options(argc, argv, &cred_data, &option_instruction)
                      != NAGIOS_ILO_SUCCESS_STATUS)
    {
      usage();
      exit(NAGIOS_UNKNOWN);
    }

  if (cred_data.option == "add") {
    if (cred_data.user == "") {
      printf("Enter user name:");
      cin>>cred_data.user;
    }

    printf("Enter password:");
    cin>>cred_data.passwd;
  
    if((_textf = fopen(file.c_str(), "r+" )) == NULL)
      _tdb = tdb_open_ex(file.c_str(),
                       0,    // hash_size
                       0,    // tdb_flags
                       O_CREAT|O_RDWR, // open_flags
                       0600,          // mode
                       NULL,   // log_fn
                       NULL);         // hash_fn
    else
      _tdb = tdb_open_ex(file.c_str(), 0, 0, O_RDWR, 0600, NULL, NULL);

    if (!_tdb) {
      cout<<"Could not create "<<file<<endl;
      return -1;
    }
    

    credential=cred_data.user+":"+cred_data.passwd;

    base64_encode(credential.c_str(),&encode_credential);

    key=cred_data.host;
    value=encode_credential;
    if (storeString(key, value) == 0) {
      printf("File succeeds to encrypt.\n");
      value="";
      value=fetchString(key);   
      cout<<key<<" "<<value<<endl;
      base64_decode(value.c_str(),&encode_credential);
      value = encode_credential;
      cout<<key<<" "<<value<<endl;
    }

    free(encode_credential);
    tdb_close(_tdb); 

  } else if (cred_data.option == "get") {
    _tdb = tdb_open_ex(file.c_str(), 0, 0, O_RDWR, 0600, NULL, NULL);

    if (!_tdb) {
      cout<<"Could not open "<<file<<endl;
      return -1;
    }

    key=cred_data.host;
    if (exists(key))
      value=fetchString(key);
    else
      value=fetchString("default");

    base64_decode(value.c_str(),&encode_credential);
    value = encode_credential;
//    cout<<value<<endl;

    free(encode_credential);
    tdb_close(_tdb); 

  }else {
    usage();
    exit(NAGIOS_UNKNOWN);
  }
 
}*/
