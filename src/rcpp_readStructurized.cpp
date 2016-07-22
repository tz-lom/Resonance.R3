#include <Rcpp.h>
#include "protocol.h"
#include <fstream>

// [[Rcpp::depends(BH)]]

using namespace Rcpp;
using namespace Resonance::R3;
using namespace std;

#include "read.h"


void rs_addType(List &types, List SI, int id)
{
  
  for(int t=types.size(); t<=id; ++t)
  {
    types.push_back(List());
  }
  types[id] = SI;
  
}


// [[Rcpp::export]]
List readStructurized(std::string fname)
{
  ifstream file(fname.c_str(), ifstream::binary);
  file.exceptions(ios::eofbit | ios::failbit | ios::badbit);
  List types;
  List blocks;
 
  Function DB_channels("DB.channels", "R3");
  Function DB_message("DB.event", "R3");
  
  try{
    
    if(file)
    {
      char head[3];
      file.read(head,3);
      if(head[0]!='!'||head[1]!='R'||head[2]!='3')
      {
        Rf_warning("Wrong header");
        return List();
      }
      
      SerializedData sd(readFromFile, &file);
      bool packed = sd.extractField<FileHeader::packed>();
      if(packed) return List(); // @todo: support packed files
      
      for(;;){
        SerializedData item(readFromFile, &file);
        switch(item.recordId())
        {
          case File_Stream::ID:
          {
            
            int id = item.extractField<File_Stream::id>();
            
            SerializedData info = item.extractAny<File_Stream::info>();
            SerializedData fields = info.extractAny<ConnectionHeader::description>();
            
            if(info.recordId() == ConnectionHeader::ID)
            {
              switch(info.extractField<ConnectionHeader::type>())
              {
                case Double::ID:
                {
                  
                  List si = List::create(
                    Named("id") = id,
                    Named("name") = item.extractString<File_Stream::name>(),
                    Named("channels") = fields.extractField<ConnectionHeader_Double::channels>(),
                    Named("samplingRate") = fields.extractField<ConnectionHeader_Double::samplingRate>(),
                    Named("type") = "channels"
                  );
                  rs_addType(types, si, id);
                }
                break;
                case Message::ID:
                {
                  List si = List::create(
                    Named("id") = id,
                    Named("name") = item.extractString<File_Stream::name>(),
                    Named("type") = "message"
                  );
                  rs_addType(types, si, id);;
                }
                break;
              }
            }
          }
          break;
          case File_DataBlock::ID:
          {
            int stream = item.extractField<File_DataBlock::stream>();
            
            SerializedData data = item.extractAny<File_DataBlock::block>();
            
            switch(data.recordId())
            {
              case Double::ID:
              {
                std::vector<double> raw(data.extractVector<Double::data>());
                RObject db = DB_channels(types[stream], (double)data.extractField<Double::created>()/1E3, raw);
                
                db.attr("created") = (double)data.extractField<Double::created>()/1E3;
                db.attr("received") = (double)data.extractField<Double::received>()/1E3;
                
                blocks.push_back(db);
                
              }
              break;
              case Message::ID:
              {
                CharacterVector raw(data.extractString<Message::message>());
                RObject db = DB_message(types[stream], (double)data.extractField<Message::created>()/1E3, raw);
                
                db.attr("created") = (double)data.extractField<Message::created>()/1E3;
                db.attr("received") = (double)data.extractField<Message::received>()/1E3;

                blocks.push_back(db);
              }
              break;
            }
          }
          break;
        }
      }
    }
  }catch(ios_base::failure f){
    
  }catch(...){
    Rf_warning("Bugs");
  }
  
  return List::create(
    Named("streams") = types,
    Named("blocks") = blocks
  );
}