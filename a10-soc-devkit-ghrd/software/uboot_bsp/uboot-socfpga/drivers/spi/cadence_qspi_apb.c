/*
 * Copyright (C) 2012 Altera Corporation <www.altera.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of the Altera Corporation nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL ALTERA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include "cadence_qspi.h"

#define CQSPI_REG_POLL_US			(1) /* 1us */
#define CQSPI_REG_RETRY				(10000)
#define CQSPI_POLL_IDLE_RETRY			(3)

#define CQSPI_FIFO_WIDTH			(4)

/* Controller sram size in word */
#define CQSPI_REG_SRAM_SIZE_WORD		(128)
#define CQSPI_REG_SRAM_RESV_WORDS		(2)
#define CQSPI_REG_SRAM_PARTITION_WR		(1)
#define CQSPI_REG_SRAM_PARTITION_RD		\
	(CQSPI_REG_SRAM_SIZE_WORD - CQSPI_REG_SRAM_RESV_WORDS)
#define CQSPI_REG_SRAM_THRESHOLD_WORDS		(50)

/* Transfer mode */
#define CQSPI_INST_TYPE_SINGLE			(0)
#define CQSPI_INST_TYPE_DUAL			(1)
#define CQSPI_INST_TYPE_QUAD			(2)

#define CQSPI_STIG_DATA_LEN_MAX			(8)
#define CQSPI_INDIRECTTRIGGER_ADDR_MASK		(0xFFFFF)

#define CQSPI_DUMMY_CLKS_PER_BYTE		(8)
#define CQSPI_DUMMY_BYTES_MAX			(4)


#define CQSPI_REG_SRAM_FILL_THRESHOLD	\
	((CQSPI_REG_SRAM_SIZE_WORD / 2) * CQSPI_FIFO_WIDTH)
/****************************************************************************
 * Controller's configuration and status register (offset from QSPI_BASE)
 ****************************************************************************/
#define	CQSPI_REG_CONFIG			0x00
#define	CQSPI_REG_CONFIG_CLK_POL_LSB		1
#define	CQSPI_REG_CONFIG_CLK_PHA_LSB		2
#define	CQSPI_REG_CONFIG_ENABLE_MASK		(1 << 0)
#define	CQSPI_REG_CONFIG_DIRECT_MASK		(1 << 7)
#define	CQSPI_REG_CONFIG_DECODE_MASK		(1 << 9)
#define	CQSPI_REG_CONFIG_XIP_IMM_MASK		(1 << 18)
#define	CQSPI_REG_CONFIG_CHIPSELECT_LSB		10
#define	CQSPI_REG_CONFIG_BAUD_LSB		19
#define	CQSPI_REG_CONFIG_IDLE_LSB		31
#define	CQSPI_REG_CONFIG_CHIPSELECT_MASK	0xF
#define	CQSPI_REG_CONFIG_BAUD_MASK		0xF

#define	CQSPI_REG_RD_INSTR			0x04
#define	CQSPI_REG_RD_INSTR_OPCODE_LSB		0
#define	CQSPI_REG_RD_INSTR_TYPE_INSTR_LSB	8
#define	CQSPI_REG_RD_INSTR_TYPE_ADDR_LSB	12
#define	CQSPI_REG_RD_INSTR_TYPE_DATA_LSB	16
#define	CQSPI_REG_RD_INSTR_MODE_EN_LSB		20
#define	CQSPI_REG_RD_INSTR_DUMMY_LSB		24
#define	CQSPI_REG_RD_INSTR_TYPE_INSTR_MASK	0x3
#define	CQSPI_REG_RD_INSTR_TYPE_ADDR_MASK	0x3
#define	CQSPI_REG_RD_INSTR_TYPE_DATA_MASK	0x3
#define	CQSPI_REG_RD_INSTR_DUMMY_MASK		0x1F

#define	CQSPI_REG_WR_INSTR			0x08
#define	CQSPI_REG_WR_INSTR_OPCODE_LSB		0

#define	CQSPI_REG_DELAY				0x0C
#define	CQSPI_REG_DELAY_TSLCH_LSB		0
#define	CQSPI_REG_DELAY_TCHSH_LSB		8
#define	CQSPI_REG_DELAY_TSD2D_LSB		16
#define	CQSPI_REG_DELAY_TSHSL_LSB		24
#define	CQSPI_REG_DELAY_TSLCH_MASK		0xFF
#define	CQSPI_REG_DELAY_TCHSH_MASK		0xFF
#define	CQSPI_REG_DELAY_TSD2D_MASK		0xFF
#define	CQSPI_REG_DELAY_TSHSL_MASK		0xFF

