/*
 *  Copyright (C) 2014-2016 Altera Corporation <www.altera.com>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __CONFIG_SOCFGPA_ARRIA10_H__
#define __CONFIG_SOCFGPA_ARRIA10_H__

#include <asm/arch/hardware.h>
#include <asm/arch/clock_manager.h>

#include <config_cmd_default.h>

#define CONFIG_SOCFPGA_ARRIA10
#define CONFIG_SOCFPGA_COMMON 1
/*
 * Device tree info
 */
#define CONFIG_UBOOT_DTB_SIZE		(10 * 1024)
#undef CONFIG_SEMIHOSTING
/*
 * High level configuration
 */
/* ARMv7 CPU Core */
#define CONFIG_ARMV7
/* SOCFPGA Specific function */
#define CONFIG_SOCFPGA
/* Support for IRQs - for ocram and SDRAM ECC */
#define CONFIG_USE_IRQ
/* Enable early stage initialization at C environment */
#define CONFIG_BOARD_EARLY_INIT_F
/* miscellaneous platform dependent initialisations */
#define CONFIG_MISC_INIT_R
/* Enable board late init for ECC setup if IRQ enabled */
#define CONFIG_BOARD_LATE_INIT

/* Cache options */
#ifdef TEST_AT_ASIMOV
#undef  CONFIG_SYS_DCACHE_OFF
#else
#define CONFIG_SYS_DCACHE_OFF
#endif
#define CONFIG_CMD_CACHE
#define CONFIG_SYS_CACHELINE_SIZE	32
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE		SOCFPGA_MPUL2_ADDRESS

/* base address for .text section. Ensure located start of OCRAM */
#ifdef TEST_AT_ASIMOV
/*#define CONFIG_SYS_TEXT_BASE		0xc0040000*/
#define CONFIG_SYS_TEXT_BASE		0x10000000
#else
#define CONFIG_SYS_TEXT_BASE		0xFFE00000
#endif
/* using linker to check all image sections fit OCRAM */
#define CONFIG_SYS_LDSCRIPT		$(TOPDIR)/$(CPUDIR)/$(SOC)/u-boot.lds
#define CONFIG_U_BOOT_BINARY_MAX_SIZE	(200 * 1024)

/*
 * Stack and malloc setup
 */
/* IRQ stack */
#define CONFIG_STACKSIZE_IRQ		(1 << 10)
/* FIQ stack */
#define CONFIG_STACKSIZE_FIQ		(1 << 10)
/* SP location before relocation, must use scratch RAM */
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_TEXT_BASE
/* Reserving 0x400 space at back of scratch RAM for debug info */
#define CONFIG_SYS_INIT_RAM_SIZE	(256 * 1024)
/* Stack pointer at on-chip RAM, leave 16kB behind for page table */
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE  - 0x4000)
/* Default load address */
#define CONFIG_SYS_LOAD_ADDR		0x8000

/*
 * Display CPU and Board Info
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

/*
 * Kernel Info
 */
/* flat device tree */
#define CONFIG_OF_LIBFDT
/* skip updating the FDT blob */
#define CONFIG_FDT_BLOB_SKIP_UPDATE
/* Initial Memory map size for Linux, minus 4k alignment for DFT blob */
#define CONFIG_SYS_BOOTMAPSZ		(32 * 1024 * 1024)



/*
 * Console setup
 */
/* Monitor Command Prompt */
#define CONFIG_SYS_PROMPT		"SOCFPGA_ARRIA10 # "

/* EMAC controller and PHY used */
#define CONFIG_EMAC_BASE		SOCFPGA_EMAC0_ADDRESS
#define CONFIG_EPHY_PHY_ADDR		CONFIG_EPHY0_PHY_ADDR
#define CONFIG_PHY_INTERFACE_MODE	PHY_INTERFACE_MODE_RGMII

/*
 * No more SPLas we want to reduce to 3 stages
 */
