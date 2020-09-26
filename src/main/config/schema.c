#include "config/schema.h"
#include "config/resource.h"

#include "util/defs.h"

static const struct action_def dm_actions[] = {
    {0x01, IDS_GENERIC_TEST},
    {0x00, IDS_GENERIC_SERVICE},

    {0x08, IDS_DM_START},
    {0x0F, IDS_DM_MENU_LEFT},
    {0x11, IDS_DM_MENU_RIGHT},

    {0x0A, IDS_DM_HI_HAT},
    {0x0C, IDS_DM_SNARE},
    {0x0E, IDS_DM_HIGH_TOM},
    {0x10, IDS_DM_LOW_TOM},
    {0x12, IDS_DM_CYMBAL},
    {0x16, IDS_DM_BASS}};

static const struct light_def dm_lights[] = {
    {0x00, IDS_DM_HI_HAT},
    {0x01, IDS_DM_SNARE},
    {0x02, IDS_DM_HIGH_TOM},
    {0x03, IDS_DM_LOW_TOM},
    {0x04, IDS_DM_CYMBAL},
    {0x09, IDS_DM_BASS},
    {0x0A, IDS_DM_SPEAKER_LIGHT},
    {0x08, IDS_DM_SPOT_LIGHT},
    {0x06, IDS_DM_START_LIGHT},
    {0x07, IDS_DM_MENU_LIGHT}};

static const struct action_def gf_actions[] = {
    {0x01, IDS_GENERIC_TEST}, {0x00, IDS_GENERIC_SERVICE},

    {0x08, IDS_GF_P1_START},  {0x0A, IDS_GF_P1_PICK},
    {0x18, IDS_GF_P1_PICK_A}, {0x19, IDS_GF_P1_PICK_B},
    {0x1C, IDS_GF_P1_EFFECT}, {0x0C, IDS_GF_P1_WAIL},
    {0x12, IDS_GF_P1_RED},    {0x14, IDS_GF_P1_GREEN},
    {0x16, IDS_GF_P1_BLUE},

    {0x09, IDS_GF_P2_START},  {0x0B, IDS_GF_P2_PICK},
    {0x1A, IDS_GF_P2_PICK_A}, {0x1B, IDS_GF_P2_PICK_B},
    {0x1D, IDS_GF_P2_EFFECT}, {0x0D, IDS_GF_P2_WAIL},
    {0x13, IDS_GF_P2_RED},    {0x15, IDS_GF_P2_GREEN},
    {0x17, IDS_GF_P2_BLUE}};

static const struct light_def gf_lights[] = {
    {0x08, IDS_GF_P1_SPOT_LIGHT},
    {0x09, IDS_GF_P2_SPOT_LIGHT},
    {0x0A, IDS_GF_P1_START},
    {0x0B, IDS_GF_P2_START}};

static const struct action_def iidx_actions[] = {
    {0x1C, IDS_GENERIC_TEST},     {0x1D, IDS_GENERIC_SERVICE},

    {0x08, IDS_IIDX_P1_1},        {0x09, IDS_IIDX_P1_2},
    {0x0A, IDS_IIDX_P1_3},        {0x0B, IDS_IIDX_P1_4},
    {0x0C, IDS_IIDX_P1_5},        {0x0D, IDS_IIDX_P1_6},
    {0x0E, IDS_IIDX_P1_7},

    {0x0F, IDS_IIDX_P2_1},        {0x10, IDS_IIDX_P2_2},
    {0x11, IDS_IIDX_P2_3},        {0x12, IDS_IIDX_P2_4},
    {0x13, IDS_IIDX_P2_5},        {0x14, IDS_IIDX_P2_6},
    {0x15, IDS_IIDX_P2_7},

    {0x18, IDS_IIDX_P1_START},    {0x19, IDS_IIDX_P2_START},
    {0x1A, IDS_IIDX_VEFX},        {0x1B, IDS_IIDX_EFFECT},

    {0x00, IDS_IIDX_P1_TT_UP},    {0x01, IDS_IIDX_P1_TT_DOWN},
    {0x02, IDS_IIDX_P1_TT_STAB},

    {0x03, IDS_IIDX_P2_TT_UP},    {0x04, IDS_IIDX_P2_TT_DOWN},
    {0x05, IDS_IIDX_P2_TT_STAB},

    {0x20, IDS_IIDX_PANEL_S1_UP}, {0x21, IDS_IIDX_PANEL_S1_DOWN},

    {0x22, IDS_IIDX_PANEL_S2_UP}, {0x23, IDS_IIDX_PANEL_S2_DOWN},

    {0x24, IDS_IIDX_PANEL_S3_UP}, {0x25, IDS_IIDX_PANEL_S3_DOWN},

    {0x26, IDS_IIDX_PANEL_S4_UP}, {0x27, IDS_IIDX_PANEL_S4_DOWN},

    {0x28, IDS_IIDX_PANEL_S5_UP}, {0x29, IDS_IIDX_PANEL_S5_DOWN}};