#define	CQSPI_READLCAPTURE			0x10
#define	CQSPI_READLCAPTURE_BYPASS_LSB		0
#define	CQSPI_READLCAPTURE_DELAY_LSB		1
#define	CQSPI_READLCAPTURE_DELAY_MASK		0xF

#define	CQSPI_REG_SIZE				0x14
#define	CQSPI_REG_SIZE_ADDRESS_LSB		0
#define	CQSPI_REG_SIZE_PAGE_LSB			4
#define	CQSPI_REG_SIZE_BLOCK_LSB		16
#define	CQSPI_REG_SIZE_ADDRESS_MASK		0xF
#define	CQSPI_REG_SIZE_PAGE_MASK		0xFFF
#define	CQSPI_REG_SIZE_BLOCK_MASK		0x3F

#define	CQSPI_REG_SRAMPARTITION			0x18
#define	CQSPI_REG_INDIRECTTRIGGER		0x1C

#define	CQSPI_REG_REMAP				0x24
#define	CQSPI_REG_MODE_BIT			0x28

#define	CQSPI_REG_SDRAMLEVEL			0x2C
#define	CQSPI_REG_SDRAMLEVEL_RD_LSB		0
#define	CQSPI_REG_SDRAMLEVEL_WR_LSB		16
#define	CQSPI_REG_SDRAMLEVEL_RD_MASK		0xFFFF
#define	CQSPI_REG_SDRAMLEVEL_WR_MASK		0xFFFF

#define	CQSPI_REG_IRQSTATUS			0x40
#define	CQSPI_REG_IRQMASK			0x44

#define	CQSPI_REG_INDIRECTRD			0x60
#define	CQSPI_REG_INDIRECTRD_START_MASK		(1 << 0)
#define	CQSPI_REG_INDIRECTRD_CANCEL_MASK	(1 << 1)
#define	CQSPI_REG_INDIRECTRD_INPROGRESS_MASK	(1 << 2)
#define	CQSPI_REG_INDIRECTRD_DONE_MASK		(1 << 5)

#define	CQSPI_REG_INDIRECTRDWATERMARK		0x64
#define	CQSPI_REG_INDIRECTRDSTARTADDR		0x68
#define	CQSPI_REG_INDIRECTRDBYTES		0x6C

#define	CQSPI_REG_CMDCTRL			0x90
#define	CQSPI_REG_CMDCTRL_EXECUTE_MASK		(1 << 0)
#define	CQSPI_REG_CMDCTRL_INPROGRESS_MASK	(1 << 1)
#define	CQSPI_REG_CMDCTRL_DUMMY_LSB		7
#define	CQSPI_REG_CMDCTRL_WR_BYTES_LSB		12
#define	CQSPI_REG_CMDCTRL_WR_EN_LSB		15
#define	CQSPI_REG_CMDCTRL_ADD_BYTES_LSB		16
#define	CQSPI_REG_CMDCTRL_ADDR_EN_LSB		19
#define	CQSPI_REG_CMDCTRL_RD_BYTES_LSB		20
#define	CQSPI_REG_CMDCTRL_RD_EN_LSB		23
#define	CQSPI_REG_CMDCTRL_OPCODE_LSB		24
#define	CQSPI_REG_CMDCTRL_DUMMY_MASK		0x1F
#define	CQSPI_REG_CMDCTRL_WR_BYTES_MASK		0x7
#define	CQSPI_REG_CMDCTRL_ADD_BYTES_MASK	0x3
#define	CQSPI_REG_CMDCTRL_RD_BYTES_MASK		0x7
#define	CQSPI_REG_CMDCTRL_OPCODE_MASK		0xFF

#define	CQSPI_REG_INDIRECTWR			0x70
#define	CQSPI_REG_INDIRECTWR_START_MASK		(1 << 0)
#define	CQSPI_REG_INDIRECTWR_CANCEL_MASK	(1 << 1)
#define	CQSPI_REG_INDIRECTWR_INPROGRESS_MASK	(1 << 2)
#define	CQSPI_REG_INDIRECTWR_DONE_MASK		(1 << 5)

#define	CQSPI_REG_INDIRECTWRWATERMARK		0x74
#define	CQSPI_REG_INDIRECTWRSTARTADDR		0x78
#define	CQSPI_REG_INDIRECTWRBYTES		0x7C

