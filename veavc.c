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

static veavc_encoder_mode encoder_mode = VEAVC_ENCODER_MODE_H264;

void veavc_select_subengine(void)
{
	uint32_t ctrl = L(VE_CTRL);
	ctrl = (ctrl & 0xfffffff0) | 0xb;
	S(ctrl, VE_CTRL);
}

void veavc_init_vle(uint8_t *J, uint32_t size)
{
	uint32_t pJ = ve_virt2phys(J);
	uint32_t end = pJ + size - 1;
	S(pJ	    , VE_AVC_VLE_ADDR);
	S(end	    , VE_AVC_VLE_END);
	S(0	    , VE_AVC_VLE_OFFSET);
	S(0x04000000, 0xb8c);
}

void veavc_init_ctrl(veavc_encoder_mode mode)
{
	uint32_t trigger = (mode & 1) << 16;
	uint32_t status;
	encoder_mode = mode;

	S(0x0000000f  , VE_AVC_CTRL);
	S(trigger     , VE_AVC_TRIGGER);
	
	status = L(VE_AVC_STATUS);
	S(status | 0x7, VE_AVC_STATUS);
}

void veavc_jpeg_parameters(uint8_t fill1, uint8_t stuff, uint32_t biasY, uint32_t biasC)
{
	uint32_t valfill1 = fill1 > 0 ? 1 : 0;
	uint32_t valstuff = stuff > 0 ? 1 : 0;
	uint32_t value = 0;
	value |= (valfill1 & 1) << 31;
	value |= (valstuff & 1) << 30;
	value |= (biasC & 0x7ff) << 16;
	value |= (biasY & 0x7ff) << 0;
	S(value, VE_AVC_PARAM);
}

void veavc_put_bits(uint8_t nbits, uint32_t data)
{
	uint32_t trigger = (encoder_mode & 1) << 16;
	trigger |= (nbits & 0x3f) << 8;
	trigger |= 1;
	S(data   , VE_AVC_BASIC_BITS);
	S(trigger, VE_AVC_TRIGGER);

	/* status register is unknown */
	L(VE_AVC_STATUS);
}

void veavc_sdram_index(uint32_t index)
{
	S(index, VE_AVC_SDRAM_INDEX);
}

void veavc_jpeg_quantization(uint16_t *tableY, uint16_t *tableC, uint32_t length)
{
	uint32_t data;
	int i;

	veavc_sdram_index(0x0);

/*
	When compared to libjpeg, there are still rounding errors in the
	coefficients values (around 1 unit of difference).
*/
	for(i = 0; i < length; i++)
	{
		data  = 0x0000ffff & (0xffff / tableY[i]);
		data |= 0x00ff0000 & (((tableY[i] + 1) / 2) << 16);
		S(data, VE_AVC_SDRAM_DATA);
	}
	for(i = 0; i < length; i++)
	{
		data  = 0x0000ffff & (0xffff / tableC[i]);
		data |= 0x00ff0000 & (((tableC[i] + 1) / 2) << 16);
		S(data, VE_AVC_SDRAM_DATA);
	}
}

void veavc_launch_encoding(void)
{
	uint32_t trigger = (encoder_mode & 1) << 16;
	trigger |= 8;
	S(trigger, VE_AVC_TRIGGER);
}

void veavc_check_status(void)
{
	L(VE_AVC_STATUS);
}

uint32_t veavc_get_written(void)
{
	uint32_t bits = L(VE_AVC_VLE_LENGTH);
	return bits / 8;
}