static const struct light_def iidx_lights[] = {
    {0x00, IDS_IIDX_P1_1},         {0x01, IDS_IIDX_P1_2},
    {0x02, IDS_IIDX_P1_3},         {0x03, IDS_IIDX_P1_4},
    {0x04, IDS_IIDX_P1_5},         {0x05, IDS_IIDX_P1_6},
    {0x06, IDS_IIDX_P1_7},

    {0x07, IDS_IIDX_P2_1},         {0x08, IDS_IIDX_P2_2},
    {0x09, IDS_IIDX_P2_3},         {0x0A, IDS_IIDX_P2_4},
    {0x0B, IDS_IIDX_P2_5},         {0x0C, IDS_IIDX_P2_6},
    {0x0D, IDS_IIDX_P2_7},

    {0x18, IDS_IIDX_P1_START},     {0x19, IDS_IIDX_P2_START},
    {0x1A, IDS_IIDX_VEFX},         {0x1B, IDS_IIDX_EFFECT},

    {0x10, IDS_IIDX_SPOT_1_LIGHT}, {0x11, IDS_IIDX_SPOT_2_LIGHT},
    {0x12, IDS_IIDX_SPOT_3_LIGHT}, {0x13, IDS_IIDX_SPOT_4_LIGHT},
    {0x14, IDS_IIDX_SPOT_5_LIGHT}, {0x15, IDS_IIDX_SPOT_6_LIGHT},
    {0x16, IDS_IIDX_SPOT_7_LIGHT}, {0x17, IDS_IIDX_SPOT_8_LIGHT},

    {0x1F, IDS_IIDX_NEON_LIGHT}};

static const struct analog_def iidx_analogs[] = {
    {0, IDS_IIDX_P1_TT}, {1, IDS_IIDX_P2_TT}};

static const struct action_def ddr_actions[] = {
    {0x04, IDS_GENERIC_TEST},      {0x06, IDS_GENERIC_SERVICE},

    {0x10, IDS_DDR_P1_START},      {0x00, IDS_DDR_P1_MENU_UP},
    {0x01, IDS_DDR_P1_MENU_DOWN},  {0x16, IDS_DDR_P1_MENU_LEFT},
    {0x17, IDS_DDR_P1_MENU_RIGHT}, {0x11, IDS_DDR_P1_UP},
    {0x12, IDS_DDR_P1_DOWN},       {0x13, IDS_DDR_P1_LEFT},
    {0x14, IDS_DDR_P1_RIGHT},

    {0x08, IDS_DDR_P2_START},      {0x02, IDS_DDR_P2_MENU_UP},
    {0x03, IDS_DDR_P2_MENU_DOWN},  {0x0E, IDS_DDR_P2_MENU_LEFT},
    {0x0F, IDS_DDR_P2_MENU_RIGHT}, {0x09, IDS_DDR_P2_UP},
    {0x0A, IDS_DDR_P2_DOWN},       {0x0B, IDS_DDR_P2_LEFT},
    {0x0C, IDS_DDR_P2_RIGHT}};