#define	CQSPI_REG_CMDADDRESS			0x94
#define	CQSPI_REG_CMDREADDATALOWER		0xA0
#define	CQSPI_REG_CMDREADDATAUPPER		0xA4
#define	CQSPI_REG_CMDWRITEDATALOWER		0xA8
#define	CQSPI_REG_CMDWRITEDATAUPPER		0xAC

#define CQSPI_REG_IS_IDLE(base)					\
	((CQSPI_READL(base + CQSPI_REG_CONFIG) >>		\
		CQSPI_REG_CONFIG_IDLE_LSB) & 0x1)

#define CQSPI_CAL_DELAY(tdelay_ns, tref_ns, tsclk_ns)		\
	((((tdelay_ns) - (tsclk_ns)) / (tref_ns)))

#define CQSPI_GET_RD_SRAM_LEVEL(reg_basse)			\
	(((CQSPI_READL(reg_base + CQSPI_REG_SDRAMLEVEL)) >>	\
	CQSPI_REG_SDRAMLEVEL_RD_LSB) & CQSPI_REG_SDRAMLEVEL_RD_MASK)

#define CQSPI_GET_WR_SRAM_LEVEL(reg_basse)			\
	(((CQSPI_READL(reg_base + CQSPI_REG_SDRAMLEVEL)) >>	\
	CQSPI_REG_SDRAMLEVEL_WR_LSB) & CQSPI_REG_SDRAMLEVEL_WR_MASK)

static unsigned int cadence_qspi_apb_cmd2addr(const unsigned char *addr_buf,
	unsigned int addr_width)
{
	unsigned int addr;

	addr = (addr_buf[0] << 16) | (addr_buf[1] << 8) | addr_buf[2];

	if (addr_width == 4)
		addr = (addr << 8) | addr_buf[3];

	return addr;
}

static void cadence_qspi_apb_read_fifo_data(void *dest,
	const void *src_ahb_addr, unsigned int bytes)
{
	unsigned int temp;
	int remaining = bytes;
	unsigned int *dest_ptr = (unsigned int *)dest;
	unsigned int *src_ptr = (unsigned int *)src_ahb_addr;

	while (remaining > 0) {
		if (remaining >= CQSPI_FIFO_WIDTH) {
			*dest_ptr = CQSPI_READL(src_ptr);
			remaining -= CQSPI_FIFO_WIDTH;
		} else {
			/* dangling bytes */
			temp = CQSPI_READL(src_ptr);
			memcpy(dest_ptr, &temp, remaining);
			break;
		}
		dest_ptr++;
	}

	return;
}

static void cadence_qspi_apb_write_fifo_data(const void *dest_ahb_addr,
	const void *src, unsigned int bytes)
{
	unsigned int temp;
	int remaining = bytes;
	unsigned int *dest_ptr = (unsigned int *)dest_ahb_addr;
	unsigned int *src_ptr = (unsigned int *)src;

	while (remaining > 0) {
		if (remaining >= CQSPI_FIFO_WIDTH) {
			CQSPI_WRITEL(*src_ptr, dest_ptr);
			remaining -= sizeof(unsigned int);
		} else {
			/* dangling bytes */
			memcpy(&temp, src_ptr, remaining);
			CQSPI_WRITEL(temp, dest_ptr);
			break;
		}
		src_ptr++;
	}

	return;
}

/* Read from SRAM FIFO with polling SRAM fill level. */
static int qspi_read_sram_fifo_poll(const void *reg_base, void *dest_addr,
			const void *src_addr,  unsigned int num_bytes)
{
	unsigned int remaining = num_bytes;
	unsigned int retry;
	unsigned int sram_level = 0;
	unsigned char *dest = (unsigned char *)dest_addr;

	while (remaining > 0) {
		retry = CQSPI_REG_RETRY;
		while (retry--) {
			sram_level = CQSPI_GET_RD_SRAM_LEVEL(reg_base);
			if (sram_level)
				break;
			udelay(1);
		}

		if (!retry) {
			printf("QSPI: No receive data after polling for %d "
				"times\n", CQSPI_REG_RETRY);
			return -1;
		}

		sram_level *= CQSPI_FIFO_WIDTH;
		sram_level = sram_level > remaining ? remaining : sram_level;

		/* Read data from FIFO. */
		cadence_qspi_apb_read_fifo_data(dest, src_addr, sram_level);
		dest += sram_level;
		remaining -= sram_level;
		udelay(1);
	}
	return 0;
}


