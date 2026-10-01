function [Filt]=Tubular_Bola3D_GPU(Th1,Th2,sig,offset)

Nsig=length(sig);
NTh1=length(Th1);
NTh2=length(Th2);
kF=1;
offset=sort(offset,'descend'); %% Sort so that x-axis is the largest one



for ksig=1:Nsig
    
    sigma=[sig(ksig)*offset(1),sig(ksig)*offset(2),sig(ksig)*offset(3)];

    for kTh1=1:NTh1
        for kTh2=1:NTh2
            
            [~,~,Gaussian]=Anisotropic_dGauss3D(sigma,[Th1(kTh1) Th2(kTh2)],0,0,0);
            Filt{kF}=Gaussian./sqrt(sum(Gaussian(:).^2));
            
            % Transfer to GPU
            Filt{kF}=gpuArray(Filt{kF});
            
            kF=kF+1;
            
        end
        
    end
end
end


