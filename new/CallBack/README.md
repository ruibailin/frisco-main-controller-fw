CallBack is a very bad design for a complicated system:
1. Make state machine to be very hard understood
2. Callback functions directly change variables maintained by other tasks without notification,
   And other tasks don't know its variables changed until they periedically check the variables
   it vialates modulization design principle and cause extra burden
3. Callback makes stack call very deep
4. There is risk to lead to dead loop.
5. Many callbacks make the relationship between receivers and senders become complicated
6. Callback actrually do OS's business

The first step of Optinization of CallBack
1. Use message machanism of OS
2. CallBack is splitted into two parts: receiver and sender
3. Receiver puts received message into event queue of OS
4. OS delivers messages to sender
5. Sender continues the rest work of CallBack
    
The second step of Optinization of CallBack
1. Merge Receiver into OS's message system
2. Merge Sender into OS's scheduler system

For I2C:
uint8_t Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet) in Module_Bus.c
change to:
1. First part of Process_Message call eos_async_send() and return
2. scheduler
3. Tasks continue to call Rest part of Process_Message	

For I2C Port:
Module_Bus_Master.RxData_Callback=Module_Bus_Data_Callback:
1. Module_Bus_Master.RxData_Callback = Module_Bus_Data_Rx;
2. Module_Bus_Data_Rx call eos_async_send and return;
3. scheduler
4. Module Bus Task continue to call Module_Bus_Data_Callback;
