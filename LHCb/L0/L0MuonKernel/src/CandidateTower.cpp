#include "GaudiKernel/MsgStream.h"
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/CandidateTower.h"

#include <algorithm>  // For make_pair

L0Muon::CandidateTower::CandidateTower(){
  m_seed = seedind(0,0);
  m_flag = false ;
  
}

L0Muon::CandidateTower::CandidateTower(std::pair<int, int> seedindex){
  m_seed = seedindex;
  m_flag = false ;
}

L0Muon::CandidateTower::~CandidateTower(){
}

void L0Muon::CandidateTower::setFoi(int sta, int xfoi, int yfoi){
  m_xFoi[sta] = xfoi;
  m_yFoi[sta] = yfoi;
}

L0Muon::CandidateTower::seedind L0Muon::CandidateTower::getFoi(int sta) {
  std::pair<int, int> foiXY;
  foiXY = seedind(m_xFoi[sta], m_yFoi[sta]);
  return foiXY ;
}

void L0Muon::CandidateTower::setSeed(std::pair<int, int> &seedindex){
  m_seed = seedindex ;
}

void  L0Muon::CandidateTower::
setBit(int sta, std::vector<boost::dynamic_bitset<> >  table, int maxXFoi, 
       int maxYFoi, int offset){
  unsigned int x, y;

  unsigned int xfoi = maxXFoi;
  unsigned int yfoi = maxYFoi;
  
  x = m_seed.first + xfoi +offset;
  y = m_seed.second + yfoi;

  
  
 
  int size = (2*m_xFoi[sta]+1)*(2*m_yFoi[sta]+1);
  boost::dynamic_bitset<> bits(size);
  m_bits = bits;
     if (m_yFoi[sta] == 0){
        bits[0]= table[y][x];
        if (bits.size()>1) {
          bits[1]=table[y][x+1];
          bits[2]=table[y][x-1];
          for (boost::dynamic_bitset<>::size_type i =1; 
               i<= (bits.size()-2)/2; i++){     
            if ( (x+i+1)< 24+2*xfoi ) {
              bits[2*i+1]=table[y][x+i+1];
            }
              if ( (x-i-1)>=0 && (x-i-1)< 999999){
                bits[2*i+2]=table[y][x-i-1];
              
              } else {
            
                // else if  ( (x-i-1)<0 && (x-i-1)> 999999) {
                bits[2*i+2]=0;
              }
              
          }
          
        }
        
     } else  if (m_yFoi[sta] > 0){

       unsigned int maxxfoi = m_xFoi[sta];
       for (boost::dynamic_bitset<>::size_type i=0; i<(2*maxxfoi+1); i++){
         bits[i] = table[y][x- maxxfoi+i];
         
       }
       for (boost::dynamic_bitset<>::size_type i=(2*maxxfoi+1); 
            i< 2*(2*maxxfoi+1); i++){
         bits[i] = table[y-1][x - m_xFoi[sta]+i-(2*maxxfoi+1)];

       }
       for (boost::dynamic_bitset<>::size_type i=2*(2*maxxfoi+1); 
            i< 3*(2*maxxfoi+1); i++){
         bits[i] = table[y+1][x - maxxfoi+i-2*(2*maxxfoi+1)];

         
       }
       
     }
     for (boost::dynamic_bitset<>::size_type i =0; i< bits.size(); i++){
       m_bits[i] =bits[i];
     }
  
}

//boost::dynamic_bitset<>  L0Muon::CandidateTower::getBit(int sta){
boost::dynamic_bitset<>  L0Muon::CandidateTower::getBit(){
  return m_bits ;
}

//void L0Muon::CandidateTower::reset(int sta){   
void L0Muon::CandidateTower::reset(){   
  m_bits.clear();
}


void L0Muon::CandidateTower::drawBits(int sta, MsgStream * log) {

  *log << MSG::DEBUG << "Bitset size in sta"<< " " << sta 
       << " " << m_bits.size() << endreq;
  
  for ( boost::dynamic_bitset<>::size_type ir = 0; ir < m_bits.size(); ir++) {
    *log << MSG::DEBUG << " " << m_bits[ir] ;
  }
}


 void L0Muon::CandidateTower::setPadIndex(int sta, int maxXFoi, int maxYFoi, 
                                          int offset, int ind)
{
  int xfoi = maxXFoi;
  int yfoi = maxYFoi;
  
  int x = m_seed.first + xfoi +offset;
  int y = m_seed.second + yfoi;

  if (sta ==2){
    m_xyindex[sta]= std::make_pair(m_seed.second, m_seed.first);
  } 
  else {
    if (m_yFoi[sta] == 0){   
      if (ind ==0)
        m_xyindex[sta] = std::make_pair(y,x);
  
      if (ind >0){
        if (ind %2 ==0){
          int xtmp = x -(ind/2);
          m_xyindex[sta] = std::make_pair(y,xtmp);
        }
        if (ind %2 !=0){
          int xtmp = x+ (ind+1)/2;
          
          m_xyindex[sta] = std::make_pair(y,xtmp);
        }
        
      }
      
    }
    
  
    if (m_yFoi[sta] > 0){
    //sta 4 & 5
      if (ind >=0 && ind <(2* m_xFoi[sta]+1)){
        int xtmp = ind;
        m_xyindex[sta] = std::make_pair(y-1,xtmp);
      }
      if (ind >=(2* m_xFoi[sta]+1) && ind < 2* (2* m_xFoi[sta]+1)){
        int xtmp = ind -2 *m_xFoi[sta];
        m_xyindex[sta] = std::make_pair(y,xtmp);
      }
      if (ind >=(2*(2* m_xFoi[sta]+1)) && ind < 3* (2* m_xFoi[sta]+1)){
        int xtmp = ind -2*(2 *m_xFoi[sta]);
        m_xyindex[sta] = std::make_pair(y,xtmp);
      }
      
    
      
    }
    
    
  }
  
}
