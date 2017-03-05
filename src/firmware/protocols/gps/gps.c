void GPS_UpdateData(void) {
    c = _uart_driver_GetByte();
    if (c != -1 ){
       NMEA_Read(c); 
    }    
}
GPS_DATA GPS_GetData
