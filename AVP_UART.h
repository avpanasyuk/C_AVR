/*
* SoftUART.h
*
* Created: 7/29/2013 2:37:48 PM
*  Author: panasyuk
*/


#ifndef HARDUART_H_
#define HARDUART_H_

#include <stdint.h>
#include <string.h>

// the AVP_UART has two ways to transfer - buffered and unbuffered. We try to fill the buffer first. If message does not fit into buffer
// we STORE a pointer to it and hope it will be valid until sent. After this we can not write any more unless we free buffer first and
// then pointer.

#ifndef PRR0
#define PRR0 PRR
#endif

namespace AVP_UART0 {
	#include "AVP_UART_insert.h"
}

#ifdef PRUSART1
namespace AVP_UART1 {
	#include "AVP_UART_insert.h"
}
#endif

#endif /* HARDUART_H_ */