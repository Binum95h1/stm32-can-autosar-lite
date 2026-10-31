#include "BuzzerIf.h"
#include "Dio.h"
Std_ReturnType BuzzerIf_Set(boolean on){
    Dio_WriteChannel(DIO_CHANNEL_BUZZER,(on!=FALSE)?STD_HIGH:STD_LOW);
    return E_OK;
}
