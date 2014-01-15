/*
 * Copyright (c) 2014 Manuel Braga <mul.braga@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "vepoc.h"

void veisp_set_buffers(uint8_t *Y, uint8_t *C)
{
	uint32_t pY = ve_virt2phys(Y) + 0x40000000;
	uint32_t pC = ve_virt2phys(C) + 0x40000000;
	S(pY, VE_ISP_PIC_LUMA);
	S(pC, VE_ISP_PIC_CHROMA);
}

inline void veisp_set_picture_size(uint32_t w, uint32_t h)
{
	uint32_t width_mb  = (w + 15) / 16;
	uint32_t height_mb = (h + 15) / 16;
	uint32_t size  = ((width_mb & 0x3ff) << 16) | (height_mb & 0x3ff);
	uint32_t stride = (width_mb & 0x3ff) << 16;

	S(size	, VE_ISP_PIC_SIZE);
	S(stride, VE_ISP_PIC_STRIDE);
}

void veisp_init_picture(uint32_t w, uint32_t h, veisp_color_format f)
{
	uint32_t format = (f & 0xf) << 28;
	veisp_set_picture_size(w, h);

	S(format, VE_ISP_CTRL);
}

