/*
 * FB driver for the ILI9341 LCD display controller
 *
 * This display uses 9-bit SPI: Data/Command bit + 8 data bits
 * For platforms that doesn't support 9-bit, the driver is capable
 * of emulating this using 8-bit transfer.
 * This is done by transfering eight 9-bit words in 9 bytes.
 *
 * Copyright (C) 2013 Christian Vogelgsang
 * Based on adafruit22fb.c by Noralf Tronnes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include "fbtft.h"

#define DRVNAME		"fb_ili9341_txt"
#define WIDTH		240
#define HEIGHT		320
#define TXBUFLEN	(4 * PAGE_SIZE)
#define DEFAULT_GAMMA	"1F 1A 18 0A 0F 06 45 87 32 0A 07 02 07 05 00\n" \
			"00 25 27 05 10 09 3A 78 4D 05 18 0D 38 3A 1F"

//----------------------------------------------------------------------------------

//
// Encapsulating peripheral functional clocks
// pll registers
//
struct cm_per_pll               // Adress 0x44E0_0000
{ 
	u32 l4ls_clkstctrl;         // offset 0x00
	u32 l3s_clkstctrl;          // offset 0x04
	u32 res_offset_0x08;        // offset 0x08
	u32 l3_clkstctrl;	        // offset 0x0c
	u32 res_offset_0x10;        // offset 0x10
	u32 cpgmac0_clkctrl;        // offset 0x14 
	u32 lcdc_clkctrl;           // offset 0x18 
	u32 usb0_clkctrl;           // offset 0x1C 
	u32 res_offset_0x20;        // offset 0x20
	u32 tptc0_clkctrl;          // offset 0x24 
	u32 emif_clkctrl;           // offset 0x28 
	u32 ocmcram_clkctrl;        // offset 0x2c 
	u32 gpmc_clkctrl;           // offset 0x30 
	u32 mcasp0_clkctrl;         // offset 0x34 
	u32 uart5_clkctrl;          // offset 0x38 
	u32 mmc0_clkctrl;           // offset 0x3C 
	u32 elm_clkctrl;            // offset 0x40 
	u32 i2c2_clkctrl;           // offset 0x44 
	u32 i2c1_clkctrl;           // offset 0x48 
	u32 spi0_clkctrl;           // offset 0x4C 
	u32 spi1_clkctrl;           // offset 0x50 
	u32 res_offset_0x54;        // offset 0x54
	u32 res_offset_0x58;        // offset 0x58
	u32 res_offset_0x5c;        // offset 0x5c
	u32 l4ls_clkctrl;           // offset 0x60 
	u32 res_offset_0x64;        // offset 0x64
	u32 mcasp1_clkctrl;         // offset 0x68 
	u32 uart1_clkctrl;          // offset 0x6C 
	u32 uart2_clkctrl;          // offset 0x70 
	u32 uart3_clkctrl;          // offset 0x74 
	u32 uart4_clkctrl;          // offset 0x78 
	u32 timer7_clkctrl;         // offset 0x7C 
	u32 timer2_clkctrl;         // offset 0x80 
	u32 timer3_clkctrl;         // offset 0x84 
	u32 timer4_clkctrl;         // offset 0x88 
	u32 res_offset_0x8c;        // offset 0x8c
	u32 res_offset_0x90;        // offset 0x90
	u32 res_offset_0x94;        // offset 0x94
	u32 res_offset_0x98;        // offset 0x98
	u32 res_offset_0x9c;        // offset 0x9c
	u32 res_offset_0xA0;        // offset 0xA0
	u32 res_offset_0xA4;        // offset 0xA4
	u32 res_offset_0xA8;        // offset 0xA8
	u32 gpio1_clkctrl;          // offset 0xAC 
	u32 gpio2_clkctrl;          // offset 0xB0 
	u32 gpio3_clkctrl;          // offset 0xB4 
	u32 res_offset_0xb8;        // offset 0xB8
	u32 tpcc_clkctrl;           // offset 0xBC 
	u32 dcan0_clkctrl;          // offset 0xC0 
	u32 dcan1_clkctrl;          // offset 0xC4 
	u32 res_offset_0xc8;        // offset 0xC8
	u32 epwmss1_clkctrl;        // offset 0xCC 
	u32 res_offset_0xd0;        // offset 0xD0
	u32 epwmss0_clkctrl;        // offset 0xD4 
	u32 epwmss2_clkctrl;        // offset 0xD8 
	u32 l3_instr_clkctrl;       // offset 0xDC 
	u32 l3_clkctrl;             // Offset 0xE0 
	u32 ieee5000_clkctrl;       // Offset 0xE4
	u32 pru_icss_clkctrl;       // Offset 0xE8
	u32 timer5_clkctrl;         // offset 0xEC
	u32 timer6_clkctrl;         // offset 0xF0
	u32 mmc1_clkctrl;           // offset 0xF4 
	u32 mmc2_clkctrl;           // offset 0xF8 
	u32 tptc1_clkctrl;          // offset 0xFC
	u32 tptc2_clkctrl;          // offset 0x100
	u32 res_offset_0x104;       // offset 0x104
	u32 res_offset_0x108;       // offset 0x108
	u32 spinlock_clkctrl;       // offset 0x10c
	u32 mailbox0_clkctrl;       // offset 0x110
	u32 res_offset_0x114;       // offset 0x114
	u32 res_offset_0x118;       // offset 0x118
	u32 l4hs_clkstctrl;         // offset 0x11C 
	u32 l4hs_clkctrl;           // offset 0x120 
	u32 res_offset_0x124;       // offset 0x124
	u32 res_offset_0x128;       // offset 0x128
	u32 ocpwp_l3_clkstctrl;     // offset 0x12C
	u32 ocpwp_l3_clkctrl;       // offset 0x130
	u32 res_offset_0x134;       // offset 0x134
	u32 res_offset_0x138;       // offset 0x138
	u32 res_offset_0x13c;       // offset 0x13C
	u32 pru_icss_clkstctrl;     // offset 0x140
	u32 cpsw_clkstctrl;         // offset 0x144
	u32 lcdc_clkstctrl;         // offset 0x148
	u32 clk32div32k_clkctrl;    // offset 0x14c
	u32 clk_24mhz_clkstct;      // offset 0x150
};

//  Encapsulating core pll registers
struct cm_wkup_pll                  // Adress 0x44E0_0400
{ 
	u32 wkup_clkstctrl;             // offset 0x00 
	u32 wkup_ctrl_clkctrl;          // offset 0x04 
	u32 wkup_gpio0_clkctrl;         // offset 0x08 
	u32 wkup_l4wkup_clkctrl;        // offset 0x0C 
	u32 wkup_timer0_clkctrl;        // offset 0x10 
	u32 wkup_debugss_clkctrl;       // offset 0x14
	u32 l3_aon_clkstctrl;           // offset 0x18
	u32 autoidle_dpll_mpu;          // offset 0x1C
	u32 idlest_dpll_mpu;            // offset 0x20 
	u32 ssc_deltamstep_dpll_mpu;    // offset 0x24
	u32 ssc_modfreqdiv_dpll_mpu;    // offset 0x28
	u32 clksel_dpll_mpu;            // offset 0x2C
	u32 autoidle_dpll_ddr;          // offset 0x30
	u32 idlest_dpll_ddr;            // offset 0x34
	u32 ssc_deltamstep_dpll_ddr;    // offset 0x38
	u32 ssc_modfreqdiv_dpll_ddr;    // offset 0x3C
	u32 clksel_dpll_ddr;            // offset 0x40
	u32 autoidle_dpll_disp;         // offset 0x44
	u32 idlest_dpll_disp;           // offset 0x48
	u32 ssc_deltamstep_dpll_disp;   // offset 0x4C
	u32 ssc_modfreqdiv_dpll_disp;   // offset 0x50
	u32 clksel_dpll_disp;           // offset 0x54
	u32 autoidle_dpll_core;         // offset 0x58
	u32 idlest_dpll_core;           // offset 0x5C
	u32 ssc_deltamstep_dpll_core;   // offset 0x60
	u32 ssc_modfreqdiv_dpll_core;   // offset 0x64
	u32 clksel_dpll_core;           // offset 0x68
	u32 autoidle_dpll_per;          // offset 0x6C
	u32 idlest_dpll_per;            // offset 0x70
	u32 ssc_deltamstep_dpll_per;    // offset 0x74
	u32 ssc_modfreqdiv_dpll_per;    // offset 0x78
	u32 clkdcoldo_dpll_per;         // offset 0x7C
	u32 div_m4_dpll_core;           // offset 0x80 
	u32 div_m5_dpll_core;           // offset 0x84 
	u32 clkmod_dpll_mpu;            // offset 0x88 
	u32 clkmod_dpll_per;            // offset 0x8c 
	u32 clkmod_dpll_core;           // offset 0x90 
	u32 clkmod_dpll_ddr;            // offset 0x94 
	u32 clkmod_dpll_disp;           // offset 0x98 
	u32 clksel_dpll_per;            // offset 0x9c 
	u32 div_m2_dpll_ddr;            // offset 0xA0 
	u32 div_m2_dpll_disp;           // offset 0xA4 
	u32 div_m2_dpll_mpu;            // offset 0xA8 
	u32 div_m2_dpll_per;            // offset 0xAC 
    u32 wkup_wkup_m3_clkctrl;       // offset 0xB0
    u32 wkup_uart0_clkctrl;         // offset 0xB4
    u32 wkup_i2c0_clkctrl;          // offset 0xB8
	u32 wkup_adc_tsc_clkctrl;       // offset 0xBC
    u32 wkup_smartreflex0_clkctrl;  // offset 0xC0
    u32 wkup_timer1_clkctrl;        // offset 0xC4
    u32 wkup_smartreflex1_clkctrl;  // offset 0xC8
    u32 l4_wkup_aon_clkstctrl;      // offset 0xCC
	u32 res_offset_0xd0;            // offset 0xD0
    u32 wkup_wdt1_clkctrl;          // offset 0xD0
	u32 div_m6_dpll_core;           // offset 0xD8 
};

//  Encapsulating cm dpll registers
struct cm_dpll                      // Adress 0x44E0_0500
{ 
	u32 res_offset_0x00;            // offset 0x00
	u32 clksel_timer7_clk;          // offset 0x04
	u32 clksel_timer2_clk;          // offset 0x08
	u32 clksel_timer3_clk;          // offset 0x0C
	u32 clksel_timer4_clk;          // offset 0x10
	u32 cm_mac_clksel;              // offset 0x14
	u32 clksel_timer5_clk;          // offset 0x18
	u32 clksel_timer6_clk;          // offset 0x1C
	u32 cm_cpts_rft_clksel;         // offset 0x20
	u32 res_offset_0x24;            // offset 0x24
	u32 clksel_timer1ms_clk;        // offset 0x28
	u32 clksel_gfx_fclk;            // offset 0x2C
	u32 clksel_pru_icss_ocp_clk;    // offset 0x30
	u32 clksel_lcdc_pixel_clk;      // offset 0x34
	u32 clksel_wdt1_clk;            // offset 0x38
	u32 clksel_gpio0_dbclk;         // offset 0x3C
};    
    
struct LCD_REGISTERS                // Adress 0x4830_E0000
{ 
    u32 pid;                        // offset 0x00 
    u32 ctrl;                       // offset 0x04 
    u32 res_offset_0x08;            // offset 0x08 
    u32 lidd_ctrl;                  // offset 0x0C 
    u32 lidd_cs0_conf;              // offset 0x10 
    u32 lidd_cs0_addr;              // offset 0x14
    u32 lidd_cs0_data;              // offset 0x18
    u32 lidd_cs1_conf;              // offset 0x1C
    u32 lidd_cs1_addr;              // offset 0x20
    u32 lidd_cs1_data;              // offset 0x24
    u32 raster_ctrl;                // offset 0x28
    u32 raster_timing_0;            // offset 0x2C
    u32 raster_timing_1;            // offset 0x30
    u32 raster_timing_2;            // offset 0x34
    u32 raster_subpanel;            // offset 0x38
    u32 raster_subpanel2;           // offset 0x3C
    u32 lcddma_ctrl;                // offset 0x40
    u32 lcddma_fb0_base;            // offset 0x44
    u32 lcddma_fb0_ceiling;         // offset 0x48
    u32 lcddma_fb1_base;            // offset 0x4C
    u32 lcddma_fb1_ceiling;         // offset 0x50
    u32 sysconfig;                  // offset 0x54
    u32 irqstatus_raw;              // offset 0x58
    u32 irqstatus;                  // offset 0x5C
    u32 irqenable_set;              // offset 0x60
    u32 irqenable_clear;            // offset 0x64
    u32 res_offset_0x68;            // offset 0x68
    u32 clkc_enable;                // offset 0x6C
    u32 clkc_reset;                 // offset 0x70
};

#define CM_PER                          0x44E00000
#define CM_WKUP                         0x44E00400
#define CM_DPLL                         0x44E00500

static struct LCD_REGISTERS *pLcdBase;
static unsigned short volatile  *pLcdCmd;
static unsigned short volatile  *pLcdData;

static struct cm_per_pll       *psCmPerReg;
static struct cm_wkup_pll      *psCmWkupReg;
static struct cm_dpll          *psCmDpllReg;

// Sets a window from top-left (0,0) corner to bottom-right corner (240,320  )
static void SetWindowSize(u32 u32ColStart, u32 u32RowStart, u32 u32ColEnd, u32 u32RowEnd)
{
    *pLcdCmd = 0x002A;                                          // Column Adress Set
    *pLcdData = (unsigned short) (u32ColStart >> 8) & 0x00FF;   // obere 8 Start-Adressbits setzen
    *pLcdData = (unsigned short) (u32ColStart & 0x00FF);        // untere 8 Start-Adressbits setzen
    *pLcdData = (unsigned short) (u32ColEnd >> 8) & 0x00FF;     // obere 8 End-Adressbits setzen   
    *pLcdData = (unsigned short) (u32ColEnd & 0x00FF);          // untere 8 End-Adressbits setzen  

    *pLcdCmd = 0x002B;                                          // Row (Page) Adress Set
    *pLcdData = (unsigned short) (u32RowStart >> 8) & 0x00FF;   // obere 8 Start-Adressbits setzen
    *pLcdData = (unsigned short) (u32RowStart & 0x00FF);        // untere 8 Start-Adressbits setzen
    *pLcdData = (unsigned short) (u32RowEnd >> 8) & 0x00FF;     // obere 8 End-Adressbits setzen   
    *pLcdData = (unsigned short) (u32RowEnd & 0x00FF);          // untere 8 End-Adressbits setzen  
}

// Clear Window 
static void ClearWindowRGB(u32 u32ColStart, u32 u32RowStart, u32 u32ColEnd, u32 u32RowEnd, u32 u32ColorRGB)
{  

    u32 u32Col,
        u32Row;

    SetWindowSize(u32ColStart, u32RowStart, u32ColEnd-1, u32RowEnd-1);

    *pLcdCmd = 0x002C;                                          // Memory Write, Neustart Adress Zeiger

    for (u32Row=0; u32Row<u32RowEnd; u32Row++)
    {
        for (u32Col=0; u32Col<u32ColEnd; u32Col++)
        {
        	*pLcdData = (unsigned short) u32ColorRGB;           // Daten schreiben
        }
    }
}

/**
 * fbtft_verify_gpios_txt() - txt verify_gpios() function
 * @par: Driver data
 *
 * Uses @spi, @pdev and @buswidth to determine which GPIOs is needed
 *
 * Return: 0 if successful, negative if error
 */
