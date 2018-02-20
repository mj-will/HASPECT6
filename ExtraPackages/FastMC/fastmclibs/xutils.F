c
        real function  PolarTheta(vx,vy,vz)
        implicit none
        real vx,vy,vz,pmod,theta
        pmod=vx*vx+vy*vy+vz*vz
        if(pmod .gt. 0 ) then
         theta=acos(vz/sqrt(pmod))
        else
         theta=-100
        endif
        PolarTheta=theta
        return
        end

        real function Azimuthalphi(vx,vy )
        implicit none
        real pi
        parameter ( PI=3.1415926)
        real vx,vy,pmod,phi,cosf
        pmod=vx*vx+vy*vy
        if(pmod .gt. 0 ) then
         pmod=sqrt(pmod)
         cosf=vx/pmod
        else
         cosf=1.0
        endif
        if(abs(cosf) .le. 1.0) phi=acos(cosf);
        if(vy .lt. 0.0) phi= 2*PI-phi;
        Azimuthalphi=phi
        return
        end
c
c
c
c
       real function phistar(ip,v4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
c
        integer ip                  ! ip=0 data, ip>0 MC
        real v4(4),tnorm(4),pi,phi,vangle,vdotm
        pi=acos(-1.0)
c
        if (ip.eq.0) then ! data
          call crossm(qiu4,v4el0,tnorm)
          phi=vangle(qiu4,v4el0,qiu4,v4)
        else
          call crossm(gqiu4,v4el0,tnorm)
          phi=vangle(gqiu4,v4el0,gqiu4,v4)
        endif
c
         phistar=phi
         if (vdotm(tnorm,v4,3).lt.0) phistar=2*pi-phi
c
         return
         end
c
c
       real function phirho(ip,v4,p4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
c
        integer ip                  ! ip=0 data, ip>0 MC
        real v4(4),p4(4),tnorm(4),tnorm2(4),pi,phi,vangle,vdotm
        pi=acos(-1.0)
c
        if (ip.eq.0) then ! data
          call crossm(v4,qiu4,tnorm)
          phi=vangle(v4,qiu4,v4,p4)
        else
          call crossm(v4,gqiu4,tnorm)
          phi=vangle(v4,gqiu4,v4,p4)
        endif
c
         phirho=phi
         if (vdotm(tnorm,p4,3).lt.0) phirho=2*pi-phi
c
         return
         end
c
c
c
       real function thadron(ip,v4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
       integer ip
       real v4(4),tnorm(4),vdotm
       if(ip.eq.0) then ! data
         call vdifm(qiu4,v4,tnorm,4)
       else
         call vdifm(gqiu4,v4,tnorm,4)
       endif
c
         thadron=vdotm(tnorm,tnorm,4)
c
         return
         end
c
c
       real function mismas(ip,v4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
       integer ip
       real v4(4),tnorm(4),vmass2
       if(ip.eq.0) then ! data
         call vdifm(v4s,v4,tnorm,4)
       else
         call vdifm(g4s,v4,tnorm,4)
       endif
c
         mismas=vmass2(tnorm)
c
         return
         end
c
c
c
       real function ptrans(ip,ebeam,v4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
c
        integer ip                  ! ip=0 data, ip>0 MC
        real v4(4),tnorm(4),vdotm,plong,ptpt2,q3,ebeam
c
        if (ip.eq.0) then ! data
          q3=sqrt(q2+(ebeam-elee)**2)
          plong=vdotm(qiu4,v4,3)/q3
        else
          q3=sqrt(gq2+(ebeam-gelee)**2)
          plong=vdotm(gqiu4,v4,3)/q3
        endif
          ptpt2=vdotm(v4,v4,3)-plong*plong
        if(ptpt2.gt.0) then
           ptrans=sqrt(ptpt2)
        else
           ptrans=-1
        endif
c
         return
         end
c
c
c
       real function cosr(v4p,v4ro)
       implicit none
       real z,ym,rhom,betta,vmass,vdotm,v4p(4),v4ro(4)
        ym=vdotm(v4ro,v4ro,3)
c
        if(ym.gt.0.0) then
          z=vdotm(v4p,v4ro,3)/ym
        else
          z=0
        endif
c
        rhom=vmass(v4ro)
c
         if (rhom.gt.0.28.and.z.gt.0) then
           betta=sqrt(1.0-0.0729/rhom/rhom)
           cosr=(2*z-1)/betta
         else
c           print *,'hoiloj!!!!!!',rhom
           cosr=-10.0
         endif
         return
         end
c
c
       real function eta(ip,v4)
       implicit none
#include "ntup22gen.inc"
#include "ntup22rec.inc"
#include "cloptions.inc"
c
        integer m,ip                  ! ip=0 data, ip>0 MC
        real v4(4),tnorm(4),h(4),q(4),pcm(4),qcm(4),hcm(4),xfain
        real mom_qcm,mom_hcm,dot,pl,pt
        real qiu2,ww,ecm,etx,vdotm,amp,cost,anu
        real ptpt2,plong
        amp=0.93827
c
       ETA=1.E6
***Construct the four-momentum of the cm-frame:
       do m=1,4
        h(m)=v4(m)
        q(m)=gqiu4(m)
        pcm(m) = q(m)
       enddo
       pcm(4) = pcm(4) + amp
***Lorenz transform to cm-frame:
       call loren4(pcm,q,qcm,1)
       call loren4(pcm,h,hcm,1)

***   Construct P_parallel in cm:
       PL=0.0

       mom_qcm = sqrt(qcm(1)**2+qcm(2)**2+qcm(3)**2)
       mom_hcm = sqrt(hcm(1)**2+hcm(2)**2+hcm(3)**2)
       dot = qcm(1)*hcm(1) + qcm(2)*hcm(2) + qcm(3)*hcm(3)
       if (mom_qcm.ne.0.and.mom_hcm.ne.0) 
     &          cost = dot/(mom_qcm*mom_hcm)
       PL=mom_hcm*cost
       pt=sqrt(mom_hcm*mom_hcm-PL*PL)
c
         qiu2=vdotm(qiu4,qiu4,4)
         anu=cl_beam_energy-gelee
         ww=amp*amp+2.0*amp*anu-qiu2
c
       ETX=2.0*PL/sqrt(ww)
c      xfain=acos(cost)
       ecm=sqrt(mom_hcm*mom_hcm+0.0196)
       etx=(ecm+pl)/(ecm-pl)

c
         return
         end