#undef CONFIG_SPL
#undef CONFIG_SPL_FRAMEWORK

/*
 * Command line configuration.
 */
/* #define CONFIG_SYS_NO_FLASH */
/* FAT file system support */


/*
 * Remove the U-Boot unused feature so can make U-Boot smaller
 * TODO: standardized for Asimov so Asimov can boot faster too
 */
#define CONFIG_SYS_THUMB_BUILD
#undef CONFIG_CMD_EXT2
#undef CONFIG_GZIP
#undef CONFIG_ZLIB
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_SETGETDCR
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_ITEST
#undef CONFIG_CMD_EXT2
#undef CONFIG_CMD_MTDPARTS
#undef CONFIG_CMD_BOOTD
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_SDRAM
#undef CONFIG_BOOTM_VXWORKS
#undef CONFIG_CMD_NFS
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS
#undef CONFIG_BOOTM_VXWORKS
/*#undef CONFIG_CMD_BOOTM*/
#undef CONFIG_CMD_CONSOLE
#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_MD5

#define CONFIG_CMD_MII

/*
 * Security support
 */
#if 0
#define CONFIG_RSA
#define CONFIG_FIT
#define CONFIG_FIT_SIGNATURE
/* enable fit_format_{error,warning}() */
#define CONFIG_FIT_VERBOSE

/*
 * Hash command support
 */
#define CONFIG_CMD_HASH
#define CONFIG_CMD_SHA1SUM
#define CONFIG_HASH_VERIFY
#define CONFIG_SHA1
#endif

/*
 * Misc
 */
/* Enable DOS partition */
#define CONFIG_DOS_PARTITION
/* Support loading of zImage */
#define CONFIG_CMD_BOOTZ

/*
 * Environment setup
 */

/* Delay before automatically booting the default image */
#define CONFIG_BOOTDELAY		5
/* write protection for vendor parameters is completely disabled */
#define CONFIG_ENV_OVERWRITE
/* Enable auto completion of commands using TAB */
#define CONFIG_AUTO_COMPLETE
/* Enable editing and history functions for interactive CLI operations */
#define CONFIG_CMDLINE_EDITING
/* Additional help message */
#define CONFIG_SYS_LONGHELP
/* use "hush" command parser */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

/*
 * Can't poll in semihosting; so turn off automatic boot command
 */
#ifdef CONFIG_SEMIHOSTING
#define CONFIG_BOOTCOMMAND ""
#elif defined(CONFIG_MMC)
#define CONFIG_BOOTCOMMAND " run core_rbf_prog; run callscript; run mmcload;" \
	"run set_initswstate; run mmcboot"
#elif defined(CONFIG_CADENCE_QSPI)
#define CONFIG_BOOTCOMMAND "run qspirbfcore_rbf_prog; run qspiload;" \
	"run set_initswstate; run qspiboot"
#elif defined(CONFIG_NAND_DENALI)
#define CONFIG_BOOTCOMMAND "run nandrbfcore_rbf_prog; run nandload;" \
	"run set_initswstate; run nandboot"
#else
#error "unsupported configuration"
#endif

/*
 * arguments passed to the bootz command. The value of
 * CONFIG_BOOTARGS goes into the environment value "bootargs".
 * Do note the value will overide also the chosen node in FDT blob.
 */