static int fbtft_verify_gpios_txt(struct fbtft_par *par)
{
        struct fbtft_platform_data *pdata;

        fbtft_par_dbg(DEBUG_VERIFY_GPIOS, par, "%s()\n", __func__);

        pdata = par->info->device->platform_data;
        if (!pdata) {
                dev_warn(par->info->device,
                        "%s(): buswidth value is not available\n", __func__);
                return 0;
        }

        if (!par->pdev)  
                return 0;

        return 0;
}  


static int init_display(struct fbtft_par *par)
{
	u32 u32Dummy;

	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);

	pLcdBase = ioremap_nocache(par->pdata->display.pLcdBase, 4096);


	psCmPerReg = ioremap_nocache(CM_PER, sizeof(struct cm_per_pll));
	psCmWkupReg = ioremap_nocache(CM_WKUP, sizeof(struct cm_wkup_pll));
	psCmDpllReg = ioremap_nocache(CM_DPLL, sizeof(struct cm_dpll));


    pLcdCmd = (unsigned short volatile *)  &pLcdBase->lidd_cs0_addr;
    pLcdData = (unsigned short volatile *) &pLcdBase->lidd_cs0_data;


    printk(KERN_INFO "ft-TXT|%s:: %p(%lX) %p(%lX)\n", __func__,
             pLcdCmd, par->pdata->display.pLcdCmd, 
             pLcdData, par->pdata->display.pLcdData);