/* Write to SRAM FIFO with polling SRAM fill level. */
static int qpsi_write_sram_fifo_push(const void *reg_base, void *dest_addr,
				const void *src_addr, unsigned int num_bytes)
{
	unsigned int retry = CQSPI_REG_RETRY;
	unsigned int sram_level;
	unsigned int wr_bytes;
	unsigned char* src = (unsigned char *)src_addr;
	int remaining = num_bytes;
	unsigned int page_size = CONFIG_CQSPI_PAGE_SIZE;
	unsigned int sram_threshold_words = CQSPI_REG_SRAM_THRESHOLD_WORDS;

	while (remaining > 0) {
		retry = CQSPI_REG_RETRY;
		while (retry--) {
			sram_level = CQSPI_GET_WR_SRAM_LEVEL(reg_base);
			if (sram_level <= sram_threshold_words)
				break;
		}
		if (!retry) {
			printf("QSPI: SRAM fill level (0x%08x) "
				"not hit lower expected level (0x%08x)",
				sram_level, sram_threshold_words);
			return -1;
		}
		/* Write a page or remaining bytes. */
		wr_bytes = (remaining > page_size) ?
					page_size : remaining;

		cadence_qspi_apb_write_fifo_data(dest_addr, src, wr_bytes);
		src += wr_bytes;
		remaining -= wr_bytes;
	}

	return 0;
}

void cadence_qspi_apb_controller_enable(void *reg_base)
{
	unsigned int reg;
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg |= CQSPI_REG_CONFIG_ENABLE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	return;
}

void cadence_qspi_apb_controller_disable(void *reg_base)
{
	unsigned int reg;
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~CQSPI_REG_CONFIG_ENABLE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	return;
}

/* Return 1 if idle, otherwise return 0 (busy). */
static unsigned int cadence_qspi_wait_idle(void *reg_base)
{
	unsigned int start, count = 0;
	/* timeout in unit of ms */
	unsigned int timeout = 5000;

	start = get_timer(0);
	for ( ; get_timer(start) < timeout ; ) {
		if (CQSPI_REG_IS_IDLE(reg_base))
			count++;
		else
			count = 0;
		/*
		 * Ensure the QSPI controller is in true idle state after
		 * reading back the same idle status consecutively
		 */
		if (count >= CQSPI_POLL_IDLE_RETRY)
			return 1;
	}

	/* Timeout, still in busy mode. */
	printf("QSPI: QSPI is still busy after poll for %d times.\n",
		CQSPI_REG_RETRY);
	return 0;
}

