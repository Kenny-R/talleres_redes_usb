#include <iostream>
using namespace std;

int jarraGrande = 0;
int jarraPequena = 0;
int temp = 0;

void jarraPequenaControl();

void jarraGrandeControl(){
    if (jarraGrande == 0){
        jarraGrande = 5;
        cout << "Se llena la jarra grande 5 unidades" << endl;
    }
    if (jarraGrande != 4) {
        temp = min(jarraGrande, 3) - jarraPequena;
        jarraGrande -= temp;
        jarraPequena += temp;
        cout << "Se pasa a la jarra pequena " << temp << " unidades" << endl;
        jarraPequenaControl();
    } else {
        cout << "La jarra grande tiene 4 unidades" << endl;
    }
}

void jarraPequenaControl(){
    if (jarraPequena == 3){
        jarraPequena = 0;
        cout << "Se vacia la jarra pequena 3 unidades" << endl;
    }
    jarraGrandeControl();
}

main(){
    jarraGrandeControl();
}