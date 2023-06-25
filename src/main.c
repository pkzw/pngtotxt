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

const char *PROGRAM_NAME = PROJECT_NAME;
const size_t BUF_INCR = 32768;

void usage(const char *arg0)
{
	fprintf(
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
					"IFN is the name of the input file (required). Use '-' or 'stdin' to read from standard input.\n"
					"OFN is the name of the output file.\n", PROGRAM_NAME);
	exit(4);
}

int main(int argc, char *argv[])
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
	int c;
	int optidx = 0;

	opterr = 0;
	while ((c = getopt_long(argc, argv, "aho:v", longopts, &optidx)) >= 0)
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
			fprintf(stderr, "pngtotxt %s\n", VERSION);
			exit(0);
		case '?':
			fprintf(stderr, "Output file name missing\n");
			exit(8);
		default:
			usage(argv[0]);
		}
	}

	if (argc - optind > 2)
		usage(argv[0]);
	else
	{
		if (argc - optind == 2)
			if (strcmp(argv[optind], content_type) == 0)
				optind++;
			else
			{
				fprintf(stderr, "Content type '%s' is not supported\n",
						argv[optind]);
				exit(8);
			}
		if (optind < argc)
			ifn = argv[optind];
		else
		{
			fprintf(stderr, "Input file name missing.\n"
					"Try 'pngtotxt --help' for more information.\n");
			exit(8);
		}
	}

	// Open input file
	if (!strcmp(ifn, "-") || !strcmp(ifn, "stdin"))
		fin = stdin;
	else
	{
		fin = fopen(ifn, "rb");
		if (fin == NULL)
		{
			fprintf(stderr, "Could not open input file '%s'\n", ifn);
			exit(8);
		}
	}

	// Read PNG image from input file
	size_t buf_size = BUF_INCR, png_size = 0, sz;
	png_bytep buf = malloc(buf_size * sizeof(*buf));
	while ((sz = fread(buf + png_size, sizeof(*buf), buf_size, fin)) > 0)
	{
		png_size += sz;
		if (png_size < buf_size)
			break;
		buf_size += BUF_INCR;
		assert(png_size < buf_size);
		png_bytep new_buf = realloc(buf, buf_size * sizeof(*buf));
		if (new_buf == NULL)
		{
			fprintf(stderr, "Could not enlarge buffer to size %d bytes\n",
					buf_size * sizeof(*buf));
			free(buf);
			buf = NULL;
			break;
		}
		buf = new_buf;
	}

	// Close input file
	if (fin != stdin)
	{
		fclose(fin);
		fin = NULL;
	}

	if (buf == NULL)
	{
		fprintf(stderr, "Reading from input file '%s' has failed\n", ifn);
		exit(8);
	}

	if (png_size == 0)
	{
		fprintf(stderr, "Input file '%s' must not be empty\n", ifn);
		free(buf);
		buf = NULL;
		exit(8);
	}
	assert(png_size < buf_size);
	buf_size = png_size;
	buf = realloc(buf, buf_size * sizeof(*buf));

	const char *txt = png_to_txt(buf, png_size);

	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}

	if (txt != NULL)
	{

		if (strcmp(ofn, "") == 0)
			fout = stdout;
		else
		{
			fout = fopen(ofn, append ? "a" : "w");
			if (fout == NULL)
			{
				fclose(fin);
				fprintf(stderr, "Could not open output file '%s'\n", ofn);
				exit(8);
			}
		}
		fprintf(fout, "%s", txt);
		if (fout != stdout)
			fclose(fout);

		free((void*) txt);
		txt = NULL;
	}

	return 0;
}
