function [MainBronchiEner,MainBronchiWallEner]=AirwayMultiResolutionEners(imaVOLVessEner,Masks,MainBronchiParams)

%% Input Parameters

LungMaskROI=Masks.LungMaskROI;
VesselMask=Masks.VesselMask;
TracheaMaskROI=Masks.TracheaMaskROI;
RedParams=MainBronchiParams.RedParams;
FParams=MainBronchiParams.FParam;
FParams.GPUUSe=MainBronchiParams.GPUUSe;
ratio=RedParams.ratio;
NLevels=RedParams.NLevels;

%% MaxPool Reduction for Recovery of Main Bronchi
imaVOLEnerRed=imaVOLVessEner;
VMaskRed=VesselMask;
TMaskRed=TracheaMaskROI;
LungMaskRed=LungMaskROI;

for k=1:NLevels
    
    
    [ imaVOLEnerRed ] = MaxPool3D( -imaVOLEnerRed,ratio );
    imaVOLEnerRed=-imaVOLEnerRed;
    [VMaskRed ]=MaxPool3D(VMaskRed,ratio);
    [TMaskRed ]=MaxPool3D(TMaskRed,ratio);
    [ LungMaskRed ] = MaxPool3D( LungMaskRed,ratio );
    
    [ MainBronchiEner{k}, MainBronchiWallEner{k}] = AirwayEners(-imaVOLEnerRed,LungMaskRed,VMaskRed,TMaskRed,FParams ); 
end



