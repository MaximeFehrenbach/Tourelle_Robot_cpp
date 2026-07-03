#include <iostream>
#include <fcntl.h>      // open()
#include <sys/ioctl.h>  // ioctl()
#include <linux/i2c-dev.h> // I2C
#include <unistd.h>     // close()
#include <thread> 
#include <chrono>

using namespace std;

// ecrire une valeur dans un registre
void write_byte(int file, int register_address, int data) {
    unsigned char buffer[2];
    buffer[0] = register_address;
    buffer[1] = data;
    
    write(file, buffer, 2);
}

int main() {
    cout << "test config freq" << endl;

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
    
    // mise en veille
    write_byte(file, 0x00, 0x10); 
    
    // fréquence à 50 Hz
    write_byte(file, 0xFE, 121); // 121 = 25MHz/(2^12*50Hz)-1
    
    // réveil du PCA
    write_byte(file, 0x00, 0x20);
    
    // on laisse l'horloge du PCA se stabiliser
    this_thread::sleep_for(chrono::milliseconds(5));

    cout << "config à 50 Hz" << endl;

    close(file);
    return 0;
}
