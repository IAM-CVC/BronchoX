function [ AirwayEner,AirwayWallEner] = AirwayEners( imaVOLROI,LungMaskROI,VesselSeg,Seg0,FParam )

GPUUse=FParam.GPUUSe;

%%% Mask Vessels
[ VOLnoVessel ] = VesselSuppression( imaVOLROI,LungMaskROI,VesselSeg,Seg0 );
clear('imaVOLROI');
clear('Seg0');


switch GPUUse
    case 'Full'
        AirwayEner=0*VOLnoVessel;
        AirwayWallEner=0*VOLnoVessel;
        
        [Tube_DNRGaussBronchiV]=TubeDetector3D_GPU(FParam,gpuArray(single(VOLnoVessel)));
        %%%%% Transfer back to PC-RAM
        Tube_DNRGaussBronchiV.MxDNRimN=gather(Tube_DNRGaussBronchiV.MxDNRimN);
        Tube_DNRGaussBronchiV.MxDNRimP=gather(Tube_DNRGaussBronchiV.MxDNRimP);
        AirwayEner=Tube_DNRGaussBronchiV.MxDNRimP.*LungMaskROI.*(1-VesselSeg);
        AirwayWallEner=-Tube_DNRGaussBronchiV.MxDNRimN.*LungMaskROI.*(1-VesselSeg);
        
    case 'Half'
        
        indL=round(size(VOLnoVessel,2)/2);
        indR=indL+1;
        AirwayEner=0*VOLnoVessel;
        AirwayWallEner=0*VOLnoVessel;
        
        
        [Tube_DNRGaussBronchiV]=TubeDetector3D_GPU(FParam,gpuArray(single(VOLnoVessel(:,1:indL+20,:))));
        AirwayEner(:,1:indL,:)=gather(Tube_DNRGaussBronchiV.MxDNRimP(:,1:indL,:)).*LungMaskROI(:,1:indL,:).*(1-VesselSeg(:,1:indL,:));
        AirwayWallEner(:,1:indL,:)=-gather(Tube_DNRGaussBronchiV.MxDNRimN(:,1:indL,:)).*LungMaskROI(:,1:indL,:).*(1-VesselSeg(:,1:indL,:));
        
        [Tube_DNRGaussBronchiV]=TubeDetector3D_GPU(FParam,gpuArray(single(VOLnoVessel(:,indR-20:end,:))));
        AirwayEner(:,indR:end,:)=gather(Tube_DNRGaussBronchiV.MxDNRimP(:,21:end,:)).*LungMaskROI(:,indR:end,:).*(1-VesselSeg(:,indR:end,:));
        AirwayWallEner(:,indR:end,:)=-gather(Tube_DNRGaussBronchiV.MxDNRimN(:,21:end,:)).*LungMaskROI(:,indR:end,:).*(1-VesselSeg(:,indR:end,:));
        
end


end

