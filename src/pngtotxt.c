/*
 * pngtotxt.c -- wrapper for libpng functions
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
 * @file   pngtotxt.c
 * @brief  PNG to text conversion
 * @author Copyright (C) 2020 Dr. Peter Zimmerer <pkzw@web.de>
 */

#include <config.h>
#include <assert.h>
#include <iconv.h>
#include <imgbuf.h>
#include <png.h>
#include <pngtotxt.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MINGW32__
# include <windows.h>
#endif

/*----------------------------------------------------------------------------*/
/*   Define symbols for black-and-white block graphics                        */
/*----------------------------------------------------------------------------*/

png_const_charp UTF8_BLOCK_ELEMENTS[4] =
  { "\xE2\x96\x88", // full block (█)
      "\xE2\x96\x80", // upper half block (▀)
      "\xE2\x96\x84", // lower half block (▄)
      "\xC2\xA0" };    // empty block (non-breaking space)

png_const_charp * BLOCK_SYMBOLS = NULL;

png_const_charp *
get_block_symbols ()
{

  const size_t buflen = 15;
  char buf[16] = "\0";

  if (BLOCK_SYMBOLS == NULL)
    {
      BLOCK_SYMBOLS = malloc (4 * sizeof(png_const_charp));
      const char * fromCs = "UTF-8";
#ifdef __MINGW32__
      CPINFOEXA cpinfo;
      char toCs[12] = "\0";
      UINT cpid = GetConsoleOutputCP();
      itoa (cpid, toCs, 10);
#else
      const char * toCs = "UTF-8";
#endif
      int rv = 0;
      iconv_t ic = NULL;
      ic = iconv_open (toCs, fromCs);
      if (ic == NULL)
	fprintf (
	    stderr,
	    "Cannot convert block element characters to current codepage %s\n",
	    toCs);
      for (size_t i = 0; i < 4; i++)
	{
	  if (ic != NULL)
	    {
	      const char * in = UTF8_BLOCK_ELEMENTS[i];
	      char * out = buf;
	      size_t inlen = strlen (UTF8_BLOCK_ELEMENTS[i]);
	      size_t outlen = buflen;
	      size_t len = iconv (ic, (char * *) &in, &inlen, &out, &outlen);
	      if (++len)
		{
		  out[0] = '\0';
		  len = buflen - outlen + 1;
		  BLOCK_SYMBOLS[i] = (png_const_charp) malloc (len);
		  memcpy ((png_charp) BLOCK_SYMBOLS[i], buf, len);
		  continue;
		}
	    }
	  BLOCK_SYMBOLS[i] = "?";
	}
      if (ic != NULL)
	{
	  if (!iconv_close (ic))
	    ic = NULL;
	}
      assert (BLOCK_SYMBOLS != NULL);
    }
  return BLOCK_SYMBOLS;
}

/*----------------------------------------------------------------------------*/
/*   Calculate greatest common divisor of two natural (unsigned) numbers      */
/*   Returns 0 if at least one of both numbers is 0.                          */
/*----------------------------------------------------------------------------*/
png_uint_32
gcd (png_uint_32 a, png_uint_32 b)
{
  if (a == b)
    return a;
  else if (a < b)
    return gcd (b, a);
  else
    {
      while (b != 0)
	{
	  png_uint_32 t = b;
	  b = a % b;
	  a = t;
	}
      return a;
    }
}

/*----------------------------------------------------------------------------*/
/*   Callback function to read a PNG data chunk from GWEN_BUFFER              */
/*----------------------------------------------------------------------------*/
void
read_data_cb (png_structp png_ptr, png_bytep out, size_t size)
{

  assert (png_ptr != NULL);
  png_voidp io_ptr = png_get_io_ptr (png_ptr);
  assert (io_ptr != NULL);
  Imgbuf * bf = (Imgbuf *) io_ptr;
  if (!Imgbuf_read (bf, out, size))
    png_error (png_ptr, "Premature end-of-data reached on image data");
  return;
}

/*----------------------------------------------------------------------------*/
/*   Read a pixel from two-dimensional bitmap                                 */
/*----------------------------------------------------------------------------*/
int
get_pixel (const png_bytepp row_pointers, png_uint_32 height, png_uint_32 width,
	   png_uint_32 i, png_uint_32 j)
{

  assert (row_pointers != NULL);

  if (i <= 0)
    i = 0;
  if (i >= height)
    i = height - 1;
  assert (row_pointers[i] != NULL);

  if (j <= 0)
    j = 0;
  if (j >= width)
    j = width - 1;

  return row_pointers[i][j / 8] >> (j % 8) & 0x01;
}

/*----------------------------------------------------------------------------*/
/*   Get possible horizontal scale factor from two-dimensional bitmap by      */
/*   analyzing distances between black-to-white and white-to-black            */
/*   transitions for each column                                              */
/*----------------------------------------------------------------------------*/
png_uint_32
get_horizontal_scale (const png_bytepp row_pointers, png_uint_32 width,
		      png_uint_32 height)
{

  png_uint_32 scale = 0;
  size_t jstart = 0;

  for (size_t i = 0; i < height; i++)
    {
      png_byte p = get_pixel (row_pointers, height, width, i, 0);
      for (size_t j = 1; j < width; j++)
	{
	  png_byte q = get_pixel (row_pointers, height, width, i, j);
	  if (p != q)
	    {
	      if (jstart == 0)
		jstart = j;
	      else
		{
		  if (j > jstart)
		    scale = gcd (j - jstart, scale);
		  else if (j < jstart)
		    {
		      scale = gcd (jstart - j, scale);
		      jstart = j;
		    }
		  if (scale == 1)
		    return scale;
		}
	      p = q;
	    }
	}
    }
  return scale ? scale : 1;
}