#define CONFIG_BOOTARGS "console=ttyS0," __stringify(CONFIG_BAUDRATE)
#define CONFIG_SYS_DTB_ADDR 0x100
#define MAX_DTB_SIZE_IN_RAM 0x7f00
#if ((CONFIG_SYS_DTB_ADDR + MAX_DTB_SIZE_IN_RAM) > CONFIG_SYS_LOAD_ADDR)
#error "MAX_DTB_SIZE_IN_RAM is too big. It will overwrite zImage in memory."
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"verify=y\0" \
	"loadaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"fdtaddr=" __stringify(CONFIG_SYS_DTB_ADDR) "\0" \
	"bootimage=zImage\0" \
	"bootimagesize=0x600000\0" \
	"fdtimage=socfpga_arria10_socdk.dtb\0" \
	"fdtimagesize=" __stringify(MAX_DTB_SIZE_IN_RAM) "\0" \
	"fdt_high=0x2000000\0" \
	"mmcloadcmd=fatload\0" \
	"mmcloadpart=1\0" \
	"mmcroot=/dev/mmcblk0p2\0" \
	"qspiloadcs=0\0" \
	"qspibootimageaddr=0x120000\0" \
	"qspifdtaddr=0x100000\0" \
	"qspirbfaddr=" __stringify(CONFIG_QSPI_RBF_ADDR) "\0" \
	"qspiroot=/dev/mtdblock1\0" \
	"qspirootfstype=jffs2\0" \
	"nandbootimageaddr=0x120000\0" \
	"nandfdtaddr=0x100000\0" \
	"nandrbfcoreimage=0x820000\0" \
	"nandroot=/dev/mtdblock1\0" \
	"nandrootfstype=jffs2\0" \
	"nandrbfcore_rbf_prog=fpga loadfs 0 nand 0:0 ${nandrbfcoreimage} core\0 "\
	"ramboot=setenv bootargs " CONFIG_BOOTARGS " printk.time=1 debug mem=${fdt_high} " \
		"lpj=3977216;fpgabr 1; bootz ${loadaddr} - ${fdtaddr}\0" \
	"mmcload=mmc rescan;" \
		"${mmcloadcmd} mmc 0:${mmcloadpart} ${loadaddr} ${bootimage};" \
		"${mmcloadcmd} mmc 0:${mmcloadpart} ${fdtaddr} ${fdtimage}\0" \
	"mmcboot=setenv bootargs " CONFIG_BOOTARGS \
		" root=${mmcroot} rw rootwait;" \
		"fpgabr 1;" \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"netboot=dhcp ${bootimage} ; " \
		"tftp ${fdtaddr} ${fdtimage} ; run ramboot\0" \
	"qspiload=sf probe ${qspiloadcs};" \
		"sf read ${loadaddr} ${qspibootimageaddr} ${bootimagesize};" \
		"sf read ${fdtaddr} ${qspifdtaddr} ${fdtimagesize};\0" \
	"qspiboot=setenv bootargs " CONFIG_BOOTARGS \
		" root=${qspiroot} rw rootfstype=${qspirootfstype};"\
		"fpgabr 1;" \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"nandload=nand read ${loadaddr} ${nandbootimageaddr} ${bootimagesize};"\
		"nand read ${fdtaddr} ${nandfdtaddr} ${fdtimagesize}\0" \
	"nandboot=setenv bootargs " CONFIG_BOOTARGS \
		" root=${nandroot} rw rootfstype=${nandrootfstype};"\
		"fpgabr 1;" \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"bootcmd=" CONFIG_BOOTCOMMAND "\0" \
	"u-boot_swstate_reg=0xffd0620c\0" \
	"u-boot_image_valid=0x49535756\0" \
	"set_initswstate=" \
		"mw ${u-boot_swstate_reg} ${u-boot_image_valid}\0" \
	"ksz9031-rgmii-ctrl-skew=0x70\0" \
	"ksz9031-rgmii-rxd-skew=0x7777\0" \
	"ksz9031-rgmii-txd-skew=0x0\0" \
	"ksz9031-rgmii-clock-skew=0x3fc\0" \
	"fpga=0\0" \
	"fpgadata=0x2000000\0" \
	"fpgadatasize=0x700000\0" \
	"rbftosdramaddr=0x40\0" \
	"rbfcoreimage=ghrd_10as066n2.core.rbf\0" \
	"qspirbfcoreimage=0x820000\0" \
	"qspirbfcore_rbf_prog=fpga loadfs 0 qspi 0:0 ${qspirbfcoreimage} core\0 "\
	"core_rbf_prog=fpga loadfs 0 mmc 0:1 ${rbfcoreimage} core\0 "\
	CONFIG_KSZ9021_CLK_SKEW_ENV "=" \
		__stringify(CONFIG_KSZ9021_CLK_SKEW_VAL) "\0" \
	CONFIG_KSZ9021_DATA_SKEW_ENV "=" \
		__stringify(CONFIG_KSZ9021_DATA_SKEW_VAL) "\0" \
	"scriptfile=u-boot.scr\0" \
	"callscript=if fatload mmc 0:1 $fpgadata $scriptfile;" \
			"then source $fpgadata; " \
		"else " \
			"echo Optional boot script not found. " \
			"Continuing to boot normally; " \
		"fi;\0"

