

#include <Arduino.h> 



long add( int a, int b) { 
    return a + b;
}

long sub( int a, int b) { 
    return a - b;
}



void setup() {
    long r; 
    Serial.begin(9600); 
    delay(200);

    r = 




}


long execFunction(long (*func)(int, int), int x) {
    return 
} 
