function [Filt]=Tubular_dGauss3D_GPU(Th1,Th2,sig,offset)

Nsig=length(sig);
NTh1=length(Th1);
NTh2=length(Th2);
kF=1;
offset=sort(offset,'descend'); %% Sort so that x-axis is the largest one



for ksig=1:Nsig
    
    sigma=[sig(ksig)*offset(1),sig(ksig)*offset(2),sig(ksig)*offset(3)];
    %%% Tubular Mask
    [Mask]=TubularMask(sigma);
    
    for kTh1=1:NTh1
        for kTh2=1:NTh2
            
            [~,MexHat2,~]=Anisotropic_dGauss3D(sigma,[Th1(kTh1) Th2(kTh2)],0,0,0);
            Fx=MexHat2.xx;
            Fy=MexHat2.yy;
            Fz=MexHat2.zz;
            Fx=Fx./sqrt(sum(Fx(:).^2));
            Fy=Fy./sqrt(sum(Fy(:).^2));
            Fz=Fz./sqrt(sum(Fz(:).^2));
            
            %Laplacian
            Filt{kF}=(Fx+Fy+Fz)./sqrt(sum((Fx(:)+Fy(:)+Fz(:)).^2));
            
            % Mask rotation
            MaskR=MaskRotation(Mask,sigma,[Th1(kTh1) Th2(kTh2)]);
            % Tubular masking
            Filt{kF}=Filt{kF}.*MaskR;
       %     Filt{kF}=(Filt{kF}>0)-(Filt{kF}<0);
            % Zero Area Correction
            AFilt=sum(Filt{kF}(:));
            AMask=sum(MaskR(:));
            MaskR=MaskR/AMask*AFilt;
            Filt{kF}=Filt{kF}-MaskR;
            % L2 normalization
            Filt{kF}=Filt{kF}./sqrt(sum(Filt{kF}(:).^2));
            
            % Transfer to GPU
            Filt{kF}=gpuArray(Filt{kF});
            
            kF=kF+1;
            
        end
        
    end
end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% TUBULAR MASKING FUNCTIONS
function [Mask]=TubularMask(sigma)



[~,MexHat2,~]=Anisotropic_dGauss3D(sigma,[0 0],0,0,0);
Fx=MexHat2.xx;
Fy=MexHat2.yy;
Fz=MexHat2.zz;
Fx=Fx./sqrt(sum(Fx(:).^2));
Fy=Fy./sqrt(sum(Fy(:).^2));
Fz=Fz./sqrt(sum(Fz(:).^2));
%Laplacian
Mask=(Fx+Fy+Fz)./sqrt(sum((Fx(:)+Fy(:)+Fz(:)).^2));
Mask2D=Mask(:,:,round(end/2));
MaskN_Length=sum(Mask2D<0);
ind= find(MaskN_Length==max(MaskN_Length));
min_val=min(ind);
max_val=max(ind);
dim_m=2;

% ThMask=max(Mask(find(Mask<0)));
% ind=find(Mask<ThMask);
% [y,x,z]=ind2sub(size(Mask),ind);
% xMx=max(x(:));
% yMx=max(y(:));
% zMx=max(z(:));
% zm=min(z(:));
% ym=min(y(:));
% xm=min(x(:));
% [max_val,dim_Mx]=max([yMx,xMx,zMx]);
% [min_val,dim_m]=min([ym,xm,zm]);
szeMask=size(Mask);
ind=[ones(size(szeMask))' szeMask'];
ind(dim_m,:)=[min_val,max_val];
MaskZero=0*Mask;
MaskZero(ind(1,1):ind(1,2),ind(2,1):ind(2,2),ind(3,1):ind(3,2))=1;
Mask=abs(Mask)>0;
Mask=Mask.*MaskZero;

end

function [Mask]=MaskRotation(Mask,sigma,Th)


Th1=Th(1);
Th2=Th(2);

width=round(4*max(sigma));
tx = (-width:width);
ty=(-width:width);
tz=(-width:width);
[x,y,z]=meshgrid(tx,ty,tz);
y=-y;

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
Mask=interp3(x,y,z,Mask,Thx,Thy,Thz,'nearest',0);

end