/*
 * Environment setup
 */
/* using environment setting for stdin, stdout, stderr */
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
/* Enable the call to overwrite_console() */
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
/* Enable overwrite of previous console environment settings */
#define CONFIG_SYS_CONSOLE_ENV_OVERWRITE
/* Environment from flash not allowed as not secure */
/* Environtment import command not allowed as not secure too */
#undef CONFIG_CMD_IMPORTENV

/*
 * Console setup
 */
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
/* max number of command args */
#define CONFIG_SYS_MAXARGS		16

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/*
 * Hardware drivers
 */

/*
 * SDRAM Memory Map
 */
/* We have 1 bank of DRAM */
#define CONFIG_NR_DRAM_BANKS		1
/* SDRAM Bank #1 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
/* Current emulator using 32MB */
#define PHYS_SDRAM_1_SIZE		0x2000000
/* SDRAM Bank #1 base address */
#define PHYS_SDRAM_1			CONFIG_SYS_SDRAM_BASE
/* U-Boot memtest setup */
/* Begin and end addresses of the area used by the simple memory test.c */
#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END		PHYS_SDRAM_1_SIZE
/* Enable mtest, SDRAM memory test console command */
#define CONFIG_CMD_MEMTEST
/* Enable alternate, more extensive memory test */
#define CONFIG_SYS_ALT_MEMTEST
#ifdef CONFIG_SYS_ALT_MEMTEST
/* We put the scratch address at end of stack which won't be used */
#define CONFIG_SYS_MEMTEST_SCRATCH	(CONFIG_SYS_INIT_SP_ADDR - \
					CONFIG_OCRAM_STACK_SIZE)
#endif

/* Enable command to do ddr calibration */
#define CONFIG_CMD_DDR

/* Enable command to control fpga bridges */
#define CONFIG_CMD_FPGABR

/*
 * Generic Interrupt Controller from ARM
 */
#define SOCFPGA_GIC_CPU_IF		(SOCFPGA_MPUSCU_ADDRESS + 0x100)
#define SOCFPGA_GIC_DIC			(SOCFPGA_MPUSCU_ADDRESS + 0x1000)

/*
 * SCU Non-secure Access Control
 */
#define SOCFPGA_SCU_SNSAC		(SOCFPGA_MPUSCU_ADDRESS + 0x54)

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#ifdef CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
/* always write in 32 bit manner */
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_CONS_INDEX               1
#define CONFIG_SYS_NS16550_COM1		SOCFPGA_UART1_ADDRESS
#define CONFIG_SYS_BAUDRATE_TABLE {4800, 9600, 19200, 38400, 57600, 115200}
#define CONFIG_SYS_NS16550_CLK		(cm_l4_sp_clk_hz)

#define CONFIG_BAUDRATE			115200
#endif /* CONFIG_SYS_NS16550 */

/*
 * FLASH
 */
#define CONFIG_SYS_NO_FLASH 

/*
 * L4 OSC1 Timer 0
 */
/* This timer use eosc1 where the clock frequency is fixed
 * throughout any condition */
