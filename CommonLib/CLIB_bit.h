/**************************************************************
**  Mbed Bit Library with C
**************************************************************/
/** 
 * @file        CLIB_bit.h
 * @brief       Bit Control API with C Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/07/02 : zhaozhenge@outlook.com
 *                  -# New
 */

#ifndef _CLIB_BIT_H_
#define _CLIB_BIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Symbol
**************************************************************/

/**
 * @addtogroup  bit_control_module
 * @ingroup     mbed_c_library
 * @brief       Bit Control API
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/07/20 : zhaozhenge@outlook.com
 *                  -# New
 * @{ 
 */

/** 
 *  @brief          Set One bit of an Integer Data
 *  @param[in,out]  a   Integer Data
 *  @param[in]      b   Which bit want to set (Count from right, 0~7)
 *  @return         None
 */
#define CLIB_BIT_SET(a,b) ((a) |= (1<<(b)))

/** 
 *  @brief          Clear One bit of an Integer Data
 *  @param[in,out]  a   Integer Data
 *  @param[in]      b   Which bit want to clear (Count from right, 0~7)
 *  @return         None
 */
#define CLIB_BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))

/**
 *  @brief          Flip One bit of an Integer Data
 *  @param[in,out]  a   Integer Data
 *  @param[in]      b   Which bit want to flip (Count from right, 0~7)
 *  @return         None
 */
#define CLIB_BIT_FLIP(a,b) ((a) ^= (1<<(b)))

/** 
 *  @brief          Check One bit of an Integer Data
 *  @param[in,out]  a   Integer Data
 *  @param[in]      b   Which bit want to check (Count from right, 0~7)
 *  @return         None
 */
#define CLIB_BIT_CHECK(a,b) ((a) & (1<<(b)))

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _CLIB_BIT_H_ */
