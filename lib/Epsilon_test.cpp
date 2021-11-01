#include <AFA/AFAut.h>
#include <fstream>

int main(){

    std::cout<<"Epsilon Begins"<<std::endl;

    std::string file2 = "Pass2AfterDeletion";

    AFAut* afa = AFAut::MakeAFAutFromDot(file2+".dot");

// afa->PrintToDot(afa->mInit,file2+"_out.dot"); (correct it by adding new method in header file)
    std::cout<<"Epsilon Ends"<<std::endl;
}
