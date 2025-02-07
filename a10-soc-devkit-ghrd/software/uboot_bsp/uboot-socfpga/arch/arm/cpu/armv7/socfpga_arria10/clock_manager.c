/*
 * Copyright (C) 2014 Altera Corporation <www.altera.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock_manager.h>
#include <fdtdec.h>

static const struct socfpga_clock_manager *clock_manager_base =
		(void *)SOCFPGA_CLKMGR_ADDRESS;

static u32 eosc1_hz;
static u32 cb_intosc_hz;
static u32 f2s_free_hz;
uint32_t cm_l4_main_clk_hz = 0;
uint32_t cm_l4_sp_clk_hz = 0;
uint32_t cm_l4_mp_clk_hz = 0;
#define LOCKED_MASK \
	(CLKMGR_CLKMGR_STAT_MAINPLLLOCKED_SET_MSK  | \
	CLKMGR_CLKMGR_STAT_PERPLLLOCKED_SET_MSK)

static inline void cm_wait_for_lock(uint32_t mask)
{
	register uint32_t inter_val;
	do {
		inter_val = readl(&clock_manager_base->stat) & mask;
	} while (inter_val != mask);
}

/* function to poll in the fsm busy bit */
static inline void cm_wait4fsm(void)
{
	register uint32_t inter_val;
	do {
		inter_val = readl(&clock_manager_base->stat) &
			CLKMGR_CLKMGR_STAT_BUSY_SET_MSK;
	} while (inter_val);
}
static u32 cm_get_main_vco(void)
{
	u32 vco1, src_hz, numer, denom, vco;
	u32 clk_src = readl(&clock_manager_base->main_pll_vco0);

	clk_src = (clk_src >> CLKMGR_MAINPLL_VCO0_PSRC_LSB) &
		CLKMGR_MAINPLL_VCO0_PSRC_MSK;

	if (clk_src == CLKMGR_MAINPLL_VCO0_PSRC_EOSC) {
		src_hz = eosc1_hz;
	} else if (clk_src == CLKMGR_MAINPLL_VCO0_PSRC_E_INTOSC) {
		src_hz = cb_intosc_hz;
	} else if (clk_src == CLKMGR_MAINPLL_VCO0_PSRC_F2S) {
		src_hz = f2s_free_hz;
	} else {
		printf("cm_get_main_vco invalid clk_src %d\n", clk_src);
		return 0;
	}
	vco1 = readl(&clock_manager_base->main_pll_vco1);
	numer = vco1 & CLKMGR_MAINPLL_VCO1_NUMER_MSK;
	denom = (vco1 >> CLKMGR_MAINPLL_VCO1_DENOM_LSB) &
			CLKMGR_MAINPLL_VCO1_DENOM_MSK;
	vco = src_hz;
	vco /= (1 + denom);
	vco *= (1 + numer);

	return vco;
}

static u32 cm_get_peri_vco(void)
{
	u32 vco1, src_hz, numer, denom, vco;
	u32 clk_src = readl(&clock_manager_base->per_pll_vco0);

	clk_src = (clk_src >> CLKMGR_PERPLL_VCO0_PSRC_LSB) &
		CLKMGR_PERPLL_VCO0_PSRC_MSK;

	if (clk_src == CLKMGR_PERPLL_VCO0_PSRC_EOSC) {
		src_hz = eosc1_hz;
	} else if (clk_src == CLKMGR_PERPLL_VCO0_PSRC_E_INTOSC) {
		src_hz = cb_intosc_hz;
	} else if (clk_src == CLKMGR_PERPLL_VCO0_PSRC_F2S) {
		src_hz = f2s_free_hz;
	} else if (clk_src == CLKMGR_PERPLL_VCO0_PSRC_MAIN) {
		src_hz = cm_get_main_vco();
		src_hz /= (readl(&clock_manager_base->main_pll_cntr15clk) &
			CLKMGR_MAINPLL_CNTRCLK_MSK) + 1;
	} else {
		printf("cm_get_per_vco invalid clk_src %d\n", clk_src);
		return 0;
	}
	vco1 = readl(&clock_manager_base->per_pll_vco1);
	numer = vco1 & CLKMGR_PERPLL_VCO1_NUMER_MSK;
	denom = (vco1 >> CLKMGR_PERPLL_VCO1_DENOM_LSB) &
			CLKMGR_PERPLL_VCO1_DENOM_MSK;
	vco = src_hz;
	vco /= (1 + denom);
	vco *= (1 + numer);

	return vco;
}

