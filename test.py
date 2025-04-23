from pymodbus.client import ModbusSerialClient
import time

# Configure the Modbus RTU client
client = ModbusSerialClient(
    port="COM9",       # Change to your port (e.g., "COM3" on Windows)
    baudrate=115200,
    timeout=1
)

if client.connect():
    print("Connected to Modbus Slave!")
    
   
    time.sleep(0.5)  # Allow time for NV load to complete
while(True):
    # Step 1: Read holding registers (for example, 8 registers starting at address 0)
    response = client.read_input_registers(address=0, count=10, slave=1)
    if response.isError():
        print("Error reading registers:", response)
    else:
        print("Read registers:", response.registers)

    # Close the connection after reading
    client.close()
else:
    print("Failed to connect to Modbus Slave!")
