/* ilo-credit-store.h -- types and prototypes used by iLO plug-in
   (C) Copyright [2015] Hewlett Packard Enterprise Development Company, L.P.

   This program is free software; you can redistribute it and/or modify
   it under the terms of version 2 of the GNU General Public License as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to:
    Free Software Foundation, Inc.
    51 Franklin Street, Fifth Floor
    Boston, MA 02110-1301, USA.  */

#ifndef _ILOCREDITSO_H
#define _ILOCREDITSO_H

#include <getopt.h>
#define OPTION_NOP      0

#ifdef __cplusplus
extern "C" {
#endif
  int set_cred(char * host, char * user, char * passwd);
  char *get_cred(char * host);
  int exist_cred(char * host);
  typedef int set_cred_t(char *, char *, char *);
  typedef char *get_cred_t(char *);
  typedef int exist_cred_t(char *);
#ifdef __cplusplus
}
#endif

#endif