/*----------------------------------------------------------------------------*/
/*   Get possible vertical scale factor from two-dimensional bitmap by        */
/*   analyzing distances between black-to-white and white-to-black            */
/*   transitions for each column                                              */
/*----------------------------------------------------------------------------*/
png_uint_32
get_vertical_scale (const png_bytepp row_pointers, png_uint_32 width,
		    png_uint_32 height)
{

  png_uint_32 scale = 0;
  size_t istart = 0;

  for (size_t j = 0; j < width; j++)
    {
      png_byte p = get_pixel (row_pointers, height, width, 0, j);
      for (size_t i = 1; i < height; i++)
	{
	  png_byte q = get_pixel (row_pointers, height, width, i, j);
	  if (p != q)
	    {
	      if (istart == 0)
		istart = i;
	      else
		{
		  if (i > istart)
		    scale = gcd (i - istart, scale);
		  else if (i < istart)
		    {
		      scale = gcd (istart - i, scale);
		      istart = i;
		    }
		  if (scale == 1)
		    return scale;
		}
	      p = q;
	    }
	}
    }
  return scale ? scale : 1;
}

/*----------------------------------------------------------------------------*/
/*   Convert PNG image to a Cstring                                           */
/*   using block graphics                                                     */
/*----------------------------------------------------------------------------*/
char *
png_to_txt (png_const_bytep img, size_t imgSize)
{

  const size_t PNG_HEADER_BYTES = 8;

  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;

  assert (img);
  char * txt = NULL;
  if (imgSize <= PNG_HEADER_BYTES)
    return txt;

  if (!png_check_sig (img, PNG_HEADER_BYTES))
    return txt;

  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    return txt;

  if (setjmp (png_jmpbuf (png_ptr)))
    {
      png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
      return txt;
    }

  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
      return txt;
    }

  Imgbuf * imgbuf = Imgbuf_new (img, imgSize);
  png_set_read_fn (png_ptr, imgbuf, read_data_cb);

  png_read_info (png_ptr, info_ptr);

  png_byte color_type = png_get_color_type (png_ptr, info_ptr);
  png_byte bit_depth = png_get_bit_depth (png_ptr, info_ptr);

  /* Transform input image to 8-bit grayscale image */
  if (color_type & PNG_COLOR_MASK_PALETTE)
    png_set_palette_to_rgb (png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8 (png_ptr);
  if (bit_depth == 16)
    png_set_strip_16 (png_ptr);
  if (color_type & (PNG_COLOR_MASK_PALETTE | PNG_COLOR_MASK_COLOR))
    png_set_rgb_to_gray_fixed (png_ptr, PNG_ERROR_ACTION_NONE,
			       PNG_RGB_TO_GRAY_DEFAULT,
			       PNG_RGB_TO_GRAY_DEFAULT);
  if (color_type & (PNG_COLOR_MASK_PALETTE | PNG_COLOR_MASK_ALPHA))
    png_set_strip_alpha (png_ptr);

  png_read_update_info (png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width (png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height (png_ptr, info_ptr);
  size_t rowbytes = png_get_rowbytes (png_ptr, info_ptr);
  png_bytep row = (png_bytep) malloc (rowbytes * sizeof(png_byte));

  rowbytes = (width + 7) / 8;
  size_t sz = height * rowbytes;
  png_bytep rows = (png_bytep) malloc (sz * sizeof(png_byte));
  for (size_t k = 0; k < sz; k++)
    rows[k] = 0;

  png_bytepp row_pointers = (png_bytepp) malloc (height * sizeof(png_bytep));
  if (row_pointers == NULL)
    png_error (png_ptr, "Could not allocate memory for row_pointers");
  for (size_t i = 0; i < height; i++)
    {
      row_pointers[i] = rows + (i * rowbytes);
      png_read_row (png_ptr, (png_bytep) row, NULL);
      for (size_t j = 0; j < rowbytes; j++)
	{
	  png_byte b = 0;
	  for (size_t k = 0; k < 8; k++)
	    {
	      b = (row[8 * j + k] & 0x80) | (b >> 1);
	    }
	  row_pointers[i][j] = b;
	}
    }
  png_read_end (png_ptr, info_ptr);

  if (imgbuf != NULL)
    {
      Imgbuf_free (imgbuf);
      imgbuf = NULL;
    }

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  png_uint_32 hscale = get_horizontal_scale (row_pointers, width, height);
  png_uint_32 vscale = get_vertical_scale (row_pointers, width, height);
  size_t txt_size = ((height + vscale - 1) / vscale)
      * (3 * ((width + hscale - 1) / hscale) + 1);
  txt = (char *) malloc (txt_size * sizeof(char));
  char * p = txt;
  png_const_charp * symbols = get_block_symbols ();
  for (size_t i = 0; i < height; i += 2 * vscale)
    {
      for (size_t j = 0; j < width; j += hscale)
	{
	  size_t sym_index = 2 * get_pixel (row_pointers, height, width, i, j)
	      + get_pixel (row_pointers, height, width, i + vscale, j);
	  size_t len = strlen (symbols[sym_index]);
	  assert (p + len + 2 <= txt + txt_size);
	  memcpy (p, symbols[sym_index], len);
	  p += len;
	}
      memcpy (p++, "\n", 1);
    }
  p[0] = '\0';

  if (row_pointers != NULL)
    {
      free (row_pointers);
      row_pointers = NULL;
    }

  if (rows != NULL)
    {
      free (rows);
      rows = NULL;
    }
  return txt;
}
