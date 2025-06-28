/*
 * HMI_Modbus.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Mario Atef
 */

#include "USB_Modbus.h"




MB_Slave_t USB_MB;
USBD_CDC_HandleTypeDef *UsbCdc;
/*
// Callback invoked when data is received over USB CDC
void USB_RxEventCallback(uint8_t *data, uint32_t length) {
	CDC_Receive_FS(data, length) == HAL_OK
			? MB_STAT_OK
					: MB_TX_ERR_HW;
}
// Error handling (optional)



// Callback invoked when USB CDC transmission is complete
void USB_TxCpltCallback(uint8_t *data, uint32_t length) {
	CDC_TransmitCplt_FS(data, length) == HAL_OK
			? MB_STAT_OK
					: MB_TX_ERR_HW;
}

// Error callback: restart Modbus listening
void USB_ErrorCallback(void) {
	USB_MB.hw_interface.MB_StartListening();
}
*/
// Transmit data over USB (Modbus master -> slave)
MB_Status_t USB_MB_Transmit_Data(uint8_t *data, uint16_t length) {
	return (CDC_Transmit_FS(data, length) == HAL_OK)
			? MB_STAT_OK
					: MB_TX_ERR_HW;
}

// Activate or deactivate TX (RS485 control); unused for USB
MB_Status_t USB_MB_Activate_TX(uint8_t tx_on) {

	return MB_STAT_OK;
}

// Start listening for incoming Modbus frames
MB_Status_t USB_MB_StartListening(void) {
	return MB_STAT_OK;
}

// Handle received Modbus request
MB_Status_t USB_MB_Request_Received(void *ptr) {
	MB_Slave_t *mb = (MB_Slave_t*) ptr;
	// User implementation here
	//(void)mb;
	return MB_STAT_OK;
}

// Initialize Modbus slave over USB CDC
MB_Status_t MB_Init_USB_MODBUS(USBD_CDC_HandleTypeDef *usb_handle, uint8_t slave_address) {
	// Store USB handle globally
	UsbCdc = usb_handle;

	// Configure buffer sizes
	USB_MB.TX_RX_Buffer_Size = 256;
	USB_MB.HoldingRegSize   = HoldingRegsSize;
	USB_MB.InputRegSize     = InputRegsSize;
	USB_MB.InputBitsSize    = 1;
	USB_MB.CoilBitsSize     = 1;

	// Assign hardware interface callbacks
	USB_MB.hw_interface.MB_Activate_TX     = USB_MB_Activate_TX;
	USB_MB.hw_interface.MB_Transmit       = USB_MB_Transmit_Data;
	USB_MB.hw_interface.MB_StartListening = USB_MB_StartListening;
	USB_MB.hw_interface.MB_Request_Recieved = USB_MB_Request_Received;

	// Set slave address and timing
	USB_MB.SLA              = slave_address;
	USB_MB.TX_Automplete    = 0;
	USB_MB.RX_Silent_Interval_MS = 2;
	USB_MB.RX_Timeout       = 100;

	// Begin in listen mode
//	USB_MB.hw_interface.MB_Activate_TX(0);

	// Initialize Modbus slave with our MB_Slave_t instance
	MB_Status_t status = MB_Slave_Init(&USB_MB);

	// Map holding and input registers for easy access
	//Hregs = (HoldingRegs_t *)USB_MB.HoldingRegs;
	//Iregs = (InputRegs_t *)USB_MB.InputRegs;

	return status;
}
