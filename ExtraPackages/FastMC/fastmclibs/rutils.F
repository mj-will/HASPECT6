c
       subroutine fillel()
       implicit none
#include "ntup22rec.inc"
#include "cloptions.inc"
c
        real vdotm,vmass2,anu,w2,amp
        parameter (amp=0.93827)

        v4el(1)=elee*sin(elet)*cos(elef)
        v4el(2)=elee*sin(elet)*sin(elef)
        v4el(3)=elee*cos(elet)
        v4el(4)=elee
c
         call vdifm(v4el0,v4el,qiu4,4)
         call vsumm(qiu4,v4pr0,v4s,4)    ! stored in common block
c
        q2=-vmass2(qiu4)
        xb=-q2/2.0/vdotm(qiu4,v4pr0,4)
        w2=vmass2(v4s)
c
         if(w2.gt.0) then
           w=sqrt(w2)
          else
           w=w2
         endif
c
         return
         end

c
c
       subroutine fillpip(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real ampi2
        parameter (ampi2=0.019488)
c
        pmom=sqrt(pipe*pipe-ampi2)
        v4pip(1)=pmom*sin(pipt)*cos(pipf)
        v4pip(2)=pmom*sin(pipt)*sin(pipf)
        v4pip(3)=pmom*cos(pipt)
        v4pip(4)=pipe
c
         pipm=mismas(0,v4pip)
         pipfs=phistar(0,v4pip)
         pippt=ptrans(0,ebeam,v4pip)
         piptt=thadron(0,v4pip)
         pipzi=pipe/qiu4(4)
c
         return
         end

c
       subroutine fillpim(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real ampi2
        parameter (ampi2=0.019488)
c
        pmom=sqrt(pime*pime-ampi2)
        v4pim(1)=pmom*sin(pimt)*cos(pimf)
        v4pim(2)=pmom*sin(pimt)*sin(pimf)
        v4pim(3)=pmom*cos(pimt)
        v4pim(4)=pime
c
         pimm=mismas(0,v4pim)
         pimfs=phistar(0,v4pim)
         pimpt=ptrans(0,ebeam,v4pim)
         pimtt=thadron(0,v4pim)
         pimzi=pime/qiu4(4)
c
         return
         end
c
c
       subroutine fillpro()
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom
        real mismas,phistar,ptrans,thadron
        real amp2
        parameter (amp2=0.88035)
c
        pmom=sqrt(proe*proe-amp2)
        v4pro(1)=pmom*sin(prot)*cos(prof)
        v4pro(2)=pmom*sin(prot)*sin(prof)
        v4pro(3)=pmom*cos(prot)
        v4pro(4)=proe
c
         prom=mismas(0,v4pro)
         prott=thadron(0,v4pro)
         profs=phistar(0,v4pro)
c
         return
         end

c
c
       subroutine fillkap(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real amka2
        parameter (amka2=0.24364)
c
        pmom=sqrt(kape*kape-amka2)
        v4kap(1)=pmom*sin(kapt)*cos(kapf)
        v4kap(2)=pmom*sin(kapt)*sin(kapf)
        v4kap(3)=pmom*cos(kapt)
        v4kap(4)=kape
c
         kapm=mismas(0,v4kap)
         kapfs=phistar(0,v4kap)
         kappt=ptrans(0,ebeam,v4kap)
         kaptt=thadron(0,v4kap)
         kapzi=kape/qiu4(4)
c
         return
         end
c
       subroutine fillkam(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real amka2
        parameter (amka2=0.24364)
c
        pmom=sqrt(kame*kame-amka2)
        v4kam(1)=pmom*sin(kamt)*cos(kamf)
        v4kam(2)=pmom*sin(kamt)*sin(kamf)
        v4kam(3)=pmom*cos(kamt)
        v4kam(4)=kame
c
         kamm=mismas(0,v4kam)
         kamfs=phistar(0,v4kam)
         kampt=ptrans(0,ebeam,v4kam)
         kamtt=thadron(0,v4kam)
         kamzi=kame/qiu4(4)
c
         return
         end
c
       subroutine fillneu()
       implicit none
#include "ntup22rec.inc"
c
        real anu,pmom
        real mismas,phistar,ptrans,thadron
        real amn2
        parameter (amn2=0.8828)
c
        pmom=sqrt(neue*neue-amn2)
        v4neu(1)=pmom*sin(neut)*cos(neuf)
        v4neu(2)=pmom*sin(neut)*sin(neuf)
        v4neu(3)=pmom*cos(neut)
        v4neu(4)=neue
c
         neum=mismas(0,v4neu)
         neutt=thadron(0,v4neu)
         neufs=phistar(0,v4neu)
c
         return
         end

c
       subroutine fillpropip()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4pro,v4pip,v4,4)
         mprpip=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillneupip()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4neu,v4pip,v4,4)
         mnepip=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillpropim()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4pro,v4pim,v4,4)
         mprpim=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillpropi0()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4pro,v4pi0,v4,4)
         mpropi0=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillneupi0()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4neu,v4pi0,v4,4)
         mneupi0=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillproroz()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4pro,v4roz,v4,4)
         mprroz=mismas(0,v4)
c
         return
         end
c
       subroutine fillprorop()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4pro,v4rop,v4,4)
         mprrop=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillneurop()
       implicit none
#include "ntup22rec.inc"
c
        real v4(4)
        real mismas,phistar,ptrans,thadron
c
         call vsumm(v4neu,v4rop,v4,4)
         mnerop=mismas(0,v4)
