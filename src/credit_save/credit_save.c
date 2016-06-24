/***************************************************************************/
/*(C) Copyright [2016] Hewlett Packard Enterprise Development Company, L.P.*/

#define CURL_STATICLIB

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include "ilo-credit-store.h"
#include "credit_save.h"
#include "base64.h"

/* The official name of this program (e.g., no 'g' prefix).  */
#define PROGRAM_NAME    "credit_save"
#define PROGRAM_VERSION "1.0"
#define MAX_RETRY_TIMES 3

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)

// If option=0, set display mode close
int set_disp_mode(int fd,int option)
{
   int err;
   struct termios term;
   if(tcgetattr(fd,&term)==-1){
     perror("Cannot get the attribution of the terminal");
     return 1;
   }
   if(option)
        term.c_lflag|=ECHOFLAGS;
   else
        term.c_lflag &=~ECHOFLAGS;
   err=tcsetattr(fd,TCSAFLUSH,&term);
   if(err==-1 && err==EINTR){
        perror("Cannot set the attribution of the terminal");
        return 1;
   }
   return 0;
}

int getpasswd(char* passwd, int size)
{
   int c;
   int n = 0;
   if (!passwd) 
     return 1;

   do{
      c=getchar();
      if (c != '\n'|c!='\r'){
         passwd[n++] = c;
      }
   }while(c != '\n' && c !='\r' && n < (size - 1));
   passwd[n] = '\0';
   return n;
}

static void
print_version (void)
{
  printf("%s version - %s\n", PROGRAM_NAME, PROGRAM_VERSION);
  return ;
}

/* Print the usage information

static void
usage (void)
{
  struct option *opt_ptr = NULL;

  printf("Usage: %s ", PROGRAM_NAME);

  for (opt_ptr=long_options;opt_ptr->name != NULL;opt_ptr++)
    {
      if (opt_ptr->has_arg == no_argument)
          printf("[-%c] ", opt_ptr->val);
      else if (opt_ptr->has_arg == required_argument)
          printf("-%c <%s> ", opt_ptr->val, opt_ptr->name);
    }

  printf("\n");

  return ;
}

static void
print_help (void)
{
  int   i, j;

  usage();

  printf("\n");

  for (i=0;i<sizeof(help_string)/sizeof(help_string[0]);i++)
    {
      printf("%s\n", help_string[i]);
    }

  return ;
}

static void
check_option_instruction (int instruction)
{
  switch (instruction)
    {
    case NAGIOS_OPTION_PRINT_VERSION:
      print_version();
      break;
    case NAGIOS_OPTION_PRINT_HELP:
      print_help();
      break;
    }

  exit(NAGIOS_UNKNOWN);
}

int
process_options (int argc, char **argv, char **host,
                 int *option_inst)
{
  char option;
  int ret = NAGIOS_ILO_SUCCESS_STATUS;
  int num_argc_proceed = 0;
  int option_index = 0;

  // Ignore the error message reported by unistd library.
  opterr = 0;

  while((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options,
                                &option_index)) != -1)
    {
      switch(option)
        {
        case 'h':
          *option_inst = NAGIOS_OPTION_PRINT_HELP;
          break;
        default:
          ret = NAGIOS_ILO_FAIL_STATUS;
        }
    }

  if(num_argc_proceed != NAGIOS_ILO_REQUIRE_ARGC &&
                                        *option_inst == NAGIOS_OPTION_NOP)
    {
      ret = NAGIOS_ILO_FAIL_STATUS;
    }

  return ret;
} */

int main(int argc, char **argv) {
  int option_instruction = OPTION_NOP;
  int loop=MAX_RETRY_TIMES;
  char *p,*p1,passwd[50],passwd1[50],name[50],con_char[10];
  if (exist_cred("zabbix") == 0) {
    printf(" Zabbix credit is exists. Do you wish to update or not? (y/n): ");
    while (loop--) {
      scanf("%s",con_char);
      getchar();
      if (strcmp(con_char,"y") == 0) {
        printf(" Zabbix credit will be updated.\n");
        break;
      }
      else if (strcmp(con_char,"n") == 0) {
        printf(" Zabbix credit does not change.\n");
        return 0;
      } else if (loop) {
        printf("ERROR: You must type 'y' or 'n'.\n");
        printf("Do you wish to update or not? (y/n): ");
      } else
        return 1;
    }

  }

  loop=MAX_RETRY_TIMES;
  do {
    int ret;
    printf("Enter user name for Zabbix server: ");
    scanf("%s",name);
    getchar();
  
    set_disp_mode(STDIN_FILENO,0);
  
    printf("Enter the password: ");
    ret=getpasswd(passwd, sizeof(passwd));  
    if (!ret)
      return 1;
 
    p=passwd;
    while(*p!='\n')
      p++;
    *p='\0';
  
    printf("\n"); 
    printf("Enter the password again:");
    ret=getpasswd(passwd1, sizeof(passwd1));  
    if (!ret)
      return 1;
 
    p1=passwd1;
    while(*p1!='\n')
      p1++;

    *p1='\0';
    set_disp_mode(STDIN_FILENO,1);
    if (strlen(passwd) == 0)
      printf("\nSorry, passwords can not be blank.\n");
    else if (strcmp(passwd,passwd1) == 0) {
      printf("\n"); 
      set_cred("zabbix",name,passwd);
      //printf("%s",get_cred("zabbix"));
      return 0;
    } else
      printf("\nSorry, passwords do not match.\n");
  } while (--loop);
  return 1;
}