static u32 cm_get_l4_noc_hz(u32 nocdivshift)
{
	u32 clk_src, dividor, nocclk, src_hz;
	u32 dividor2 = 1 <<
		((readl(&clock_manager_base->main_pll_nocdiv) >>
			nocdivshift) & CLKMGR_MAINPLL_NOCDIV_MSK);

	nocclk = readl(&clock_manager_base->main_pll_nocclk);
	clk_src = (nocclk >> CLKMGR_MAINPLL_NOCCLK_SRC_LSB) &
		CLKMGR_MAINPLL_NOCCLK_SRC_MSK;

	dividor = 1 + (nocclk & CLKMGR_MAINPLL_NOCDIV_MSK);

	if (clk_src == CLKMGR_PERPLLGRP_SRC_MAIN) {
		src_hz = cm_get_main_vco();
		src_hz /= 1 +
		(readl(SOCFPGA_CLKMGR_ADDRESS+CLKMGR_MAINPLL_NOC_CLK_OFFSET)&
		CLKMGR_MAINPLL_NOCCLK_CNT_MSK);
	} else if (clk_src == CLKMGR_PERPLLGRP_SRC_PERI) {
		src_hz = cm_get_peri_vco();
		src_hz /= 1 +
		((readl(SOCFPGA_CLKMGR_ADDRESS+CLKMGR_MAINPLL_NOC_CLK_OFFSET)>>
			CLKMGR_MAINPLL_NOCCLK_PERICNT_LSB)&
			CLKMGR_MAINPLL_NOCCLK_CNT_MSK);
	} else if (clk_src == CLKMGR_PERPLLGRP_SRC_OSC1) {
		src_hz = eosc1_hz;
	} else if (clk_src == CLKMGR_PERPLLGRP_SRC_INTOSC) {
		src_hz = cb_intosc_hz;
	} else if (clk_src == CLKMGR_PERPLLGRP_SRC_FPGA) {
		src_hz = f2s_free_hz;
	} else {
		src_hz = 0;
	}
	return src_hz/dividor/dividor2;
}

unsigned int cm_get_mmc_controller_clk_hz(void)
{
	u32 clk_hz = 0;
	u32 clk_input = readl(&clock_manager_base->per_pll_cntr6clk);
	clk_input = (clk_input >> CLKMGR_PERPLL_CNTR6CLK_SRC_LSB) &
		CLKMGR_PERPLLGRP_SRC_MSK;

	switch (clk_input) {
	case CLKMGR_PERPLLGRP_SRC_MAIN:
		clk_hz = cm_get_main_vco();
		clk_hz /= 1 + (readl(&clock_manager_base->main_pll_cntr6clk) &
			CLKMGR_MAINPLL_CNTRCLK_MSK);
		break;

	case CLKMGR_PERPLLGRP_SRC_PERI:
		clk_hz = cm_get_peri_vco();
		clk_hz /= 1 + (readl(&clock_manager_base->per_pll_cntr6clk) &
			CLKMGR_PERPLL_CNTRCLK_MSK);
		break;

	case CLKMGR_PERPLLGRP_SRC_OSC1:
		clk_hz = eosc1_hz;
		break;

	case CLKMGR_PERPLLGRP_SRC_INTOSC:
		clk_hz = cb_intosc_hz;
		break;

	case CLKMGR_PERPLLGRP_SRC_FPGA:
		clk_hz = f2s_free_hz;
		break;
	}
	return clk_hz/4;
}