/*
    // First Set PLL 
    psCmWkupReg->clkmod_dpll_disp = (psCmWkupReg->clkmod_dpll_disp & 0x0007) | 0x00000004;    // set Bypass Mode

    u32Dummy = 0;

    do
    {
        u32Dummy++;
    } while (!(psCmWkupReg->idlest_dpll_disp & 0x00000100));           // wait for BYPASS Mode

    printk(KERN_INFO "%s:: wait Bypass\n", __func__);
    
    psCmWkupReg->clksel_dpll_disp = 0x00006417;                     // set PLL Values
    psCmWkupReg->div_m2_dpll_disp = 0x00000301;                     // set PLL Values

    psCmWkupReg->clkmod_dpll_disp = (psCmWkupReg->clkmod_dpll_disp & 0x0007) | 0x00000007;    // set Lock Mode

    u32Dummy = 0;
    do
    {
        u32Dummy++;
    }
    while ((((psCmWkupReg->idlest_dpll_disp) & 0x00000001) == 0x00000000) );           // wait for Lock Mode 
    printk(KERN_INFO "%s:: wait Lock\n", __func__);


    // now PRCM Switching
    psCmDpllReg->clksel_lcdc_pixel_clk = 0x00000000;                // set to use Display PLL
    psCmPerReg->lcdc_clkctrl = 0x00000002;                          // Enable LCD Module

    // now Init LCD-Module
    pLcdBase->ctrl = 0x00000000;                // set LIDD Mode
    pLcdBase->lidd_ctrl = 0x00000003;           // MPU80 Mode   
    pLcdBase->lidd_cs0_conf = 0x1064394F;       // 0001 0000 0110 0100 0011 1001 0100 1111 = 0x1064 394F
    pLcdBase->clkc_enable = 0x00000002;         // LIDD Clock enable
*/
    // Display initialisieren
    *pLcdCmd = 0x0001;                          // 1.   Software Reset schreiben
    mdelay(120);                             //      Wait 120msek

    *pLcdCmd = 0x0028;                          // 2.   Display off

    *pLcdCmd = 0x00CF;                          // 3.   Power Control B
    *pLcdData = 0x0000;
    *pLcdData = 0x0081;
    *pLcdData = 0x0030;

    *pLcdCmd = 0x00ED;                          // 4.   Power On Sequence Control
    *pLcdData = 0x0003;
    *pLcdData = 0x0012;
    *pLcdData = 0x0081;

    *pLcdCmd = 0x00E8;                          // 5.   Driver Timing
    *pLcdData = 0x0085;
    *pLcdData = 0x0001;
    *pLcdData = 0x0079;

    *pLcdCmd = 0x00CB;                          // 6.   Driver Timing
    *pLcdData = 0x0039;
    *pLcdData = 0x002C;
    *pLcdData = 0x0000;
    *pLcdData = 0x0034;
    *pLcdData = 0x0002;

    *pLcdCmd = 0x00F7;                          // 7.   Pump ratio control
    *pLcdData = 0x0020;

    *pLcdCmd = 0x00EA;                          // 8.   Driver Timing Control B
    *pLcdData = 0x0000;
    *pLcdData = 0x0000;

    *pLcdCmd = 0x00C0;                          // 9.   Power Control
    *pLcdData = 0x0026;

    *pLcdCmd = 0x00C1;                          // 10.  Power Control 2
    *pLcdData = 0x0011;

    *pLcdCmd = 0x00C5;                          // 11.  VCOM Control
    *pLcdData = 0x0035;
    *pLcdData = 0x003E;

    *pLcdCmd = 0x00C7;                          // 12.  VCOM Control 2
    *pLcdData = 0x00BE;

    *pLcdCmd = 0x0036;                          // 13.  Memory Access Control
    *pLcdData = 0x0048;                         //      0100 0000 --> an X spiegeln, ferner statt RGB, BGR verwenden

    *pLcdCmd = 0x003A;                          // 14.  Pixel Format
    *pLcdData = 0x0055;

    *pLcdCmd = 0x00B1;                          // 15.  Frame Rate
    *pLcdData = 0x0000;
    *pLcdData = 0x001B;

    *pLcdCmd = 0x00B4;                          // 16.  Display Inversion Control
    *pLcdData = 0x0000;

    *pLcdCmd = 0x00F2;                          // 17.  Enable 3G
    *pLcdData = 0x0002;

    *pLcdCmd = 0x0026;                          // 18.  Gamma Set
    *pLcdData = 0x0001;

    *pLcdCmd = 0x00E0;                          // 19.  Positive Gamma Correction
    *pLcdData = 0x001F;
    *pLcdData = 0x001A;
    *pLcdData = 0x0018;
    *pLcdData = 0x000A;
    *pLcdData = 0x000F;
    *pLcdData = 0x0006;
    *pLcdData = 0x0045;
    *pLcdData = 0x0087;
    *pLcdData = 0x0032;
    *pLcdData = 0x000A;
    *pLcdData = 0x0007;
    *pLcdData = 0x0002;
    *pLcdData = 0x0007;
    *pLcdData = 0x0005;
    *pLcdData = 0x0000;

    *pLcdCmd = 0x00E1;                          // 20.  Negative Gamma Correction
    *pLcdData = 0x0000;
    *pLcdData = 0x0025;
    *pLcdData = 0x0027;
    *pLcdData = 0x0005;
    *pLcdData = 0x0010;
    *pLcdData = 0x0009;
    *pLcdData = 0x003A;
    *pLcdData = 0x0078;
    *pLcdData = 0x004D;
    *pLcdData = 0x0005;
    *pLcdData = 0x0018;
    *pLcdData = 0x000D;
    *pLcdData = 0x0038;
    *pLcdData = 0x003A;
    *pLcdData = 0x001F;

    *pLcdCmd = 0x002A;                          // 21.  Column Address Set
    *pLcdData = 0x0000;
    *pLcdData = 0x0000;
    *pLcdData = 0x0000;
    *pLcdData = 0x00EF;

    *pLcdCmd = 0x002B;                          // 22.  Page Address Set
    *pLcdData = 0x0000;
    *pLcdData = 0x0000;
    *pLcdData = 0x0001;
    *pLcdData = 0x003F;

    *pLcdCmd = 0x00B7;                          // 23.  Entry Mode Set
    *pLcdData = 0x0007;

    *pLcdCmd = 0x00B6;                          // 24.  Display Function Control
    *pLcdData = 0x000A;
    *pLcdData = 0x0082;
    *pLcdData = 0x0027;
    *pLcdData = 0x0000;

    *pLcdCmd = 0x0011;                          // 25.  Sleep Out
    mdelay(5);                               //      Wait 5msek
    *pLcdCmd = 0x0029;                          // 26.  Display On


