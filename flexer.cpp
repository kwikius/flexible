
#include <quan/gc.hpp>
#include "flexer.hpp"
#include <stdexcept>
#include <fstream>

quan_lexer::flexer::flexer (std::string const & filename,std::ostream * out)
      : m_filename(filename),m_istream(quan::gc_malloc<std::ifstream>(filename.c_str())),
        m_ostream(out),m_mode(nullptr),m_buf_length(100),
        m_source_pos(1,1)
      {
           if ( ! m_istream ){
             throw std::runtime_error("cannot allocate istream in flexer .. out of memory?");
           }
           if ( ! *m_istream ){
             throw std::runtime_error("cannot open \"" + filename + "\"");
           }
           if ( ! m_ostream ){
             throw std::runtime_error("flexer - no output file");
           }
       }