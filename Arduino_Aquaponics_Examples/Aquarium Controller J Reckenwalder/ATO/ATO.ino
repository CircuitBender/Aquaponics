
void ato (){
    t.oscillate(ATO, ato_ontime * 1000, LOW, 1); //pin. time , starting/ending state, # times to run  
        ato_hour = time;            //only allow the ATO to run once per hour.
        pump_count++; 
        ato_last = mil_time;    //Prints to the LCD the time the ATO ran last 
}
