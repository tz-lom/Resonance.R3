#include <Rcpp.h>
#include "R2/protocol.h"
#include <fstream>

// [[Rcpp::depends(BH)]]

using namespace Rcpp;
using namespace Resonance::R2;
using namespace std;

#include "read.h"


// [[Rcpp::export]]
List blockLevelRead(std::string fname)
{
  ifstream file(fname.c_str(), ifstream::binary);
  file.exceptions(ios::eofbit | ios::failbit | ios::badbit);
  List out;
  
  try{
  
  if(file)
  {
    // @todo: need to control errors
    char head[3];
    file.read(head,3);
    if(head[0]!='!'||head[1]!='R'||head[2]!='3')
    {
      Rf_warning("Wrong header");
      return out;
    }
    
    
    SerializedData sd(readFromFile, &file);
    bool packed = sd.extractField<FileHeader::packed>();
    if(packed) return out; // @todo: support packed files

    for(;;){
      SerializedData item(readFromFile, &file);
      switch(item.recordId())
      {
        case File_Stream::ID:
        {
        
          int id = item.extractField<File_Stream::id>();
        
          SerializedData info = item.extractAny<File_Stream::info>();
        
          switch(info.recordId())
          {
          case ConnectionHeader_Double::ID:
            {
            
              List si = List::create(
                Named("id") = id,
                Named("name") = item.extractString<File_Stream::name>(),
                Named("channels") = info.extractField<ConnectionHeader_Int64::channels>(),
                Named("type") = "double"
              );
              si.attr("class") = "StreamDescription";
              out.push_back(si);
            }
            break;
          case ConnectionHeader_Message::ID:
            {
              List si = List::create(
                Named("id") = id,
                Named("name") = "",
                Named("channels") = 0,
                Named("type") = "message"
              );
              si.attr("class") = "StreamDescription";
              out.push_back(si);
            }
            break;
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
              int samples = data.extractField<Double::samples>();
              std::vector<double> raw(data.extractVector<Double::data>());
              NumericMatrix db(raw.size()/samples, samples, raw.begin() );
              db = transpose(db);
              
              db.attr("class") = "DataBlock";
              db.attr("created") = (double)data.extractField<Double::created>()/1E3;
              db.attr("received") = (double)data.extractField<Double::received>()/1E3;
              db.attr("stream") = stream;
              out.push_back(db);
            }
            break;
          case Resonance::R2::Int32::ID:
          {
            int samples = data.extractField<Resonance::R2::Int32::samples>();
            std::vector<i32> raw(data.extractVector<Resonance::R2::Int32::data>());
            NumericMatrix db(raw.size()/samples, samples, raw.begin() );
            db = transpose(db);
            
            db.attr("class") = "DataBlock";
            db.attr("created") = (double)data.extractField<Resonance::R2::Int32::created>()/1E3;
            db.attr("received") = (double)data.extractField<Resonance::R2::Int32::received>()/1E3;
            db.attr("stream") = stream;
            out.push_back(db);
          }
            break;
          case Message::ID:
            {
              CharacterVector db(data.extractString<Message::message>());
              db.attr("class") = "DataBlock";
              db.attr("created") = (double)data.extractField<Message::created>()/1E3;
              db.attr("received") = (double)data.extractField<Message::received>()/1E3;
              db.attr("stream") = stream;
              out.push_back(db);
            }
            break;
          }
        }
        break;
      }
    }
  }
  }catch(Eof f){

  }catch(...){
    Rf_warning("Bugs");
  }
  
  return out;
}
