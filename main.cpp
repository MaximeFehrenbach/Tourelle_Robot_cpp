#include <iostream>
#include <fcntl.h>      // open()
#include <sys/ioctl.h>  // ioctl()
#include <linux/i2c-dev.h> // I2C
#include <unistd.h>     // close()
#include <thread> 
#include <chrono>

using namespace std;

const int TICK_MIN = 115; // 0 degre (0.5 ms)
const int TICK_MAX = 510; // 180 degres (2.5 ms)

// ecrire une valeur dans un registre
void write_byte(int file, int register_address, int data) {
    unsigned char buffer[2];
    buffer[0] = register_address;
    buffer[1] = data;
     
    
    write(file, buffer, 2);
}

void init_50Hz(int file){
    // mise en veille
    write_byte(file, 0x00, 0x10); 
    // fréquence à 50 Hz
    write_byte(file, 0xFE, 121); // 121 = 25MHz/(2^12*50Hz)-1
    // réveil du PCA
    write_byte(file, 0x00, 0x20);
    // on laisse l'horloge du PCA se stabiliser
    this_thread::sleep_for(chrono::milliseconds(5));
    cout << "config à 50 Hz" << endl;
}

// faire tourner un moteur
void set_servo_ticks(int file, int canal, int off_value) {
    int servo_address = 0x06 + (4 * canal); // adresse de départ du canal
    write_byte(file, servo_address, 0); // ON_L
    write_byte(file, servo_address + 1, 0); // ON_H
    write_byte(file, servo_address + 2, off_value & 0xFF); // OFF_L
    write_byte(file, servo_address + 3, off_value >> 8); // OFF_H
}

void set_servo_angle(int file, int canal, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    int ticks = TICK_MIN + (angle * (TICK_MAX - TICK_MIN) / 180); // [0 ; 180] -> [TICK_MAX ; TICK_MIN]
    set_servo_ticks(file, canal, ticks);
}

int main() {
    cout << "test séquence de mouvements" << endl;

    // ouverture port I2C
    int file = open("/dev/i2c-1", O_RDWR); // O_RDWR pour les droits de lecture et d'ecriture
    
    if (file < 0) {
        cerr << "erreur I2C" << endl;
        return 1;
    }

    // on cible précisément la PCA en hexa
    int adresse_PCA = 0x40;
    
    if (ioctl(file, I2C_SLAVE, adresse_PCA) < 0) { //configuration du port
        cerr << "erreur PCA" << endl;
        close(file);
        return 1;
    }
    
    init_50Hz(file);

    set_servo_angle(file, 0, 0); 
    this_thread::sleep_for(chrono::seconds(2));

    set_servo_angle(file, 1, 120); 
    this_thread::sleep_for(chrono::seconds(2));

    set_servo_angle(file, 0, 90); 
    this_thread::sleep_for(chrono::seconds(2));

    set_servo_angle(file, 0, 180); 
    this_thread::sleep_for(chrono::seconds(2));

        set_servo_angle(file, 1, 45); 
    this_thread::sleep_for(chrono::seconds(2));

    set_servo_angle(file, 0, 90); 
    this_thread::sleep_for(chrono::seconds(2));

    set_servo_angle(file, 0, 0); 
    this_thread::sleep_for(chrono::seconds(2));

    close(file);
    return 0;
}
