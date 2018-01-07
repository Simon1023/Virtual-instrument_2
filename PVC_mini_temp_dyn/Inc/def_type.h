#ifndef def_type_h
#define def_type_h
/*
#define sd_uchar  section ("sdram_data") unsigned char
#define sd_ushort section ("sdram_data") unsigned short
#define sd_uint   section ("sdram_data") unsigned int
#define sd_char   section ("sdram_data") char
#define sd_short  section ("sdram_data") short
#define sd_int    section ("sdram_data") int
#define sd_Mpt    section ("sdram_data") struct Mpt

#define fd_uchar  section ("flash_data") unsigned char
#define fd_ushort section ("flash_data") unsigned short 
#define fd_uint   section ("flash_data") unsigned int
#define fd_char   section ("flash_data") char
#define fd_short  section ("flash_data") short
#define fd_int    section ("flash_data") int
#define fd_Mpt    section ("flash_data") struct Mpt

#define ft_uchar  section ("feature_data") unsigned char
#define ft_ushort section ("feature_data") unsigned short 
#define ft_uint   section ("feature_data") unsigned int
#define ft_char   section ("feature_data") char           
#define ft_short  section ("feature_data") short
#define ft_int    section ("feature_data") int
#define ft_Mpt    section ("feature_data") struct Mpt

*/
#define sd_uchar  __attribute__ ((section ("SDRAM"))) unsigned char
#define sd_ushort __attribute__ ((section ("SDRAM"))) unsigned short
#define sd_uint   __attribute__ ((section ("SDRAM"))) unsigned int
#define sd_char   __attribute__ ((section ("SDRAM"))) signed char
#define sd_short  __attribute__ ((section ("SDRAM"))) signed short
#define sd_int    __attribute__ ((section ("SDRAM"))) signed int
#define sd_Mpt    __attribute__ ((section ("SDRAM"))) struct Mpt

//#define fd_uchar  __attribute__ ((section ("SDRAM"))) unsigned char
//#define fd_ushort __attribute__ ((section ("SDRAM"))) unsigned short
//#define fd_uint   __attribute__ ((section ("SDRAM"))) unsigned int
//#define fd_char   __attribute__ ((section ("SDRAM"))) signed char
//#define fd_short  __attribute__ ((section ("SDRAM"))) signed short
//#define fd_int    __attribute__ ((section ("SDRAM"))) signed int
//#define fd_Mpt    __attribute__ ((section ("SDRAM"))) struct Mpt

//#define ft_uchar  __attribute__ ((section ("SDRAM"))) unsigned char
//#define ft_ushort __attribute__ ((section ("SDRAM"))) unsigned short
//#define ft_uint   __attribute__ ((section ("SDRAM"))) unsigned int
//#define ft_char   __attribute__ ((section ("SDRAM"))) signed char
//#define ft_short  __attribute__ ((section ("SDRAM"))) signed short
//#define ft_int    __attribute__ ((section ("SDRAM"))) signed int
//#define ft_Mpt    __attribute__ ((section ("SDRAM"))) struct Mpt



#endif
