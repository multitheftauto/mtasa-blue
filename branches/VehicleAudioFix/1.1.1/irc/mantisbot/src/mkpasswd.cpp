/*
 * Copyright (c) 2007, Alberto Alonso Pinto
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions
 *       and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *       and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Mantis Bot nor the names of its contributors may be used to endorse or
 *       promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "keys.h"

int main(int argc, char *argv[])
{
  char* pass;
  char* result;
  char* confirm;
  int n;
  size_t len;
  size_t reslen;


  /* Ask for password and disallow empty passwords */
  do
  {
    confirm = getpass("Type your password: ");
    len = strlen(confirm) + 1;
  } while (len < 2);

  /* Calculate number of blocks to encode and allocate memory */
  n = strlen(confirm) / sizeof(uint32_t) + !!(strlen(confirm) % sizeof(uint32_t));
  len = n * sizeof(uint32_t);
  pass = new char[len+1];
  memset(pass, 0, len+1);
  strcpy(pass, confirm);
  reslen = n * 6 + 1;
  result = new char[reslen];
  memset(confirm, 0, strlen(confirm));

  keysEncode(pass, result);

  /* Clear password from memory */
  memset(pass, 0, len);
  delete [] pass;

  /* Show result */
  fprintf(stderr, "%s\n", result);

  return EXIT_SUCCESS;
}