struct mainpll_cfg {
	u32 vco0_psrc;
	u32 vco1_denom;
	u32 vco1_numer;
	u32 mpuclk;
	u32 mpuclk_cnt;
	u32 mpuclk_src;
	u32 nocclk;
	u32 nocclk_cnt;
	u32 nocclk_src;
	u32 cntr2clk_cnt;
	u32 cntr3clk_cnt;
	u32 cntr4clk_cnt;
	u32 cntr5clk_cnt;
	u32 cntr6clk_cnt;
	u32 cntr7clk_cnt;
	u32 cntr7clk_src;
	u32 cntr8clk_cnt;
	u32 cntr9clk_cnt;
	u32 cntr9clk_src;
	u32 cntr15clk_cnt;
	u32 nocdiv_l4mainclk;
	u32 nocdiv_l4mpclk;
	u32 nocdiv_l4spclk;
	u32 nocdiv_csatclk;
	u32 nocdiv_cstraceclk;
	u32 nocdiv_cspdbclk;
};

struct perpll_cfg {
	u32 vco0_psrc;
	u32 vco1_denom;
	u32 vco1_numer;
	u32 cntr2clk_cnt;
	u32 cntr2clk_src;
	u32 cntr3clk_cnt;
	u32 cntr3clk_src;
	u32 cntr4clk_cnt;
	u32 cntr4clk_src;
	u32 cntr5clk_cnt;
	u32 cntr5clk_src;
	u32 cntr6clk_cnt;
	u32 cntr6clk_src;
	u32 cntr7clk_cnt;
	u32 cntr8clk_cnt;
	u32 cntr8clk_src;
	u32 cntr9clk_cnt;
	u32 cntr9clk_src;
	u32 emacctl_emac0sel;
	u32 emacctl_emac1sel;
	u32 emacctl_emac2sel;
	u32 gpiodiv_gpiodbclk;
};

struct strtou32 {
	const char *str;
	const u32 val;
};

static const struct strtou32 mailpll_cfg_tab[] = {
	{ "vco0-psrc", offsetof(struct mainpll_cfg, vco0_psrc) },
	{ "vco1-denom", offsetof(struct mainpll_cfg, vco1_denom) },
	{ "vco1-numer", offsetof(struct mainpll_cfg, vco1_numer) },
	{ "mpuclk-cnt", offsetof(struct mainpll_cfg, mpuclk_cnt) },
	{ "mpuclk-src", offsetof(struct mainpll_cfg, mpuclk_src) },
	{ "nocclk-cnt", offsetof(struct mainpll_cfg, nocclk_cnt) },
	{ "nocclk-src", offsetof(struct mainpll_cfg, nocclk_src) },
	{ "cntr2clk-cnt", offsetof(struct mainpll_cfg, cntr2clk_cnt) },
	{ "cntr3clk-cnt", offsetof(struct mainpll_cfg, cntr3clk_cnt) },
	{ "cntr4clk-cnt", offsetof(struct mainpll_cfg, cntr4clk_cnt) },
	{ "cntr5clk-cnt", offsetof(struct mainpll_cfg, cntr5clk_cnt) },
	{ "cntr6clk-cnt", offsetof(struct mainpll_cfg, cntr6clk_cnt) },
	{ "cntr7clk-cnt", offsetof(struct mainpll_cfg, cntr7clk_cnt) },
	{ "cntr7clk-src", offsetof(struct mainpll_cfg, cntr7clk_src) },
	{ "cntr8clk-cnt", offsetof(struct mainpll_cfg, cntr8clk_cnt) },
	{ "cntr9clk-cnt", offsetof(struct mainpll_cfg, cntr9clk_cnt) },
	{ "cntr9clk-src", offsetof(struct mainpll_cfg, cntr9clk_src) },
	{ "cntr15clk-cnt", offsetof(struct mainpll_cfg, cntr15clk_cnt) },
	{ "nocdiv-l4mainclk", offsetof(struct mainpll_cfg, nocdiv_l4mainclk) },
	{ "nocdiv-l4mpclk", offsetof(struct mainpll_cfg, nocdiv_l4mpclk) },
	{ "nocdiv-l4spclk", offsetof(struct mainpll_cfg, nocdiv_l4spclk) },
	{ "nocdiv-csatclk", offsetof(struct mainpll_cfg, nocdiv_csatclk) },
	{ "nocdiv-cstraceclk", offsetof(struct mainpll_cfg, nocdiv_cstraceclk) },
	{ "nocdiv-cspdbgclk", offsetof(struct mainpll_cfg, nocdiv_cspdbclk) },
};

