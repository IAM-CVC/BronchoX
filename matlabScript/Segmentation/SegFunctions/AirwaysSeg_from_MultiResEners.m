function [ AirwaySeg ] = AirwaysSeg_from_MultiResEners( Masks,Eners,MainBronchiParams )

SegParam=MainBronchiParams.SegParam;
RedParams=MainBronchiParams.RedParams;
ratio=RedParams.ratio;
NLevels=RedParams.NLevels;

MainBronchiEner=Eners.MainBronchiEner;
MainBronchiWallEner=Eners.MainBronchiWallEner;

% Masks
TracheaMaskROI=Masks.TracheaMaskROI;
LungMaskROI=Masks.LungMaskROI;
CurrentSeg=TracheaMaskROI;
%LungMaskRed{1}=LungMaskROI;
for k=1:NLevels
    szeVol{k}=size(CurrentSeg);
    [CurrentSeg ]=MaxPool3D(CurrentSeg,ratio);
    %  LungMaskRed{k}=MaxPool3D(LungMaskRed{k},ratio);
end

%Multilevel Segmentation
for k=NLevels:-1:1
    ThEner=MainBronchiEner{k}.*CurrentSeg;
    [ SegParam ] = SetThreshold( ThEner, SegParam);
    SegParam.LungMaskROI=ones(size(CurrentSeg));
    [ MainAirwaysRed ] = AirwaysSeg_from_Eners( CurrentSeg,MainBronchiEner{k},MainBronchiWallEner{k},SegParam );
    [ AirwaySeg] = MaxPool3DBack( MainAirwaysRed,ratio,szeVol{k} );
    CurrentSeg=AirwaySeg;
end

%Final Segmentation
AirwaySeg=max(AirwaySeg,TracheaMaskROI);
AirwaySeg=PrincipalConnComp(AirwaySeg.*LungMaskROI,6,1);
[AirwaySeg]=VolumeMaskClose(AirwaySeg,SegParam.d_se);
AirwaySeg(:,:,end)=0;
AirwaySeg=imfill(AirwaySeg,26,'holes');
AirwaySeg=max(TracheaMaskROI,AirwaySeg);
end