void cadence_qspi_apb_readdata_capture(void *reg_base,
				unsigned int bypass, unsigned int delay)
{
	unsigned int reg;
	cadence_qspi_apb_controller_disable(reg_base);

	reg = CQSPI_READL(reg_base + CQSPI_READLCAPTURE);

	if (bypass)
		reg |= (1 << CQSPI_READLCAPTURE_BYPASS_LSB);
	else
		reg &= ~(1 << CQSPI_READLCAPTURE_BYPASS_LSB);

	reg &= ~(CQSPI_READLCAPTURE_DELAY_MASK
		<< CQSPI_READLCAPTURE_DELAY_LSB);

	reg |= ((delay & CQSPI_READLCAPTURE_DELAY_MASK)
		<< CQSPI_READLCAPTURE_DELAY_LSB);

	CQSPI_WRITEL(reg, reg_base + CQSPI_READLCAPTURE);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

void cadence_qspi_apb_config_baudrate_div(void *reg_base,
	unsigned int ref_clk_hz, unsigned int sclk_hz)
{
	unsigned int reg;
	unsigned int div;

	cadence_qspi_apb_controller_disable(reg_base);
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(CQSPI_REG_CONFIG_BAUD_MASK << CQSPI_REG_CONFIG_BAUD_LSB);

	div = ref_clk_hz / sclk_hz;

	if (div > 32)
		div = 32;

	/* Check if even number. */
	if ((div & 1))
		div = (div / 2);
	else {
		if (ref_clk_hz % sclk_hz)
			/* ensure generated SCLK doesn't exceed user
			specified sclk_hz */
			div = (div / 2);
		else
			div = (div / 2) - 1;
	}

	debug("%s: ref_clk %dHz sclk %dHz Div 0x%x\n", __func__,
		ref_clk_hz, sclk_hz, div);

	div = (div & CQSPI_REG_CONFIG_BAUD_MASK) << CQSPI_REG_CONFIG_BAUD_LSB;
	reg |= div;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

void cadence_qspi_apb_set_clk_mode(void *reg_base,
	unsigned int clk_pol, unsigned int clk_pha)
{
	unsigned int reg;

	cadence_qspi_apb_controller_disable(reg_base);
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(1 <<
		(CQSPI_REG_CONFIG_CLK_POL_LSB | CQSPI_REG_CONFIG_CLK_PHA_LSB));

	reg |= ((clk_pol & 0x1) << CQSPI_REG_CONFIG_CLK_POL_LSB);
	reg |= ((clk_pha & 0x1) << CQSPI_REG_CONFIG_CLK_PHA_LSB);

	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

void cadence_qspi_apb_chipselect(void *reg_base,
	unsigned int chip_select, unsigned int decoder_enable)
{
	unsigned int reg;

	cadence_qspi_apb_controller_disable(reg_base);

	debug("%s : chipselect %d decode %d\n", __func__, chip_select,
		decoder_enable);

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	/* docoder */
	if (decoder_enable)
		reg |= CQSPI_REG_CONFIG_DECODE_MASK;
	else {
		reg &= ~CQSPI_REG_CONFIG_DECODE_MASK;
		/* Convert CS if without decoder.
		 * CS0 to 4b'1110
		 * CS1 to 4b'1101
		 * CS2 to 4b'1011
		 * CS3 to 4b'0111
		 */
		chip_select = 0xF & ~(1 << chip_select);
	}

	reg &= ~(CQSPI_REG_CONFIG_CHIPSELECT_MASK
			<< CQSPI_REG_CONFIG_CHIPSELECT_LSB);
	reg |= (chip_select & CQSPI_REG_CONFIG_CHIPSELECT_MASK)
			<< CQSPI_REG_CONFIG_CHIPSELECT_LSB;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

void cadence_qspi_apb_delay(void *reg_base,
	unsigned int ref_clk, unsigned int sclk_hz,
	unsigned int tshsl_ns, unsigned int tsd2d_ns,
	unsigned int tchsh_ns, unsigned int tslch_ns)
{
	unsigned int ref_clk_ns;
	unsigned int sclk_ns;
	unsigned int tshsl, tchsh, tslch, tsd2d;
	unsigned int reg;

	cadence_qspi_apb_controller_disable(reg_base);

	/* Convert to ns. */
	ref_clk_ns = (1000000000) / ref_clk;

	/* Convert to ns. */
	sclk_ns = (1000000000) / sclk_hz;

	/* Plus 1 to round up 1 clock cycle. */
	tshsl = CQSPI_CAL_DELAY(tshsl_ns, ref_clk_ns, sclk_ns) + 1;
	tchsh = CQSPI_CAL_DELAY(tchsh_ns, ref_clk_ns, sclk_ns) + 1;
	tslch = CQSPI_CAL_DELAY(tslch_ns, ref_clk_ns, sclk_ns) + 1;
	tsd2d = CQSPI_CAL_DELAY(tsd2d_ns, ref_clk_ns, sclk_ns) + 1;

	reg = ((tshsl & CQSPI_REG_DELAY_TSHSL_MASK)
			<< CQSPI_REG_DELAY_TSHSL_LSB);
	reg |= ((tchsh & CQSPI_REG_DELAY_TCHSH_MASK)
			<< CQSPI_REG_DELAY_TCHSH_LSB);
	reg |= ((tslch & CQSPI_REG_DELAY_TSLCH_MASK)
			<< CQSPI_REG_DELAY_TSLCH_LSB);
	reg |= ((tsd2d & CQSPI_REG_DELAY_TSD2D_MASK)
			<< CQSPI_REG_DELAY_TSD2D_LSB);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_DELAY);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

void cadence_qspi_apb_controller_init(void *reg_base)
{
	unsigned reg;

	cadence_qspi_apb_controller_disable(reg_base);

	/* Configure the device size and address bytes */
	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	/* Clear the previous value */
	reg &= ~(CQSPI_REG_SIZE_PAGE_MASK << CQSPI_REG_SIZE_PAGE_LSB);
	reg &= ~(CQSPI_REG_SIZE_BLOCK_MASK << CQSPI_REG_SIZE_BLOCK_LSB);
	reg |= (CONFIG_CQSPI_PAGE_SIZE << CQSPI_REG_SIZE_PAGE_LSB);
	reg |= (CONFIG_CQSPI_BLOCK_SIZE << CQSPI_REG_SIZE_BLOCK_LSB);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_SIZE);

	/* Configure the remap address register, no remap */
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_REMAP);

	/* Disable all interrupts */
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_IRQMASK);

	cadence_qspi_apb_controller_enable(reg_base);
	return;
}

static int cadence_qspi_apb_exec_flash_cmd(void *reg_base,
	unsigned int reg)
{
	unsigned int retry = CQSPI_REG_RETRY;

	/* Write the CMDCTRL without start execution. */
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CMDCTRL);
	/* Start execute */
	reg |= CQSPI_REG_CMDCTRL_EXECUTE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CMDCTRL);

	while (retry--) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_CMDCTRL);
		if ((reg & CQSPI_REG_CMDCTRL_INPROGRESS_MASK) == 0)
			break;
		udelay(1);
	}

	if (!retry) {
		printf("QSPI: flash command execution timeout\n");
		return -EIO;
	}

	/* Polling QSPI idle status. */
	if (!cadence_qspi_wait_idle(reg_base))
		return -EIO;

	return 0;
}