static const struct light_def ddr_lights[] = {
    /* These are split between non-overlapping P3IO and EXTIO state words */

    // P3IO: SD
    {0x00, IDS_DDR_P1_MENU_LIGHT},
    {0x07, IDS_DDR_P1_TOP_LIGHT},
    {0x06, IDS_DDR_P1_BOTTOM_LIGHT},

    {0x01, IDS_DDR_P2_MENU_LIGHT},
    {0x05, IDS_DDR_P2_TOP_LIGHT},
    {0x04, IDS_DDR_P2_BOTTOM_LIGHT},

    // P3IO: HD
    {0x02, IDS_DDR_HD_SPOT_RED_TOP},
    {0x03, IDS_DDR_HD_SPOT_BLUE_TOP},

    // P3IO: HDXS
    {0x08, IDS_DDR_HD_P1_START},
    {0x09, IDS_DDR_HD_P1_UP_DOWN},
    {0x0A, IDS_DDR_HD_P1_LEFT_RIGHT},

    {0x0B, IDS_DDR_HD_P2_START},
    {0x0C, IDS_DDR_HD_P2_UP_DOWN},
    {0x0D, IDS_DDR_HD_P2_LEFT_RIGHT},

    {0x20, IDS_DDR_HD_P1_SPEAKER_F_R},
    {0x21, IDS_DDR_HD_P1_SPEAKER_F_G},
    {0x22, IDS_DDR_HD_P1_SPEAKER_F_B},
    {0x23, IDS_DDR_HD_P2_SPEAKER_F_R},
    {0x24, IDS_DDR_HD_P2_SPEAKER_F_G},
    {0x25, IDS_DDR_HD_P2_SPEAKER_F_B},
    {0x26, IDS_DDR_HD_P1_SPEAKER_W_R},
    {0x27, IDS_DDR_HD_P1_SPEAKER_W_G},
    {0x28, IDS_DDR_HD_P1_SPEAKER_W_B},
    {0x29, IDS_DDR_HD_P2_SPEAKER_W_R},
    {0x2A, IDS_DDR_HD_P2_SPEAKER_W_G},
    {0x2B, IDS_DDR_HD_P2_SPEAKER_W_B},


    // EXTIO
    {0x1E, IDS_DDR_P1_UP},
    {0x1D, IDS_DDR_P1_DOWN},
    {0x1C, IDS_DDR_P1_LEFT},
    {0x1B, IDS_DDR_P1_RIGHT},

    {0x16, IDS_DDR_P2_UP},
    {0x15, IDS_DDR_P2_DOWN},
    {0x14, IDS_DDR_P2_LEFT},
    {0x13, IDS_DDR_P2_RIGHT},

    {0x0E, IDS_DDR_BASS_LIGHT}};

static const struct action_def pnm_actions[] = {
    {0x07, IDS_GENERIC_TEST},
    {0x06, IDS_GENERIC_SERVICE},

    {0x08, IDS_PNM_BTN1},
    {0x09, IDS_PNM_BTN2},
    {0x0A, IDS_PNM_BTN3},
    {0x0B, IDS_PNM_BTN4},
    {0x0C, IDS_PNM_BTN5},
    {0x0D, IDS_PNM_BTN6},
    {0x0E, IDS_PNM_BTN7},
    {0x0F, IDS_PNM_BTN8},
    {0x10, IDS_PNM_BTN9}};

static const struct light_def pnm_lights[] = {
    {0x17, IDS_PNM_BTN1},
    {0x18, IDS_PNM_BTN2},
    {0x19, IDS_PNM_BTN3},
    {0x1A, IDS_PNM_BTN4},
    {0x1B, IDS_PNM_BTN5},
    {0x1C, IDS_PNM_BTN6},
    {0x1D, IDS_PNM_BTN7},
    {0x1E, IDS_PNM_BTN8},
    {0x1F, IDS_PNM_BTN9},
    {0x20, IDS_PNM_TOP1},
    {0x21, IDS_PNM_TOP2},
    {0x22, IDS_PNM_TOP3},
    {0x23, IDS_PNM_TOP4},
    {0x24, IDS_PNM_TOP5},
    {0x25, IDS_PNM_LB},
    {0x26, IDS_PNM_LR},
    {0x27, IDS_PNM_RB},
    {0x28, IDS_PNM_RR},
};

static const struct action_def jb_actions[] = {
    {0x10, IDS_GENERIC_TEST},
    {0x11, IDS_GENERIC_SERVICE},

    {0x00, IDS_JB_PANEL1},
    {0x01, IDS_JB_PANEL2},
    {0x02, IDS_JB_PANEL3},
    {0x03, IDS_JB_PANEL4},
    {0x04, IDS_JB_PANEL5},
    {0x05, IDS_JB_PANEL6},
    {0x06, IDS_JB_PANEL7},
    {0x07, IDS_JB_PANEL8},
    {0x08, IDS_JB_PANEL9},
    {0x09, IDS_JB_PANEL10},
    {0x0A, IDS_JB_PANEL11},
    {0x0B, IDS_JB_PANEL12},
    {0x0C, IDS_JB_PANEL13},
    {0x0D, IDS_JB_PANEL14},
    {0x0E, IDS_JB_PANEL15},
    {0x0F, IDS_JB_PANEL16}};

