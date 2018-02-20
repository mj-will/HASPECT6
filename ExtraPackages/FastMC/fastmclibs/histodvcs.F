C--------------------------------------- 
      subroutine INIT_HISTDVCS
C--------------------------------------- 
      implicit none
#include "clhistdvcs.inc"
c
         call VZERO(semigentn,870912)
         call VZERO(semigentp,870912)
         call VZERO(semigenin,252)
         call VZERO(semigenip,252)
         call VZERO(semihistn,870912)
         call VZERO(semihistp,870912)
         call VZERO(semihisin,252)
         call VZERO(semihisip,252)
      return
      end
C--------------------------------------- 
      subroutine write_dvcs_file()
C--------------------------------------- 
      implicit none
#include "clhistdvcs.inc"
c
       integer i,j,k,ii,jj,iii,jjj
       real sp0,sn0,spp,snp,spm,snm
       return
c
c    generated sample
c
         OPEN(unit=25,file='generated.sidis',status='unknown')
c
c inclusive
c
      write(25,115) (((semigenip(i,j,jjj),i=1,nqq),j=1,nxx),jjj=1,2)
      write(25,115) (((semigenin(i,j,jjj),i=1,nqq),j=1,nxx),jjj=1,2)
c
c semi-inclusive
c
      print *,'now sidis part'
