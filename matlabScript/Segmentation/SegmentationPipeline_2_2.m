%% 2.2 REFINEMENT OF INITIAL MAIN AIRWAYS SEGMENTATION USING 
%% A GLOBAL THRESHOLD EQUAL FOR ALL CASES
%% INPUT:
%%  FROM STEP1: imaVOLVessEner,LungMaskROIAll,VesselSeg,TracheaMaskROI 
%%  FROM STEP2.1: MainBronchiSeg
%% PARAMETERS:
%%  MainAirwaysEnerParams: Main airways energy 
%% OUTPUT VARIABLES:
%%   AirwayEner,AirwayWallEner: Energies for characterization of airways and their wall
%%   MainAirwaySeg: Refinement of initial segmentation of airways
%% 
%% POSSIBLE ACCELERATION: 
%%   1. All energies are computed using convolutions with a bank of flters. 
%%   I'm using GPU in a serial (loop) implementation

% MainAirways Energy
disp('MainAirways Energy - start');
MainAirwaysEnerParams.GPUUSe=MainBronchiEnerParams.GPUUSe;
%MainAirwaysEnerParams.GPUUSe='Half'; % Parameter to control GPU Use. Set it to 'Full' to process full volume
[ AirwayEner,AirwayWallEner] = AirwayEners(-imaVOLVessEner,LungMaskROIAll,VesselSeg,TracheaMaskROI,MainAirwaysEnerParams );
ThEner=AirwayEner.*MainBronchiSeg;
[ MainAirwaysSegParam ] = SetThreshold( ThEner, MainAirwaysSegParam);
MainAirwaysSegParam.LungMaskROI=LungMaskROIAllHoles;
[ MainAirwaySeg] = AirwaysSeg_from_Eners(MainBronchiSeg, AirwayEner,AirwayWallEner,MainAirwaysSegParam);
save([ OutPutDataFolder filesep 'MainAirwaySeg'],'MainAirwaySeg','AirwayEner','AirwayWallEner')
disp('MainAirways Energy - done');