/* For command RDID, RDSR. */
int cadence_qspi_apb_command_read(void *reg_base,
	unsigned int cmdlen, const u8 *cmdbuf, unsigned int rxlen,
	u8 *rxbuf)
{
	unsigned int reg;
	unsigned int read_len;
	int status;

	if (!cmdlen || rxlen > CQSPI_STIG_DATA_LEN_MAX || rxbuf == NULL) {
		printf("QSPI: Invalid input arguments cmdlen %d "
			"rxlen %d\n", cmdlen, rxlen);
		return -EINVAL;
	}

	reg = cmdbuf[0] << CQSPI_REG_CMDCTRL_OPCODE_LSB;

	reg |= (0x1 << CQSPI_REG_CMDCTRL_RD_EN_LSB);

	/* 0 means 1 byte. */
	reg |= (((rxlen - 1) & CQSPI_REG_CMDCTRL_RD_BYTES_MASK)
		<< CQSPI_REG_CMDCTRL_RD_BYTES_LSB);
	status = cadence_qspi_apb_exec_flash_cmd(reg_base, reg);
	if (status != 0)
		return status;

	reg = CQSPI_READL(reg_base + CQSPI_REG_CMDREADDATALOWER);

	/* Put the read value into rx_buf */
	read_len = (rxlen > 4) ? 4 : rxlen;
	memcpy(rxbuf, &reg, read_len);
	rxbuf += read_len;

	if (rxlen > 4) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_CMDREADDATAUPPER);

		read_len = rxlen - read_len;
		memcpy(rxbuf, &reg, read_len);
	}
	return 0;
}

