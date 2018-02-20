c
       subroutine gfillel()
       implicit none
#include "ntup22gen.inc"
#include "cloptions.inc"
c
        real anu,vdotm,vmass2,w2,amp
        parameter (amp=0.93827)
c
        g4el(1)=gelee*sin(gelet)*cos(gelef)
        g4el(2)=gelee*sin(gelet)*sin(gelef)
        g4el(3)=gelee*cos(gelet)
        g4el(4)=gelee
c
         call vdifm(v4el0,g4el,gqiu4,4)
         call vsumm(gqiu4,v4pr0,g4s,4)   ! stored in common block
         gq2=-vmass2(gqiu4)
         gxb=-gq2/2.0/vdotm(gqiu4,v4pr0,4)
         w2=vmass2(g4s)
c
         if(w2.gt.0) then
           gw=sqrt(w2)
          else
           gw=w2
         endif
         return
         end

c
c
       subroutine gfillpip(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom
        real ebeam,mismas,phistar,ptrans,thadron
        real ampi2
        parameter (ampi2=0.019488)
c
        pmom=sqrt(gpipe*gpipe-ampi2)
        g4pip(1)=pmom*sin(gpipt)*cos(gpipf)
        g4pip(2)=pmom*sin(gpipt)*sin(gpipf)
        g4pip(3)=pmom*cos(gpipt)
        g4pip(4)=gpipe
c
         gpipm=mismas(1,g4pip)
         gpipfs=phistar(1,g4pip)
         gpippt=ptrans(1,ebeam,g4pip)
         gpiptt=thadron(1,g4pip)
         gpipzi=gpipe/gqiu4(4)
c
         return
         end

c
       subroutine gfillpim(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real ampi2
        parameter (ampi2=0.019488)
c
        pmom=sqrt(gpime*gpime-ampi2)
        g4pim(1)=pmom*sin(gpimt)*cos(gpimf)
        g4pim(2)=pmom*sin(gpimt)*sin(gpimf)
        g4pim(3)=pmom*cos(gpimt)
        g4pim(4)=gpime
c
         gpimm=mismas(1,g4pim)
         gpimfs=phistar(1,g4pim)
         gpimpt=ptrans(1,ebeam,g4pim)
         gpimtt=thadron(1,g4pim)
         gpimzi=gpime/gqiu4(4)
c
         return
         end
c
c
       subroutine gfillpi0(ebeam)
       implicit none
#include "ntup22gen.inc"
        real vmass2,Azimuthalphi,PolarTheta
        real ebeam,mismas,phistar,ptrans,thadron
c
          g4g1(1)=ggame*sin(ggamt)*cos(ggamf)
          g4g1(2)=ggame*sin(ggamt)*sin(ggamf)
          g4g1(3)=ggame*cos(ggamt)
          g4g1(4)=ggame
c
          g4g2(1)=ggam2e*sin(ggam2t)*cos(ggam2f)
          g4g2(2)=ggam2e*sin(ggam2t)*sin(ggam2f)
          g4g2(3)=ggam2e*cos(ggam2t)
          g4g2(4)=ggam2e
c
          call vsumm(g4g1,g4g2,g4pi0,4)
          gpi0e=g4pi0(4)
          gpi0f=Azimuthalphi(g4pi0(1),g4pi0(2))
          gpi0t=PolarTheta(g4pi0(1),g4pi0(2),g4pi0(3))
          gpim =vmass2(g4pi0)
          gpi0m=mismas(1,g4pi0)
          gpi0tt=thadron(1,g4pi0)
          gpi0pt=ptrans(1,ebeam,g4pi0)
          gpi0fs=phistar(1,g4pi0)
          gpi0zi=gpi0e/gqiu4(4)
        return
        end
c
c
       subroutine gfillpro(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom,ebeam
        real mismas,phistar,ptrans,thadron
        real amp2
        parameter (amp2=0.88035)
c
        pmom=sqrt(gproe*gproe-amp2)
        g4pro(1)=pmom*sin(gprot)*cos(gprof)
        g4pro(2)=pmom*sin(gprot)*sin(gprof)
        g4pro(3)=pmom*cos(gprot)
        g4pro(4)=gproe
c
         gprom=mismas(1,g4pro)
         gprott=thadron(1,g4pro)
         gprofs=phistar(1,g4pro)
c
         return
         end

c
       subroutine gfillkap(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom
        real ebeam,mismas,phistar,ptrans,thadron
        real amka2
        parameter (amka2=0.24364)
c
        pmom=sqrt(gkape*gkape-amka2)
        g4kap(1)=pmom*sin(gkapt)*cos(gkapf)
        g4kap(2)=pmom*sin(gkapt)*sin(gkapf)
        g4kap(3)=pmom*cos(gkapt)
        g4kap(4)=gkape
c
         gkapm=mismas(1,g4kap)
         gkapfs=phistar(1,g4kap)
         gkappt=ptrans(1,ebeam,g4kap)
         gkaptt=thadron(1,g4kap)
         gkapzi=gkape/gqiu4(4)
c
         return
         end
c
       subroutine gfillkam(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom
        real ebeam,mismas,phistar,ptrans,thadron
        real amka2
        parameter (amka2=0.24364)
c
        pmom=sqrt(gkame*gkame-amka2)
        g4kam(1)=pmom*sin(gkamt)*cos(gkamf)
        g4kam(2)=pmom*sin(gkamt)*sin(gkamf)
        g4kam(3)=pmom*cos(gkamt)
        g4kam(4)=gkame
c
         gkamm=mismas(1,g4kam)
         gkamfs=phistar(1,g4kam)
         gkampt=ptrans(1,ebeam,g4kam)
         gkamtt=thadron(1,g4kam)
         gkamzi=gkame/gqiu4(4)
c
         return
         end
c
       subroutine gfillpropip(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4pro,g4pip,v4,4)
         gmprpip=mismas(1,v4)
c
         return
         end
c
c
       subroutine gfillpropim(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4pro,g4pim,v4,4)
         gmprpim=mismas(1,v4)
c
         return
         end
c
       subroutine gfillpropi0(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4pro,g4pi0,v4,4)
         gmpropi0=mismas(1,v4)
c
         return
         end
c
       subroutine gfillproroz(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4pro,g4roz,v4,4)
         gmprroz=mismas(1,v4)
c
         return
         end
c
       subroutine gfillprorop(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4pro,g4rop,v4,4)
         gmprrop=mismas(1,v4)
c
         return
         end
c
       subroutine gfillroz(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real vmass,Azimuthalphi,PolarTheta
        real ebeam,mismas,phistar,ptrans,thadron,cosr,phirho
c
         gnroz=gnroz+1
         call vsumm(g4pip,g4pim,g4roz,4)
         groze=g4roz(4)
         grozf=Azimuthalphi(g4roz(1),g4roz(2))
         grozt=PolarTheta(g4roz(1),g4roz(2),g4roz(3))
c
         grozm=mismas(1,g4roz)
         gmroz=vmass(g4roz)
c
         grozm=mismas(1,g4roz)
         grozfs=phistar(1,g4roz)
         grozpt=ptrans(1,ebeam,g4roz)
         groztt=thadron(1,g4roz)
         grozzi=g4roz(4)/gqiu4(4)
         gtroz=cosr(g4pip,g4roz)
         gfroz=phirho(1,g4roz,g4pip)
c
         return
         end
c
c
       subroutine gfillrop(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real vmass,Azimuthalphi,PolarTheta
        real ebeam,mismas,phistar,ptrans,thadron,cosr,phirho
c
         gnrop=gnrop+1
         call vsumm(g4pip,g4pi0,g4rop,4)
         grope=g4rop(4)
         gropf=Azimuthalphi(g4rop(1),g4rop(2))
         gropt=PolarTheta(g4rop(1),g4rop(2),g4rop(3))
c
         gropm=mismas(1,g4rop)
         gmrop=vmass(g4rop)
c
         gropfs=phistar(1,g4rop)
         groppt=ptrans(1,ebeam,g4rop)
         groptt=thadron(1,g4rop)
         gropzi=g4rop(4)/gqiu4(4)
         gtrop=cosr(g4pip,g4rop)
         gfrop=phirho(1,g4rop,g4pip)
c
         return
         end
c
c
       subroutine gfillneu(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real anu,pmom
        real ebeam,mismas,phistar,ptrans,thadron
        real amn2
        parameter (amn2=0.8828)
c
        pmom=sqrt(gneue*gneue-amn2)
        g4neu(1)=pmom*sin(gneut)*cos(gneuf)
        g4neu(2)=pmom*sin(gneut)*sin(gneuf)
        g4neu(3)=pmom*cos(gneut)
        g4neu(4)=gneue
c
         gneum=mismas(1,g4neu)
         gneutt=thadron(1,g4neu)
         gneufs=phistar(1,g4neu)
c
         return
         end

c
       subroutine gfillneupip(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4neu,g4pip,v4,4)
         gmnepip=mismas(1,v4)
c
         return
         end
c
c
       subroutine gfillneupi0(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4neu,g4pi0,v4,4)
         gmneupi0=mismas(1,v4)
c
         return
         end
c
       subroutine gfillneurop(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real v4(4)
        real ebeam,mismas,phistar,ptrans,thadron
c
         call vsumm(g4neu,g4rop,v4,4)
         gmnerop=mismas(1,v4)
c
         return
         end
c
c
c
       subroutine gfillneukap(ebeam)
       implicit none
#include "ntup22gen.inc"
c
        real ebeam,vmass,v4tmp(4)
c
         call vsumm(g4neu,g4kap,v4tmp,4)
         gmnkap=vmass(v4tmp)
c
         return
         end
c
c
c
       subroutine gfillgam()
       implicit none
#include "ntup22gen.inc"
        real vmass2,Azimuthalphi,PolarTheta
        real mismas,phistar,ptrans,thadron
c
          g4g1(1)=ggame*sin(ggamt)*cos(ggamf)
          g4g1(2)=ggame*sin(ggamt)*sin(ggamf)
          g4g1(3)=ggame*cos(ggamt)
          g4g1(4)=ggame
c
          ggamm=mismas(1,g4g1)
          ggamtt=thadron(1,g4g1)
          ggamfs=phistar(1,g4g1)
        return
        end
