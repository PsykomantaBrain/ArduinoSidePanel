/* 
	Editor: https://www.visualmicro.com/
			This file is for intellisense purpose only.
			Visual micro (and the arduino ide) ignore this code during compilation. This code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			The contents of the _vm sub folder can be deleted prior to publishing a project
			All non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			Note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: HRV Yoke (native USB port)(arduino_due_x_hrv_yoke), Platform=sam, Package=arduino
*/

#if defined(_VMICRO_INTELLISENSE)

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define _VMDEBUG 1
#define F_CPU 84000000L
#define ARDUINO 108010
#define ARDUINO_SAM_DUE 1
#define ARDUINO_ARCH_SAM 1
#define __SAM3X8E__ 1
#define USB_VID 0x1209
#define USB_PID 0x7700
#define USBCON 1
#define USB_MANUFACTURER "\"HRV
#define USB_PRODUCT "\"HRV
#define __cplusplus 201103L
#pragma diag_suppress=338,2925
// User Defined Errors to Ignore below... [vm.intellisense.ignore-error-codes]
#ifdef __CLEARCORE__
#define __ARMCC_VERSION 6010050
#define HIDE_FROM_DOXYGEN
#endif

#define _Pragma(x)
#define __ARM__
#define __arm__
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __volatile__
#define _HAVE_STDC
#define __ASM
#define __INLINE
#define __builtin_va_list void

#define __attribute__(noinline)

typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

#define __INTPTR_TYPE__ long
#define __INT32_TYPE__ long
#define __UINTPTR_TYPE__ long
#define __UINT32_TYPE__ long
#define _Pragma(x) pragma


#if (defined(ARDUINO_SAM_DUE))
	#include "sam3xa.h"
#endif

#include <arduino.h>
#include <pins_arduino.h> 
#include <variant.h> 
#include <variant.cpp> 
# define cli()  __asm__ __volatile__ ("cli" ::: "memory")
# define sei()  __asm__ __volatile__ ("sei" ::: "memory")

/* GNU gcc specific functions */
/** \brief  Enable IRQ Interrupts

This function enables IRQ interrupts by clearing the I-bit in the CPSR.
Can only be executed in Privileged modes.
*/
__attribute__((always_inline)) static __INLINE void __enable_irq(void)
{
	//__ASM volatile ("cpsie i");
}


/** \brief  Disable IRQ Interrupts

This function disables IRQ interrupts by setting the I-bit in the CPSR.
Can only be executed in Privileged modes.
*/
__attribute__((always_inline)) static __INLINE void __disable_irq(void)
{
	//__ASM volatile ("cpsid i");
}

#ifndef __CLEARCORE__
/** \brief  Get Control Register

This function returns the content of the Control Register.

\return               Control Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_CONTROL(void)
{
	uint32_t result;

	//__ASM volatile ("MRS %0, control" : "=r" (result));
	return(result);
}


/** \brief  Set Control Register

This function writes the given value to the Control Register.

\param [in]    control  Control Register value to set
*/
__attribute__((always_inline)) static __INLINE void __set_CONTROL(uint32_t control)
{
	//__ASM volatile ("MSR control, %0" : : "r" (control));
}


/** \brief  Get ISPR Register

This function returns the content of the ISPR Register.

\return               ISPR Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_IPSR(void)
{
	uint32_t result;

	//__ASM volatile ("MRS %0, ipsr" : "=r" (result));
	return(result);
}


/** \brief  Get APSR Register

This function returns the content of the APSR Register.

\return               APSR Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_APSR(void)
{
	uint32_t result;

	//__ASM volatile ("MRS %0, apsr" : "=r" (result));
	return(result);
}


/** \brief  Get xPSR Register

This function returns the content of the xPSR Register.

\return               xPSR Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_xPSR(void)
{
	uint32_t result;

	//__ASM volatile ("MRS %0, xpsr" : "=r" (result));
	return(result);
}


/** \brief  Get Process Stack Pointer

This function returns the current value of the Process Stack Pointer (PSP).

\return               PSP Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_PSP(void)
{
	register uint32_t result;

	//__ASM volatile ("MRS %0, psp\n"  : "=r" (result));
	return(result);
}


/** \brief  Set Process Stack Pointer

This function assigns the given value to the Process Stack Pointer (PSP).

\param [in]    topOfProcStack  Process Stack Pointer value to set
*/
__attribute__((always_inline)) static __INLINE void __set_PSP(uint32_t topOfProcStack)
{
	//__ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack));
}


/** \brief  Get Main Stack Pointer

This function returns the current value of the Main Stack Pointer (MSP).

\return               MSP Register value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_MSP(void)
{
	register uint32_t result;

	//__ASM volatile ("MRS %0, msp\n" : "=r" (result));
	return(result);
}


/** \brief  Set Main Stack Pointer

This function assigns the given value to the Main Stack Pointer (MSP).

\param [in]    topOfMainStack  Main Stack Pointer value to set
*/
__attribute__((always_inline)) static __INLINE void __set_MSP(uint32_t topOfMainStack)
{
	//__ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack));
}


/** \brief  Get Priority Mask

This function returns the current state of the priority mask bit from the Priority Mask Register.

\return               Priority Mask value
*/
__attribute__((always_inline)) static __INLINE uint32_t __get_PRIMASK(void)
{
	uint32_t result;

	//__ASM volatile ("MRS %0, primask" : "=r" (result));
	return(result);
}


/** \brief  Set Priority Mask

This function assigns the given value to the Priority Mask Register.

\param [in]    priMask  Priority Mask
*/
__attribute__((always_inline)) static __INLINE void __set_PRIMASK(uint32_t priMask)
{
	//__ASM volatile ("MSR primask, %0" : : "r" (priMask));
}
#else
// Additions for Clear Core to reduce errors

class ShiftRegister { public: enum Masks {};   Masks m_ledMask; };
class PeripheralRoute {};
class BlinkCodeDriver {
public:
	friend class StatusManager;
	typedef enum {
	} BlinkCodeGroups;
};
typedef enum {
} DmaChannels;
class Iir16 {};
#endif

#include "..\Yoke.ino"
#endif
#endif
