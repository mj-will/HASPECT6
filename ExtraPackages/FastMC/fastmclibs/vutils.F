
c
       real function vmass2(a)
       implicit none
       real vm,vdotm,a(4)
c
        vm= vdotm(a,a,4)
        vmass2=-vm
       return
       end
 
c/HISTOGRAM/CREATE/PROFILE '103' ' ' '6' '0.2' '2' '-0.1' '0.1'
c        A set of vector utilities
c
c
c 1.    call crossm(a,b,c)
c       c=a x b for  3 vectors a,b
c
c 2.    real function vangle(a,b,c,d)
c      angle between 2 planes a x b  and c x d
c
c 3.    call vmult(a,r,n)
c      a=r*a n-dimension vectors a multiply with real number r
c
c 4.    call vsumm(a,b,c,n)
c      c=a-b n-dimension vectors a,b
c
c 5.   call vdifm(a,b,c,n)
c      c=a-b n-dimension vectors a,b
c
c 6.   real function vdotm(a,b,n)
c      return a.b for n a,b vectors of n -dimension
c
       real function ang(x,y)
       implicit none
       real x(4),y(4),xm,ym,vcos,vdotm
c
         xm=vdotm(x,x,3)
         ym=vdotm(y,y,3)
       if(xm.gt.0.0 .and. ym.gt.0.0) then
         vcos=vdotm(x,y,3)/sqrt(xm)/sqrt(ym)
       else
         vcos=1.0
       endif
         if(abs(vcos).lt.1.0) then
            ang=acos(vcos)
         else
c            print *,xm,ym,vcos
            ang=-1
         endif
       return
       end
c

       subroutine crossm(a,b,c)
       real a(4),b(4),c(4)
       c(1)=a(2)*b(3)-a(3)*b(2)
       c(2)=a(3)*b(1)-a(1)*b(3)
       c(3)=a(1)*b(2)-a(2)*b(1)
       return
       end
c
c
       subroutine vmult(a,r,n)
       implicit none
       real a(4),r
       integer i,n
        do i=1,n
          a(i)=r*a(i)
        enddo
       return
       end
c
c
       subroutine vsumm(a,b,c,n)
       implicit none
       real a(4),b(4),c(4)
       integer i,n
        do i=1,n
          c(i)=a(i)+b(i)
        enddo
       return
       end
c
       subroutine vdifm(a,b,c,n)
       implicit none
       real a(4),b(4),c(4)
       integer i,n
        do i=1,n
          c(i)=a(i)-b(i)
        enddo
       return
       end
c
c
       real function vdotm(a,b,n)
       implicit none
       real a(4),b(4),s
       integer i,n
       s=0.0
       do i=1,3
         s=s+a(i)*b(i)
       enddo
       if(n.eq.4) s=s-a(n)*b(n)
       vdotm=s
       return
       end
c
c
       real function vmass(a)
       implicit none
       real vm,a(4),vdotm
        vm= vdotm(a,a,4)
        if (vm.le.0.0) then
          vmass=sqrt(-vm)
        else
          vmass=-10.0
        endif 
       return
       end
c   
       real function vangle(a,b,c,d)
       implicit none
       real a(4),b(4),c(4),d(4),xm,ym,vcos
       real x(4),y(4),pi,vdotm
c
       pi=acos(-1.0)
       call crossm(a,b,x)
       call crossm(c,d,y)
       xm=vdotm(x,x,3)
       ym=vdotm(y,y,3)
       if(xm.gt.0.0 .and. ym.gt.0.0) then
         vcos=vdotm(x,y,3)/sqrt(xm)/sqrt(ym)
         if(abs(vcos).lt.1.0) then
            vangle=acos(vcos)
         else
            if(vcos.ge.1.0)  vangle=0
            if(vcos.le.-1.0)  vangle=pi
         endif 
       else
         vangle=0
       endif
       return
       end
