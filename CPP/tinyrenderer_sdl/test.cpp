#include <iostream>
#include "model.h"

using namespace std;

int main(int argc, char const *argv[])
{
    Model *model = new Model("obj/african_head.obj");
    cout<<model->nfaces()<<endl;
    system("pause");
    return 0;
}
