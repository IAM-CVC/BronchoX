%% 1. ROI AND MAIN LUNG STRUCTURES
%% THE FIRST STEP IS TO COMPUTE A ROI CONTAINING LUNGS (FOR MEMORY SAVING ISSUES)
%% AND EXTRACT LUNG MAIN STRUCTURES FOR THEIR USE IN THE DEFINITION OF AIRWAYS ENERGY
%% MAIN STRUCTURES ARE ALSO EXTRACTED BY THRESHOLDING OF AN ENERGY MAP
%% 
%% INPUT: imaVOL, CT volume to be processed. Load any CTData.mat file
%% 
%% PARAMETERS:
%%  LungSegParam: Parameters for Lung Segmentation
%%  TracheaEnerParams: Parameters defining energy map for trachea segmentation
%% OUTPUT VARIABLES:
%%  BodyMaskROI: Mask of patient's body
%%  imaVOLROI: ROI containing lungs. It is the volume to be processed
%%  ROI: structure containing i,j,k defining ROI in original input volume
%%  imaVOLTracheaEner: Energy for segmenting Main Lung structures (lung and trachea)
%%  LungMaskROIAll,LungMaskROIAllHoles: Lung Segmentation, full volume LungMaskROIAll; volume excluding vessels and bronchi:LungMaskROIAllHoles
%%  TracheaMaskROI: Trachea Segmentation
%%  VesselSeg, VesselSegHoles: Segmentation of vessels
%%  imaVOLVessEner: Energy used for vessel segmentation
%%
%% POSSIBLE ACCELERATION: 
%%  1. All energies are computed using convolutions. 
%%  I'm using GPU in a serial (loop) implementation
%%  2. LungSeparation has a loop until a condition is met. Maybe this could be speed-up
%%  3. Closing, opening, computation of connected components and distance maps are also used.
%%  I call matlab's native functions. Can these opertations be accelerated?

%%% Volume Body ROI
disp('Volume Body ROI - start');
[ BodyMaskROI ] = BodyMask( imaVOL, LungSegParam );
% ROI Computation
[ ROI,imaVOLROI ] = CTVolROI_02_03_2017( imaVOL,BodyMaskROI,LungSegParam );
BodyMaskROI=BodyMaskROI(ROI.i(1):ROI.i(2),ROI.j(1):ROI.j(2),ROI.k(1):ROI.k(2));
clear('imaVOL');
save([ OutPutDataFolder filesep 'BodyMaskROI'],'BodyMaskROI')
save([ OutPutDataFolder filesep 'imaVOLROI'],'imaVOLROI','ROI')
disp('Volume Body ROI - done');


%%% Lungs and Trachea
disp('Lungs and Trachea - start');
imaVOLTracheaEner=IntensityEner(imaVOLROI,TracheaEnerParams);

save([ OutPutDataFolder filesep 'imaVOLTracheaEner'],'imaVOLTracheaEner')

[ LungMaskROIAll,LungMaskROIAllHoles,~,~,TracheaMaskROI,LungSegParam] = ...
    CTLungSeg_09_05_2017( imaVOLROI,BodyMaskROI,LungSegParam,imaVOLTracheaEner );

save([ OutPutDataFolder filesep 'LungMasks'],'LungMaskROIAll','LungMaskROIAllHoles',...
'TracheaMaskROI','LungSegParam')
disp('Lungs and Trachea - done');

%%% Vessels
disp('Vessels - start');
[ VesselSeg, VesselSegHoles,imaVOLVessEner] = CTVesselSeg_22_02_2017( imaVOLROI,LungMaskROIAll,LungMaskROIAllHoles );
save([ OutPutDataFolder filesep 'VesselMasks'],'VesselSeg', 'VesselSegHoles','imaVOLVessEner')
disp('Vessels - done');


