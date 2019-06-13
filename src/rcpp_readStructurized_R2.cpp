#include <Rcpp.h>

#include "R2/protocol.h"
#include <fstream>

// [[Rcpp::depends(BH)]]

using namespace Rcpp;
using namespace Resonance::R2;
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
List readStructurized_R2(std::string fname)
{
  ifstream file(fname.c_str(), ifstream::binary);
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
                case Resonance::R2::Int32::ID:
                {
                  List si = List::create(
                    Named("id") = id,
                    Named("name") = item.extractString<File_Stream::name>(),
                    Named("channels") = fields.extractField<ConnectionHeader_Int32::channels>(),
                    Named("samplingRate") = fields.extractField<ConnectionHeader_Int32::samplingRate>(),
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
                    Named("type") = "event"
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
                RObject db = DB_channels(types[stream], (double)data.extractField<Double::created>()/TIME_SCALING_FACTOR, raw);
                
                db.attr("created") = (double)data.extractField<Double::created>()/TIME_SCALING_FACTOR;
                db.attr("received") = (double)data.extractField<Double::received>()/TIME_SCALING_FACTOR;
                
                blocks.push_back(db);
                
              }
              break;
              case Resonance::R2::Int32::ID:
              {
                std::vector<int> raw(data.extractVector<Resonance::R2::Int32::data>());
                RObject db = DB_channels(types[stream], (double)data.extractField<Resonance::R2::Int32::created>()/TIME_SCALING_FACTOR, raw);
                
                db.attr("created") = (double)data.extractField<Resonance::R2::Int32::created>()/TIME_SCALING_FACTOR;
                db.attr("received") = (double)data.extractField<Resonance::R2::Int32::received>()/TIME_SCALING_FACTOR;
                
                blocks.push_back(db);
                
              }
              break;
              case Message::ID:
              {
                CharacterVector raw(data.extractString<Message::message>());
                RObject db = DB_message(types[stream], (double)data.extractField<Message::created>()/TIME_SCALING_FACTOR, raw);
                
                db.attr("created") = (double)data.extractField<Message::created>()/TIME_SCALING_FACTOR;
                db.attr("received") = (double)data.extractField<Message::received>()/TIME_SCALING_FACTOR;

                blocks.push_back(db);
              }
              break;
            }
          }
          break;
        }
      }
    }
  }
  catch(const Eof &f){
  }
  catch(...){
    Rf_warning("Bugs (%i blocks readed)", blocks.length());
    /*std::cout << "\nUnknown exception type: '" << (__cxa_current_exception_type()->name()) << "'" << std::endl;
    int status;
    std::cout << "fuck: " << abi::__cxa_demangle(__cxa_current_exception_type()->name(), 0, 0, &status) << std::endl;*/
  }
  
  return List::create(
    Named("streams") = types,
    Named("blocks") = blocks
  );
}