c
         return
         end
c
c
       subroutine fillrop(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real ebeam,vmass,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron,cosr,phirho
c
         nrop=nrop+1
         call vsumm(v4pip,v4pi0,v4rop,4)
         rope=v4rop(4)
         ropf=Azimuthalphi(v4rop(1),v4rop(2))
         ropt=PolarTheta(v4rop(1),v4rop(2),v4rop(3))
c
         ropm=mismas(0,v4rop)
         mrop=vmass(v4rop)
c
         ropfs=phistar(0,v4rop)
         roppt=ptrans(0,ebeam,v4rop)
         roptt=thadron(0,v4rop)
         ropzi=v4rop(4)/qiu4(4)
         trop=cosr(v4pip,v4rop)
         frop=phirho(0,v4rop,v4pip)
c
         return
         end
c
c
       subroutine fillroz(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real ebeam,vmass,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron,cosr,phirho
c
         nroz=nroz+1
         call vsumm(v4pip,v4pim,v4roz,4)
         roze=v4roz(4)
         rozf=Azimuthalphi(v4roz(1),v4roz(2))
         rozt=PolarTheta(v4roz(1),v4roz(2),v4roz(3))
c
         rozm=mismas(0,v4roz)
         mroz=vmass(v4roz)
c
         rozm=mismas(0,v4roz)
         rozfs=phistar(0,v4roz)
         rozpt=ptrans(0,ebeam,v4roz)
         roztt=thadron(0,v4roz)
         rozzi=v4roz(4)/qiu4(4)
         troz=cosr(v4pip,v4roz)
         froz=phirho(0,v4roz,v4pip)
c
         return
         end
c
c
       subroutine fillrozc(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real ebeam,vmass,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron,cosr,phirho
        real v4pimc(4),v4f(4)
        real mpi2
        parameter (mpi2=0.019488)
c
         nroz=nroz+1
         call vsumm(v4pip,v4pro,v4f,4)
         call vdifm(v4s,v4f,v4pimc,4)
         v4pimc(4)=sqrt(v4pimc(1)**2+v4pimc(2)**2+v4pimc(3)**2+mpi2)
c
         call vsumm(v4pip,v4pimc,v4roz,4)
         roze=v4roz(4)
         rozf=Azimuthalphi(v4roz(1),v4roz(2))
         rozt=PolarTheta(v4roz(1),v4roz(2),v4roz(3))
c
         rozm=mismas(0,v4roz)
         mroz=vmass(v4roz)
c
         rozm=mismas(0,v4roz)
         rozfs=phistar(0,v4roz)
         rozpt=ptrans(0,ebeam,v4roz)
         roztt=thadron(0,v4roz)
         rozzi=v4roz(4)/qiu4(4)
         troz=cosr(v4pip,v4roz)
         froz=phirho(0,v4roz,v4pip)
c
         return
         end
c
c
       subroutine fillpi0(ebeam)
       implicit none
#include "ntup22rec.inc"
        real ebeam,vmass2,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron
c
          v4g1(1)=game*sin(gamt)*cos(gamf)
          v4g1(2)=game*sin(gamt)*sin(gamf)
          v4g1(3)=game*cos(gamt)
          v4g1(4)=game
c
          v4g2(1)=gam2e*sin(gam2t)*cos(gam2f)
          v4g2(2)=gam2e*sin(gam2t)*sin(gam2f)
          v4g2(3)=gam2e*cos(gam2t)
          v4g2(4)=gam2e
c
          call vsumm(v4g1,v4g2,v4pi0,4)
          pi0e=v4pi0(4)
          pi0f=Azimuthalphi(v4pi0(1),v4pi0(2))
          pi0t=PolarTheta(v4pi0(1),v4pi0(2),v4pi0(3))
          pim =vmass2(v4pi0)
          pi0m=mismas(0,v4pi0)
          pi0tt=thadron(0,v4pi0)
          pi0fs=phistar(0,v4pi0)
          pi0pt=ptrans(0,ebeam,v4pi0)
          pi0zi=pi0e/qiu4(4)
        return
        end
c
       subroutine fillneukap(ebeam)
       implicit none
#include "ntup22rec.inc"
c
        real ebeam,vmass,v4tmp(4)
c
         call vsumm(v4neu,v4kap,v4tmp,4)
         mnkap=vmass(v4tmp)
c
         return
         end
c
       subroutine fillgam()
       implicit none
#include "ntup22rec.inc"
        real vmass2,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron
c
          v4g1(1)=game*sin(gamt)*cos(gamf)
          v4g1(2)=game*sin(gamt)*sin(gamf)
          v4g1(3)=game*cos(gamt)
          v4g1(4)=game
c
          gamm=mismas(0,v4g1)
          gamtt=thadron(0,v4g1)
          gamfs=phistar(0,v4g1)
        return
        end
c
       subroutine fillgampro()
       implicit none
#include "ntup22rec.inc"
           real gal,pim2,phistarg,phistarc
c
           gaml=gal(1)
c           pi2g=pim2()
c
       return
       end
c
       real function gal(ip)
       implicit none
#include "ntup22rec.inc"
c
c calculate angle between photons
c
       integer ip
       real ang
c
       real tgamm(4)
c
         call vdifm(v4s,v4pro,tgamm,4)
        if(ip.eq.1) then
          gal=ang(v4g1,tgamm)
        else
          gal=ang(v4pi0,tgamm)
        endif
         return
         end