static const struct light_def jb_lights[] = {
    {0x00, IDS_JB_RGB_FRONT_R},
    {0x01, IDS_JB_RGB_FRONT_G},
    {0x02, IDS_JB_RGB_FRONT_B},
    {0x03, IDS_JB_RGB_TOP_R},
    {0x04, IDS_JB_RGB_TOP_G},
    {0x05, IDS_JB_RGB_TOP_B},
    {0x06, IDS_JB_RGB_LEFT_R},
    {0x07, IDS_JB_RGB_LEFT_G},
    {0x08, IDS_JB_RGB_LEFT_B},
    {0x09, IDS_JB_RGB_RIGHT_R},
    {0x0A, IDS_JB_RGB_RIGHT_G},
    {0x0B, IDS_JB_RGB_RIGHT_B},
    {0x0C, IDS_JB_RGB_TITLE_R},
    {0x0D, IDS_JB_RGB_TITLE_G},
    {0x0E, IDS_JB_RGB_TITLE_B},
    {0x0F, IDS_JB_RGB_WOOFER_R},
    {0x10, IDS_JB_RGB_WOOFER_G},
    {0x11, IDS_JB_RGB_WOOFER_B},
};

static const struct action_def sdvx_actions[] = {
    {0x05, IDS_GENERIC_TEST},
    {0x04, IDS_GENERIC_SERVICE},
    {0x0B, IDS_SDVX_START},
    {0x0A, IDS_SDVX_BTN_A},
    {0x09, IDS_SDVX_BTN_B},
    {0x08, IDS_SDVX_BTN_C},
    {0x15, IDS_SDVX_BTN_D},
    {0x14, IDS_SDVX_FX_L},
    {0x13, IDS_SDVX_FX_R}};

static const struct light_def sdvx_lights[] = {
    {0x0D, IDS_SDVX_BTN_A},  {0x0E, IDS_SDVX_BTN_B},  {0x0F, IDS_SDVX_BTN_C},
    {0x00, IDS_SDVX_BTN_D},  {0x01, IDS_SDVX_FX_L},   {0x02, IDS_SDVX_FX_R},
    {0x0C, IDS_SDVX_START},  {0x10, IDS_SDVX_RGB1_R}, {0x11, IDS_SDVX_RGB1_G},
    {0x12, IDS_SDVX_RGB1_B}, {0x13, IDS_SDVX_RGB2_R}, {0x14, IDS_SDVX_RGB2_G},
    {0x15, IDS_SDVX_RGB2_B}, {0x16, IDS_SDVX_RGB3_R}, {0x17, IDS_SDVX_RGB3_G},
    {0x18, IDS_SDVX_RGB3_B}, {0x19, IDS_SDVX_RGB4_R}, {0x1A, IDS_SDVX_RGB4_G},
    {0x1B, IDS_SDVX_RGB4_B}, {0x1C, IDS_SDVX_RGB5_R}, {0x1D, IDS_SDVX_RGB5_G},
    {0x1E, IDS_SDVX_RGB5_B}, {0x1F, IDS_SDVX_RGB6_R}, {0x20, IDS_SDVX_RGB6_G},
    {0x21, IDS_SDVX_RGB6_B}};

static const struct analog_def sdvx_analogs[] = {
    {0, IDS_SDVX_VOL_L}, {1, IDS_SDVX_VOL_R}};

static const struct action_def rb_actions[] = {
    {0x00, IDS_GENERIC_TEST}, {0x01, IDS_GENERIC_SERVICE}};

static const struct action_def bst_actions[] = {
    {0x05, IDS_GENERIC_TEST},
    {0x04, IDS_GENERIC_SERVICE},
};

