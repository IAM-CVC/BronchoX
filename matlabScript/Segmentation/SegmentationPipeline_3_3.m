%% 3.3 DISTAL REFINMENT USING A LOCAL THRESHOLD ADAPTATION 
%% INPUT:
%% FROM 3.1:AirwayEner,AirwayWallEner
%% FROM 3.2:AirwaySeg,Leakage
%% PARAMETERS:DistalSegParam
%% OUTPUT VARIABLES:
%%  AirwaySeg: Final segmentation
%%
%% POSSIBLE ACCELERATION: 
%% 1. AirwaySeg_with_LocalThAdaptation refines each branch (connected component as in 2.3)
%% optimizing using Bolzano a function similar to the one optimized in 3.1. 
%%  Aside, it calls two functions that could be accelerated:
%%  1.1 DistalRegion4LocRefinement_29_12_2017 computes again local skeletons
%%  1.2 RefineDistalBranch_11_12_2017 
%%  Refines each branch in serial implementation over branches. 
%%  For each branch computes skeleton

%%DistalSegParam.AreaMxIncr=.5;
%%DistalSegParam.AreaMnIncr=0;
%%DistalSegParam.CITh_Diff=25; %Decrease this parameter for refined segmentation (at higher computational cost)
%%[AirwaySeg,DistalSegParam]=AirwaySeg_with_LocalThAdaptation( AirwaySeg,Leakage,AirwayEner,AirwayWallEner,DistalSegParam);
%%save([ OutPutDataFolder filesep 'AirwaySegLocal'],'AirwaySeg','DistalSegParam');
%%disp('DISTAL LOCAL SEGMENTATION - done');