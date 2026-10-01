function [FiltR]=DescriptorsResponse3D_GPU(im,F,ConvType)

N=length(F);
im=single(im);


switch ConvType
    case 'Space'
        %Negative Responses
        FiltR.MxDNRimN=0*im;
        %Positive (dark) Responses
        FiltR.MxDNRimP=0*im;
        %%%%% Convolution in Space (Zero Padding)
        for k=1:N
            s=size(F{k});
            % Not supported in GPU for 3D
            % DNR{k}=imfilter(im,(F{k}),'replicate','same');
            DNR=convn(im,(F{k}),'same');
            FiltR.MxDNRimN=min(FiltR.MxDNRimN,DNR);
            FiltR.MxDNRimP=max(FiltR.MxDNRimP,DNR);
        end
        
        
        
    case 'Fourier'
        
        %%%%% Convolution in Fourier Domain (Periodic Padding)
        imF=fftn(im);
%         im=gather(im);
%         clear im;
        %Negative Responses
        FiltR.MxDNRimN=0*im;
        %Positive (dark) Responses
        FiltR.MxDNRimP=0*im;
        sK=round(size(imF)*0.5);
        for k=1:N
            
            s=round((size(F{k})-1)*0.5);
            Kernel=F{k};
            if(sK>size(F{k})*0.5)
                Kernel=0*imF;
                Kernel(sK(1)-s(1)+1:sK(1)+s(1)+1,sK(2)-s(2)+1:sK(2)+s(2)+1,sK(3)-s(3)+1:sK(3)+s(3)+1)=F{k};
            end
            Kernel=fftn(Kernel);
            DNR=real(fftshift(ifftn(Kernel.*imF)));
            
            FiltR.MxDNRimN=min(FiltR.MxDNRimN,DNR);
            FiltR.MxDNRimP=max(FiltR.MxDNRimP,DNR);
        end
        
        
end