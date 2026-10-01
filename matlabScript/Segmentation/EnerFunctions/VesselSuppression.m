function [ NewVOL ] = VesselSuppression( imaVOLROI,LungMaskROI,VesselSeg,BronchiSeg )


medV=mean(imaVOLROI(find(BronchiSeg)));
medL=mean(imaVOLROI(find(LungMaskROI)));
% 
% [FX,FY,FZ] = gradient(LungMaskROI);
% EnerGrad=FX.^2+FY.^2+FZ.^2;
% LungBoundary=EnerGrad>0;
% medL=mean(imaVOLROI(find(LungBoundary)));

NewVOL=((imaVOLROI).*(1-VesselSeg)+medV*VesselSeg).*LungMaskROI+(1-LungMaskROI)*medL;


end

