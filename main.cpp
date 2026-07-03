#include <iostream>
#include <fcntl.h>      // open()
#include <sys/ioctl.h>  // ioctl()
#include <linux/i2c-dev.h> // I2C
#include <unistd.h>     // close()

using namespace std;

int main() {
    cout << "test" << endl;

    // ouverture port I2C
    int file = open("/dev/i2c-1", O_RDWR); // O_RDWR pour les droits de lecture et d'ecriture
    
    if (file < 0) {
        cerr << "erreur I2C" << endl;
        return 1;
    }
    cout << "I2C valide" << endl;

    // on cible précisément la puce en hexa
    int adresse_puce = 0x40;
    
    if (ioctl(file, I2C_SLAVE, adresse_puce) < 0) { //configuration du port
        cerr << "erreur puce" << endl;
        close(file);
        return 1;
    }
    cout << "puce valide" << endl;

    //fermeture du port
    close(file);
    cout << "fin test" << endl;
    
    return 0;
}