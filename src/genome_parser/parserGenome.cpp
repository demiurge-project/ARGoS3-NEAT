#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void checkInput(string& id) {
   int nId = stoi(id);
   if(nId<=25) {
      if(nId>0 && nId<=8) {
         id = "P"+id;
      } else if (nId>8 && nId<=16) {
         id = "L"+id;
      } else if (nId>16 && nId<=19) {
         id = "G"+id;
      } else if (nId>19 && nId<=24) {
         id = "R"+id;
      } else {
         id = "B"+id;
      }
   }
}


int main(int argc, char* argv[]) {

   if(argc < 3) {
      cout << "Arg1: the path to the genome file" << endl;
      cout << "Arg2: the name of the output file (without the extension)" << endl;
      return -1;
   }

   ifstream iFile(argv[1]);

   vector<string> vecstrInputs;
   vector<string> vecstrHidden;
   vector<string> vecstrOutputs;
   vector<string> vecstrConnections;


   if(iFile) {

   size_t index;
   string str, id, input;

   while(getline(iFile,str)) {
      
      if(str.find("node") != string::npos) { //nodes
         index = str.size()-1;
         id = str.substr(str.find(" ")+1);
         id = id.substr(0, id.find(" "));

         if(str[index] == '1') { //input

            checkInput(id); 
            vecstrInputs.push_back(id);

         } else if(str[index] == '2') { //output

            vecstrOutputs.push_back(id);

         } else if(str[index] == '3') { //bias

            if(str[index-2] == '1') { //sensory type
               id = "B"+id;
               vecstrInputs.push_back(id);
            } else { //node type
               vecstrHidden.push_back(id);
            }

         } else { //hidden

            vecstrHidden.push_back(id);

         }

      } else if (str.find("gene") != string::npos) { //connections
         index = str.size()-1;
         id = str.substr(str.find(" ")+1); //remove the "gene ", thus id="traitNb input output weight ..."
         id = id.substr(id.find(" ")+1); //remove the "traitNb ", thus id="input output weight ..."
         input = id.substr(0, id.find(" ")); //input=input
         checkInput(input);
         id = id.substr(id.find(" ")+1); //id="output weight ..."
         id = input + " -> " + id.substr(0, id.find(" ")); //id = "input -> output"

         if(str[index] == '1') { //connection enabled
            vecstrConnections.push_back(id);
         }

      }
   }

   } else {
      cout << "Error during the reading of the file..." << endl;
   }

   string str = argv[2];
   str = str + ".gv";
   ofstream oFile(str.c_str());

   if(oFile) {
      oFile << "digraph G {" << endl;

      oFile << "  rankdir=LR;" << endl;
      oFile << "  node [shape=circle];" << endl;
      oFile << "  ranksep=1.5;" << endl << endl;

      // Inputs
      oFile << "  /* Inputs */" << endl;
      oFile << "  {" << endl;
      oFile << "     rank=same" << endl;
      oFile << "     ";
      for(size_t i=0; i<vecstrInputs.size(); i++) {
         oFile << vecstrInputs[i] << "; "; 
      }
      oFile << endl << endl;
      for(size_t i=1; i<vecstrInputs.size(); i++) {
         oFile << "	" << vecstrInputs[i-1] << " -> " << vecstrInputs[i] << " [style=invis];" << endl;
      }
      oFile << "  }" << endl << endl;

      //Hidden
      if(vecstrHidden.size() > 0) {
         oFile << "  /* Hidden */" << endl;
         oFile << "  {" << endl;
         oFile << "     rank=same" << endl;
         oFile << "     ";
         for(size_t i=0; i<vecstrHidden.size(); i++) {
            oFile << vecstrHidden[i] << "; "; 
         }
         oFile << endl << endl;
         for(size_t i=1; i<vecstrHidden.size(); i++) {
            oFile << "	   " << vecstrHidden[i-1] << " -> " << vecstrHidden[i] << " [style=invis];" << endl;
         }
         oFile << "  }" << endl << endl;
      }


      //Outputs
      oFile << "  /* Outputs */" << endl;
      oFile << "  {" << endl;
      oFile << "     rank=same" << endl;
      oFile << "     ";
      for(size_t i=0; i<vecstrOutputs.size(); i++) {
         oFile << vecstrOutputs[i] << "; ";
      }
      oFile << endl << endl;
      for(size_t i=1; i<vecstrOutputs.size(); i++) {
         oFile << "	" << vecstrOutputs[i-1] << " -> " << vecstrOutputs[i] << " [style=invis];" << endl;
      }
      oFile << "  }" << endl << endl;

      //Connections
      oFile << "  /* Connections */" << endl;
      for(size_t i=0; i<vecstrConnections.size(); i++) {
         oFile << "  " << vecstrConnections[i] << ";" << endl;
      }

      oFile << "}";
   } else {
      cout << "Error during the writing of the file..." << endl;
   }

   cout << "The output file is a gv (GraphViz) file, that can be opened with gvedit for example. ( e.g. gvedit output.gv )" << endl;
   return 0;
}
