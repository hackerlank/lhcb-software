#ifndef SYMM_LORENTZ_MATRIX_HH
#define SYMM_LORENTZ_MATRIX_HH


#include "TLorentzVector.h"

class SymmLorentzMatrix{
 protected:
  static SymmLorentzMatrix* __gmunu;
  static void makeGmunu();

  TLorentzVector _v[4];
  // we'll follow the x, y, z, E convention, i.e. E is 4
  bool symmetrize(){
    // clumsy but save
    X().SetY(Y().X());
    X().SetZ(Z().X());
    X().SetT(T().X());

    Y().SetX(X().Y());
    Y().SetZ(Z().Y());
    Y().SetT(T().Y());

    Z().SetX(X().Z());
    Z().SetY(Y().Z());
    Z().SetT(T().Z());

    T().SetX(X().T());
    T().SetY(Y().T());
    T().SetZ(Z().T());
    return true;
  }
  bool makeZero(){
    X().SetXYZT(0,0,0,0);
    Y().SetXYZT(0,0,0,0);
    Z().SetXYZT(0,0,0,0);
    T().SetXYZT(0,0,0,0);
    return true;
  }
 public:
  static const SymmLorentzMatrix& gmunu();
  const TLorentzVector& v(int i) const{return _v[i];}

  SymmLorentzMatrix(){};
  SymmLorentzMatrix(const TLorentzVector p[4]){
    for(int i=0; i<4; i++) _v[i] = p[i];
  }
  SymmLorentzMatrix(const TLorentzVector p){
    X().SetX(p.X() * p.X());
    X().SetY(p.X() * p.Y());
    X().SetZ(p.X() * p.Z());
    X().SetT(p.X() * p.T());

    Y().SetX(p.Y() * p.X());
    Y().SetY(p.Y() * p.Y());
    Y().SetZ(p.Y() * p.Z());
    Y().SetT(p.Y() * p.T());

    Z().SetX(p.Z() * p.X());
    Z().SetY(p.Z() * p.Y());
    Z().SetZ(p.Z() * p.Z());
    Z().SetT(p.Z() * p.T());

    T().SetX(p.T() * p.X());
    T().SetY(p.T() * p.Y());
    T().SetZ(p.T() * p.T());
    T().SetT(p.T() * p.T());
  }
  SymmLorentzMatrix(const SymmLorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other._v[i];
  }
  const TLorentzVector& X() const{return _v[0];}
  const TLorentzVector& Y() const{return _v[1];}
  const TLorentzVector& Z() const{return _v[2];}
  const TLorentzVector& T() const{return _v[3];}

  TLorentzVector& X() {return _v[0];}
  TLorentzVector& Y() {return _v[1];}
  TLorentzVector& Z() {return _v[2];}
  TLorentzVector& T() {return _v[3];}
  
  SymmLorentzMatrix& add(const SymmLorentzMatrix& other){
    for(int i=0; i < 4; i++) _v[i] += other._v[i];
    return *this;
  }
  SymmLorentzMatrix& subtract(const SymmLorentzMatrix& other){
    for(int i=0; i < 4; i++) _v[i] -= other._v[i];
    return *this;
  }
  SymmLorentzMatrix& mult(double s){
    for(int i=0; i < 4; i++) _v[i] *= s;
    return *this;
  }
  SymmLorentzMatrix& div(double s){
    for(int i=0; i < 4; i++) _v[i] *= (1./s);
    return *this;
  }

  TLorentzVector Contract(const TLorentzVector& vec){
    // M^{mu nu} g_{nu alpha} v^{alpha}
    // M^{mu nu} v_{alpha}
    return vec.T()*T() - vec.X()*X() - vec.Y()*Y() -vec.Z()*Z();
  }
  SymmLorentzMatrix Contract_1(const SymmLorentzMatrix& M){
    // O^{mu alpha} g_{alpha beta} M^{beta nu} = R^{mu nu}
    // O^{mu alpha} M_{beta}^{nu}
    SymmLorentzMatrix R;
    R.X() =  this->Contract(M.X());
    R.Y() =  this->Contract(M.Y());
    R.Z() =  this->Contract(M.Z());
    R.T() =  this->Contract(M.T()); // signs?

    return R;
  }
  double Contract_2(const SymmLorentzMatrix& M){
    // O^{mu alpha} g_{alpha beta} M^{beta nu} g_{mu nu}
    SymmLorentzMatrix R(Contract_1(M));
    // R^{mu nu} R_{mu nu}
    double xx =  R.X().Dot(R.X());
    double yy =  R.Y().Dot(R.Y());
    double zz =  R.Z().Dot(R.Z());
    double tt =  R.T().Dot(R.T());
    
    return tt - xx - yy - zz; // signs?
  }

  SymmLorentzMatrix& operator+=(const SymmLorentzMatrix& rhs){
    return add(rhs);
  }
  SymmLorentzMatrix& operator*=(double rhs){
    return mult(rhs);
  }
  SymmLorentzMatrix& operator-=(const SymmLorentzMatrix& rhs){
    return subtract(rhs);
  }
  SymmLorentzMatrix& operator/=(double rhs){
    return div(rhs);
  }
  SymmLorentzMatrix& operator=(const SymmLorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other._v[i];
    return *this;
  }
  SymmLorentzMatrix operator+(const SymmLorentzMatrix& rhs)const{
    SymmLorentzMatrix returnVal(*this);
    returnVal += rhs;
    return returnVal;
  }
  SymmLorentzMatrix operator-(const SymmLorentzMatrix& rhs)const{
    SymmLorentzMatrix returnVal(*this);
    returnVal -= rhs;
    return returnVal;
  }
  SymmLorentzMatrix operator*(double rhs)const{
    SymmLorentzMatrix returnVal(*this);
    returnVal *= rhs;
    return returnVal;
  }
  SymmLorentzMatrix operator/(double rhs)const{
    SymmLorentzMatrix returnVal(*this);
    returnVal /= rhs;
    return returnVal;
  }

};

SymmLorentzMatrix operator*(double lhs, const SymmLorentzMatrix& rhs);
SymmLorentzMatrix operator/(double lhs, const SymmLorentzMatrix& rhs);

#endif
//