c
      write(25,114) (((((((semigentp(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
      write(6,114) (((((((semigentp(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
      write(25,114) (((((((semigentn(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
      write(6,114) (((((((semigentn(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
 104  format(1x,10i6)
 114  format(1x,12i7)
 115  format(1x,12i10)
         CLOSE(25) ! close the data file
c
c
         OPEN(unit=26,file='accepted.sidis',status='unknown')
c
      write(26,115) (((semihisip(i,j,jjj),i=1,nqq),j=1,nxx),jjj=1,2)
      write(26,115) (((semihisin(i,j,jjj),i=1,nqq),j=1,nxx),jjj=1,2)
c
      write(26,114) (((((((semihistp(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
      write(26,114) (((((((semihistn(i,j,k,ii,jj,iii,jjj),jj=1,nphi),i=1,nqq),
     >  j=1,nxx),k=1,nzz),ii=1,npt),iii=1,3),jjj=1,2)
         CLOSE(26) ! close the data file
c
c
      print *,'************ write histo ********************'
      print *,nxx,nqq,nzz,npt,nphi,xmin,dxx
      print *,'*********************************************'
c
      do j=1,nxx
       sp0=0
       sn0=0
       spp=0
       snp=0
       spm=0
       snm=0
       do i=3,nqq
        do k=1,nzz
         do ii=1,npt
          do jj=1,nphi
           do jjj=1,2
       sp0=sp0+semihistp(i,j,k,ii,jj,3,jjj)
       sn0=sn0+semihistn(i,j,k,ii,jj,3,jjj)
       spp=spp+semihistp(i,j,k,ii,jj,1,jjj)
       snp=snp+semihistn(i,j,k,ii,jj,1,jjj)
       spm=spm+semihistp(i,j,k,ii,jj,2,jjj)
       snm=snm+semihistn(i,j,k,ii,jj,2,jjj)
           enddo            
          enddo            
         enddo            
        enddo            
       enddo
       print *,'pions-',j,xmin+dxx*(2*j-1)/2,'*'
     6,sn0/sp0,snp/spp
     6 ,snm/spm,spm/spp,snm/snp,'**',spp,snp,sp0,sn0,snp,snm
      enddo           

       return
       end
c
C--------------------------------------- 
      subroutine fill_dvcs()
C--------------------------------------- 
      implicit none
#include "ntup22rec.inc"
#include "ntup22gen.inc"
#include "clhistdvcs.inc"
#include "cloptions.inc"
c
       integer i,j,k0,kp,km,jh,inu,iopt
       integer ip,i0,im,fp,f0,fm,jjj
       real yel,mxmin,ymax
c
       return
c
       inu=ghpar
       mxmin=cl_mx   ! Mx^2>1.1
       ymax=0.85
           ip=0
           kp=0
           fp=0
           im=0
           km=0
           fm=0
           i0=0
           k0=0
           f0=0
c
           j=0
           i=0                 ! q-bin
c
        if(gproh.eq.1) then 
         jh=1
        else
         jh=2       ! also unpol
        endif
c
        yel=1.0-gelee/cl_beam_energy
        if(yel.lt.ymax) then       ! only those events
        
        
c
c        fill generated
c
c
c
c q-bin
c     
            do i=1,nqq
             if (qval(i).lt.gq2.and.qval(i+1).ge.gq2) goto 1
            enddo
 1         continue
c
c x-bin
c     
           j=int((gxb-xmin)/dxx)+1
c
           if(i.gt.0.and.i.le.nqq.and.j.gt.0.and.j.le.nxx) then
c
           if(inu.eq.1) then
            semigenip(i,j,jh)=semigenip(i,j,jh)+1 
           else
            semigenin(i,j,jh)=semigenin(i,j,jh)+1 
           endif
c
c   fill hadrons
c
c pi0
c
           if(gdnpi0.gt.0) k0=
     6    int((gdpi0e/(cl_beam_energy-gelee)-zmin)/dzz)+1
           if(k0.gt.0.and.k0.le.nzz.and.gpi0m.gt.mxmin) then           !(2)
             i0=int(gpi0pt/dpt)+1
             if(i0.gt.npt) i0=npt
             f0=int(gpi0fs/dphi)+1
       if(i0.gt.0.and.i0.le.npt.and.f0.gt.0.and.f0.le.nphi) then ! (3)
       if(inu.eq.1) then
       semigentp(i,j,k0,i0,f0,3,jh)=semigentp(i,j,k0,i0,f0,3,jh)+1 
       else
       semigentn(i,j,k0,i0,f0,3,jh)=semigentn(i,j,k0,i0,f0,3,jh)+1 
       endif
             endif                                               ! (3)
           endif                                             !(2)
c
c
c pi+
c
           if(gnpip.gt.0) kp=int((gpipzi-zmin)/dzz)+1
           if(kp.gt.0.and.kp.le.nzz.and.gpipm.gt.mxmin) then
             ip=int(gpippt/dpt)+1
             if(ip.gt.npt) ip=npt
             fp=int(gpipfs/dphi)+1
       if(ip.gt.0.and.ip.le.npt.and.fp.gt.0.and.fp.le.nphi) then
       if(inu.eq.1) then
       semigentp(i,j,kp,ip,fp,1,jh)=semigentp(i,j,kp,ip,fp,1,jh)+1
       else
       semigentn(i,j,kp,ip,fp,1,jh)=semigentn(i,j,kp,ip,fp,1,jh)+1
       endif
             endif
           endif
c
c pi-
           if(gnpim.gt.0) km=int((gpimzi-zmin)/dzz)+1
           if(km.gt.0.and.km.le.nzz.and.gpimm.gt.mxmin) then
             im=int(gpimpt/dpt)+1
             if(im.gt.npt) im=npt
             fm=int(gpimfs/dphi)+1
         if(im.gt.0.and.im.le.npt.and.fm.gt.0.and.fm.le.nphi) then
        if(inu.eq.1) then
       semigentp(i,j,km,im,fm,2,jh)=semigentp(i,j,km,im,fm,2,jh)+1
       else
       semigentn(i,j,km,im,fm,2,jh)=semigentn(i,j,km,im,fm,2,jh)+1
        endif
         endif
           endif
         endif        ! good x,Q^2
       endif          ! y<ymax
c
c
c
c fill if reconstructed e    (1)
c
          if(nele.eq.1) then
           ip=0
           kp=0
           fp=0
           im=0
           km=0
           fm=0
           i0=0
           k0=0
           f0=0
c
           j=0
           i=0                 
c
        yel=1.0-elee/cl_beam_energy
        if(yel.gt.ymax) return
c
c q-bin
c     
            do i=1,nqq
             if (qval(i).lt.q2.and.qval(i+1).ge.q2) goto 11
            enddo
 11         continue
c
c x-bin
c     
           j=int((xb-xmin)/dxx)+1
c
           if(i.le.0.or.i.gt.nqq.or.j.le.0.or.j.gt.nxx) return
c
           if(inu.eq.1) then
            semihisip(i,j,jh)=semihisip(i,j,jh)+1 
           else
            semihisin(i,j,jh)=semihisin(i,j,jh)+1 
           endif
c
c
c
c pi0
c
           if(ngam.gt.1) k0=
     6    int(((game+gam2e)/(cl_beam_energy-gelee)-zmin)/dzz)+1
           if(k0.gt.0.and.k0.le.nzz.and.pi0m.gt.mxmin) then                     !(2)
             i0=int(pi0pt/dpt)+1
             if(i0.gt.npt) i0=npt
             f0=int(pi0fs/dphi)+1
       if(i0.gt.0.and.i0.le.npt.and.f0.gt.0.and.f0.le.nphi) then ! (3)
c       semisum(i,j,k0,3,inu)=semisum(i,j,k0,3,inu)+1
       if(inu.eq.1) then
       semihistp(i,j,k0,i0,f0,3,jh)=semihistp(i,j,k0,i0,f0,3,jh)+1 
       else
       semihistn(i,j,k0,i0,f0,3,jh)=semihistn(i,j,k0,i0,f0,3,jh)+1 
       endif
             endif                                               ! (3)
           endif                                             !(2)
c
c pip
           if(npip.gt.0) kp=int((pipzi-zmin)/dzz)+1
c
           if(kp.gt.0.and.kp.le.nzz.and.pipm.gt.mxmin) then
             ip=int(pippt/dpt)+1
             if(ip.gt.npt) ip=npt
             fp=int(pipfs/dphi)+1
       if(ip.gt.0.and.ip.le.npt.and.fp.gt.0.and.fp.le.nphi) then
c       semisum(i,j,kp,1,inu)=semisum(i,j,kp,1,inu)+1
       if(inu.eq.1) then
       semihistp(i,j,kp,ip,fp,1,jh)=semihistp(i,j,kp,ip,fp,1,jh)+1
       else
       semihistn(i,j,kp,ip,fp,1,jh)=semihistn(i,j,kp,ip,fp,1,jh)+1
       endif
             endif
           endif
c
c pi-
           if(npim.gt.0) km=int((pimzi-zmin)/dzz)+1
c
           if(km.gt.0.and.km.le.nzz.and.pimm.gt.mxmin) then
             im=int(pimpt/dpt)+1
             if(im.gt.npt) im=npt
             fm=int(pimfs/dphi)+1
       if(im.gt.0.and.im.le.npt.and.fm.gt.0.and.fm.le.nphi) then
c       semisum(i,j,km,2,inu)=semisum(i,j,km,2,inu)+1
       if(inu.eq.1) then
       semihistp(i,j,km,im,fm,2,jh)=semihistp(i,j,km,im,fm,2,jh)+1
       else
       semihistn(i,j,km,im,fm,2,jh)=semihistn(i,j,km,im,fm,2,jh)+1
       endif
             endif
           endif
       endif         ! nele=1
       return
       end
c