#define CONFIG_SYS_TIMERBASE		SOCFPGA_OSC1TIMER0_ADDRESS
/* address of timer counter value */
#define CONFIG_SYS_TIMER_COUNTER	(CONFIG_SYS_TIMERBASE + 0x4)
/* reload value when timer count to zero */
#define TIMER_LOAD_VAL			0xFFFFFFFF
/* Clocks source frequency to timer */
#ifdef TEST_AT_ASIMOV
/* Preloader and U-Boot need to know the clock source frequency from handoff*/
#define CONFIG_TIMER_CLOCK_KHZ		(CONFIG_HPS_CLK_OSC1_HZ / 1000)
#define CONFIG_SYS_TIMER_RATE		(CONFIG_HPS_CLK_OSC1_HZ)
#else
#define CONFIG_TIMER_CLOCK_HZ		(100000000)
#define CONFIG_TIMER_CLOCK_KHZ		(CONFIG_TIMER_CLOCK_HZ/1000)
#define CONFIG_SYS_TIMER_RATE		(100000000)
#endif
/* DesignWare timer is a countdown timer */
#define CONFIG_SYS_TIMER_COUNTS_DOWN

/*
 * L4 Watchdog
 */
#if (CONFIG_PRELOADER_WATCHDOG_ENABLE == 1)
#define CONFIG_HW_WATCHDOG
#define CONFIG_HW_WATCHDOG_TIMEOUT_MS	(2000)
#define CONFIG_DESIGNWARE_WATCHDOG
#define CONFIG_DW_WDT_BASE		SOCFPGA_L4WD0_ADDRESS
/* Clocks source frequency to watchdog timer */
/* Preloader and U-Boot need to know the clock source frequency from handoff*/
#define CONFIG_DW_WDT_CLOCK_KHZ		(CONFIG_HPS_CLK_OSC1_HZ / 1000)
#endif /* CONFIG_PRELOADER_WATCHDOG_ENABLE */

/*
 * network support
 */
#define CONFIG_DESIGNWARE_ETH
#ifdef CONFIG_DESIGNWARE_ETH
#define CONFIG_TX_DESCR_NUM		2
#define CONFIG_RX_DESCR_NUM		2
/* console support for network */
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
/* designware */
#define CONFIG_NET_MULTI
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_DW_SEARCH_PHY
#define CONFIG_MII
#define CONFIG_PHY_GIGE
#define CONFIG_DW_AUTONEG
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#ifdef TEST_AT_ASIMOV
#define CONFIG_PHY_MICREL_KSZ9021
#endif
/* phy */
#define CONFIG_EPHY0_PHY_ADDR		7
#endif	/* CONFIG_DESIGNWARE_ETH */

/* these are in devault environment so they must be always defined */
#define CONFIG_KSZ9021_CLK_SKEW_ENV	"micrel-ksz9021-clk-skew"
#define CONFIG_KSZ9021_CLK_SKEW_VAL	0xf0f0
#define CONFIG_KSZ9021_DATA_SKEW_ENV	"micrel-ksz9021-data-skew"
#define CONFIG_KSZ9021_DATA_SKEW_VAL	0x0

/*
 * MMC support
 */

#ifdef CONFIG_MMC

#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0/* device 0 */
#define CONFIG_ENV_OFFSET		512/* just after the MBR */

#define CONFIG_CMD_FAT
#define CONFIG_CMD_MMC
/* Enable FAT write support */
#define CONFIG_FAT_WRITE

/* configure a clustsize smaller than the default 64k */
#define CONFIG_FS_FAT_MAX_CLUSTSIZE 16384
/* MMC support */
#define CONFIG_SDMMC_BASE		(SOCFPGA_SDMMC_ADDRESS)
#define CONFIG_GENERIC_MMC
#define CONFIG_SYS_MMC_MAX_BLK_COUNT    256
#define CONFIG_DWMMC
#define CONFIG_SOCFPGA_DWMMC
#define CONFIG_SOCFPGA_DWMMC_DRVSEL     3
#define CONFIG_SOCFPGA_DWMMC_SMPSEL     0
#define CONFIG_SOCFPGA_DWMMC_FIFO_DEPTH	1024
/* using smaller max blk cnt to avoid flooding the limited stack we have */
#define CONFIG_SOCFPGA_DWMMC_BUS_HZ	CONFIG_HPS_CLK_SDMMC_HZ
#define CONFIG_SOCFPGA_DWMMC_BUS_WIDTH	4
/* requird for dw_mmc driver */
#define CONFIG_BOUNCE_BUFFER
#endif	/* CONFIG_MMC */