/*
	ClearWindowRGB( 0, 0, 240, 320, 0xF800);
	ClearWindowRGB( 40, 40, 200, 280, 0x07F0);
	ClearWindowRGB( 80, 80, 160, 240, 0x001F);
*/

/*
	iounmap(pLcdBase);
	iounmap(psCmPerReg);
	iounmap(psCmWkupReg);
	iounmap(psCmDpllReg);
*/
	return 0;
	
//	par->fbtftops.reset(par);

	return 0;
}



static int write_lidd16_wr(struct fbtft_par *par, void *buf, size_t len)
{
        static int calls;
	u16 data;

	fbtft_par_dbg_hex(DEBUG_WRITE, par, par->info->device, u8, buf, len,
		"%s(len=%d): ", __func__, len);
		
        /*printk(KERN_INFO "ft-TXT|%s:: %d\n", __func__, calls);*/
        
        if(calls > 8) {
        	while (len) {
	        	data = *(u16 *) buf;

        	    *pLcdData = data;

		        buf += 2;
	        	len -= 2;
        	} 
	}else ++calls;
	return 0;
}

static int write_lidd16_cmd(struct fbtft_par *par, void *buf, size_t len)
{
	u16 data;

	fbtft_par_dbg_hex(DEBUG_WRITE, par, par->info->device, u8, buf, len,
		"%s(len=%d): ", __func__, len);

	if(len) {
		data = *(u16 *) buf;

    	*pLcdCmd = data;

		buf += 2;
		len -= 2;
	}

	while (len) {
		data = *(u16 *) buf;

	    *pLcdData = data;

		buf += 2;
		len -= 2;
	}

	return 0;
}