/* For commands: WRSR, WREN, WRDI, CHIP_ERASE, BE, etc. */
int cadence_qspi_apb_command_write(void *reg_base, unsigned int cmdlen,
	const u8 *cmdbuf, unsigned int txlen,  const u8 *txbuf)
{
	unsigned int reg = 0;
	unsigned int addr_value;
	unsigned int wr_data;
	unsigned int wr_len;

	if (!cmdlen || cmdlen > 5 || txlen > 8 || cmdbuf == NULL) {
		printf("QSPI: Invalid input arguments cmdlen %d txlen %d\n",
			cmdlen, txlen);
		return -EINVAL;
	}

	reg |= cmdbuf[0] << CQSPI_REG_CMDCTRL_OPCODE_LSB;

	if (cmdlen == 4 || cmdlen == 5) {
		/* Command with address */
		reg |= (0x1 << CQSPI_REG_CMDCTRL_ADDR_EN_LSB);
		/* Number of bytes to write. */
		reg |= ((cmdlen - 2) & CQSPI_REG_CMDCTRL_ADD_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_ADD_BYTES_LSB;
		/* Get address */
		addr_value = cadence_qspi_apb_cmd2addr(&cmdbuf[1],
			cmdlen >= 5 ? 4 : 3);

		CQSPI_WRITEL(addr_value, reg_base + CQSPI_REG_CMDADDRESS);
	}

	if (txlen) {
		/* writing data = yes */
		reg |= (0x1 << CQSPI_REG_CMDCTRL_WR_EN_LSB);
		reg |= ((txlen - 1) & CQSPI_REG_CMDCTRL_WR_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_WR_BYTES_LSB;

		wr_len = txlen > 4 ? 4 : txlen;
		memcpy(&wr_data, txbuf, wr_len);
		CQSPI_WRITEL(wr_data, reg_base +
			CQSPI_REG_CMDWRITEDATALOWER);

		if (txlen > 4) {
			txbuf += wr_len;
			wr_len = txlen - wr_len;
			memcpy(&wr_data, txbuf, wr_len);
			CQSPI_WRITEL(wr_data, reg_base +
				CQSPI_REG_CMDWRITEDATAUPPER);
		}
	}

	/* Execute the command */
	return cadence_qspi_apb_exec_flash_cmd(reg_base, reg);
}

/* Opcode + Address (3/4 bytes) + dummy bytes (0-4 bytes) */
int cadence_qspi_apb_indirect_read_setup(void *reg_base,
	unsigned int ahb_phy_addr, unsigned int cmdlen, const u8 *cmdbuf)
{
	unsigned int reg;
	unsigned int rd_reg;
	unsigned int addr_value;
	unsigned int dummy_clk;
	unsigned int dummy_bytes;
	unsigned int addr_bytes;

	/*
	 * Identify addr_byte. All NOR flash device drivers are using fast read
	 * which always expecting 1 dummy byte, 1 cmd byte and 3/4 addr byte.
	 * With that, the length is in value of 5 or 6. Only FRAM chip from
	 * ramtron using normal read (which won't need dummy byte).
	 * Unlikely NOR flash using normal read due to performance issue.
	 */
	if (cmdlen >= 5)
		/* to cater fast read where cmd + addr + dummy */
		addr_bytes = cmdlen - 2;
	else
		/* for normal read (only ramtron as of now) */
		addr_bytes = cmdlen - 1;

	/* Setup the indirect trigger address */
	CQSPI_WRITEL((ahb_phy_addr & CQSPI_INDIRECTTRIGGER_ADDR_MASK),
		reg_base + CQSPI_REG_INDIRECTTRIGGER);

	/* Configure SRAM partition for read. */
	CQSPI_WRITEL(CQSPI_REG_SRAM_PARTITION_RD, reg_base +
		CQSPI_REG_SRAMPARTITION);

	/* Configure the opcode */
	rd_reg = cmdbuf[0] << CQSPI_REG_RD_INSTR_OPCODE_LSB;

#if (CONFIG_SPI_FLASH_QUAD == 1)
	/* Instruction and address at DQ0, data at DQ0-3. */
	rd_reg |= CQSPI_INST_TYPE_QUAD << CQSPI_REG_RD_INSTR_TYPE_DATA_LSB;
#endif

	/* Get address */
	addr_value = cadence_qspi_apb_cmd2addr(&cmdbuf[1], addr_bytes);
	CQSPI_WRITEL(addr_value, reg_base + CQSPI_REG_INDIRECTRDSTARTADDR);

	/* The remaining lenght is dummy bytes. */
	dummy_bytes = cmdlen - addr_bytes - 1;
	if (dummy_bytes) {

		if (dummy_bytes > CQSPI_DUMMY_BYTES_MAX)
			dummy_bytes = CQSPI_DUMMY_BYTES_MAX;

		rd_reg |= (1 << CQSPI_REG_RD_INSTR_MODE_EN_LSB);
#if defined(CONFIG_SPL_SPI_XIP) && defined(CONFIG_SPL_BUILD)
		CQSPI_WRITEL(0x0, reg_base + CQSPI_REG_MODE_BIT);
#else
		CQSPI_WRITEL(0xFF, reg_base + CQSPI_REG_MODE_BIT);
#endif

		/* Convert to clock cycles. */
		dummy_clk = dummy_bytes * CQSPI_DUMMY_CLKS_PER_BYTE;
		/* Need to minus the mode byte (8 clocks). */
		dummy_clk -= CQSPI_DUMMY_CLKS_PER_BYTE;

		if (dummy_clk)
			rd_reg |= (dummy_clk & CQSPI_REG_RD_INSTR_DUMMY_MASK)
				<< CQSPI_REG_RD_INSTR_DUMMY_LSB;
	}

	CQSPI_WRITEL(rd_reg, reg_base + CQSPI_REG_RD_INSTR);

	/* set device size */
	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (addr_bytes - 1);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_SIZE);
	return 0;
}

int cadence_qspi_apb_indirect_read_execute(void *reg_base,
	void *ahb_base_addr, unsigned int rxlen, u8 *rxbuf)
{
	unsigned int reg;

	CQSPI_WRITEL(rxlen, reg_base + CQSPI_REG_INDIRECTRDBYTES);

	/* Start the indirect read transfer */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTRD_START_MASK,
			reg_base + CQSPI_REG_INDIRECTRD);

	if (qspi_read_sram_fifo_poll(reg_base, (void *)rxbuf,
				(const void *)ahb_base_addr, rxlen)) {
		goto failrd;
	}

	/* Check flash indirect controller */
	reg = CQSPI_READL(reg_base + CQSPI_REG_INDIRECTRD);
	if (!(reg & CQSPI_REG_INDIRECTRD_DONE_MASK)) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_INDIRECTRD);
		printf("QSPI: indirect completion status "
			"error with reg 0x%08x\n", reg);
		goto failrd;
	}

	/* Clear indirect completion status */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTRD_DONE_MASK,
		reg_base + CQSPI_REG_INDIRECTRD);
	return 0;