/*
 * QSPI support
 */
#define CONFIG_CQSPI_BASE		(SOCFPGA_QSPIREGS_ADDRESS)
#define CONFIG_CQSPI_AHB_BASE		(SOCFPGA_QSPIDATA_ADDRESS)
#ifdef CONFIG_CADENCE_QSPI
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_QSPI_RBF_ADDR 		0x720000
#define CONFIG_SPI_FLASH		/* SPI flash subsystem */
#define CONFIG_SPI_FLASH_STMICRO	/* Micron/Numonyx flash */
#define CONFIG_SPI_FLASH_SPANSION	/* Spansion flash */
#define CONFIG_SPI_FLASH_BAR		/* Enable access > 16MiB */
#define CONFIG_CMD_SF			/* Serial flash commands */
/* Flash device info */
#define CONFIG_SF_DEFAULT_SPEED		(50000000)
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#define CONFIG_SPI_FLASH_QUAD		(1)
/* QSPI reference clock */
#define CONFIG_CQSPI_REF_CLK		(cm_l4_main_clk_hz)
/* QSPI page size and block size */
#define CONFIG_CQSPI_PAGE_SIZE		(256)
#define CONFIG_CQSPI_BLOCK_SIZE		(16)
/* QSPI Delay timing */
#define CONFIG_CQSPI_TSHSL_NS		(200)
#define CONFIG_CQSPI_TSD2D_NS		(255)
#define CONFIG_CQSPI_TCHSH_NS		(20)
#define CONFIG_CQSPI_TSLCH_NS		(20)
#define CONFIG_CQSPI_DECODER		(0)
#endif	/* CONFIG_CADENCE_QSPI */

/*
 * NAND
 */
#ifdef CONFIG_NAND_DENALI

#ifdef CONFIG_MMC
#error Cannot define CONFIG_MMC when CONFIG_NAND_DENALI is also defined.
#endif
#ifdef CONFIG_CADENCE_QSPI
#error Cannot define CONFIG_CADENCE_QSPI when \
CONFIG_NAND_DENALI is also defined.
#endif

/* remove a bunch of stuff so we have enough room for the nand code */
#undef CONFIG_CMD_PCMCIA
#undef CONFIG_CMD_PING
#undef CONFIG_CMD_MII
#undef CONFIG_CMD_FLASH
#undef CONFIG_AUTO_COMPLETE
#undef CONFIG_CMDLINE_EDITING
#undef CONFIG_SYS_HUSH_PARSER
#undef CONFIG_SYS_PROMPT_HUSH_PS2
#undef CONFIG_CMD_MEMTEST
#undef CONFIG_SYS_ALT_MEMTEST
#undef CONFIG_SYS_MEMTEST_SCRATCH
#undef CONFIG_CMD_DDR
#undef CONFIG_DESIGNWARE_ETH
#undef CONFIG_TX_DESCR_NUM
#undef CONFIG_RX_DESCR_NUM
#undef CONFIG_CMD_DHCP
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_PING
#undef CONFIG_NET_MULTI
#undef CONFIG_DW_ALTDESCRIPTOR
#undef CONFIG_DW_SEARCH_PHY
#undef CONFIG_MII
#undef CONFIG_PHY_GIGE
#undef CONFIG_DW_AUTONEG
#undef CONFIG_PHYLIB
#undef CONFIG_PHY_MICREL
#undef CONFIG_PHY_MICREL_KSZ9021
#undef CONFIG_EPHY0_PHY_ADDR
#undef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_SYS_MMC_ENV_DEV
#undef CONFIG_ENV_OFFSET

