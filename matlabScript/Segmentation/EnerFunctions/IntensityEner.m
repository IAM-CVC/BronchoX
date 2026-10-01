function [ EnerM ] = IntensityEner( imaVOL,FParam )


% N=1;
% hTh=pi/N;
% FParam.sig=[1];
% FParam.scale=1;
% FParam.Th1=[[0:1:N-1]*hTh];
% FParam.Th2=[[0:1:N-1]*hTh]';
% FParam.offset=[1,1,1];
% FParam.FiltType='DNR_Bola_GPU';


ConvType=FParam.ConvType;
EnerM=gpuArray(0*imaVOL);


switch ConvType
    case 'Fourier'
        %%% Partition to drop GPU RAM load
        indL=round(size(imaVOL,2)/2);
        indR=indL+1;
        
        [TubeROI_DNRGauss]=TubeDetector3D_GPU(FParam,gpuArray(imaVOL(:,1:indL+20,:)));
        EnerM(:,1:indL,:)=TubeROI_DNRGauss.MxDNRimN(:,1:indL,:);
        [TubeROI_DNRGauss]=TubeDetector3D_GPU(FParam,gpuArray(imaVOL(:,indR-20:end,:)));
        EnerM(:,indR:end,:)=TubeROI_DNRGauss.MxDNRimN(:,21:end,:);
        
    case 'Space'
        [TubeROI_DNRGauss]=TubeDetector3D_GPU(FParam,gpuArray(imaVOL));
        EnerM=TubeROI_DNRGauss.MxDNRimN;
end


EnerM=-EnerM;
EnerM=(EnerM-min(EnerM(:)))/(max(EnerM(:))-min(EnerM(:)));

EnerM=gather(EnerM);

end