static const struct action_def gd_actions[] = {
    {0x31, IDS_GENERIC_TEST},
    {0x32, IDS_GENERIC_SERVICE},
    
    {0x00, IDS_GD_BTN_P1_R},
    {0x01, IDS_GD_BTN_P1_G},
    {0x02, IDS_GD_BTN_P1_B},
    {0x03, IDS_GD_BTN_P1_Y},
    {0x04, IDS_GD_BTN_P1_P},
    {0x05, IDS_GD_BTN_P1_PICK_UP},
    {0x06, IDS_GD_BTN_P1_PICK_DOWN},
    {0x07, IDS_GD_BTN_P1_WAILING_UP},
    {0x08, IDS_GD_BTN_P1_WAILING_DOWN},
    {0x09, IDS_GD_BTN_P1_WAILING_SIDE},
    
    {0x0A, IDS_GD_BTN_P2_R},
    {0x0B, IDS_GD_BTN_P2_G},
    {0x0C, IDS_GD_BTN_P2_B},
    {0x0D, IDS_GD_BTN_P2_Y},
    {0x0E, IDS_GD_BTN_P2_P},
    {0x0F, IDS_GD_BTN_P2_PICK_UP},
    {0x10, IDS_GD_BTN_P2_PICK_DOWN},
    {0x11, IDS_GD_BTN_P2_WAILING_UP},
    {0x12, IDS_GD_BTN_P2_WAILING_DOWN},
    {0x13, IDS_GD_BTN_P2_WAILING_SIDE},

    {0x28, IDS_GD_PAD_LC},
    {0x29, IDS_GD_PAD_HH},
    {0x2A, IDS_GD_PAD_HT},
    {0x2B, IDS_GD_PAD_SN},
    {0x2C, IDS_GD_PAD_LT},
    {0x2D, IDS_GD_PAD_FT},
    {0x2E, IDS_GD_PAD_RC},
    {0x2F, IDS_GD_PEDAL_LP},
    {0x30, IDS_GD_PEDAL_BP},

    {0x14, IDS_GD_BTN_P1_START},
    {0x19, IDS_GD_BTN_P1_HELP},
    {0x15, IDS_GD_BTN_P1_UP},
    {0x16, IDS_GD_BTN_P1_DOWN},
    {0x17, IDS_GD_BTN_P1_LEFT},
    {0x18, IDS_GD_BTN_P1_RIGHT},
    {0x1A, IDS_GD_BTN_P1_EFF1},
    {0x1B, IDS_GD_BTN_P1_EFF2},
    {0x1C, IDS_GD_BTN_P1_EFF3},
    {0x1D, IDS_GD_BTN_P1_PEDAL},
    
    {0x1E, IDS_GD_BTN_P2_START},
    {0x23, IDS_GD_BTN_P2_HELP},
    {0x1F, IDS_GD_BTN_P2_UP},
    {0x20, IDS_GD_BTN_P2_DOWN},
    {0x21, IDS_GD_BTN_P2_LEFT},
    {0x22, IDS_GD_BTN_P2_RIGHT},
    {0x24, IDS_GD_BTN_P2_EFF1},
    {0x25, IDS_GD_BTN_P2_EFF2},
    {0x26, IDS_GD_BTN_P2_EFF3},
    {0x27, IDS_GD_BTN_P2_PEDAL},

    };

