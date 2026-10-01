%% 3. DISTAL BRONCHI

%% 3.1 INITIAL SEGMENTATION USING A GLOBAL THRESHOLD ADAPTATION
%% INPUT:
%%  FROM STEP 1: imaVOLROI,LungMaskROIAll,VesselSeg
%%  FROM STEP 2.3: MainAirwaySeg
%% PARAMETERS:
%%  DistalEnerParams: Energy of distal airways
%%  DistalSegParam: Segmentation of distal airways
%% OUTPUT VARIABLES:
%%  AirwayEner,AirwayWallEner: Energies for characterization of distal airways and their wall
%%  AirwaySeg: Distal airways segmentation
%%  G: Graph of distal airways
%%
%% POSSIBLE ACCELERATION: 
%%   1. All energies are computed using convolutions with a bank of flters. 
%%   I'm using GPU in a serial (loop) implementation
%%   2. AirwaySeg_with_GlobalThAdaptation_29_10_2018. This function searches for 
%%   an optimal threshold meeting a criteria given by the 0 of a function
%%        2.1 A very rough Bolzano method is implemented in a while loop. 
%%        Sure this can be done better
%%        2.2 At each Bolzano's iteration we compute the completxity score 
%%        of the segmentation (step 2.3). So we need to extract the skeleton from
%%        the segmentation and codify it as a graph. 
%%            2.2.1 For the skeleton we compute it once for the whole volume using 
%%            a different code than in 2.3 : Skeleton3D. This was motivated by the artifacts
%%            that the C++ code produces at main airways which drop performance of 
%%            pathbetweennodes_carles
%%            2.2.2 The pathbetweennodes_carles computes the compleaxity of the graph encoding
%%            the skeleton. It is not efficient


% Distal Energy
disp('Distal Energy - start');
DistalEnerParams.GPUUSe='Half';
[ AirwayEner,AirwayWallEner] = AirwayEners(imaVOLROI,LungMaskROIAll,VesselSeg,MainAirwaySeg,DistalEnerParams );
save([ OutPutDataFolder filesep 'AirwayEner'],'AirwayEner', 'AirwayWallEner');
% Global Segmentation with threshold adaptation
DistalSegParam.CITh=[500,975];
DistalSegParam.Th=mean(DistalSegParam.CITh);
DistalSegParam.GlobalComplexityTh=0.15; 
DistalSegParam.CITh_Diff=50;
DistalSegParam.LungMaskROI=ones(size(LungMaskROIAllHoles));
[AirwaySeg,G,DistalSegParam]=AirwaySeg_with_GlobalThAdaptation_29_10_2018( MainAirwaySeg,AirwayEner,AirwayWallEner,DistalSegParam);
save([ OutPutDataFolder filesep 'AirwaySegGlobal'],'AirwaySeg','G','DistalSegParam')
disp('Distal Energy - done');