static void write_reg16_lidd16(struct fbtft_par *par, int len, ...)
{
	va_list args;
	int i, ret;
	u8 *buf = par->buf;

	va_start(args, len);
	for (i = 0; i < len; i++)
		*buf++ = (u8)va_arg(args, unsigned int);
	va_end(args);

	fbtft_par_dbg_hex(DEBUG_WRITE_REGISTER, par,
		par->info->device, u8, par->buf, len, "%s: ", __func__);

	
	ret = write_lidd16_cmd(par, par->buf, len);
	if (ret < 0) {
		dev_err(par->info->device,
			"%s: write() failed and returned %d\n", __func__, ret);
		return;
	}
}


static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	fbtft_par_dbg(DEBUG_SET_ADDR_WIN, par,
		"%s(xs=%d, ys=%d, xe=%d, ye=%d)\n", __func__, xs, ys, xe, ye);

	/* Column address set */
	*pLcdCmd = 0x2A,
	*pLcdData =	(xs >> 8) & 0xFF;
	*pLcdData =  xs & 0xFF;
	*pLcdData = (xe >> 8) & 0xFF;
	*pLcdData =  xe & 0xFF;

	/* Row adress set */
	*pLcdCmd = 0x2B,
	*pLcdData =	(ys >> 8) & 0xFF;
	*pLcdData =  ys & 0xFF;
	*pLcdData = (ye >> 8) & 0xFF;
	*pLcdData = ye & 0xFF;

	/* Memory write */
	*pLcdCmd = 0x2C;
}

