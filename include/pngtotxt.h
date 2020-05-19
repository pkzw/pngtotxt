/*
 * pngtotxt.h -- wrapper for libpng functions
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
 * @file   pngtotxt.h
 * @brief  PNG to text conversion
 * @author Copyright (C) 2020 Dr. Peter Zimmerer <pkzw@web.de>
 */

#include <png.h>

char * png_to_txt(png_const_bytep img, png_size_t len);

