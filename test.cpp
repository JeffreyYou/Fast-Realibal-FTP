#include <fstream>
using namespace std;

int main(){
    ofstream out;
    out.open("b.txt", ios_base::app);
    string s = "Data";
    out <<s; 
    return 0;
}