static const struct strtou32 perpll_cfg_tab[] = {
	{ "vco0-psrc", offsetof(struct perpll_cfg, vco0_psrc) },
	{ "vco1-denom", offsetof(struct perpll_cfg, vco1_denom) },
	{ "vco1-numer", offsetof(struct perpll_cfg, vco1_numer) },
	{ "cntr2clk-cnt", offsetof(struct perpll_cfg, cntr2clk_cnt) },
	{ "cntr2clk-src", offsetof(struct perpll_cfg, cntr2clk_src) },
	{ "cntr3clk-cnt", offsetof(struct perpll_cfg, cntr3clk_cnt) },
	{ "cntr3clk-src", offsetof(struct perpll_cfg, cntr3clk_src) },
	{ "cntr4clk-cnt", offsetof(struct perpll_cfg, cntr4clk_cnt) },
	{ "cntr4clk-src", offsetof(struct perpll_cfg, cntr4clk_src) },
	{ "cntr5clk-cnt", offsetof(struct perpll_cfg, cntr5clk_cnt) },
	{ "cntr5clk-src", offsetof(struct perpll_cfg, cntr5clk_src) },
	{ "cntr6clk-cnt", offsetof(struct perpll_cfg, cntr6clk_cnt) },
	{ "cntr6clk-src", offsetof(struct perpll_cfg, cntr6clk_src) },
	{ "cntr7clk-cnt", offsetof(struct perpll_cfg, cntr7clk_cnt) },
	{ "cntr8clk-cnt", offsetof(struct perpll_cfg, cntr8clk_cnt) },
	{ "cntr8clk-src", offsetof(struct perpll_cfg, cntr8clk_src) },
	{ "cntr9clk-cnt", offsetof(struct perpll_cfg, cntr9clk_cnt) },
	{ "emacctl-emac0sel", offsetof(struct perpll_cfg, emacctl_emac0sel) },
	{ "emacctl-emac1sel", offsetof(struct perpll_cfg, emacctl_emac1sel) },
	{ "emacctl-emac2sel", offsetof(struct perpll_cfg, emacctl_emac2sel) },
	{ "gpiodiv-gpiodbclk", offsetof(struct perpll_cfg, gpiodiv_gpiodbclk) },
};

static const struct strtou32 alteragrp_cfg_tab[] = {
	{ "nocclk", offsetof(struct mainpll_cfg, nocclk) },
	{ "mpuclk", offsetof(struct mainpll_cfg, mpuclk) },
};

int of_to_struct(const void *blob, int node, const struct strtou32* cfg_tab,
		 int cfg_tab_len, void *cfg)
{
	int i;
	u32 val;

	for (i = 0; i < cfg_tab_len; i++) {
		if (fdtdec_get_int_array(blob, node, cfg_tab[i].str, &val, 1)) {
			/* could not find required property */
			return 100;
		}
		*(u32*)(cfg + cfg_tab[i].val) = val;
	}

	return 0;
}

struct strtopu32 {
	const char *str;
	u32 *p;
};
const struct strtopu32 dt_to_val[] = {
	{"/clocks/altera_arria10_hps_eosc1", &eosc1_hz},
	{"/clocks/altera_arria10_hps_cb_intosc_ls", &cb_intosc_hz},
	{"/clocks/altera_arria10_hps_f2h_free", &f2s_free_hz},
};
static void of_get_input_clks(const void *blob)
{
	int node, i;

	for (i = 0; i < ARRAY_SIZE(dt_to_val); i++) {
		node = fdt_path_offset(blob, dt_to_val[i].str);

		if (node < 0)
			continue;

		fdtdec_get_int_array(blob, node, "clock-frequency",
			dt_to_val[i].p, 1);
	}
}

int of_get_clk_cfg(const void *blob, struct mainpll_cfg *main_cfg,
			  struct perpll_cfg *per_cfg)
{
	int node, child, len;
	const char *node_name;

	of_get_input_clks(blob);

