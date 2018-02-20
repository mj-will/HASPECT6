c
          subroutine smear(pid,e,t,f)
          IMPLICIT NONE
#include "cloptions.inc"
c
          integer pid
          real p,e,t,f,rn1,rn2,rn3,rn4,sin6
          real dts,ps,es,ts,fs
           es=e
           ts=t
           fs=f
           sin6=9.6                  ! sin6^o
           call rannor(rn1,rn2)
           call rannor(rn3,rn4)
c
          if(pid.eq.22) then ! photon separately
           e=es+cl_sme*sqrt(es)*rn1
           t=ts+cl_smf*rn2
           f=fs+cl_smg*rn3
           return
          endif
c
          if(pid.eq.11) then ! electron
           p=e
          else if (pid.eq.2212) then ! proton
           p=e*e-0.83827**2
           if(p.gt.0) then 
            p=sqrt(p)
           else
            p=0
           endif
          else if (pid.eq.211) then! pi+
           p=e*e-0.139**2
           if(p.gt.0) then 
            p=sqrt(p)
           else
            p=0
           endif
          else if (pid.eq.-211) then! pi-
           p=e*e-0.139**2
           if(p.gt.0) then 
            p=sqrt(p)
           else
            p=0
           endif
          else if (pid.eq.321) then ! K+
           p=e*e-0.494**2
           if(p.gt.0) then 
            p=sqrt(p)
           else
            p=0
           endif
          else
          endif
c
           ps=p+(cl_sma+cl_smb*p)*p*rn1
           dts=(cl_smd+cl_smc/p)*rn2
           ts=t+dts
c           fs=f+(cl_smd+cl_smc/p)/sin6*rn3
           fs=f+(cl_smd+cl_smc/p)*rn3

          if(pid.eq.11) then ! electron
           es=ps
          else if (pid.eq.2212) then ! proton
           es=sqrt(ps*ps+0.83827**2)
          else if (pid.eq.211) then! pi+
           es=sqrt(ps*ps+0.139**2)
          else if (pid.eq.-211) then! pi-
           es=sqrt(ps*ps+0.139**2)
          else if (pid.eq.321) then ! K+
           es=sqrt(ps*ps+0.494**2)
          else
          endif
           e=es
           f=fs
           t=ts
          return
          end
