%==========================================================================
% FUNCTION NAME :
% [dg,ddg,g]=Anisotropic_dGauss3D(sigma,Th,x0,y0,z0,size);
%--------------------------------------------------------------------------
% DESCRIPTION :3 DIMENSIONAL ANISOTROPIC GAUSSIAN ORIENTED DERIVATIVES
% Computation of the directional derivative of an anisotropic gaussian
% kernel given by sigma oriented in the direction 'Th' (in radians) centered at x0,y0,z0:
% 
% g= 1/((2 \pi)^(2/3) sigma) exp^(Thx^2/(2*sigma(1)^2)+Thy^2/(2*sigma(2)^2)+Thz^2/(2*sigma(3)^2))
%
% for Thx,Thy,Thz the axis rotated to transform x-axis (Major Axis) into the unitary
% vector given Th and centered at (x0,y0,z0).
%--------------------------------------------------------------------------
% IMPORTANT: NO NORMALIZATION (I.E. KERNEL PROBABILITY EQUAL 1)
% IS CARRIED OUT. FOR 'SIGMAS' LESS THAN 1, SUM OF COEFFICIENTS
% IS NOT ONE. THAT IS, THE RIEMMAN APPROXIMATION TO THE INTEGRAL
% IS NOT GOOD ENOUGH.
%--------------------------------------------------------------------------
% INPUTS :  
%    sigma: 3D scale of gaussian kernel
%    Th: 2D vector of the orientation of kernel z-axis 
%    x0,y0,z0=origin of gaussian
%    size [optional]= size of gaussian kernel.If no 'size' is given, 
%        the size of the kernels is the interval containing 99% of the probability. 
%--------------------------------------------------------------------------
% OUTPUTS :
%    
%  1> dg: first derivative dg.x,dg.y,dg.z
%  2> dgg: second derivative dgg.x,dgg.y,dgg.z
%  3> g: gaussian kernel
%--------------------------------------------------------------------------
% EXTERNAL FUNCTIONS : 
%--------------------------------------------------------------------------
% RELATED BIBLIOGRAPHY : 
%--------------------------------------------------------------------------
% CREATION DATE : 02/04/2013
%--------------------------------------------------------------------------
% LAST MODIFICATION : Debora 21/11/2013
% Bug detected in the computation of the gaussian kernel: the z-axis is not
% rotated to the unitary vector given by Th. It is the x-axis!
%--------------------------------------------------------------------------
% AUTHOR : debora, 02/04/2013
%--------------------------------------------------------------------------
% USAGE EXAMPLES : Default kernel size
% 
%  [~,MexHat2,~]=Anisotropic_dGauss3D([sig*offset(1),sig*offset(2),sig*offs
%  et(3)],[Th Th2],0,0,0); 
%==========================================================================

function [dg,ddg,g]=Anisotropic_dGauss3D(sigma,Th,x0,y0,z0,varargin)

sig1=sigma(1);
sig2=sigma(2);
sig3=sigma(3);
Th1=Th(1);
Th2=Th(2);
% w1=cos(Th1);
% w2=sin(Th1);
% v1=cos(Th2);
% v2=sin(Th2);
ssq1=sig1*sig1;
ssq2=sig2*sig2;
ssq3=sig3*sig3;

%gaussian range
if(nargin<6)
    %gaussian range
    width=round(4*max(sigma));
    tx = (-width:width);
    ty=(-width:width);
    tz=(-width:width);
else
    size=varargin{1};
    width=floor(size*0.5);
    tx = (-width(2):width(2));
    ty=(-width(1):width(1));
    tz=(-width(3):width(3));
end

[x,y,z]=meshgrid(tx,ty,tz);
y=-y;
x=x-x0;
y=y-y0;
z=z-z0;

%Rotation
m1 = rotationmat3D(Th1,[0 0 1]); % rotation matrix of x axis around z axis
newyaxis = m1*[0 1 0]';
newxaxis = m1*[1 0 0]';
m2 = rotationmat3D(-Th2,newyaxis);
rm = m2*m1; % rotation matrix;
rm=rm';
Thx=rm(1,1)*x+rm(1,2)*y+rm(1,3)*z;
Thy=rm(2,1)*x+rm(2,2)*y+rm(2,3)*z;
Thz=rm(3,1)*x+rm(3,2)*y+rm(3,3)*z;
upvector = rm*[1,0,0]';

%Mascara per evitar suports massa grans en el cas anisotropic
width1=3*sigma(1);
width2=3*sigma(2);
width3=3*sigma(3);
Mask=((x.^2./width1.^2)+(y.^2./width2.^2)+(z.^2./width3.^2))<1;
%Mask=Mask.*((z.^2)<=width3.^2);
Mask=interp3(x,y,z,Mask,Thx,Thy,Thz,'nearest',0);
%Mask=imrotate(Mask,Th*180/pi,'crop'); % NO 3D Allowed!!!!!!

%gaussian
E=exp(-(Thx.*Thx/ssq1 + Thy.*Thy/ssq2 + Thz.*Thz/ssq3)/2)/ (2*pi*sig1*sig2*sig3);
E=E.*Mask;
Ener=sum(E(:));
g=E/Ener;
%first derivative of an anisotropic gaussian
dg.y =-Thy.*g/ssq2; %Derivative perpendicular to Th direction
dg.x =-Thx.*g/ssq1;
dg.z =-Thz.*g/ssq3;
%second derivatives
ddg.yy=-(g+Thy.*dg.y)/ssq2; %Derivative perpendicular to Th direction
ddg.xx=-(g+Thx.*dg.x)/ssq1;
ddg.zz=-(g+Thz.*dg.z)/ssq3;
ddg.xy=dg.y.*Thx/ssq1;
ddg.yz=dg.y.*Thz/ssq3;
ddg.xz=dg.x.*Thz/ssq3;