static const struct light_def gd_lights[] = {
    {0x3A, IDS_GD_BTN_P1_START},  {0x3D, IDS_GD_BTN_P1_HELP},  {0x3B, IDS_GD_BTN_P1_UPDOWN},
    {0x3C, IDS_GD_BTN_P1_LEFTRIGHT},  {0x3E, IDS_GD_BTN_P2_START},  {0x41, IDS_GD_BTN_P2_HELP},
    {0x3F, IDS_GD_BTN_P2_UPDOWN},  {0x40, IDS_GD_BTN_P2_LEFTRIGHT},  {0x42, IDS_GD_PAD_LC},
    {0x43, IDS_GD_PAD_HH},  {0x44, IDS_GD_PAD_HT},  {0x45, IDS_GD_PAD_SN},
    {0x46, IDS_GD_PAD_LT},  {0x47, IDS_GD_PAD_FT},  {0x48, IDS_GD_PAD_RC},
    {0x0, IDS_GD_RGB1_R},  {0x1, IDS_GD_RGB1_G},  {0x2, IDS_GD_RGB1_B},
    {0x3, IDS_GD_RGB2_R},  {0x4, IDS_GD_RGB2_G},  {0x5, IDS_GD_RGB2_B},
    {0x6, IDS_GD_RGB3_R},  {0x7, IDS_GD_RGB3_G},  {0x8, IDS_GD_RGB3_B},
    {0x9, IDS_GD_RGB4_R},  {0xA, IDS_GD_RGB4_G},  {0xB, IDS_GD_RGB4_B},
    {0xC, IDS_GD_RGB5_R},  {0xD, IDS_GD_RGB5_G},  {0xE, IDS_GD_RGB5_B},
    {0xF, IDS_GD_RGB6_R},  {0x10, IDS_GD_RGB6_G},  {0x11, IDS_GD_RGB6_B},
    {0x12, IDS_GD_RGB7_R},  {0x13, IDS_GD_RGB7_G},  {0x14, IDS_GD_RGB7_B},
    {0x15, IDS_GD_RGB8_R},  {0x16, IDS_GD_RGB8_G},  {0x17, IDS_GD_RGB8_B},
    {0x18, IDS_GD_RGB9_R},  {0x19, IDS_GD_RGB9_G},  {0x1A, IDS_GD_RGB9_B},
    {0x1B, IDS_GD_RGB10_R},  {0x1C, IDS_GD_RGB10_G},  {0x1D, IDS_GD_RGB10_B},
    {0x1E, IDS_GD_RGB11_R},  {0x1F, IDS_GD_RGB11_G},  {0x20, IDS_GD_RGB11_B},
    {0x21, IDS_GD_RGB12_R},  {0x22, IDS_GD_RGB12_G},  {0x23, IDS_GD_RGB12_B},
    {0x24, IDS_GD_RGB13_R},  {0x25, IDS_GD_RGB13_G},  {0x26, IDS_GD_RGB13_B},
    {0x27, IDS_GD_RGB14_R},  {0x28, IDS_GD_RGB14_G},  {0x29, IDS_GD_RGB14_B},
    {0x2A, IDS_GD_RGB15_R},  {0x2B, IDS_GD_RGB15_G},  {0x2C, IDS_GD_RGB15_B},
    {0x2D, IDS_GD_RGB16_R},  {0x2E, IDS_GD_RGB16_G},  {0x2F, IDS_GD_RGB16_B},
    {0x30, IDS_GD_LED1},  {0x31, IDS_GD_LED2},  {0x32, IDS_GD_LED3},
    {0x33, IDS_GD_LED4},  {0x34, IDS_GD_LED5},  {0x35, IDS_GD_LED6},
    {0x36, IDS_GD_LED7},  {0x37, IDS_GD_LED8},  {0x38, IDS_GD_LED9},
    {0x39, IDS_GD_LED10}};

static const struct eam_unit_def schema_eam_unit_defs[] = {
    {IDS_READER_P1, 0}, {IDS_READER_P2, 1}};

const struct schema schemas[] = {
    {"iidx",
     IDS_IIDX_SCHEMA,
     iidx_actions,
     lengthof(iidx_actions),
     iidx_lights,
     lengthof(iidx_lights),
     iidx_analogs,
     lengthof(iidx_analogs),
     schema_eam_unit_defs,
     2},

    {"pnm",
     IDS_PNM_SCHEMA,
     pnm_actions,
     lengthof(pnm_actions),
     pnm_lights,
     lengthof(pnm_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     1},

    {"gf",
     IDS_GF_SCHEMA,
     gf_actions,
     lengthof(gf_actions),
     gf_lights,
     lengthof(gf_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     2},

    {"dm",
     IDS_DM_SCHEMA,
     dm_actions,
     lengthof(dm_actions),
     dm_lights,
     lengthof(dm_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     1},

    {"ddr",
     IDS_DDR_SCHEMA,
     ddr_actions,
     lengthof(ddr_actions),
     ddr_lights,
     lengthof(ddr_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     2},

    {"jb",
     IDS_JB_SCHEMA,
     jb_actions,
     lengthof(jb_actions),
     jb_lights,
     lengthof(jb_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     1},

    {"sdvx",
     IDS_SDVX_SCHEMA,
     sdvx_actions,
     lengthof(sdvx_actions),
     sdvx_lights,
     lengthof(sdvx_lights),
     sdvx_analogs,
     lengthof(sdvx_analogs),
     schema_eam_unit_defs,
     1},

    {"rb",
     IDS_RB_SCHEMA,
     rb_actions,
     lengthof(rb_actions),
     NULL,
     0,
     NULL,
     0,
     schema_eam_unit_defs,
     1},

    {"bst",
     IDS_BST_SCHEMA,
     bst_actions,
     lengthof(bst_actions),
     NULL,
     0,
     NULL,
     0,
     schema_eam_unit_defs,
     1},

    {"gd",
     IDS_GD_SCHEMA,
     gd_actions,
     lengthof(gd_actions),
     gd_lights,
     lengthof(gd_lights),
     NULL,
     0,
     schema_eam_unit_defs,
     2},
};

const size_t nschemas = lengthof(schemas);
