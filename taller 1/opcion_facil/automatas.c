#include <stdio.h>

int jarraGrande = 0;
int jarraPequena = 0;
int temp = 0;

void jarraPequenaControl();

void jarraGrandeControl() {
    if (jarraGrande == 0) {
        jarraGrande = 5;
        printf("Se llena la jarra grande 5 unidades\n");
    }
    if (jarraGrande != 4) {
        temp = (jarraGrande < 3 ? jarraGrande : 3) - jarraPequena;
        jarraGrande -= temp;
        jarraPequena += temp;
        printf("Se pasa a la jarra pequena %d unidades\n", temp);
        jarraPequenaControl();
    } else {
        printf("La jarra grande tiene 4 unidades\n");
    }
}

void jarraPequenaControl() {
    if (jarraPequena == 3) {
        jarraPequena = 0;
        printf("Se vacia la jarra pequena 3 unidades\n");
    }
    jarraGrandeControl();
}

int main() {
    jarraGrandeControl();
    return 0;
}
