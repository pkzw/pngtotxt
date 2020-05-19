/*
 * imgbuf.h -- buffer for PNG image data
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
 * @file   imgbuf.h
 * @brief  PNG to text conversion
 * @author Copyright (C) 2020 Dr. Peter Zimmerer <pkzw@web.de>
 */
 
#include <config.h>
#include <png.h>

typedef struct {
    png_const_bytep ptr;
    png_size_t      pos;
    png_size_t      size;
} Imgbuf;


Imgbuf * Imgbuf_new(png_const_bytep img, png_size_t size);

void *   Imgbuf_free(Imgbuf * bf);

int      Imgbuf_read(Imgbuf * bf, png_bytep out, png_size_t size);
