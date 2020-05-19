/*
 * main.c -- main entry point of pngtotxt
 *
 * This file is part of pngtotxt.
 *
 * pngtotxt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pngtotxt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pngtotxt.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file   main.c
 * @brief  PNG to text conversion
 * @author Copyright (C) 2020 Dr. Peter Zimmerer <pkzw@web.de>
 */

#include <assert.h>
#include <config.h>
#include <getopt.h>
#include <pngtotxt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char * PROGRAM_NAME = PROJECT_NAME;

void
usage (const char *arg0)
{
  fprintf (
      stderr,
      "Usage: %s [OPTIONS] [CTY] IFN\n"
      "This program converts a PNG image to text using block graphic symbols.\n"
      "\n"
      "Options:\n"
      "  -a, --append      Append text to output file\n"
      "  -h, --help        Display detailed help information\n"
      "  -o, --output=OFN  Output file name (default is standard output)\n"
      "  -v, --version     Display version information\n"
      "\n"
      "CTY is the content-type of the input file (Default is 'image/png').\n"
      "IFN is the name of the input file (required).\n"
      "OFN is the name of the output file.\n",
      PROGRAM_NAME);
  exit (4);
}

size_t
get_filesize (FILE *fh)
{
  off_t pos = ftello (fh);
  fseeko (fh, 0, SEEK_END);
  off_t size = ftello (fh);
  fseeko (fh, pos, SEEK_SET);
  return size;
}

int
main (int argc, char *argv[])
{
  int append = 0;
  const char *content_type = "image/png";
  const char *ifn, *ofn = "";
  const struct option longopts[] =
    {
      { "append", no_argument, NULL, 'a' },
      { "help", no_argument, NULL, 'h' },
      { "output", required_argument, NULL, 'o' },
      { "version", no_argument, NULL, 'v' } };
  FILE *fin, *fout;
  png_bytep buf;
  size_t size;
  int c;
  int optidx = 0;

  opterr = 0;
  while ((c = getopt_long (argc, argv, "aho:v", longopts, &optidx)) >= 0)
    {
      switch (c)
	{
	case 'a':
	  append = 1;
	  break;
	case 'o':
	  ofn = optarg;
	  break;
	case 'v':
	  fprintf (stderr, "pngtotxt %s\n", VERSION);
	  exit (0);
	case '?':
	  fprintf (stderr, "Output file name missing\n");
	  exit (8);
	default:
	  usage (argv[0]);
	}
    }

  if (argc - optind > 2)
    usage (argv[0]);
  else
    {
      if (argc - optind == 2)
	if (strcmp (argv[optind], content_type) == 0)
	  optind++;
	else
	  {
	    fprintf (stderr, "Content type '%s' is not supported\n",
		     argv[optind]);
	    exit (8);
	  }
      if (optind < argc)
	ifn = argv[optind];
      else
	{
	  fprintf (stderr, "Input file name missing.\n"
		   "Try 'pngtotxt --help' for more information.\n");
	  exit (8);
	}
    }

  fin = fopen (ifn, "rb");
  if (fin == NULL)
    {
      fprintf (stderr, "Could not open input file '%s'\n", ifn);
      exit (8);
    }

  size = get_filesize (fin);
  if (size == 0)
    {
      fprintf (stderr, "Input file '%s' must not be empty\n", argv[2]);
      fclose (fin);
      exit (8);
    }

  if (strcmp (ofn, "") == 0)
    fout = stdout;
  else
    {
      fout = fopen (ofn, append ? "a" : "w");
      if (fout == NULL)
	{
	  fclose (fin);
	  fprintf (stderr, "Could not open output file '%s'\n", ofn);
	  exit (8);
	}
    }

  buf = (png_bytep) malloc (size);
  fread (buf, size, 1, fin);
  fclose (fin);

  const char * txt = png_to_txt (buf, size);
  if (txt != NULL)
    {
      fprintf (fout, "%s", txt);
      free ((void *) txt);
      txt = NULL;
    }

  if (fout != stdout)
    fclose (fout);

  return 0;
}
