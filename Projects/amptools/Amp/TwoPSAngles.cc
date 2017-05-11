
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#include "IUAmpTools/Kinematics.h"
#include "TwoPSAngles.h"
#include "clebschGordan.h"
#include "wignerD.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"
#include "TVector3.h"

TwoPSAngles::TwoPSAngles( int j, int m, int e ) :
Clas12PhotonsAmplitude(),
m_j( j ), // total J of resonance?
m_m( m ),
m_e( e )
{
  // make sure values are reasonable
  assert( abs( e ) == 1 );
  assert( m <= j );
  
  if( m_m == 0 ) m_bigTheta = 0.5;
  if( m_m  > 0 ) m_bigTheta = sqrt( 0.5 );
  if( m_m  < 0 ) m_bigTheta = 0;
  
  // the "reflectivity factor" is e*(-1)^(m)
  m_reflectivityFactor = ( m_m % 2 == 0 ? m_e : -m_e );
  
  //set electron indices here
  m_Ielectron=0;
  m_Iscattered=1;
}


complex< GDouble >
TwoPSAngles::calcHelicityAmplitude( int helicity, GDouble** pKin ) const {

  //first calculate virtual photon from beam and scattered e-
  TLorentzVector beam   ( pKin[m_Ielectron][1]-pKin[m_Iscattered][1], pKin[m_Ielectron][2]-pKin[m_Iscattered][2], pKin[m_Ielectron][3]-pKin[m_Iscattered][3], pKin[m_Ielectron][0]-pKin[m_Iscattered][0] );
  
  TLorentzVector recoil ( pKin[2][1], pKin[2][2], pKin[2][3], pKin[2][0] ); 
  TLorentzVector p1     ( pKin[3][1], pKin[3][2], pKin[3][3], pKin[3][0] ); 
  TLorentzVector p2     ( pKin[4][1], pKin[4][2], pKin[4][3], pKin[4][0] ); 

  cout<<beam.M()<< " "<<recoil.M()<<" "<<p1.M()<<" "<<p2.M()<<endl;
  TLorentzVector resonance = p1 + p2;
  
  TLorentzRotation resRestBoost( -resonance.BoostVector() );
  
  TLorentzVector beam_res   = resRestBoost * beam;
  TLorentzVector recoil_res = resRestBoost * recoil;
  TLorentzVector p1_res = resRestBoost * p1;
  
  // phi (angle) is generated randomly, we just need to pick a direction?
  // T3Vector z = beam_res.vect().unit();
  //  T3Vector y = recoil_res.vect().cross(z).unit();
  TVector3 z = -recoil_res.Vect().Unit();
  TVector3 y = beam_res.Vect().Cross(z).Unit();
  TVector3 x = y.Cross(z);
  
  TVector3 angles( (p1_res.Vect()).Dot(x),
                    (p1_res.Vect()).Dot(y),
                    (p1_res.Vect()).Dot(z) );
  
  GDouble cosTheta = angles.CosTheta();
  GDouble phi = angles.Phi();
  
  GDouble coef = sqrt( ( 2. * m_j + 1 ) / ( 4 * 3.1416 ) );

  cout<<coef * m_bigTheta *( wignerD( m_j, m_m, 0, cosTheta, phi ) - static_cast< GDouble>( m_reflectivityFactor ) * wignerD( m_j, -m_m, 0, cosTheta, phi ) )<<endl;
  
  return complex< GDouble >( coef * m_bigTheta * 
                            ( wignerD( m_j, m_m, 0, cosTheta, phi ) - 
                             static_cast< GDouble>( m_reflectivityFactor ) * 
                             wignerD( m_j, -m_m, 0, cosTheta, phi ) ) );  
}

TwoPSAngles*
TwoPSAngles::newAmplitude( const vector< string >& args ) const {
  
	assert( args.size() == 3 );
	
	int j = atoi( args[0].c_str() );
	int m = atoi( args[1].c_str() );
	int e = atoi( args[2].c_str() );
	
	return new TwoPSAngles( j, m, e );
}

TwoPSAngles*
TwoPSAngles::clone() const {
  
	return ( isDefault() ? new TwoPSAngles() : 
          new TwoPSAngles( m_j, m_m, m_e ) );
}

#ifdef GPU_ACCELERATION
void
TwoPSAngles::launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const {

  GPUTwoPSAngles_exec( dimGrid,  dimBlock, GPU_AMP_ARGS,
                       m_j, m_m, m_bigTheta, 
                       static_cast< GDouble >( m_reflectivityFactor ) );
}
#endif //GPU_ACCELERATION