	node = fdtdec_next_compatible(blob, 0, COMPAT_ARRIA10_CLK_INIT);

	if (node < 0)
		return 1;

	child = fdt_first_subnode(blob, node);

	if (child < 0)
		return 2;

	node_name = fdt_get_name(blob, child, &len);

	while (node_name) {
		if (!strcmp(node_name, "mainpll")) {
			if (of_to_struct(blob, child, mailpll_cfg_tab,
				ARRAY_SIZE(mailpll_cfg_tab), main_cfg))
				return 101;
		} else if (!strcmp(node_name, "perpll")) {
			if (of_to_struct(blob, child, perpll_cfg_tab,
				ARRAY_SIZE(perpll_cfg_tab), per_cfg))
				return 102;
		} else if (!strcmp(node_name, "alteragrp")) {
			if (of_to_struct(blob, child, alteragrp_cfg_tab,
				ARRAY_SIZE(alteragrp_cfg_tab), main_cfg))
				return 103;
		}
		child = fdt_next_subnode(blob, child);

		if (child < 0)
			break;

		node_name = fdt_get_name(blob, child, &len);
	}

	return 0;
}

/*
 * Setup clocks while making no assumptions of the
 * previous state of the clocks.
 *
 * Start by being paranoid and gate all sw managed clocks
 *
 * Put all plls in bypass
 *
 * Put all plls VCO registers back to reset value (bgpwr dwn).
 *
 * Put peripheral and main pll src to reset value to avoid glitch.
 *
 * Delay 5 us.
 *
 * Deassert bg pwr dn and set numerator and denominator
 *
 * Start 7 us timer.
 *
 * set internal dividers
 *
 * Wait for 7 us timer.
 *
 * Enable plls
 *
 * Set external dividers while plls are locking
 *
 * Wait for pll lock
 *
 * Assert/deassert outreset all.
 *
 * Take all pll's out of bypass
 *
 * Clear safe mode
 *
 * set source main and peripheral clocks
 *
 * Ungate clocks
 */

