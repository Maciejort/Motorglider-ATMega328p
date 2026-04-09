#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t rc_channels[14];
volatile uint8_t frame_ready = 0;

// Konfiguracja sprzętowego portu szeregowego do odbioru i-BUS
void uart_init() {
    // Prędkość 115200 baud dla zegara 16 MHz
    UCSR0A = (1 << U2X0);
    UBRR0H = 0;
    UBRR0L = 16;
    // Włączenie odbiornika i przerwań od odebranego bajtu
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
}

// Przerwanie wywoływane automatycznie za każdym razem, gdy na pinie RX pojawi się bajt
ISR(USART_RX_vect) {
    static uint8_t buffer[32];
    static uint8_t index = 0;
    uint8_t data = UDR0;

    // Sprawdzanie nagłówka ramki i-BUS (0x20, 0x40)
    if (index == 0 && data != 0x20) return;
    if (index == 1 && data != 0x40) { index = 0; return; }

    buffer[index++] = data;

    // Gdy bufor się zapełni (32 bajty)
    if (index == 32) {
        index = 0;
        uint16_t checksum = 0xFFFF;
        
        // Obliczanie sumy kontrolnej
        int i;
        for (i = 0; i < 30; i++) 
        {
            checksum -= buffer[i];
        }
        
        // Odczyt sumy kontrolnej z dwóch ostatnich bajtów
        uint16_t rx_checksum = buffer[30] | (buffer[31] << 8);

        // Jeśli dane nie uległy przekłamaniu, przepisz je do zmiennych
        if (checksum == rx_checksum) {
            int i;
            for (i = 0; i < 14; i++) {
                rc_channels[i] = buffer[2 + i*2] | (buffer[3 + i*2] << 8);
            }
            frame_ready = 1;
        }
    }
}

// Konfiguracja Timera 1 do generowania sygnału PWM dla serw (50 Hz)
void timer1_init() {
    // Piny 9 (PB1) i 10 (PB2) jako wyjścia sprzętowe Timera 1
    DDRB |= (1 << PB1) | (1 << PB2);

    // Tryb Fast PWM, TOP = ICR1
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    
    // Preskaler 8 -> 1 takt = 0.5 mikrosekundy
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // Całkowita długość ramki PWM: 20ms = 40000 taktów
    ICR1 = 39999; 

    // Pozycja domyślna serw po starcie (1500 us -> 3000 taktów)
    OCR1A = 3000; 
    OCR1B = 3000;
}

int main(void) {
    uart_init();
    timer1_init();
    
    // Globalne włączenie przerwań
    sei(); 

    while(1) {
        if (frame_ready) {
            // 1. WYŁĄCZ przerwania globalne (nic nie przerwie tej operacji)
            cli();
            
            // 2. Skopiuj 16-bitowe wartości do lokalnych zmiennych "atomowo"
            uint16_t ch1 = rc_channels[0];
            uint16_t ch2 = rc_channels[1];
            frame_ready = 0; // Zresetuj flagę
            
            // 3. WŁĄCZ przerwania globalne z powrotem (odblokuj nasłuch UART)
            sei();

            // 4. Bezpiecznie ustaw sprzętowy Timer
            OCR1A = ch1 * 2; 
            OCR1B = ch2 * 2; 
        }
    }
}