%% 2. MAIN BRONCHI SEGMENTATION
%% 2.1 INITIAL SEGMENTATION USING MULTIRESOLUTION SCHEME
%% 
%% INPUT: LUNG STRUCTURES FROM PREVIOUS STEP: VesselSegHoles, TracheaMaskROI,LungMaskROIAll
%% 
%% PARAMETERS:
%%  MainBronchiEnerParams: Main Bronchi energy
%%  MainBronchiParams: Main Bronchi Segmentation
%%
%% OUTPUT VARIABLES:
%%  AirwayMultiResEner: Cell array containing energy maps at different resolutions for main bronchi segmentation. 
%%  A threshold on this energies segments airways
%%  AirwayWallMultiResEner: Cell array containing energy maps at different resolutions for main bronchi wall segmentation
%%  This is used as a post-filtering to remove structures and leakage attached to AirwayMultiResEner thresholding
%%  MainBronchiSeg: Segmentation of main airways obtained using a multiresolution approach
%% 
%% POSSIBLE ACCELERATION: 
%%  1. All energies are computed using convolutions. 
%%  I'm using GPU in a serial (loop) implementation
%%  2. I ignore whether a multiresolution scheme can be accelerated. 
%%  There are steps that I think could be computed for each level in parallel 
%%  (energy and thresholding, for instance)
% Multiresolution Energy
disp('Multiresolution Energy - start');
Masks.VesselMask=VesselSegHoles;
Masks.TracheaMaskROI=TracheaMaskROI;
Masks.LungMaskROI=LungMaskROIAll;
%MainBronchiEnerParams.GPUUSe='Half';
[AirwayMultiResEner,AirwayWallMultiResEner]=AirwayMultiResolutionEners(imaVOLVessEner,Masks,MainBronchiEnerParams);
save([ OutPutDataFolder filesep 'AirwayMultiResEner'],'AirwayMultiResEner','AirwayWallMultiResEner')
disp('Multiresolution Energy - done');

%Multiresolution Segmentation

disp('Multiresolution Segmentation - start');
MainBronchiParams.SegParam=MainBronchiSegParams;
MainBronchiParams.RedParams=MainBronchiEnerParams.RedParams;
Masks.LungMaskROI=LungMaskROIAll;
Masks.VesselMask=VesselSegHoles;
Masks.TracheaMaskROI=TracheaMaskROI;
EnerMaps.MainBronchiEner=AirwayMultiResEner;
EnerMaps.MainBronchiWallEner=AirwayWallMultiResEner;
[ MainBronchiSeg] = AirwaysSeg_from_MultiResEners( Masks,EnerMaps,MainBronchiParams );
save([ OutPutDataFolder filesep 'MainBronchiSeg'],'MainBronchiSeg')
disp('Multiresolution Segmentation - done');