/* here are the defines for NAND */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_NAND_RBF_ADDR		0x720000
#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
/* the following, when defined, requries 128K from malloc! */
#undef CONFIG_SYS_NAND_USE_FLASH_BBT
#define CONFIG_SYS_NAND_ONFI_DETECTION
/* will not compile with the following defined */
#undef CONFIG_SYS_NAND_SELF_INIT
#define CONFIG_SYS_NAND_REGS_BASE	0xffb80000
#define CONFIG_SYS_NAND_DATA_BASE	0xffb90000
#define CONFIG_SYS_NAND_BASE	        CONFIG_SYS_NAND_REGS_BASE
/* The ECC size which either 512 or 1024 */
#define CONFIG_NAND_DENALI_ECC_SIZE			(512)
#endif /* CONFIG_NAND_DENALI */

/*
 * FPGA support
 */
/* Enables FPGA subsystem */
#define CONFIG_FPGA
#ifdef CONFIG_FPGA
/* Altera FPGA */
#define CONFIG_FPGA_ALTERA
/* Family type */
#define CONFIG_FPGA_SOCFPGA_ARRIA10
/* Only support single device */
#define CONFIG_FPGA_COUNT		(1)
/* Enable FPGA command at console */
#define CONFIG_CMD_FPGA
#define CONFIG_CMD_FPGA_LOADFS
#endif /* CONFIG_FPGA */

/*
 * Memory allocation (MALLOC)
 */

/* size of stack and malloc in ocram */
#ifdef CONFIG_DESIGNWARE_ETH
#define CONFIG_OCRAM_MALLOC_SIZE	(35 * 1024)
#elif defined(CONFIG_NAND_DENALI)
#define CONFIG_OCRAM_MALLOC_SIZE	(45 * 1024)
#else
#define CONFIG_OCRAM_MALLOC_SIZE	(16 * 1024)
#endif

#define CONFIG_OCRAM_STACK_SIZE		(20 * 1024)

/* Room required on the stack for the environment data */
#define CONFIG_ENV_SIZE			4096
/* Size of DRAM reserved for malloc() use */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_OCRAM_MALLOC_SIZE - \
					CONFIG_ENV_SIZE)

/*
 * DMA support
 */
#define CONFIG_PL330_DMA

#define CONFIG_SYS_GENERIC_BOARD

/*
 * SPI support
 */
#undef CONFIG_DESIGNWARE_SPI
#ifdef CONFIG_DESIGNWARE_SPI
#define CONFIG_CMD_SPI
/* SPI reference clock */
#define CONFIG_SPI_REF_CLK	(cm_l4_mp_clk_hz)
/* The base address of the SPI master */
#define CONFIG_SYS_SPI_BASE	0xffda5000
/* Platform frequency */
#define SPI_MAX_FREQUENCY	1000000
#endif

/*
 * I2C support
 * Disabled by default to free up OCRAM space. Compile using linaro 4.9
 * appears to produce more efficient code.
 */
#undef CONFIG_I2C
#ifdef CONFIG_I2C
#define CONFIG_HARD_I2C
#define CONFIG_DW_I2C
#define CONFIG_SYS_I2C_BASE             SOCFPGA_I2C1_ADDRESS
/* using standard mode which the speed up to 100Kb/s) */
#define CONFIG_SYS_I2C_SPEED            (100000)
/* address of device when used as slave */
#define CONFIG_SYS_I2C_SLAVE            (0x02)
#define CONFIG_HPS_CLK_I2C_HZ		(cm_l4_sp_clk_hz)
/* clock supplied to I2C controller in unit of MHz */
#define IC_CLK                          (CONFIG_HPS_CLK_I2C_HZ / 1000000)
#define CONFIG_CMD_I2C
#endif	/* #ifdef CONFIG_I2C */

#endif	/* __CONFIG_H */