failrd:
	/* Cancel the indirect read */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTRD_CANCEL_MASK,
		reg_base + CQSPI_REG_INDIRECTRD);
	return -1;
}

/* Opcode + Address (3/4 bytes) */
int cadence_qspi_apb_indirect_write_setup(void *reg_base,
	unsigned int ahb_phy_addr, unsigned int cmdlen, const u8 *cmdbuf)
{
	unsigned int reg;
	unsigned int addr_bytes = cmdlen > 4 ? 4 : 3;

	if (cmdlen < 4 || cmdbuf == NULL) {
		printf("QSPI: iInvalid input argument, len %d cmdbuf 0x%08x\n",
			cmdlen, (unsigned int)cmdbuf);
		return -EINVAL;
	}
	/* Setup the indirect trigger address */
	CQSPI_WRITEL((ahb_phy_addr & CQSPI_INDIRECTTRIGGER_ADDR_MASK),
		reg_base + CQSPI_REG_INDIRECTTRIGGER);

	CQSPI_WRITEL(CQSPI_REG_SRAM_PARTITION_WR,
		reg_base + CQSPI_REG_SRAMPARTITION);

	/* Configure the opcode */
	reg = cmdbuf[0] << CQSPI_REG_WR_INSTR_OPCODE_LSB;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_WR_INSTR);

	/* Setup write address. */
	reg = cadence_qspi_apb_cmd2addr(&cmdbuf[1], addr_bytes);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_INDIRECTWRSTARTADDR);

	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (addr_bytes - 1);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_SIZE);
	return 0;
}

int cadence_qspi_apb_indirect_write_execute(void *reg_base,
	void *ahb_base_addr, unsigned int txlen, const u8 *txbuf)
{
	unsigned int reg = 0;
	unsigned int retry;

	/* Configure the indirect read transfer bytes */
	CQSPI_WRITEL(txlen, reg_base + CQSPI_REG_INDIRECTWRBYTES);

	/* Start the indirect write transfer */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_START_MASK,
			reg_base + CQSPI_REG_INDIRECTWR);

	if (qpsi_write_sram_fifo_push(reg_base, ahb_base_addr,
		(const void *)txbuf, txlen)) {
		goto failwr;
	}

	/* Wait until last write is completed (FIFO empty) */
	retry = CQSPI_REG_RETRY;
	while (retry--) {
		reg = CQSPI_GET_WR_SRAM_LEVEL(reg_base);
		if (reg == 0)
			break;

		udelay(1);
	}
	if (reg != 0) {
		printf("QSPI: timeout for indirect write\n");
		goto failwr;
	}

	/* Check flash indirect controller status */
	retry = CQSPI_REG_RETRY;
	while (retry--) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_INDIRECTWR);
		if (reg & CQSPI_REG_INDIRECTWR_DONE_MASK)
			break;
		udelay(1);
	}
	if (!(reg & CQSPI_REG_INDIRECTWR_DONE_MASK)) {
		printf("QSPI: indirect completion "
			"status error with reg 0x%08x\n", reg);
		goto failwr;
	}

	/* Clear indirect completion status */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_DONE_MASK,
		reg_base + CQSPI_REG_INDIRECTWR);
	return 0;

failwr:
	/* Cancel the indirect write */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
			reg_base + CQSPI_REG_INDIRECTWR);
	return -1;
}

void cadence_qspi_apb_enter_xip(void *reg_base, char xip_dummy)
{
	unsigned int reg;

	/* enter XiP mode immediately and enable direct mode */
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg |= CQSPI_REG_CONFIG_ENABLE_MASK;
	reg |= CQSPI_REG_CONFIG_DIRECT_MASK;
	reg |= CQSPI_REG_CONFIG_XIP_IMM_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	/* keep the XiP mode */
	CQSPI_WRITEL(xip_dummy, reg_base + CQSPI_REG_MODE_BIT);

	/* Enable mode bit at devrd */
	reg = CQSPI_READL(reg_base + CQSPI_REG_RD_INSTR);
	reg |= (1 << CQSPI_REG_RD_INSTR_MODE_EN_LSB);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_RD_INSTR);
}
