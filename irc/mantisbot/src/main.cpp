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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "config.h"
#include "mantisbot.h"

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif

void ShowHelp(int argc, char* argv[])
{
  fprintf(stdout, PACKAGE_STRING " <" PACKAGE_BUGREPORT ">\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Usage: %s [options]\n\n", argv[0]);
  fprintf(stdout, "Options are:\n");
  fprintf(stdout, "\t-h|--help\tShows this help\n");
  fprintf(stdout, "\t-f|--conffile\tUse a specific config file\n");
  fprintf(stdout, "\t-v|--verbose\tVerbose mode\n");
}

static void sighandler(int signum)
{
  switch (signum)
  {
    case SIGINT:
    {
      debugentry("Got SIGINT, closing bot");
      MantisBot* bot = MantisBot::Instance();
      bot->Close("Got SIGINT! Closing...");
      delete bot;
      debugok();
      exit(EXIT_SUCCESS);
      break;
    }
    case SIGPIPE:
      break;
    case SIGSEGV:
    {
      debugentry("Got segmentation fault, dumping core and closing bot");
      int p = getpid();
      if (fork() > 0)
      {
        kill(p, 3);
        kill(p, 9);
      }

      MantisBot* bot = MantisBot::Instance();
      bot->Close("Segmentation Fault! Core dumped");
      delete bot;
      debugok();
      exit(EXIT_SUCCESS);
      break;
    }
  }
}

void debugentry(const char* message, ...)
{
  va_list vl;
  char msg[2048];

  va_start(vl, message);
  vsnprintf(msg, sizeof(msg), message, vl);
  va_end(vl);

  printf("\033[1;32m*\033[;0m %s... ", msg);
  fflush(stdout);
}

void debugok()
{
  puts("\033[1;34m[\033[1;32m OK \033[1;34m]\033[;0m");
}

void debugerror()
{
  puts("\033[1;31m[\033[1;33m ERROR \033[1;31m]\033[;0m");
}

int main(int argc, char* argv[], char* environment[])
{
#if defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_SETRLIMIT)
  debugentry("Setting the core size limit");
  /* Set the core limit size */
  rlimit corelimit;

  if (getrlimit(RLIMIT_CORE, &corelimit))
  {
    printf("Warning: Couldnt get the core size limit, defaulting to infinity: %s\n", strerror(errno));
    corelimit.rlim_max = RLIM_INFINITY;
    corelimit.rlim_cur = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &corelimit);
  }
  else
  {
    corelimit.rlim_cur = corelimit.rlim_max;
    setrlimit(RLIMIT_CORE, &corelimit);
    debugok();
  }
#endif

  /* Handle signals */
  debugentry("Handling signals");
  signal(SIGINT, sighandler);
//  signal(SIGSEGV, sighandler);
  signal(SIGPIPE, sighandler);
  debugok();

  debugentry("Creating the bot instance");
  MantisBot* bot = MantisBot::Instance();
  debugok();

  debugentry("Initializing bot");
  if (!bot->Initialize(argc, argv, environment))
  {
    fprintf(stderr, "Error initializing the bot:\n%s\n", bot->Error());
    return EXIT_FAILURE;
  }
  debugok();

  debugentry("Running bot");
  if (!bot->Run())
  {
    fprintf(stderr, "Error running the bot:\n%s\n", bot->Error());
    return EXIT_FAILURE;
  }
  debugok();

  delete bot;
  return EXIT_SUCCESS;
}