#define MEM_Y   (7) /* MY row address order */
#define MEM_X   (6) /* MX column address order */
#define MEM_V   (5) /* MV row / column exchange */
#define MEM_L   (4) /* ML vertical refresh order */
#define MEM_H   (2) /* MH horizontal refresh order */
#define MEM_BGR (3) /* RGB-BGR Order */
static int set_var(struct fbtft_par *par)
{
	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);


	return 0;

	switch (par->info->var.rotate) {
	case 0:
		write_reg(par, 0x36, (1 << MEM_X) | (par->bgr << MEM_BGR));
		break;
	case 270:
		write_reg(par, 0x36,
			(1<<MEM_V) | (1 << MEM_L) | (par->bgr << MEM_BGR));
		break;
	case 180:
		write_reg(par, 0x36, (1 << MEM_Y) | (par->bgr << MEM_BGR));
		break;
	case 90:
		write_reg(par, 0x36, (1 << MEM_Y) | (1 << MEM_X) |
				     (1 << MEM_V) | (par->bgr << MEM_BGR));
		break;
	}

	return 0;
}

/*
  Gamma string format:
    Positive: Par1 Par2 [...] Par15
    Negative: Par1 Par2 [...] Par15
*/
#define CURVE(num, idx)  curves[num*par->gamma.num_values + idx]
static int set_gamma(struct fbtft_par *par, unsigned long *curves)
{
	int i;

	return 0;

	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);

	for (i = 0; i < par->gamma.num_curves; i++)
		write_reg(par, 0xE0 + i,
			CURVE(i, 0), CURVE(i, 1), CURVE(i, 2),
			CURVE(i, 3), CURVE(i, 4), CURVE(i, 5),
			CURVE(i, 6), CURVE(i, 7), CURVE(i, 8),
			CURVE(i, 9), CURVE(i, 10), CURVE(i, 11),
			CURVE(i, 12), CURVE(i, 13), CURVE(i, 14));

	return 0;
}
#undef CURVE


static struct fbtft_display display = {
	.regwidth = 16,
	.width = WIDTH,
	.height = HEIGHT,
	/*.txbuflen = TXBUFLEN,*/
	.txbuflen = -1,
	.gamma_num = 2,
	.gamma_len = 15,
	.gamma = DEFAULT_GAMMA,
	.fbtftops = {
		.write = write_lidd16_wr,
		.write_register = write_reg16_lidd16,
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.verify_gpios = fbtft_verify_gpios_txt,
	},
};
FBTFT_REGISTER_DRIVER(DRVNAME, &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);

MODULE_DESCRIPTION("FB driver for the ILI9341 LCD display controller for TXT");
MODULE_AUTHOR("Christian Vogelgsang");
MODULE_LICENSE("GPL");
