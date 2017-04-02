#include <Rcpp.h>

#include "R3/protocol.switch.h"
#include <fstream>

// [[Rcpp::depends(BH)]]

using namespace Rcpp;
using namespace Resonance::R3;
using namespace std;

#include "read.h"


void rs_addType2(List &types, List SI, int id)
{
  
  for(int t=types.size(); t<=id; ++t)
  {
    types.push_back(List());
  }
  types[id] = SI;
  
}


// [[Rcpp::export]]
List readStructurized_R3(std::string fname)
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
      if(head[0]!='@'||head[1]!='R'||head[2]!='3')
      {
        return List();
      }
      
      SerializedData sd(readFromFile, &file);
      bool packed = sd.extractField<FileHeader::packed>();
      if(packed) return List(); // @todo: support packed files
      
      for(;;){
        SerializedData item(readFromFile, &file);
        switch(item.id())
        {
          case File_Stream::ID:
          {
            
            int id = item.extractField<File_Stream::id>();
            
            SerializedData info = item.extractAny<File_Stream::info>();

          
            switch(info.id())
            {
              case ConnectionHeader_Float64::ID:
              {
                List si = List::create(
                  Named("id") = id,
                  Named("name") = item.extractString<File_Stream::name>(),
                  Named("channels") = info.extractField<ConnectionHeader_Float64::channels>(),
                  Named("samplingRate") = info.extractField<ConnectionHeader_Float64::samplingRate>(),
                  Named("type") = "channels"
                );
                rs_addType2(types, si, id);
              }
              break;
              case ConnectionHeader_Int32::ID:
              {
                List si = List::create(
                  Named("id") = id,
                  Named("name") = item.extractString<File_Stream::name>(),
                  Named("channels") = info.extractField<ConnectionHeader_Int32::channels>(),
                  Named("samplingRate") = info.extractField<ConnectionHeader_Int32::samplingRate>(),
                  Named("type") = "channels"
                );
                rs_addType2(types, si, id);
              }
              break;
              case ConnectionHeader_Message::ID:
              {
                List si = List::create(
                  Named("id") = id,
                  Named("name") = item.extractString<File_Stream::name>(),
                  Named("type") = "event"
                );
                rs_addType2(types, si, id);
              }
              break;
            }
          }
          break;
          case File_DataBlock::ID:
          {
            int stream = item.extractField<File_DataBlock::stream>();
            
            SerializedData data = item.extractAny<File_DataBlock::block>();
            
            switch(data.id())
            {
              case Float64::ID:
              {
                std::vector<double> raw(data.extractVector<Float64::data>());
                RObject db = DB_channels(types[stream], (double)data.extractField<Float64::created>()/1E3, raw);
                
                db.attr("created") = (double)data.extractField<Float64::created>()/1E3;
                db.attr("received") = (double)data.extractField<Float64::received>()/1E3;
                
                blocks.push_back(db);
                
              }
              break;
              case Resonance::R3::Int32::ID:
              {
                std::vector<int> raw(data.extractVector<Resonance::R3::Int32::data>());
                RObject db = DB_channels(types[stream], (double)data.extractField<Resonance::R3::Int32::created>()/1E3, raw);
                
                db.attr("created") = (double)data.extractField<Resonance::R3::Int32::created>()/1E3;
                db.attr("received") = (double)data.extractField<Resonance::R3::Int32::received>()/1E3;
                
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