static int cm_full_cfg(struct mainpll_cfg *main_cfg, struct perpll_cfg *per_cfg)
{
#ifndef TEST_AT_ASIMOV
	/* gate off all mainpll clock excpet HW managed clock */
	writel(CLKMGR_MAINPLL_EN_S2FUSER0CLKEN_SET_MSK |
		CLKMGR_MAINPLL_EN_HMCPLLREFCLKEN_SET_MSK,
		&clock_manager_base->main_pll_enr);

	/* now we can gate off the rest of the peripheral clocks */
	writel(0, &clock_manager_base->per_pll_en);

	/* Put all plls in external bypass */
	writel(CLKMGR_MAINPLL_BYPASS_RESET,
		&clock_manager_base->main_pll_bypasss);
	writel(CLKMGR_PERPLL_BYPASS_RESET,
		&clock_manager_base->per_pll_bypasss);

	/*
	 * Put all plls VCO registers back to reset value.
	 * Some code might have messed with them. At same time set the
	 * desired clock source
	 */
	writel(CLKMGR_MAINPLL_VCO0_RESET |
		CLKMGR_MAINPLL_VCO0_REGEXTSEL_SET_MSK |
		(main_cfg->vco0_psrc <<
			CLKMGR_MAINPLL_VCO0_PSRC_LSB),
		&clock_manager_base->main_pll_vco0);

	writel(CLKMGR_PERPLL_VCO0_RESET |
		CLKMGR_PERPLL_VCO0_REGEXTSEL_SET_MSK |
		(per_cfg->vco0_psrc <<
			CLKMGR_PERPLL_VCO0_PSRC_LSB),
		&clock_manager_base->per_pll_vco0);

	writel(CLKMGR_MAINPLL_VCO1_RESET,
		&clock_manager_base->main_pll_vco1);
	writel(CLKMGR_PERPLL_VCO1_RESET,
		&clock_manager_base->per_pll_vco1);

	/* clear the interrupt register status register */
	writel(CLKMGR_CLKMGR_INTR_MAINPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLFBSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLFBSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLACHIEVED_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLACHIEVED_SET_MSK,
		&clock_manager_base->intr);

	/* Program VCO “Numerator” and “Denominator” */
	writel((main_cfg->vco1_denom <<
		CLKMGR_MAINPLL_VCO1_DENOM_LSB) |
		main_cfg->vco1_numer,
		&clock_manager_base->main_pll_vco1);
	writel((per_cfg->vco1_denom <<
		CLKMGR_PERPLL_VCO1_DENOM_LSB) |
		per_cfg->vco1_numer,
		&clock_manager_base->per_pll_vco1);

	/* Wait for at least 5 us */
	udelay(5);

	/* Now deassert BGPWRDN and PWRDN */
	clrbits_le32(&clock_manager_base->main_pll_vco0,
		CLKMGR_MAINPLL_VCO0_BGPWRDN_SET_MSK |
		CLKMGR_MAINPLL_VCO0_PWRDN_SET_MSK);
	clrbits_le32(&clock_manager_base->per_pll_vco0,
		CLKMGR_PERPLL_VCO0_BGPWRDN_SET_MSK |
		CLKMGR_PERPLL_VCO0_PWRDN_SET_MSK);
#endif /***************** TEST_AT_ASIMOV *****************/

	/* Wait for at least 7 us */
	udelay(7);

#ifndef TEST_AT_ASIMOV
	/* enable the VCO and disable the external regulator to PLL */
	writel((readl(&clock_manager_base->main_pll_vco0) &
		~CLKMGR_MAINPLL_VCO0_REGEXTSEL_SET_MSK) |
		CLKMGR_MAINPLL_VCO0_EN_SET_MSK,
		&clock_manager_base->main_pll_vco0);
	writel((readl(&clock_manager_base->per_pll_vco0) &
		~CLKMGR_PERPLL_VCO0_REGEXTSEL_SET_MSK) |
		CLKMGR_PERPLL_VCO0_EN_SET_MSK,
		&clock_manager_base->per_pll_vco0);

	/* setup all the main PLL counter and clock source */
	writel(main_cfg->nocclk,
	       SOCFPGA_CLKMGR_ADDRESS + CLKMGR_MAINPLL_NOC_CLK_OFFSET);
	writel(main_cfg->mpuclk,
	       SOCFPGA_CLKMGR_ADDRESS + CLKMGR_ALTERAGRP_MPU_CLK_OFFSET);

	/* main_emaca_clk divider */
	writel(main_cfg->cntr2clk_cnt, &clock_manager_base->main_pll_cntr2clk);
	/* main_emacb_clk divider */
	writel(main_cfg->cntr3clk_cnt, &clock_manager_base->main_pll_cntr3clk);
	/* main_emac_ptp_clk divider */
	writel(main_cfg->cntr4clk_cnt, &clock_manager_base->main_pll_cntr4clk);
	/* main_gpio_db_clk divider */
	writel(main_cfg->cntr5clk_cnt, &clock_manager_base->main_pll_cntr5clk);
	/* main_sdmmc_clk divider */
	writel(main_cfg->cntr6clk_cnt, &clock_manager_base->main_pll_cntr6clk);
	/* main_s2f_user0_clk divider */
	writel(main_cfg->cntr7clk_cnt |
		(main_cfg->cntr7clk_src <<
			CLKMGR_MAINPLL_CNTR7CLK_SRC_LSB),
		&clock_manager_base->main_pll_cntr7clk);
	/* main_s2f_user1_clk divider */
	writel(main_cfg->cntr8clk_cnt, &clock_manager_base->main_pll_cntr8clk);
	/* main_hmc_pll_clk divider */
	writel(main_cfg->cntr9clk_cnt |
		(main_cfg->cntr9clk_src <<
			CLKMGR_MAINPLL_CNTR9CLK_SRC_LSB),
		&clock_manager_base->main_pll_cntr9clk);
	/* main_periph_ref_clk divider */
	writel(main_cfg->cntr15clk_cnt,
		&clock_manager_base->main_pll_cntr15clk);

	/* setup all the peripheral PLL counter and clock source */
	/* peri_emaca_clk divider */
	writel(per_cfg->cntr2clk_cnt |
		(per_cfg->cntr2clk_src << CLKMGR_PERPLL_CNTR2CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr2clk);
	/* peri_emacb_clk divider */
	writel(per_cfg->cntr3clk_cnt |
		(per_cfg->cntr3clk_src << CLKMGR_PERPLL_CNTR3CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr3clk);
	/* peri_emac_ptp_clk divider */
	writel(per_cfg->cntr4clk_cnt |
		(per_cfg->cntr4clk_src << CLKMGR_PERPLL_CNTR4CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr4clk);
	/* peri_gpio_db_clk divider */
	writel(per_cfg->cntr5clk_cnt |
		(per_cfg->cntr5clk_src << CLKMGR_PERPLL_CNTR5CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr5clk);
	/* peri_sdmmc_clk divider */
	writel(per_cfg->cntr6clk_cnt |
		(per_cfg->cntr6clk_src << CLKMGR_PERPLL_CNTR6CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr6clk);
	/* peri_s2f_user0_clk divider */
	writel(per_cfg->cntr7clk_cnt, &clock_manager_base->per_pll_cntr7clk);
	/* peri_s2f_user1_clk divider */
	writel(per_cfg->cntr8clk_cnt |
		(per_cfg->cntr8clk_src << CLKMGR_PERPLL_CNTR8CLK_SRC_LSB),
		&clock_manager_base->per_pll_cntr8clk);
	/* peri_hmc_pll_clk divider */
	writel(per_cfg->cntr9clk_cnt,
		&clock_manager_base->per_pll_cntr9clk);

	/* setup all the external PLL counter */
	/* mpu wrapper / external divider */
	writel(main_cfg->mpuclk_cnt |
		(main_cfg->mpuclk_src <<
			CLKMGR_MAINPLL_MPUCLK_SRC_LSB),
		&clock_manager_base->main_pll_mpuclk);
	/* NOC wrapper / external divider */
	writel(main_cfg->nocclk_cnt |
		(main_cfg->nocclk_src << CLKMGR_MAINPLL_NOCCLK_SRC_LSB),
		&clock_manager_base->main_pll_nocclk);
	/* NOC subclock divider such as l4 */
	writel(main_cfg->nocdiv_l4mainclk |
		(main_cfg->nocdiv_l4mpclk <<
			CLKMGR_MAINPLL_NOCDIV_L4MPCLK_LSB) |
		(main_cfg->nocdiv_l4spclk <<
			CLKMGR_MAINPLL_NOCDIV_L4SPCLK_LSB) |
		(main_cfg->nocdiv_csatclk <<
			CLKMGR_MAINPLL_NOCDIV_CSATCLK_LSB) |
		(main_cfg->nocdiv_cstraceclk <<
			CLKMGR_MAINPLL_NOCDIV_CSTRACECLK_LSB) |
		(main_cfg->nocdiv_cspdbclk <<
			CLKMGR_MAINPLL_NOCDIV_CSPDBGCLK_LSB),
		&clock_manager_base->main_pll_nocdiv);
	/* gpio_db external divider */
	writel(per_cfg->gpiodiv_gpiodbclk, &clock_manager_base->per_pll_gpiodiv);

	/* setup the EMAC clock mux select */
	writel((per_cfg->emacctl_emac0sel <<
			CLKMGR_PERPLL_EMACCTL_EMAC0SEL_LSB) |
		(per_cfg->emacctl_emac1sel <<
			CLKMGR_PERPLL_EMACCTL_EMAC1SEL_LSB) |
		(per_cfg->emacctl_emac2sel <<
			CLKMGR_PERPLL_EMACCTL_EMAC2SEL_LSB),
		&clock_manager_base->per_pll_emacctl);

	/* at this stage, check for PLL lock status */
	cm_wait_for_lock(LOCKED_MASK);

	/*
	 * after locking, but before taking out of bypass,
	 * assert/deassert outresetall
	 */
	/* assert mainpll outresetall */
	setbits_le32(&clock_manager_base->main_pll_vco0,
		CLKMGR_MAINPLL_VCO0_OUTRSTALL_SET_MSK);
	/* assert perpll outresetall */
	setbits_le32(&clock_manager_base->per_pll_vco0,
		CLKMGR_PERPLL_VCO0_OUTRSTALL_SET_MSK);
	/* de-assert mainpll outresetall */
	clrbits_le32(&clock_manager_base->main_pll_vco0,
		CLKMGR_MAINPLL_VCO0_OUTRSTALL_SET_MSK);
	/* de-assert perpll outresetall */
	clrbits_le32(&clock_manager_base->per_pll_vco0,
		CLKMGR_PERPLL_VCO0_OUTRSTALL_SET_MSK);

	/* Take all PLLs out of bypass when boot mode is cleared. */
	/* release mainpll from bypass */
	writel(CLKMGR_MAINPLL_BYPASS_RESET,
		&clock_manager_base->main_pll_bypassr);
	/* wait till Clock Manager is not busy */
	cm_wait4fsm();

	/* release perpll from bypass */
	writel(CLKMGR_PERPLL_BYPASS_RESET,
		&clock_manager_base->per_pll_bypassr);
	/* wait till Clock Manager is not busy */
	cm_wait4fsm();

	/* clear boot mode */
	clrbits_le32(&clock_manager_base->ctrl,
		CLKMGR_CLKMGR_CTL_BOOTMOD_SET_MSK);
	/* wait till Clock Manager is not busy */
	cm_wait4fsm();

	/* Now ungate non-hw-managed clocks */
	writel(CLKMGR_MAINPLL_EN_S2FUSER0CLKEN_SET_MSK |
		CLKMGR_MAINPLL_EN_HMCPLLREFCLKEN_SET_MSK,
		&clock_manager_base->main_pll_ens);
	writel(CLKMGR_PERPLL_EN_RESET,
		&clock_manager_base->per_pll_ens);

	/* Clear the loss lock and slip bits as they might set during
	clock reconfiguration */
	writel(CLKMGR_CLKMGR_INTR_MAINPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLFBSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLFBSLIP_SET_MSK,
		&clock_manager_base->intr);
#endif /***************** TEST_AT_ASIMOV *****************/
	return 0;
}
int cm_basic_init(const void *blob)
{
	struct mainpll_cfg main_cfg;
	struct perpll_cfg per_cfg;
	int rval;

	/* initialize to zero for use case of optional node */
	memset(&main_cfg, 0, sizeof(main_cfg));

	if (of_get_clk_cfg(blob, &main_cfg, &per_cfg)) {
		return 1;
	}

	rval =  cm_full_cfg(&main_cfg, &per_cfg);

	cm_l4_main_clk_hz =
		cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4MAINCLK_LSB);

	cm_l4_mp_clk_hz =
		cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4MPCLK_LSB);

	cm_l4_sp_clk_hz =
		cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4SPCLK_LSB);

	return rval;
}

void cm_use_intosc(void)
{
	setbits_le32(&clock_manager_base->ctrl,
		     CLKMGR_CLKMGR_CTL_BOOTCLK_INTOSC_SET_MSK);
}

static void cm_print_clock_quick_summary(void)
{
	printf("EOSC1       %8d kHz\n", eosc1_hz / 1000);
	printf("cb_intosc   %8d kHz\n", cb_intosc_hz / 1000);
	printf("f2s_free    %8d kHz\n", f2s_free_hz / 1000);
	printf("MMC         %8d kHz\n", cm_get_mmc_controller_clk_hz() / 1000);
	printf("Main VCO    %8d kHz\n", cm_get_main_vco()/1000);
	printf("L4 Main	    %8d kHz\n",
	       cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4MAINCLK_LSB)/1000);
	printf("L4 MP       %8d kHz\n",
	       cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4MPCLK_LSB)/1000);
	printf("L4 SP       %8d kHz\n",
	       cm_get_l4_noc_hz(CLKMGR_MAINPLL_NOCDIV_L4SPCLK_LSB)/1000);
}

int do_showclocks(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cm_print_clock_quick_summary();
	return 0;
}

U_BOOT_CMD(
	clocks, CONFIG_SYS_MAXARGS, 1, do_showclocks,
	"display clocks",
	""
);
